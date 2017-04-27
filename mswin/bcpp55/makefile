#	Borland C++ 5.5 makefile for
#		MicroEMACS 5.0
#		(C)Copyright 1993 by Daniel Lawrence
#		all rights reserved

.AUTODEPEND

# This makefile for Borland C++ 5.5 compiles MEWIN.EXE (with source-level
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
COPTS = +mewin.cfg
SRC = ..\..\src
INC = ..\..\h
OBJ = obj
BIN = bin
RES = ..

#		*List Macros*
Link_Exclude =  \
  mewin.res

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
  $(OBJ)\random.obj \
  $(OBJ)\region.obj \
  $(OBJ)\replace.obj \
  $(OBJ)\screen.obj \
  $(OBJ)\search.obj \
  $(OBJ)\tags.obj \
  $(OBJ)\undo.obj \
  $(OBJ)\window.obj \
  $(OBJ)\word.obj \
  $(OBJ)\mswdisp.obj \
  $(OBJ)\mswdrv.obj \
  $(OBJ)\mswemacs.obj \
  $(OBJ)\mswexec.obj \
  $(OBJ)\mswfile.obj \
  $(OBJ)\mswfont.obj \
  $(OBJ)\mswinput.obj \
  $(OBJ)\mswmem.obj \
  $(OBJ)\mswmenu.obj \
  $(OBJ)\mswsys.obj \
  $(OBJ)\htmlhelp.lib

#		*Explicit Rules*
$(BIN)\MEWIN.exe: directories MEWIN.cfg $(Link_Include) $(Link_Exclude)
  $(ILINK) /aa/v/s/c/Tpe/P-/L$(LIBPATH) @&&|
c0w32.obj+
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
$(OBJ)\random.obj+
$(OBJ)\region.obj+
$(OBJ)\replace.obj+
$(OBJ)\screen.obj+
$(OBJ)\search.obj+
$(OBJ)\tags.obj+
$(OBJ)\undo.obj+
$(OBJ)\window.obj+
$(OBJ)\word.obj+
$(OBJ)\mswdisp.obj+
$(OBJ)\mswdrv.obj+
$(OBJ)\mswemacs.obj+
$(OBJ)\mswexec.obj+
$(OBJ)\mswfile.obj+
$(OBJ)\mswfont.obj+
$(OBJ)\mswinput.obj+
$(OBJ)\mswmem.obj+
$(OBJ)\mswmenu.obj+
$(OBJ)\mswsys.obj
$(BIN)\MEWIN,MEWIN
$(OBJ)\htmlhelp.lib+
import32.lib+
cw32.lib,
mewin.res
|

#		*Individual File Dependencies*
$(OBJ)\htmlhelp.lib:
	implib $(OBJ)\htmlhelp.lib $(SYSTEMROOT)\system32\hhctrl.ocx

mewin.res: $(RES)\mewin.rc $(INC)\mswrid.h $(INC)\mswmenu.h $(RES)\mswmenu.rc \
	   $(RES)\mswfile.dlg $(RES)\mswabout.dlg $(RES)\mswfonts.dlg \
	   $(RES)\mswmodes.dlg $(RES)\mswprg.dlg $(RES)\mswmlh.dlg \
           $(RES)\mswapp.ico $(RES)\mswscr.ico $(RES)\mswcur.cur \
	   $(RES)\mswnot.cur $(RES)\mswwait.ico $(RES)\mswgrin1.cur \
	   $(RES)\mswgrin2.cur $(RES)\mswgrin3.cur $(RES)\mswgrin4.cur \
           $(RES)\mswgrin5.cur $(RES)\mswgrin6.cur $(RES)\mswgrin7.cur \
	   $(RES)\mswgrin8.cur
	brc32 -R -i$(INC);$(RES);$(INCLUDEPATH) -fomewin.res $(RES)\MEWIN.RC

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

$(OBJ)\mswdisp.obj: $(SRC)\mswdisp.c 
	$(CC) $(COPTS) -c -o$(OBJ)\mswdisp.obj $(SRC)\mswdisp.c

$(OBJ)\mswdrv.obj: $(SRC)\mswdrv.c 
	$(CC) $(COPTS) -c -o$(OBJ)\mswdrv.obj $(SRC)\mswdrv.c

$(OBJ)\mswemacs.obj: $(SRC)\mswemacs.c 
	$(CC) $(COPTS) -c -o$(OBJ)\mswemacs.obj $(SRC)\mswemacs.c

$(OBJ)\mswexec.obj: $(SRC)\mswexec.c 
	$(CC) $(COPTS) -c -o$(OBJ)\mswexec.obj $(SRC)\mswexec.c

$(OBJ)\mswfile.obj: $(SRC)\mswfile.c 
	$(CC) $(COPTS) -c -o$(OBJ)\mswfile.obj $(SRC)\mswfile.c

$(OBJ)\mswfont.obj: $(SRC)\mswfont.c 
	$(CC) $(COPTS) -c -o$(OBJ)\mswfont.obj $(SRC)\mswfont.c

$(OBJ)\mswinput.obj: $(SRC)\mswinput.c 
	$(CC) $(COPTS) -c -o$(OBJ)\mswinput.obj $(SRC)\mswinput.c

$(OBJ)\mswmem.obj: $(SRC)\mswmem.c 
	$(CC) $(COPTS) -c -o$(OBJ)\mswmem.obj $(SRC)\mswmem.c

$(OBJ)\mswmenu.obj: $(SRC)\mswmenu.c 
	$(CC) $(COPTS) -c -o$(OBJ)\mswmenu.obj $(SRC)\mswmenu.c

$(OBJ)\mswsys.obj: $(SRC)\mswsys.c 
	$(CC) $(COPTS) -c -o$(OBJ)\mswsys.obj $(SRC)\mswsys.c

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

mewin.cfg: makefile
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
| mewin.cfg

#		Create directories for the build

directories:
  if not exist $(BIN) mkdir $(BIN)
  if not exist $(OBJ) mkdir $(OBJ)

#		Clean up the build

clean:
  if exist $(BIN) rmdir /Q /S $(BIN)
  if exist $(OBJ) rmdir /Q /S $(OBJ)
  del memacs.cfg
  del memacs.map
  del cmdfix.map