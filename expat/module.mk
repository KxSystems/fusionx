objects += $S/_.$O

$S/_.$O: CFLAGS +=-Wno-discarded-qualifiers

$S/_.$O: $S/expat.c