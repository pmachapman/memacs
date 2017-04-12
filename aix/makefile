#	AIX makefile for
#		MicroEMACS 3.12
#		(C)Copyright 1993 by Daniel Lawrence
#		all rights reserved

# modify the following lines with the proper relative directories

SRCD = ../../src
OBJD = ./obj

HDIR = ../../h

# options and arguments to the C compiler

CFLAGS=	-I. -I$(HDIR) -g

# list of header files

HFILES = estruct.h $(HDIR)/edef.h $(HDIR)/efunc.h $(HDIR)/epath.h $(HDIR)/ebind.h $(HDIR)/eproto.h $(HDIR)/elang.h

#list of object files

F1 = $(OBJD)/abbrev.o $(OBJD)/basic.o $(OBJD)/bind.o $(OBJD)/buffer.o
F2 = $(OBJD)/char.o $(OBJD)/crypt.o $(OBJD)/display.o $(OBJD)/dolock.o
F3 = $(OBJD)/eval.o $(OBJD)/exec.o $(OBJD)/file.o
F4 = $(OBJD)/fileio.o $(OBJD)/unix.o $(OBJD)/input.o
F5 = $(OBJD)/isearch.o $(OBJD)/line.o $(OBJD)/lock.o
F6 = $(OBJD)/main.o $(OBJD)/mouse.o $(OBJD)/screen.o
F7 = $(OBJD)/random.o $(OBJD)/region.o $(OBJD)/search.o $(OBJD)/replace.o
F8 = $(OBJD)/tags.o $(OBJD)/window.o $(OBJD)/word.o

# product list

all:	bin/emacs

OFILES =	$(F1) $(F2) $(F3) $(F4) $(F5) $(F6) $(F7) $(F8)

bin/emacs:	$(OFILES)
		$(CC) $(CFLAGS) $(OFILES) -lcurses -ltermcap -lc -o bin/emacs

$(OBJD)/abbrev.o: $(SRCD)/abbrev.c $(HFILES)
	$(CC) $(CFLAGS) -c -o abbrev.o $(SRCD)/abbrev.c
	mv abbrev.o $(OBJD)
$(OBJD)/basic.o: $(SRCD)/basic.c $(HFILES)
	$(CC) $(CFLAGS) -c -o basic.o $(SRCD)/basic.c
	mv basic.o $(OBJD)
$(OBJD)/bind.o: $(SRCD)/bind.c $(HFILES)
	$(CC) $(CFLAGS) -c -o bind.o $(SRCD)/bind.c
	mv bind.o $(OBJD)
$(OBJD)/buffer.o: $(SRCD)/buffer.c $(HFILES)
	$(CC) $(CFLAGS) -c -o buffer.o $(SRCD)/buffer.c
	mv buffer.o $(OBJD)
$(OBJD)/char.o: $(SRCD)/char.c $(HFILES)
	$(CC) $(CFLAGS) -c -o char.o $(SRCD)/char.c
	mv char.o $(OBJD)
$(OBJD)/crypt.o: $(SRCD)/crypt.c $(HFILES)
	$(CC) $(CFLAGS) -c -o crypt.o $(SRCD)/crypt.c
	mv crypt.o $(OBJD)
$(OBJD)/cmdfix.o: $(SRCD)/cmdfix.c $(HFILES)
	$(CC) $(CFLAGS) -c -o cmdfix.o $(SRCD)/cmdfix.c
	mv cmdfix.o $(OBJD)
$(OBJD)/dolock.o: $(SRCD)/dolock.c $(HFILES)
	$(CC) $(CFLAGS) -c -o dolock.o $(SRCD)/dolock.c
	mv dolock.o $(OBJD)
$(OBJD)/eval.o: $(SRCD)/eval.c $(HFILES) $(HDIR)/evar.h
	$(CC) $(CFLAGS) -c -o eval.o $(SRCD)/eval.c
	mv eval.o $(OBJD)
