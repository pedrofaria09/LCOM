# Makefile for lab3

COMPILER_TYPE= gnu

CC= gcc

PROG= lab3
SRCS= test3.c lab3.c

CCFLAGS= -Wall

DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -ldriver -lsys

LDFLAGS+= -L .

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>
