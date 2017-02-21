# Makefile for Projeto

COMPILER_TYPE= gnu

CC= gcc

PROG= projeto
SRCS= projeto.c jogo.c

CCFLAGS= -Wall

DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -ldriver -lsys

LDFLAGS+= -L .

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>