$(OBJD)/exec.o: $(SRCD)/exec.c $(HFILES)
	$(CC) $(CFLAGS) -c -o exec.o $(SRCD)/exec.c
	mv exec.o $(OBJD)
$(OBJD)/file.o: $(SRCD)/file.c $(HFILES)
	$(CC) $(CFLAGS) -c -o file.o $(SRCD)/file.c
	mv file.o $(OBJD)
$(OBJD)/fileio.o: $(SRCD)/fileio.c $(HFILES)
	$(CC) $(CFLAGS) -c -o fileio.o $(SRCD)/fileio.c
	mv fileio.o $(OBJD)
$(OBJD)/input.o: $(SRCD)/input.c $(HFILES)
	$(CC) $(CFLAGS) -c -o input.o $(SRCD)/input.c
	mv input.o $(OBJD)
$(OBJD)/isearch.o: $(SRCD)/isearch.c $(HFILES)
	$(CC) $(CFLAGS) -c -o isearch.o $(SRCD)/isearch.c
	mv isearch.o $(OBJD)
$(OBJD)/line.o: $(SRCD)/line.c $(HFILES)
	$(CC) $(CFLAGS) -c -o line.o $(SRCD)/line.c
	mv line.o $(OBJD)
$(OBJD)/lock.o: $(SRCD)/lock.c $(HFILES)
	$(CC) $(CFLAGS) -c -o lock.o $(SRCD)/lock.c
	mv lock.o $(OBJD)
$(OBJD)/main.o: $(SRCD)/main.c $(HFILES)
	$(CC) $(CFLAGS) -c -o main.o $(SRCD)/main.c
	mv main.o $(OBJD)
$(OBJD)/mouse.o: $(SRCD)/mouse.c $(HFILES)
	$(CC) $(CFLAGS) -c -o mouse.o $(SRCD)/mouse.c
	mv mouse.o $(OBJD)
$(OBJD)/unix.o: $(SRCD)/unix.c $(HFILES)
	$(CC) $(CFLAGS) -c -o unix.o $(SRCD)/unix.c
	mv unix.o $(OBJD)
$(OBJD)/random.o: $(SRCD)/random.c $(HFILES)
	$(CC) $(CFLAGS) -c -o random.o $(SRCD)/random.c
	mv random.o $(OBJD)
$(OBJD)/region.o: $(SRCD)/region.c $(HFILES)
	$(CC) $(CFLAGS) -c -o region.o $(SRCD)/region.c
	mv region.o $(OBJD)
$(OBJD)/screen.o: $(SRCD)/screen.c $(HFILES)
	$(CC) $(CFLAGS) -c -o screen.o $(SRCD)/screen.c
	mv screen.o $(OBJD)
$(OBJD)/search.o: $(SRCD)/search.c $(HFILES)
	$(CC) $(CFLAGS) -c -o search.o $(SRCD)/search.c
	mv search.o $(OBJD)
$(OBJD)/replace.o: $(SRCD)/replace.c $(HFILES)
	$(CC) $(CFLAGS) -c -o replace.o $(SRCD)/replace.c
	mv replace.o $(OBJD)
$(OBJD)/tags.o: $(SRCD)/tags.c $(HFILES)
	$(CC) $(CFLAGS) -c -o tags.o $(SRCD)/tags.c
	mv tags.o $(OBJD)
$(OBJD)/window.o: $(SRCD)/window.c $(HFILES)
	$(CC) $(CFLAGS) -c -o window.o $(SRCD)/window.c
	mv window.o $(OBJD)
$(OBJD)/word.o: $(SRCD)/word.c $(HFILES)
	$(CC) $(CFLAGS) -c -o word.o $(SRCD)/word.c
	mv word.o $(OBJD)
$(OBJD)/display.o: $(SRCD)/display.c $(HFILES)
	$(CC) $(CFLAGS) -c -o display.o $(SRCD)/display.c
	mv display.o $(OBJD)
