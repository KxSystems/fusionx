#ifdef _WIN32
#include <windows.h>
#else
#include<dlfcn.h>
#endif
#include"k.h"
#include"fusion.h"
#include<stdlib.h>
#include<stdio.h>

ZK REG;V fusion_reg(S s,V*f){K x=REG;if(!x)x=xD(ktn(KS,0),ktn(0,0));js(&xx,ss(s));jk(&xy,dl(f,1));REG=x;}

#ifdef _WIN32
ZV*lib(S s){R LoadLibraryA(s);}V*fusion_sym(V*h,S s){R GetProcAddress(h,s);}
S fusion_err() { 
  static char b[65535]; 
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), b, (DWORD)(sizeof(b)), NULL); 
  R b;
}
#else
ZV*lib(S s){R dlopen(s,RTLD_LAZY|RTLD_LOCAL);}V*fusion_sym(V*h,S s){R dlsym(h,s);}S fusion_err(){R dlerror();}
#endif
V*fusion_lib(S s){S e=getenv("CONDA_PREFIX");C b[4096];V*r=0;if(e&&*e){snprintf(b,4095,"%s/lib/%s",e,s);r=lib(b);}R r?r:lib(s);}

Z K2(at){K z=knk(1,r1(y)),r=dot(x,z);R r0(z),r;}Z K1(call){K r=at(REG,*kK(x));P(!r||r->t!=112,krr("domain"))R at(r,x);}

F_EXP K1(fusion){K y,r;P(xt==101&&!xg,kp("hello"))P(xt==-KS,(r=fusion(y=knk(1,r1(x))),r0(y),r))
 P(!xt,!xn?krr("length"):xK[0]->t!=-KS?krr("type"):call(x))R 0;}

Z K1(libs){R k(0, "key", r1(REG), 0);}

F_EXP K1(kexport){R k(0,"{([libs:x])}",dl(libs, 1),0);}
F_EXP K2(kprovide){
  K z = y->t==98? knk(2,r1(x),r1(y->k)) : x;
  R k(0,"k){.[`/:(.z.M;x);();:;``export!(::;y)]}",r1(x),fusion(z),0);
}

Z K1(dir){R r1(*kK(REG));}
F_INIT{fusion_reg("",dir);}
