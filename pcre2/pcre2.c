#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include<stdarg.h>
#include"fusion.h"
#include"k.h"
#define FNS _(int,pcre2_pattern_info,(void*,uint32_t,void*))_(int,pcre2_get_error_message,(int,char*,size_t))_(int,pcre2_config,(uint32_t,void*))_(void*,pcre2_compile,(char*,size_t,uint32_t,int*,size_t*,void*))_(void*,pcre2_match_data_create_from_pattern,(void*,void*))\
_(int,pcre2_match,(void*,char*,size_t,size_t,uint32_t,void*,void*))_(size_t*,pcre2_get_ovector_pointer,(void*))_(void,pcre2_match_data_free,(void*))_(void,pcre2_code_free,(void*))_(int,pcre2_substitute,(void*,char*,size_t,size_t,uint32_t,void*,void*,char*,size_t,uint8_t*,size_t*))\
_(int,pcre2_next_match,(void*,size_t*,uint32_t*))
#define _(t,f,a) t(*f)a;
FNS
#undef _
enum{PCRE2_INFO_CAPTURECOUNT=4,PCRE2_INFO_NAMECOUNT=17,PCRE2_INFO_NAMEENTRYSIZE=18,PCRE2_INFO_NAMETABLE=19,PCRE2_CONFIG_VERSION=11,PCRE2_SUBSTITUTE_GLOBAL=256,PCRE2_SUBSTITUTE_OVERFLOW_LENGTH=4096,PCRE2_ERROR_NOMEMORY=-48};
V opt0(char*s,uint32_t*po){struct{char*n;uint32_t o;int f;}o[]={{"CASELESS", 0x008,0},
  {"MULTILINE",0x400,0},{"DUPNAMES", 0x040,0},{"NOTEMPTY", 0x004,1}};
 DO(sizeof(o)/sizeof(*o),if(!strcasecmp(s,o[i].n))po[o[i].f]|=o[i].o)}
K perr(I e,S s,...){ZC b[4096];S p=b+7;int n=sizeof b-7,m;va_list v;va_start(v,s);
 memcpy(b,"pcre2: ",7);m=vsnprintf(p,n,s,v);va_end(v);p+=m;n-=m;memcpy(p,": ",2);p+=2;n-=2;
 pcre2_get_error_message(e,p,n);R krr(b);}
K1(version){char b[256];R pcre2_config(PCRE2_CONFIG_VERSION,b),kp(b);}
I qg(K x){R xt==KC||xt==KG;}I qn(K x){R xt==101&&!xg;}I vt(K x){R xt<0?-xt:xt;}

V opt(K x, uint32_t*po) {if(!qn(x))DO(xt<0?1:xn,opt0(x->t<0?x->s:kS(x)[i],po))}

Z void* compile(K x, uint32_t po) {
 int en;size_t eo;void*re=pcre2_compile(kC(x),xn,po,&en,&eo,0);
 P(!re,perr(en,"compile at %zu",eo))
 R re;
}

ZI nm(void *md,size_t *off,uint32_t *po,size_t *ov) { /* zero width matches not supported unless PCRE2 version >=10.47 */
  P(pcre2_next_match,pcre2_next_match(md,off,po))
  P(ov[0]!=ov[1],(*off=ov[1], 1))
  R 0;
}

K match(K x,K y,K z){P(!qg(x)||!qg(y),krr("type"))P(!qn(z)&&vt(z)!=KS,krr("type"))
 uint32_t po[]={0,0};opt(z,po);
 void *re=compile(x,po[0]);P(!re,0);
 uint32_t cc,nc,nes;char*tp;
 pcre2_pattern_info(re,PCRE2_INFO_CAPTURECOUNT,&cc);
 pcre2_pattern_info(re,PCRE2_INFO_NAMECOUNT,&nc);
 pcre2_pattern_info(re,PCRE2_INFO_NAMEENTRYSIZE,&nes);
 pcre2_pattern_info(re,PCRE2_INFO_NAMETABLE,&tp);
 K k=ktn(KS,cc+1);DO(k->n,kS(k)[i]=0)K v=ktn(0,cc+1);DO(v->n,kK(v)[i]=ktn(0,0))
 DO(nc,kS(k)[(tp[0]<<8)|tp[1]]=ss(tp+2);tp+=nes)
 DO(cc+1,C b[32];if(!kS(k)[i])kS(k)[i]=(sprintf(b,"x%lld",i),ss(b)))
 void *md=pcre2_match_data_create_from_pattern(re,0);
 K r;size_t off=0;J lim = y->n; /* limit used as guard against infinite loop which should not happen */
 for (J i=0;i<lim;i++) {
  int rc=pcre2_match(re, kC(y),y->n,off,po[1],md,0);
  if(rc==-1) break;if(rc<-1){r0(k);r0(v);r=perr(rc,"match");goto L0;}
  size_t s,e,*ov=pcre2_get_ovector_pointer(md);
  if(!nm(md,&off,&po[1],ov)) break;
  DO(rc,s=ov[2*i];e=ov[2*i+1];if(s<~(size_t)0)jk(kK(v)+i,knk(3,kpn(kC(y)+s,e-s),kj(s),kj(e))))
 }
 r=xD(k,v);L0:pcre2_match_data_free(md);pcre2_code_free(re);R r;}

 K replace(K x1,K x2,K x3,K x4){P(!qg(x1)||!qg(x2)||!qg(x3),krr("type"))P(!qn(x4)&&vt(x4)!=KS,krr("type"))
 uint32_t po[]={0,0}; opt(x4,po);
 void *re=compile(x1,po[0]);P(!re,0);
 uint8_t *out = NULL; size_t outlen = 0;int rc, count = 0;K r;
 /* Should only repeat once. First call to pcre2_substitute sets output buffer length, second call does replacement. */
 do{
  rc=pcre2_substitute(re,kC(x2),x2->n,0,PCRE2_SUBSTITUTE_GLOBAL|PCRE2_SUBSTITUTE_OVERFLOW_LENGTH | po[1],NULL,NULL,kC(x3),x3->n,out,&outlen);
  if (rc<0) {
    if (rc == PCRE2_ERROR_NOMEMORY && outlen > 0) out=malloc(outlen);
    else {r=perr(rc,"replace");if(out)free(out);goto L0;}
  }
 }while (rc == PCRE2_ERROR_NOMEMORY && ++count < 2);
 r = kpn(out, outlen);
 L0:pcre2_code_free(re);R r;
}

F_REG{
  void*h=fusion_lib(F_W_U_M("pcre2-8.dll","libpcre2-8.so.0","libpcre2-8.dylib"));if(!h)R krr(fusion_err());
#define _(t,f,a) if(!(f=fusion_sym(h,#f"_8"))&&f!=(V*)pcre2_next_match)R krr(fusion_err());
FNS
#undef _
 K n=ktn(KS,0),f=ktn(0,0);
#define _(s,a) js(&n,ss(#s));jk(&f,dl(s,a));
 _(version,1)_(match,3)_(replace,4) R xD(n,f);}
