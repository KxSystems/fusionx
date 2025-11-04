#define F_INIT __attribute__((constructor))ZV __()
#define F_EXP __attribute__((visibility("default")))
void fusion_reg(char*,void*);
void*fusion_lib(char*),*fusion_sym(void*,char*);char*fusion_err();
#define F_REG Z K1(_);F_INIT{fusion_reg(F_MOD,_);}Z K1(_)
#if _WIN32
#define F_W_U(w,u) w
#else
#define F_W_U(w,u) u
#endif
#if _WIN32
#define F_W_U_M(w,u,m) w
#elif __APPLE__
#define F_W_U_M(w,u,m) m
#else
#define F_W_U_M(w,u,m) u
#endif
