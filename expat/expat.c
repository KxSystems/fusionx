#include"fusion.h"
#include"k.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct vi { int major; int minor; int micro; };
struct ps { int p; unsigned char fb; };

#define FNS _(V*,XML_ParserCreate,(const C*))_(V, XML_SetElementHandler,(V*,V(*)(V*,const C*,const C**),V(*)(V*,const C*)))\
_(V,XML_SetCharacterDataHandler,(V*,V(*)(V*,const C*,I)))_(V,XML_SetUserData,(V*,V*))_(I,XML_Parse,(V*,const C*,I,I))\
_(V,XML_ParserFree,(V*))_(unsigned char,XML_ParserReset,(V*,const C*))_(struct vi,XML_ExpatVersionInfo,(V))_(I,XML_StopParser,(V*,unsigned C))\
_(I,XML_ResumeParser,(V*))_(I,XML_GetErrorCode,(V*))_(S,XML_ErrorString,(I))_(V,XML_GetParsingStatus,(V*,struct ps*))\
_(unsigned long,XML_GetCurrentLineNumber,(V*))_(unsigned long,XML_GetCurrentColumnNumber,(V*))_(long,XML_GetCurrentByteIndex,(V*))

#define _(r,n,a) Z r(*n)a;
FNS
#undef _

typedef struct { V* p; K f; K seh; K eeh; K cdh;} ctx;

Z K1(dtr) {
  //printf("dtr\n");
  ctx* c=(ctx*) kK(x)[1];
  K t = kK(x)[2];
  XML_ParserFree(c->p);
  #define FR(h) if(c->h)r0(c->h);
  FR(seh)
  FR(eeh)
  FR(cdh)
  #undef FR
  free(c);
  R (K)0;
}

/**
 * @brief Start element handler.
 * @param ud User data.
 * @param e Element name.
 * @param a Attributes.
*/
ZV seh(V*ud,const char*e,const char**a) {
  ctx *c = (ctx*)ud;
  if(!c->seh) return;
  K d;
  if (*a) {
    d = xD(knk(0,0),knk(0,0));
    for (int i=0; a[i]; i += 2) {
      jk(&kK(d)[0],ks(a[i]));
      jk(&kK(d)[1],kp(a[i+1]));
    }
    kK(d)[0]=vk(kK(d)[0]);
  } else {
    d = k(0,"::",(K)0);
  }
  K r=k(0,".",r1(c->seh),knk(3,r1(c->f),ks(e),d),(K)0);
  r0(r);
}

/**
 * @brief End element handler.
 * @param ud User data.
 * @param e Element name.
*/
ZV eeh(V*ud,const char*e) {
  ctx *c = (ctx*)ud;
  if(!c->eeh) return;
  K r=k(0,".",r1(c->eeh),knk(2,r1(c->f),ks(e)),(K)0);
  r0(r);
}

/**
* @brief Character data handler.
* @param ud User data.
* @param s Data string.
* @param n Data string length.
*/
ZV cdh(V*ud,const char*s,I n) {
  ctx *c = (ctx*)ud;
  if(!c->cdh) return;
  K r=k(0,".",r1(c->cdh),knk(2,r1(c->f),kpn((S)s,n)),(K)0);
  r0(r);
}

ZV sh(ctx *c) {
  XML_SetElementHandler(c->p,seh,eeh);
  XML_SetCharacterDataHandler(c->p,cdh);
  XML_SetUserData(c->p,c);
}

/**
 * @brief Create a parser foreign object.
 * @param x Dictionary of callback functions.
 * @return Parser foreign object.
*/
Z K1(create) {
  P(xt!=XD,krr("type"))
  ctx *c = malloc(sizeof(ctx));
  c->p=c->f=c->seh=c->eeh=c->cdh=NULL;
  c->p = XML_ParserCreate(NULL);
  sh(c);
  K h;
  #define SH(s) h = k(0,"@",r1(x),ks(#s),(K)0); if(h->t==100){c->s=h;} else {r0(h);}
  SH(seh)
  SH(eeh)
  SH(cdh)
  #undef SH
  K f = knk(2,dtr,c);
  f->t = 112;
  c->f=f;
  R f;
}

