#	Borland C++ 5.5 makefile for
#		MicroEMACS 5.0
#		(C)Copyright 1993 by Daniel Lawrence
#		all rights reserved

.AUTODEPEND

# This makefile for Borland C++ 5.5 compiles MEMACS.EXE (with source-level
# debugging information which you may want to remove with TDSTRIP)
# Be advised that you need to change the path of the INCLUDE and LIB
# directories to match their real location on your computer.

#	Compiler Information
CC = bcc32
LIBPATH = C:\Borland\Bcc55\LIB
INCLUDEPATH = C:\Borland\Bcc55\INCLUDE
TASM = TASM
ILINK = ilink32

#	Compilation Information
COPTS = +memacs.cfg
SRC = ..\..\src
INC = ..\..\h
OBJ = obj
BIN = bin

#		*List Macros*

Link_Include =  \
  $(OBJ)\abbrev.obj \
  $(OBJ)\basic.obj \
  $(OBJ)\bind.obj \
  $(OBJ)\buffer.obj \
  $(OBJ)\char.obj \
  $(OBJ)\crypt.obj \
  $(OBJ)\display.obj \
  $(OBJ)\dolock.obj \
  $(OBJ)\eval.obj \
  $(OBJ)\exec.obj \
  $(OBJ)\file.obj \
  $(OBJ)\fileio.obj \
  $(OBJ)\input.obj \
  $(OBJ)\isearch.obj \
  $(OBJ)\line.obj \
  $(OBJ)\lock.obj \
  $(OBJ)\main.obj \
  $(OBJ)\mouse.obj \
  $(OBJ)\nt.obj \
  $(OBJ)\ntconio.obj \
  $(OBJ)\random.obj \
  $(OBJ)\region.obj \
  $(OBJ)\replace.obj \
  $(OBJ)\screen.obj \
  $(OBJ)\search.obj \
  $(OBJ)\tags.obj \
  $(OBJ)\undo.obj \
  $(OBJ)\window.obj \
  $(OBJ)\word.obj

#		*Explicit Rules*
$(BIN)\MEMACS.exe: MEMACS.cfg $(Link_Include)
  $(ILINK) /v/s/c/Tpe/P-/L$(LIBPATH) @&&|
c0x32.obj+
$(OBJ)\abbrev.obj+
$(OBJ)\basic.obj+
$(OBJ)\bind.obj+
$(OBJ)\buffer.obj+
$(OBJ)\char.obj+
$(OBJ)\crypt.obj+
$(OBJ)\display.obj+
$(OBJ)\dolock.obj+
$(OBJ)\eval.obj+
$(OBJ)\exec.obj+
$(OBJ)\file.obj+
$(OBJ)\fileio.obj+
$(OBJ)\input.obj+
$(OBJ)\isearch.obj+
$(OBJ)\line.obj+
$(OBJ)\lock.obj+
$(OBJ)\main.obj+
$(OBJ)\mouse.obj+
$(OBJ)\nt.obj+
$(OBJ)\ntconio.obj+
$(OBJ)\random.obj+
$(OBJ)\region.obj+
$(OBJ)\replace.obj+
$(OBJ)\screen.obj+
$(OBJ)\search.obj+
$(OBJ)\tags.obj+
$(OBJ)\undo.obj+
$(OBJ)\window.obj+
$(OBJ)\word.obj
$(BIN)\MEMACS,MEMACS
import32.lib+
cw32.lib
|

$(OBJ)\abbrev.obj: $(SRC)\abbrev.c 
	$(CC) $(COPTS) -c -o$(OBJ)\abbrev.obj $(SRC)\abbrev.c

$(OBJ)\basic.obj: $(SRC)\basic.c 
	$(CC) $(COPTS) -c -o$(OBJ)\basic.obj $(SRC)\basic.c

$(OBJ)\bind.obj: $(SRC)\bind.c 
	$(CC) $(COPTS) -c -o$(OBJ)\bind.obj $(SRC)\bind.c

$(OBJ)\buffer.obj: $(SRC)\buffer.c 
	$(CC) $(COPTS) -c -o$(OBJ)\buffer.obj $(SRC)\buffer.c

$(OBJ)\char.obj: $(SRC)\char.c 
	$(CC) $(COPTS) -c -o$(OBJ)\char.obj $(SRC)\char.c

