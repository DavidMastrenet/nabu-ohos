#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <stdio.h>
#include <stdlib.h>
static void check(int ok,const char* what) { if(!ok) {printf("FAIL %s EGL=%x GL=%x\n",what,eglGetError(),glGetError());exit(1);} }
int main(void) {
 EGLDisplay d=eglGetDisplay(EGL_DEFAULT_DISPLAY);EGLint major,minor;
 check(d!=EGL_NO_DISPLAY,"display");
 check(eglInitialize(d,&major,&minor),"initialize");
 printf("EGL %d.%d vendor=%s\n",major,minor,eglQueryString(d,EGL_VENDOR));
 EGLint attrs[]={EGL_SURFACE_TYPE,EGL_PBUFFER_BIT,EGL_RENDERABLE_TYPE,EGL_OPENGL_ES2_BIT,EGL_RED_SIZE,8,EGL_GREEN_SIZE,8,EGL_BLUE_SIZE,8,EGL_NONE};
 EGLConfig config;EGLint count;
 check(eglChooseConfig(d,attrs,&config,1,&count)&&count,"choose config");
 EGLint pbuf[]={EGL_WIDTH,64,EGL_HEIGHT,64,EGL_NONE};
 EGLSurface s=eglCreatePbufferSurface(d,config,pbuf);check(s!=EGL_NO_SURFACE,"pbuffer");
 EGLint ctx[]={EGL_CONTEXT_CLIENT_VERSION,2,EGL_NONE};
 EGLContext c=eglCreateContext(d,config,EGL_NO_CONTEXT,ctx);check(c!=EGL_NO_CONTEXT,"context");
 check(eglMakeCurrent(d,s,s,c),"make current");
 printf("GPU %s; GL %s\n",glGetString(GL_RENDERER),glGetString(GL_VERSION));
 glClearColor(1,0,0,1);glClear(GL_COLOR_BUFFER_BIT);glFinish();
 unsigned char p[4]={0};glReadPixels(10,10,1,1,GL_RGBA,GL_UNSIGNED_BYTE,p);
 printf("Readback %u,%u,%u,%u\n",p[0],p[1],p[2],p[3]);
 check(p[0]>240&&p[1]<10&&p[2]<10,"pixel readback");
 eglMakeCurrent(d,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT);
 eglDestroyContext(d,c);eglDestroySurface(d,s);eglTerminate(d);
 puts("NABU OHOS EGL GPU TEST PASS");
}
