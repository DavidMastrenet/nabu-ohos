import difflib
import pathlib
import re
import shutil

workspace=pathlib.Path('/root/nabu-package')
kernel=pathlib.Path('/root/nabu-build/linux-6.17')
reference=workspace/'sources/kernel_ohos510'
originals={}
pending={}

# Recover the four early edits made by the initial failed preflight. Future
# patches are held in memory until all anchors have been checked.
if not (kernel/'.ohos-patched').exists():
    recovery={
        'include/linux/sched.h': '''#ifdef CONFIG_ACCESS_TOKENID
	/* OHOS access tokens are inherited with task_struct across fork/clone. */
	u64 token;
	u64 ftoken;
#endif

''',
        'drivers/Kconfig': 'source "drivers/accesstokenid/Kconfig"\n',
        'drivers/Makefile': '\nobj-$(CONFIG_ACCESS_TOKENID) += accesstokenid/\n',
        'drivers/android/Kconfig': '''
config BINDER_SENDER_INFO
	bool "OHOS Binder sender identity extensions"
	depends on ANDROID_BINDER_IPC && ACCESS_TOKENID
	default n
''',
    }
    for name,block in recovery.items():
        p=kernel/name
        text=p.read_text()
        if block in text:
            p.write_text(text.replace(block,'',1))

def read(name):
    if name in pending:
        return pending[name]
    p=kernel/name
    data=p.read_text()
    originals.setdefault(name,data)
    return data

def write(name,data):
    pending[name]=data

def replace_once(data,old,new):
    if data.count(old)!=1:
        raise ValueError(f'Expected one anchor, got {data.count(old)}: {old[:100]!r}')
    return data.replace(old,new,1)

if (kernel/'.ohos-patched').exists():
    print('OHOS kernel patches already applied')
    raise SystemExit(0)

# Keep the published OHOS driver permission checks and per-task token semantics.
dest=kernel/'drivers/accesstokenid'
dest.mkdir(exist_ok=True)
for p in (reference/'drivers/accesstokenid').iterdir():
    if p.is_file():
        name=str((dest/p.name).relative_to(kernel))
        originals[name]=''
        data=p.read_text().replace('\r\n','\n')
        data=data.replace('check_permission_for_set_token_permission()', 'check_permission_for_set_token_permission(void)',1)
        if p.name == 'access_tokenid.c':
            data=data.replace('#define ACCESS_TOKEN_UID KUIDT_INIT(3020)', '#define ACCESS_TOKEN_UID KUIDT_INIT(3020)\n#define NWEBSPAWN_UID KUIDT_INIT(3081)',1)
        write(name,data)

name='include/linux/sched.h'
data=read(name)
data=replace_once(data,'\t/* CPU-specific state of this task: */', '''#ifdef CONFIG_ACCESS_TOKENID
	/* OHOS access tokens are inherited with task_struct across fork/clone. */
	u64 token;
	u64 ftoken;
#endif

	/* CPU-specific state of this task: */''')
write(name,data)

name='drivers/Kconfig'
write(name,replace_once(read(name),'source "drivers/android/Kconfig"','source "drivers/android/Kconfig"\nsource "drivers/accesstokenid/Kconfig"'))
name='drivers/Makefile'
write(name,read(name)+'\nobj-$(CONFIG_ACCESS_TOKENID) += accesstokenid/\n')
name='drivers/android/Kconfig'
write(name,read(name)+'''
config BINDER_SENDER_INFO
	bool "OHOS Binder sender identity extensions"
	depends on ANDROID_BINDER_IPC && ACCESS_TOKENID
	default n
''')

name='include/uapi/linux/android/binder.h'
data=read(name)
types='''
/* OHOS Binder extensions, compatible with the published OHOS 5.10 UAPI. */
struct binder_feature_set {
	__u64 feature_set;
};

struct access_token {
	__u64 sender_tokenid;
	__u64 first_tokenid;
	__u64 reserved[2];
};

struct binder_sender_info {
	struct access_token tokens;
	__u64 sender_pid_nr;
};

#define BINDER_FEATURE_SET _IOWR('b', 30, struct binder_feature_set)
#define BINDER_GET_ACCESS_TOKEN _IOWR('b', 31, struct access_token)
#define BINDER_GET_SENDER_INFO _IOWR('b', 32, struct binder_sender_info)

'''
enum=re.search(r'\nenum\s*(?:[A-Za-z0-9_]+\s*)?\{\s*\n\s*BINDER_WRITE_READ',data)
if not enum:
    raise ValueError('Cannot find Binder ioctl enum in Linux 6.17 UAPI')