$(OBJ)\crypt.obj: $(SRC)\crypt.c 
	$(CC) $(COPTS) -c -o$(OBJ)\crypt.obj $(SRC)\crypt.c

$(OBJ)\display.obj: $(SRC)\display.c 
	$(CC) $(COPTS) -c -o$(OBJ)\display.obj $(SRC)\display.c

$(OBJ)\dolock.obj: $(SRC)\dolock.c 
	$(CC) $(COPTS) -c -o$(OBJ)\dolock.obj $(SRC)\dolock.c

$(OBJ)\eval.obj: $(SRC)\eval.c 
	$(CC) $(COPTS) -c -o$(OBJ)\eval.obj $(SRC)\eval.c

$(OBJ)\exec.obj: $(SRC)\exec.c 
	$(CC) $(COPTS) -c -o$(OBJ)\exec.obj $(SRC)\exec.c

$(OBJ)\file.obj: $(SRC)\file.c 
	$(CC) $(COPTS) -c -o$(OBJ)\file.obj $(SRC)\file.c

$(OBJ)\fileio.obj: $(SRC)\fileio.c 
	$(CC) $(COPTS) -c -o$(OBJ)\fileio.obj $(SRC)\fileio.c

$(OBJ)\input.obj: $(SRC)\input.c 
	$(CC) $(COPTS) -c -o$(OBJ)\input.obj $(SRC)\input.c

$(OBJ)\isearch.obj: $(SRC)\isearch.c 
	$(CC) $(COPTS) -c -o$(OBJ)\isearch.obj $(SRC)\isearch.c

$(OBJ)\line.obj: $(SRC)\line.c 
	$(CC) $(COPTS) -c -o$(OBJ)\line.obj $(SRC)\line.c

$(OBJ)\lock.obj: $(SRC)\lock.c 
	$(CC) $(COPTS) -c -o$(OBJ)\lock.obj $(SRC)\lock.c

$(OBJ)\main.obj: $(SRC)\main.c 
	$(CC) $(COPTS) -c -o$(OBJ)\main.obj $(SRC)\main.c

$(OBJ)\mouse.obj: $(SRC)\mouse.c 
	$(CC) $(COPTS) -c -o$(OBJ)\mouse.obj $(SRC)\mouse.c

$(OBJ)\nt.obj: $(SRC)\nt.c 
	$(CC) $(COPTS) -c -o$(OBJ)\nt.obj $(SRC)\nt.c

$(OBJ)\ntconio.obj: $(SRC)\ntconio.c 
	$(CC) $(COPTS) -c -o$(OBJ)\ntconio.obj $(SRC)\ntconio.c

$(OBJ)\search.obj: $(SRC)\search.c 
	$(CC) $(COPTS) -c -o$(OBJ)\search.obj $(SRC)\search.c

$(OBJ)\tags.obj: $(SRC)\tags.c 
	$(CC) $(COPTS) -c -o$(OBJ)\tags.obj $(SRC)\tags.c

$(OBJ)\random.obj: $(SRC)\random.c 
	$(CC) $(COPTS) -c -o$(OBJ)\random.obj $(SRC)\random.c

$(OBJ)\region.obj: $(SRC)\region.c 
	$(CC) $(COPTS) -c -o$(OBJ)\region.obj $(SRC)\region.c

$(OBJ)\replace.obj: $(SRC)\replace.c 
	$(CC) $(COPTS) -c -o$(OBJ)\replace.obj $(SRC)\replace.c

$(OBJ)\screen.obj: $(SRC)\screen.c 
	$(CC) $(COPTS) -c -o$(OBJ)\screen.obj $(SRC)\screen.c

$(OBJ)\undo.obj: $(SRC)\undo.c 
	$(CC) $(COPTS) -c -o$(OBJ)\undo.obj $(SRC)\undo.c

$(OBJ)\window.obj: $(SRC)\window.c 
	$(CC) $(COPTS) -c -o$(OBJ)\window.obj $(SRC)\window.c

$(OBJ)\word.obj: $(SRC)\word.c 
	$(CC) $(COPTS) -c -o$(OBJ)\word.obj $(SRC)\word.c

#		Create the Compiler Configuration File

memacs.cfg: makefile
  copy &&|
-6
-K
-v
-Os
-d
-WE
-w-par
-w-cpt
-w-rng
-w-pia
-w-rvl
-w-rpt
-I$(INCLUDEPATH)
-I$(INC)
| memacs.cfg

