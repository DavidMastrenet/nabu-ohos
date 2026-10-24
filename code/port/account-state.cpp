/* Small adapter to the image's existing account SDK, keeping its Binder ABI. */
#include <cstdio>
#include <cstring>
namespace OHOS::AccountSA {
class OsAccountManager {
public:
    static int IsOsAccountVerified(int,bool&);
    static int SetOsAccountIsVerified(int,bool);
};
}
int main(int argc,char **argv){
    using OHOS::AccountSA::OsAccountManager;
    bool verified=false;int ret=OsAccountManager::IsOsAccountVerified(100,verified);
    printf("account100 query=%d verified=%d\n",ret,verified);
    if(ret)return 1;
    if(argc==2&&!strcmp(argv[1],"--mark-unlocked")&&!verified){
        ret=OsAccountManager::SetOsAccountIsVerified(100,true);printf("set verified=%d\n",ret);
        if(!ret){ret=OsAccountManager::IsOsAccountVerified(100,verified);printf("account100 query=%d verified=%d\n",ret,verified);}
    }
    return ret!=0;
}
