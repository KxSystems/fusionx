CFLAGS+=-std=gnu99 -fvisibility=hidden -fPIC -I.
ifdef DEBUG
CFLAGS+=-gdwarf-2 -g3 -O0
else
CFLAGS+=-O2
endif


S = $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
objects :=
extras :=
library = fusion.$A.$E

M:=$(shell $(CC) -dumpmachine)
A:=$(if $(filter %-linux-gnu,$M),l)$(if $(findstring -apple-,$M),m)$(if $(filter %-mingw32 %-msvc,$M),w)
P:=$A
A:=$(if $A,$A$(if $(filter x86_64-%,$M),i64)$(if $(filter aarch64-% arm64-%,$M),a64))
$(if $A,,$(error couldn't determine the target platform: $(CC) -dumpmachine reports $M))
H=$(shell uname -o)

O=o
E=so
ifeq ($P,w)
O=obj
E=dll
CFLAGS+=-m64
endif

CROSS:=$(if $(and $(findstring w,$P),$(findstring Linux,$H)),x86_64-w64-mingw32-)

modules := blas pcre2 

ifeq ($A,li64)
CFLAGS+=-z noexecstack -include force_link_glibc_2.17.h
LDLIBS+=-ldl
endif

ifeq ($P,m)
CFLAGS+=-Wno-parentheses -Wno-pointer-sign
LDFLAGS=-undefined dynamic_lookup -bundle
else
LDFLAGS=-shared
endif

all:$(library)

ifeq ($P,w)
$(library):libq.a
endif

RM:=rm -f
DLLTOOL:=$(CROSS)dlltool
NM:=$(CROSS)nm

include $(addsuffix /module.mk, $(modules))

$(library):fusion.c $(objects);$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

q.lib:;wget -q https://github.com/KxSystems/kdb/raw/master/w64/q.lib

q.def:q.lib;echo 'LIBRARY q.exe'>$@;echo EXPORTS>>$@; $(NM) -p $< | grep 'T [a-zA-Z]' | sed 's/0* T //' >>$@;echo vk >>$@

libq.a:q.def;$(DLLTOOL) -l $@ -d $<

%/_.$O:;$(CC) $(CFLAGS) -DF_MOD=\"$(subst /,,$(@D))\" -r -nostdlib $^ -o $@

test: $(modules:%=test-%)
test-%: %/t.q $(library) test.q;q test.q -q -test $<
clean:;$(RM) $(objects) $(library) $(extras) *.o

.PHONY: test clean all