data=data[:enum.start()]+types+data[enum.start():]
write(name,data)

name='drivers/android/binder_internal.h'
data=read(name)
for structure,fields in [
    ('binder_thread','''
#ifdef CONFIG_ACCESS_TOKENID
	struct access_token tokens;
#endif
#ifdef CONFIG_BINDER_SENDER_INFO
	u64 sender_pid_nr;
#endif'''),
    ('binder_transaction','''
#ifdef CONFIG_ACCESS_TOKENID
	u64 sender_tokenid;
	u64 first_tokenid;
#endif'''),
]:
    start=data.index('struct '+structure+' {')
    end=data.index('\n};',start)
    data=data[:end]+fields+data[end:]
write(name,data)

name='drivers/android/binder.c'
data=read(name)
data=replace_once(data,'\tt->sender_euid = task_euid(proc->tsk);','''	t->sender_euid = task_euid(proc->tsk);
#ifdef CONFIG_ACCESS_TOKENID
	t->sender_tokenid = current->token;
	t->first_tokenid = current->ftoken;
#endif''')
old='''			trd->sender_pid =
				task_tgid_nr_ns(sender,
						task_active_pid_ns(current));
		} else {
			trd->sender_pid = 0;
		}'''
new='''			trd->sender_pid =
				task_tgid_nr_ns(sender,
						task_active_pid_ns(current));
#ifdef CONFIG_BINDER_SENDER_INFO
			binder_inner_proc_lock(thread->proc);
			thread->sender_pid_nr = task_tgid_nr(sender);
			binder_inner_proc_unlock(thread->proc);
#endif
		} else {
			trd->sender_pid = 0;
#ifdef CONFIG_BINDER_SENDER_INFO
			binder_inner_proc_lock(thread->proc);
			thread->sender_pid_nr = 0;
			binder_inner_proc_unlock(thread->proc);
#endif
		}'''
data=replace_once(data,old,new)
data=replace_once(data,'\t\tt->buffer->allow_user_free = 1;','''		t->buffer->allow_user_free = 1;
#ifdef CONFIG_ACCESS_TOKENID
		binder_inner_proc_lock(thread->proc);
		thread->tokens.sender_tokenid = t->sender_tokenid;
		thread->tokens.first_tokenid = t->first_tokenid;
		binder_inner_proc_unlock(thread->proc);
#endif''')
cases='''	case BINDER_FEATURE_SET: {
		struct binder_feature_set features = {0};
#ifdef CONFIG_ACCESS_TOKENID
		features.feature_set |= 1ULL << 0;
#endif
#ifdef CONFIG_BINDER_SENDER_INFO
		features.feature_set |= 1ULL << 2;
#endif
		if (copy_to_user(ubuf, &features, sizeof(features))) {
			ret = -EFAULT;
			goto err;
		}
		break;
	}
#ifdef CONFIG_ACCESS_TOKENID
	case BINDER_GET_ACCESS_TOKEN: {
		struct access_token tokens;
		binder_inner_proc_lock(proc);
		tokens = thread->tokens;
		binder_inner_proc_unlock(proc);
		if (copy_to_user(ubuf, &tokens, sizeof(tokens))) {
			ret = -EFAULT;
			goto err;
		}
		break;
	}
#endif
#ifdef CONFIG_BINDER_SENDER_INFO
	case BINDER_GET_SENDER_INFO: {
		struct binder_sender_info sender = {0};
		binder_inner_proc_lock(proc);
		sender.tokens = thread->tokens;
		sender.sender_pid_nr = thread->sender_pid_nr;
		binder_inner_proc_unlock(proc);
		if (copy_to_user(ubuf, &sender, sizeof(sender))) {
			ret = -EFAULT;
			goto err;
		}
		break;
	}
#endif
'''
data=replace_once(data,'\tcase BINDER_VERSION: {',cases+'\tcase BINDER_VERSION: {')
write(name,data)

for name,data in pending.items():
    (kernel/name).write_text(data)
patch=[]
for name,before in originals.items():
    after=(kernel/name).read_text()
    patch.extend(difflib.unified_diff(before.splitlines(keepends=True),after.splitlines(keepends=True),fromfile='a/'+name,tofile='b/'+name))
out=workspace/'port/patches'
out.mkdir(exist_ok=True)
(out/'ohos-linux-6.17.patch').write_text(''.join(patch))
(kernel/'.ohos-patched').write_text('OHOS AccessToken and Binder UAPI port from the published OHOS Linux 5.10 implementation.\n')
print('Patched',len(originals),'kernel files; saved port/patches/ohos-linux-6.17.patch')