/**
* @brief Parse some or all of the document.
* @param x Parser foreign object.
* @param y XML text.
* @param z Boolean flag: is this the last piece of the document?
* @return 0 Error, 1 Success, 2 Suspended.
*/
ZK parse(K x, K y, K z) {
  P(112!=xt,  krr("type"))
  P(KC!=y->t, krr("type"))
  P(-KB!=z->t,krr("type"))
  ctx* c=(ctx*) kK(x)[1];
  I e = XML_Parse(c->p,(S)y->G0, y->n, z->g);
  R ki(e);
}

/**
 * @brief Returns Expat version.
 * @return Integer list: (major;minor;micro)
 */
Z K1(version) {
  struct vi v = XML_ExpatVersionInfo();
  R vk(knk(3, ki(v.major),ki(v.minor),ki(v.micro)));
}

/**
 * @brief Stops parsing causing @ref parse to return. Must be called within 
 * a callback handler, except when aborting (when y is False) an already suspended parser.
 * @param x Parser foreign object
 * @param y Boolean flag: is parsing resumable?
 * @return 0 Error, 1 Success.
*/
Z K2(stop) {
  P(112!=xt,  krr("type"))
  P(-KB!=y->t,krr("type"))
  ctx* c=(ctx*) kK(x)[1];
  I r = XML_StopParser(c->p, y->g);
  R ki(r);
}

/**
 * @brief Resume suspended parser. Must not be called from within a handler callback.
 * @param x Parser foreign object.
 * @return 0 Error, 1 Success.
 */
Z K1(resume) {
  P(112!=xt,krr("type"))
  ctx* c=(ctx*) kK(x)[1];
  I r = XML_ResumeParser(c->p);
  R ki(r);
}

/**
 * @brief Prepare a parser object to be reused.  After this has been called, 
 * the parser is ready to start parsing a new document.
 * @param x Parser foreign object.
 * @return Bool: success.
 */
Z K1(reset) {
  P(112!=xt,krr("type"))
  ctx* c=(ctx*) kK(x)[1];
  unsigned char r = XML_ParserReset(c->p,NULL);
  sh(c);
  R kb(r);
}

/**
 * @brief Returns status of parser.
 * @param x Parser foreign object.
 * @return List: (parser status;final buffer)
 */
Z K1(status) {
  static char* s[] = {"initialized", "parsing", "finished", "suspended"};
  P(112!=xt,krr("type"))
  ctx* c=(ctx*) kK(x)[1];
  struct ps ps;
  XML_GetParsingStatus(c->p,&ps);
  R knk(2,ks(s[ps.p]),kb(ps.fb));
}

/**
 * @brief If parse returns an error, returns what type of error has occurred.
 * @param x Parser foreign object.
 * @return Error code.
 */
Z K1(errorCode) {
  P(112!=xt,krr("type"))
  ctx* c=(ctx*) kK(x)[1];
  I r= XML_GetErrorCode(c->p);
  R ki(r);
}

/**
 * @brief Returns a string describing the error corresponding to the given error code.
 * @param x Error code.
 * @return String describing the error.
 */
Z K1(errorString) {
  P(-KI!=xt,krr("type"))
  S s= XML_ErrorString(x->i);
  K r = s ? kp(s) : kp("");
  R r;
}

/**
 * @brief Returns the line number of the current position. The first line is reported as 1.
 * @param x Parser foreign object.
 */
Z K1(lineNumber) {
  P(112!=xt,krr("type"))
  ctx* c=(ctx*) kK(x)[1];
  J n = XML_GetCurrentLineNumber(c->p);
  R kj(n);
}

/**
 * @brief Returns the offset, from the beginning of the current line, of the current position. 
 * The first column is reported as 0.
 * @param x Parser foreign object.
 */
Z K1(columnNumber) {
  P(112!=xt,krr("type"))
  ctx* c=(ctx*) kK(x)[1];
  J n = XML_GetCurrentColumnNumber(c->p);
  R kj(n);
}

F_REG {
  void*h=fusion_lib("libexpat"F_W_U_M(".dll",".so.1",".dylib"));if(!h)R krr(fusion_err());
  #define _(r,n,a) P(!(n=fusion_sym(h,#n)),krr(fusion_err()))
  FNS
  #undef _
  K n=ktn(KS,0),f=ktn(0,0);
  #define _(s,a) js(&n,ss(#s));jk(&f,dl(s,a));
  _(create,1) _(parse,3) _(status,1) _(stop,2) _(resume,1) _(reset,1) _(errorCode,1) 
  _(errorString,1) _(errorString,1) _(lineNumber,1) _(columnNumber,1) _(version,1) 
  R xD(n,f);
}
