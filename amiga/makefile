#
# Makefile for microEmacs 3.12 using SAS/C for AmigaDOS
#
CC = gcc
#CFLAGS=$(OPT) ErrRexx CODE=F DATA=F UCHAR NOSTKCHK NOSINT
CFLAGS=-c -DNO_INLINE_STDARG
LNFLAGS=

HDIR = ../h
SRCD = ../src
INCLUDE = -I$(HDIR)

HFILES=$(HDIR)/estruct.h $(HDIR)/edef.h $(HDIR)/efunc.h $(HDIR)/epath.h $(HDIR)/ebind.h $(HDIR)/eproto.h

OFILES=amigados.o ansi.o basic.o bind.o buffer.o char.o crypt.o display.o \
	edebug.o eval.o exec.o file.o fileio.o input.o isearch.o dolock.o lock.o \
	line.o main.o mouse.o random.o region.o search.o screen.o replace.o \
	window.o word.o

uemacs:	$(OFILES)
	gcc -o uemacs $(LNFLAGS) $(OFILES)

ansi.o: $(SRCD)/ansi.c $(HFILES)
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCD)/ansi.c -o ansi.o

amigados.o: $(SRCD)/amigados.c $(HFILES)
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCD)/amigados.c -o amigados.o

basic.o: $(SRCD)/basic.c $(HFILES)
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCD)/basic.c -o basic.o

bind.o: $(SRCD)/bind.c $(HFILES)
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCD)/bind.c -o bind.o

buffer.o: $(SRCD)/buffer.c $(HFILES)
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCD)/buffer.c -o buffer.o

char.o: $(SRCD)/char.c $(HFILES)
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCD)/char.c -o char.o

crypt.o: $(SRCD)/crypt.c $(HFILES)
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCD)/crypt.c -o crypt.o

cmdfix.o: $(SRCD)/cmdfix.c $(HFILES)
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCD)/cmdfix.c -o cmdfix.o

display.o: $(SRCD)/display.c $(HFILES)
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCD)/display.c -o display.o

edebug.o: $(SRCD)/edebug.c ../h/edebug.h
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCD)/edebug.c -o edebug.o

dolock.o: $(SRCD)/dolock.c $(HFILES)
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCD)/dolock.c -o dolock.o

eval.o: $(SRCD)/eval.c $(HFILES) $(HDIR)/evar.h
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCD)/eval.c -o eval.o

exec.o: $(SRCD)/exec.c $(HFILES)
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCD)/exec.c -o exec.o

file.o: $(SRCD)/file.c $(HFILES)
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCD)/file.c -o file.o

fileio.o: $(SRCD)/fileio.c $(HFILES)
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCD)/fileio.c -o fileio.o

input.o: $(SRCD)/input.c $(HFILES)
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCD)/input.c -o input.o

isearch.o: $(SRCD)/isearch.c $(HFILES)
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCD)/isearch.c -o isearch.o

line.o: $(SRCD)/line.c $(HFILES)
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCD)/line.c -o line.o

lock.o: $(SRCD)/lock.c $(HFILES)
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCD)/lock.c -o lock.o

main.o: $(SRCD)/main.c $(HFILES)
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCD)/main.c -o main.o

mouse.o: $(SRCD)/mouse.c $(HFILES)
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCD)/mouse.c -o mouse.o

unix.o: $(SRCD)/unix.c $(HFILES)
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCD)/unix.c -o unix.o

random.o: $(SRCD)/random.c $(HFILES)
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCD)/random.c -o random.o

region.o: $(SRCD)/region.c $(HFILES)
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCD)/region.c -o region.o

search.o: $(SRCD)/search.c $(HFILES)
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCD)/search.c -o search.o

screen.o: $(SRCD)/screen.c $(HFILES)
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCD)/screen.c -o screen.o

replace.o: $(SRCD)/replace.c $(HFILES)
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCD)/replace.c -o replace.o

tags.o: $(SRCD)/tags.c $(HFILES)
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCD)/tags.c -o tags.o

window.o: $(SRCD)/window.c $(HFILES)
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCD)/window.c -o window.o

word.o: $(SRCD)/word.c $(HFILES)
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCD)/word.c -o word.o

