# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 Release" && "$(CFG)" != "Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "ntemacs.mak" CFG="Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 Debug"
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : .\WinRel\ntemacs.exe .\WinRel\ntemacs.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /ML /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /ML /W3 /GX /O2 /I "C:\dev\memacs\h" /I "C:\dev\memacs" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "BUILD_NTCONSOLE" /FR /c
# SUBTRACT CPP /YX
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "C:\dev\memacs\h" /I "C:\dev\memacs" /D\
 "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "BUILD_NTCONSOLE" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "C:\dev\memacs\h" /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"MEWIN16.res" /i "C:\dev\memacs\h" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"ntemacs.bsc" 
BSC32_SBRS= \
	.\WinRel\tags.sbr \
	.\WinRel\bind.sbr \
	.\WinRel\nt.sbr \
	.\WinRel\display.sbr \
	.\WinRel\replace.sbr \
	.\WinRel\ntconio.sbr \
	.\WinRel\undo.sbr \
	.\WinRel\mouse.sbr \
	.\WinRel\main.sbr \
	.\WinRel\input.sbr \
	.\WinRel\exec.sbr \
	.\WinRel\eval.sbr \
	.\WinRel\crypt.sbr \
	.\WinRel\abbrev.sbr \
	.\WinRel\char.sbr \
	.\WinRel\region.sbr \
	.\WinRel\basic.sbr \
	.\WinRel\isearch.sbr \
	.\WinRel\search.sbr \
	.\WinRel\random.sbr \
	.\WinRel\keyboard.sbr \
	.\WinRel\buffer.sbr \
	.\WinRel\word.sbr \
	.\WinRel\fileio.sbr \
	.\WinRel\screen.sbr \
	.\WinRel\lock.sbr \
	.\WinRel\line.sbr \
	.\WinRel\file.sbr \
	.\WinRel\dolock.sbr \
	.\WinRel\window.sbr

.\WinRel\ntemacs.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:console /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:console /MACHINE:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /NOLOGO /SUBSYSTEM:console /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"ntemacs.pdb" /MACHINE:I386 /OUT:$(OUTDIR)/"ntemacs.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinRel\tags.obj \
	.\WinRel\MEWIN16.res \
	.\WinRel\bind.obj \
	.\WinRel\nt.obj \
	.\WinRel\display.obj \
	.\WinRel\replace.obj \
	.\WinRel\ntconio.obj \
	.\WinRel\undo.obj \
	.\WinRel\mouse.obj \
	.\WinRel\main.obj \
	.\WinRel\input.obj \
	.\WinRel\exec.obj \
	.\WinRel\eval.obj \
	.\WinRel\crypt.obj \
	.\WinRel\abbrev.obj \
	.\WinRel\char.obj \
	.\WinRel\region.obj \
	.\WinRel\basic.obj \
	.\WinRel\isearch.obj \
	.\WinRel\search.obj \
	.\WinRel\random.obj \
	.\WinRel\keyboard.obj \
	.\WinRel\buffer.obj \
	.\WinRel\word.obj \
	.\WinRel\fileio.obj \
	.\WinRel\screen.obj \
	.\WinRel\lock.obj \
	.\WinRel\line.obj \
	.\WinRel\file.obj \
	.\WinRel\dolock.obj \
	.\WinRel\window.obj

.\WinRel\ntemacs.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : .\WinDebug\ntemacs.exe .\WinDebug\ntemacs.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /ML /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /ML /W3 /GX /Zi /Od /I "C:\dev\memacs\h" /I "C:\dev\memacs" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "BUILD_NTCONSOLE" /FR /c
# SUBTRACT CPP /YX
CPP_PROJ=/nologo /ML /W3 /GX /Zi /Od /I "C:\dev\memacs\h" /I "C:\dev\memacs" /D\
 "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "BUILD_NTCONSOLE" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"ntemacs.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "C:\dev\memacs\h" /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"MEWIN16.res" /i "C:\dev\memacs\h" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"ntemacs.bsc" 
BSC32_SBRS= \
	.\WinDebug\tags.sbr \
	.\WinDebug\bind.sbr \
	.\WinDebug\nt.sbr \
	.\WinDebug\display.sbr \
	.\WinDebug\replace.sbr \
	.\WinDebug\ntconio.sbr \
	.\WinDebug\undo.sbr \
	.\WinDebug\mouse.sbr \
	.\WinDebug\main.sbr \
	.\WinDebug\input.sbr \
	.\WinDebug\exec.sbr \
	.\WinDebug\eval.sbr \
	.\WinDebug\crypt.sbr \
	.\WinDebug\abbrev.sbr \
	.\WinDebug\char.sbr \
	.\WinDebug\region.sbr \
	.\WinDebug\basic.sbr \
	.\WinDebug\isearch.sbr \
	.\WinDebug\search.sbr \
	.\WinDebug\random.sbr \
	.\WinDebug\keyboard.sbr \
	.\WinDebug\buffer.sbr \
	.\WinDebug\word.sbr \
	.\WinDebug\fileio.sbr \
	.\WinDebug\screen.sbr \
	.\WinDebug\lock.sbr \
	.\WinDebug\line.sbr \
	.\WinDebug\file.sbr \
	.\WinDebug\dolock.sbr \
	.\WinDebug\window.sbr

.\WinDebug\ntemacs.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /NOLOGO /SUBSYSTEM:console /INCREMENTAL:yes\
 /PDB:$(OUTDIR)/"ntemacs.pdb" /DEBUG /MACHINE:I386 /OUT:$(OUTDIR)/"ntemacs.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinDebug\tags.obj \
	.\WinDebug\MEWIN16.res \
	.\WinDebug\bind.obj \
	.\WinDebug\nt.obj \
	.\WinDebug\display.obj \
	.\WinDebug\replace.obj \
	.\WinDebug\ntconio.obj \
	.\WinDebug\undo.obj \
	.\WinDebug\mouse.obj \
	.\WinDebug\main.obj \
	.\WinDebug\input.obj \
	.\WinDebug\exec.obj \
	.\WinDebug\eval.obj \
	.\WinDebug\crypt.obj \
	.\WinDebug\abbrev.obj \
	.\WinDebug\char.obj \
	.\WinDebug\region.obj \
	.\WinDebug\basic.obj \
	.\WinDebug\isearch.obj \
	.\WinDebug\search.obj \
	.\WinDebug\random.obj \
	.\WinDebug\keyboard.obj \
	.\WinDebug\buffer.obj \
	.\WinDebug\word.obj \
	.\WinDebug\fileio.obj \
	.\WinDebug\screen.obj \
	.\WinDebug\lock.obj \
	.\WinDebug\line.obj \
	.\WinDebug\file.obj \
	.\WinDebug\dolock.obj \
	.\WinDebug\window.obj

.\WinDebug\ntemacs.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=\dev\memacs\src\tags.c
DEP_TAGS_=\
	\dev\memacs\h\estruct.h\
	\dev\memacs\h\eproto.h\
	\dev\memacs\h\edef.h\
	\dev\memacs\h\elang.h\
	\dev\memacs\h\estruct_orig.h\
	\dev\memacs\h\english.h\
	\dev\memacs\h\frenchis.h\
	\dev\memacs\h\french.h\
	\dev\memacs\h\spanish.h\
	\dev\memacs\h\german.h\
	\dev\memacs\h\dutch.h\
	\dev\memacs\h\platin.h\
	\dev\memacs\h\japan.h\
	\dev\memacs\h\latin.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\tags.obj :  $(SOURCE)  $(DEP_TAGS_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\tags.obj :  $(SOURCE)  $(DEP_TAGS_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\dev\memacs\mswin\MEWIN16.RC
DEP_MEWIN=\
	\dev\memacs\mswin\mswcur.cur\
	\dev\memacs\mswin\mswnot.cur\
	\dev\memacs\mswin\mswgrin1.cur\
	\dev\memacs\mswin\mswgrin2.cur\
	\dev\memacs\mswin\mswgrin3.cur\
	\dev\memacs\mswin\mswgrin4.cur\
	\dev\memacs\mswin\mswgrin5.cur\
	\dev\memacs\mswin\mswgrin6.cur\
	\dev\memacs\mswin\mswgrin7.cur\
	\dev\memacs\mswin\mswgrin8.cur\
	\dev\memacs\mswin\mswapp.ico\
	\dev\memacs\mswin\mswscr.ico\
	\dev\memacs\mswin\mswwait.ico\
	\dev\memacs\mswin\mswmenu.rc\
	\dev\memacs\mswin\mswfile.dlg\
	\dev\memacs\mswin\mswabout.dlg\
	\dev\memacs\mswin\mswfonts.dlg\
	\dev\memacs\mswin\mswmodes.dlg\
	\dev\memacs\mswin\mswprg.dlg\
	\dev\memacs\mswin\mswmlh.dlg

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\MEWIN16.res :  $(SOURCE)  $(DEP_MEWIN) $(INTDIR)
   $(RSC) /l 0x409 /fo$(INTDIR)/"MEWIN16.res" /i "C:\dev\memacs\h" /i\
 "\dev\memacs\mswin" /d "NDEBUG"  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\MEWIN16.res :  $(SOURCE)  $(DEP_MEWIN) $(INTDIR)
   $(RSC) /l 0x409 /fo$(INTDIR)/"MEWIN16.res" /i "C:\dev\memacs\h" /i\
 "\dev\memacs\mswin" /d "_DEBUG"  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\dev\memacs\src\bind.c
DEP_BIND_=\
	\dev\memacs\h\estruct.h\
	\dev\memacs\h\eproto.h\
	\dev\memacs\h\edef.h\
	\dev\memacs\h\elang.h\
	\dev\memacs\h\epath.h\
	\dev\memacs\h\estruct_orig.h\
	\dev\memacs\h\english.h\
	\dev\memacs\h\frenchis.h\
	\dev\memacs\h\french.h\
	\dev\memacs\h\spanish.h\
	\dev\memacs\h\german.h\
	\dev\memacs\h\dutch.h\
	\dev\memacs\h\platin.h\
	\dev\memacs\h\japan.h\
	\dev\memacs\h\latin.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\bind.obj :  $(SOURCE)  $(DEP_BIND_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\bind.obj :  $(SOURCE)  $(DEP_BIND_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\dev\memacs\src\nt.c
DEP_NT_C5=\
	\dev\memacs\h\estruct.h\
	\dev\memacs\h\eproto.h\
	\dev\memacs\h\elang.h\
	\dev\memacs\h\edef.h\
	\dev\memacs\h\estruct_orig.h\
	\dev\memacs\h\english.h\
	\dev\memacs\h\frenchis.h\
	\dev\memacs\h\french.h\
	\dev\memacs\h\spanish.h\
	\dev\memacs\h\german.h\
	\dev\memacs\h\dutch.h\
	\dev\memacs\h\platin.h\
	\dev\memacs\h\japan.h\
	\dev\memacs\h\latin.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\nt.obj :  $(SOURCE)  $(DEP_NT_C5) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\nt.obj :  $(SOURCE)  $(DEP_NT_C5) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\dev\memacs\src\display.c
DEP_DISPL=\
	\dev\memacs\h\estruct.h\
	\dev\memacs\h\eproto.h\
	\dev\memacs\h\edef.h\
	\dev\memacs\h\elang.h\
	\dev\memacs\h\estruct_orig.h\
	\dev\memacs\h\english.h\
	\dev\memacs\h\frenchis.h\
	\dev\memacs\h\french.h\
	\dev\memacs\h\spanish.h\
	\dev\memacs\h\german.h\
	\dev\memacs\h\dutch.h\
	\dev\memacs\h\platin.h\
	\dev\memacs\h\japan.h\
	\dev\memacs\h\latin.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\display.obj :  $(SOURCE)  $(DEP_DISPL) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\display.obj :  $(SOURCE)  $(DEP_DISPL) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\dev\memacs\src\replace.c
DEP_REPLA=\
	\dev\memacs\h\estruct.h\
	\dev\memacs\h\eproto.h\
	\dev\memacs\h\edef.h\
	\dev\memacs\h\elang.h\
	\dev\memacs\h\estruct_orig.h\
	\dev\memacs\h\english.h\
	\dev\memacs\h\frenchis.h\
	\dev\memacs\h\french.h\
	\dev\memacs\h\spanish.h\
	\dev\memacs\h\german.h\
	\dev\memacs\h\dutch.h\
	\dev\memacs\h\platin.h\
	\dev\memacs\h\japan.h\
	\dev\memacs\h\latin.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\replace.obj :  $(SOURCE)  $(DEP_REPLA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\replace.obj :  $(SOURCE)  $(DEP_REPLA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\dev\memacs\src\ntconio.c
DEP_NTCON=\
	\dev\memacs\h\estruct.h\
	\dev\memacs\h\eproto.h\
	\dev\memacs\h\edef.h\
	\dev\memacs\h\elang.h\
	\dev\memacs\h\estruct_orig.h\
	\dev\memacs\h\english.h\
	\dev\memacs\h\frenchis.h\
	\dev\memacs\h\french.h\
	\dev\memacs\h\spanish.h\
	\dev\memacs\h\german.h\
	\dev\memacs\h\dutch.h\
	\dev\memacs\h\platin.h\
	\dev\memacs\h\japan.h\
	\dev\memacs\h\latin.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\ntconio.obj :  $(SOURCE)  $(DEP_NTCON) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\ntconio.obj :  $(SOURCE)  $(DEP_NTCON) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\dev\memacs\src\undo.c
DEP_UNDO_=\
	\dev\memacs\h\estruct.h\
	\dev\memacs\h\eproto.h\
	\dev\memacs\h\edef.h\
	\dev\memacs\h\elang.h\
	\dev\memacs\h\estruct_orig.h\
	\dev\memacs\h\english.h\
	\dev\memacs\h\frenchis.h\
	\dev\memacs\h\french.h\
	\dev\memacs\h\spanish.h\
	\dev\memacs\h\german.h\
	\dev\memacs\h\dutch.h\
	\dev\memacs\h\platin.h\
	\dev\memacs\h\japan.h\
	\dev\memacs\h\latin.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\undo.obj :  $(SOURCE)  $(DEP_UNDO_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\undo.obj :  $(SOURCE)  $(DEP_UNDO_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\dev\memacs\src\mouse.c
DEP_MOUSE=\
	\dev\memacs\h\estruct.h\
	\dev\memacs\h\eproto.h\
	\dev\memacs\h\edef.h\
	\dev\memacs\h\elang.h\
	\dev\memacs\h\estruct_orig.h\
	\dev\memacs\h\english.h\
	\dev\memacs\h\frenchis.h\
	\dev\memacs\h\french.h\
	\dev\memacs\h\spanish.h\
	\dev\memacs\h\german.h\
	\dev\memacs\h\dutch.h\
	\dev\memacs\h\platin.h\
	\dev\memacs\h\japan.h\
	\dev\memacs\h\latin.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\mouse.obj :  $(SOURCE)  $(DEP_MOUSE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\mouse.obj :  $(SOURCE)  $(DEP_MOUSE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\dev\memacs\src\main.c
DEP_MAIN_=\
	\dev\memacs\h\estruct.h\
	\dev\memacs\h\eproto.h\
	\dev\memacs\h\efunc.h\
	\dev\memacs\h\edef.h\
	\dev\memacs\h\elang.h\
	\dev\memacs\h\ebind.h\
	\dev\memacs\h\estruct_orig.h\
	\dev\memacs\h\english.h\
	\dev\memacs\h\frenchis.h\
	\dev\memacs\h\french.h\
	\dev\memacs\h\spanish.h\
	\dev\memacs\h\german.h\
	\dev\memacs\h\dutch.h\
	\dev\memacs\h\platin.h\
	\dev\memacs\h\japan.h\
	\dev\memacs\h\latin.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\main.obj :  $(SOURCE)  $(DEP_MAIN_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\main.obj :  $(SOURCE)  $(DEP_MAIN_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\dev\memacs\src\input.c
DEP_INPUT=\
	\dev\memacs\h\estruct.h\
	\dev\memacs\h\eproto.h\
	\dev\memacs\h\edef.h\
	\dev\memacs\h\elang.h\
	\dev\memacs\h\estruct_orig.h\
	\dev\memacs\h\english.h\
	\dev\memacs\h\frenchis.h\
	\dev\memacs\h\french.h\
	\dev\memacs\h\spanish.h\
	\dev\memacs\h\german.h\
	\dev\memacs\h\dutch.h\
	\dev\memacs\h\platin.h\
	\dev\memacs\h\japan.h\
	\dev\memacs\h\latin.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\input.obj :  $(SOURCE)  $(DEP_INPUT) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\input.obj :  $(SOURCE)  $(DEP_INPUT) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\dev\memacs\src\exec.c
DEP_EXEC_=\
	\dev\memacs\h\estruct.h\
	\dev\memacs\h\eproto.h\
	\dev\memacs\h\edef.h\
	\dev\memacs\h\elang.h\
	\dev\memacs\h\estruct_orig.h\
	\dev\memacs\h\english.h\
	\dev\memacs\h\frenchis.h\
	\dev\memacs\h\french.h\
	\dev\memacs\h\spanish.h\
	\dev\memacs\h\german.h\
	\dev\memacs\h\dutch.h\
	\dev\memacs\h\platin.h\
	\dev\memacs\h\japan.h\
	\dev\memacs\h\latin.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\exec.obj :  $(SOURCE)  $(DEP_EXEC_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\exec.obj :  $(SOURCE)  $(DEP_EXEC_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\dev\memacs\src\eval.c
DEP_EVAL_=\
	\dev\memacs\h\estruct.h\
	\dev\memacs\h\eproto.h\
	\dev\memacs\h\edef.h\
	\dev\memacs\h\elang.h\
	\dev\memacs\h\evar.h\
	\dev\memacs\h\estruct_orig.h\
	\dev\memacs\h\english.h\
	\dev\memacs\h\frenchis.h\
	\dev\memacs\h\french.h\
	\dev\memacs\h\spanish.h\
	\dev\memacs\h\german.h\
	\dev\memacs\h\dutch.h\
	\dev\memacs\h\platin.h\
	\dev\memacs\h\japan.h\
	\dev\memacs\h\latin.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\eval.obj :  $(SOURCE)  $(DEP_EVAL_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\eval.obj :  $(SOURCE)  $(DEP_EVAL_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\dev\memacs\src\crypt.c
DEP_CRYPT=\
	\dev\memacs\h\estruct.h\
	\dev\memacs\h\eproto.h\
	\dev\memacs\h\edef.h\
	\dev\memacs\h\elang.h\
	\dev\memacs\h\estruct_orig.h\
	\dev\memacs\h\english.h\
	\dev\memacs\h\frenchis.h\
	\dev\memacs\h\french.h\
	\dev\memacs\h\spanish.h\
	\dev\memacs\h\german.h\
	\dev\memacs\h\dutch.h\
	\dev\memacs\h\platin.h\
	\dev\memacs\h\japan.h\
	\dev\memacs\h\latin.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\crypt.obj :  $(SOURCE)  $(DEP_CRYPT) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\crypt.obj :  $(SOURCE)  $(DEP_CRYPT) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\dev\memacs\src\abbrev.c
DEP_ABBRE=\
	\dev\memacs\h\estruct.h\
	\dev\memacs\h\eproto.h\
	\dev\memacs\h\edef.h\
	\dev\memacs\h\elang.h\
	\dev\memacs\h\estruct_orig.h\
	\dev\memacs\h\english.h\
	\dev\memacs\h\frenchis.h\
	\dev\memacs\h\french.h\
	\dev\memacs\h\spanish.h\
	\dev\memacs\h\german.h\
	\dev\memacs\h\dutch.h\
	\dev\memacs\h\platin.h\
	\dev\memacs\h\japan.h\
	\dev\memacs\h\latin.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\abbrev.obj :  $(SOURCE)  $(DEP_ABBRE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\abbrev.obj :  $(SOURCE)  $(DEP_ABBRE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\dev\memacs\src\char.c
DEP_CHAR_=\
	\dev\memacs\h\estruct.h\
	\dev\memacs\h\eproto.h\
	\dev\memacs\h\edef.h\
	\dev\memacs\h\elang.h\
	\dev\memacs\h\estruct_orig.h\
	\dev\memacs\h\english.h\
	\dev\memacs\h\frenchis.h\
	\dev\memacs\h\french.h\
	\dev\memacs\h\spanish.h\
	\dev\memacs\h\german.h\
	\dev\memacs\h\dutch.h\
	\dev\memacs\h\platin.h\
	\dev\memacs\h\japan.h\
	\dev\memacs\h\latin.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\char.obj :  $(SOURCE)  $(DEP_CHAR_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\char.obj :  $(SOURCE)  $(DEP_CHAR_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\dev\memacs\src\region.c
DEP_REGIO=\
	\dev\memacs\h\estruct.h\
	\dev\memacs\h\eproto.h\
	\dev\memacs\h\edef.h\
	\dev\memacs\h\elang.h\
	\dev\memacs\h\estruct_orig.h\
	\dev\memacs\h\english.h\
	\dev\memacs\h\frenchis.h\
	\dev\memacs\h\french.h\
	\dev\memacs\h\spanish.h\
	\dev\memacs\h\german.h\
	\dev\memacs\h\dutch.h\
	\dev\memacs\h\platin.h\
	\dev\memacs\h\japan.h\
	\dev\memacs\h\latin.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\region.obj :  $(SOURCE)  $(DEP_REGIO) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\region.obj :  $(SOURCE)  $(DEP_REGIO) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\dev\memacs\src\basic.c
DEP_BASIC=\
	\dev\memacs\h\estruct.h\
	\dev\memacs\h\eproto.h\
	\dev\memacs\h\edef.h\
	\dev\memacs\h\elang.h\
	\dev\memacs\h\estruct_orig.h\
	\dev\memacs\h\english.h\
	\dev\memacs\h\frenchis.h\
	\dev\memacs\h\french.h\
	\dev\memacs\h\spanish.h\
	\dev\memacs\h\german.h\
	\dev\memacs\h\dutch.h\
	\dev\memacs\h\platin.h\
	\dev\memacs\h\japan.h\
	\dev\memacs\h\latin.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\basic.obj :  $(SOURCE)  $(DEP_BASIC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\basic.obj :  $(SOURCE)  $(DEP_BASIC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\dev\memacs\src\isearch.c
DEP_ISEAR=\
	\dev\memacs\h\estruct.h\
	\dev\memacs\h\eproto.h\
	\dev\memacs\h\edef.h\
	\dev\memacs\h\elang.h\
	\dev\memacs\h\estruct_orig.h\
	\dev\memacs\h\english.h\
	\dev\memacs\h\frenchis.h\
	\dev\memacs\h\french.h\
	\dev\memacs\h\spanish.h\
	\dev\memacs\h\german.h\
	\dev\memacs\h\dutch.h\
	\dev\memacs\h\platin.h\
	\dev\memacs\h\japan.h\
	\dev\memacs\h\latin.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\isearch.obj :  $(SOURCE)  $(DEP_ISEAR) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\isearch.obj :  $(SOURCE)  $(DEP_ISEAR) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\dev\memacs\src\search.c
DEP_SEARC=\
	\dev\memacs\h\estruct.h\
	\dev\memacs\h\eproto.h\
	\dev\memacs\h\edef.h\
	\dev\memacs\h\elang.h\
	\dev\memacs\h\estruct_orig.h\
	\dev\memacs\h\english.h\
	\dev\memacs\h\frenchis.h\
	\dev\memacs\h\french.h\
	\dev\memacs\h\spanish.h\
	\dev\memacs\h\german.h\
	\dev\memacs\h\dutch.h\
	\dev\memacs\h\platin.h\
	\dev\memacs\h\japan.h\
	\dev\memacs\h\latin.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\search.obj :  $(SOURCE)  $(DEP_SEARC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\search.obj :  $(SOURCE)  $(DEP_SEARC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\dev\memacs\src\random.c
DEP_RANDO=\
	\dev\memacs\h\estruct.h\
	\dev\memacs\h\eproto.h\
	\dev\memacs\h\edef.h\
	\dev\memacs\h\elang.h\
	\dev\memacs\h\estruct_orig.h\
	\dev\memacs\h\english.h\
	\dev\memacs\h\frenchis.h\
	\dev\memacs\h\french.h\
	\dev\memacs\h\spanish.h\
	\dev\memacs\h\german.h\
	\dev\memacs\h\dutch.h\
	\dev\memacs\h\platin.h\
	\dev\memacs\h\japan.h\
	\dev\memacs\h\latin.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\random.obj :  $(SOURCE)  $(DEP_RANDO) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\random.obj :  $(SOURCE)  $(DEP_RANDO) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\dev\memacs\src\keyboard.c
DEP_KEYBO=\
	\dev\memacs\h\estruct.h\
	\dev\memacs\h\eproto.h\
	\dev\memacs\h\edef.h\
	\dev\memacs\h\elang.h\
	\dev\memacs\h\estruct_orig.h\
	\dev\memacs\h\english.h\
	\dev\memacs\h\frenchis.h\
	\dev\memacs\h\french.h\
	\dev\memacs\h\spanish.h\
	\dev\memacs\h\german.h\
	\dev\memacs\h\dutch.h\
	\dev\memacs\h\platin.h\
	\dev\memacs\h\japan.h\
	\dev\memacs\h\latin.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\keyboard.obj :  $(SOURCE)  $(DEP_KEYBO) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\keyboard.obj :  $(SOURCE)  $(DEP_KEYBO) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\dev\memacs\src\buffer.c
DEP_BUFFE=\
	\dev\memacs\h\estruct.h\
	\dev\memacs\h\eproto.h\
	\dev\memacs\h\edef.h\
	\dev\memacs\h\elang.h\
	\dev\memacs\h\estruct_orig.h\
	\dev\memacs\h\english.h\
	\dev\memacs\h\frenchis.h\
	\dev\memacs\h\french.h\
	\dev\memacs\h\spanish.h\
	\dev\memacs\h\german.h\
	\dev\memacs\h\dutch.h\
	\dev\memacs\h\platin.h\
	\dev\memacs\h\japan.h\
	\dev\memacs\h\latin.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\buffer.obj :  $(SOURCE)  $(DEP_BUFFE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\buffer.obj :  $(SOURCE)  $(DEP_BUFFE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\dev\memacs\src\word.c
DEP_WORD_=\
	\dev\memacs\h\estruct.h\
	\dev\memacs\h\eproto.h\
	\dev\memacs\h\edef.h\
	\dev\memacs\h\elang.h\
	\dev\memacs\h\estruct_orig.h\
	\dev\memacs\h\english.h\
	\dev\memacs\h\frenchis.h\
	\dev\memacs\h\french.h\
	\dev\memacs\h\spanish.h\
	\dev\memacs\h\german.h\
	\dev\memacs\h\dutch.h\
	\dev\memacs\h\platin.h\
	\dev\memacs\h\japan.h\
	\dev\memacs\h\latin.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\word.obj :  $(SOURCE)  $(DEP_WORD_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\word.obj :  $(SOURCE)  $(DEP_WORD_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\dev\memacs\src\fileio.c
DEP_FILEI=\
	\dev\memacs\h\estruct.h\
	\dev\memacs\h\eproto.h\
	\dev\memacs\h\edef.h\
	\dev\memacs\h\elang.h\
	\dev\memacs\h\estruct_orig.h\
	\dev\memacs\h\english.h\
	\dev\memacs\h\frenchis.h\
	\dev\memacs\h\french.h\
	\dev\memacs\h\spanish.h\
	\dev\memacs\h\german.h\
	\dev\memacs\h\dutch.h\
	\dev\memacs\h\platin.h\
	\dev\memacs\h\japan.h\
	\dev\memacs\h\latin.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\fileio.obj :  $(SOURCE)  $(DEP_FILEI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\fileio.obj :  $(SOURCE)  $(DEP_FILEI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\dev\memacs\src\screen.c
DEP_SCREE=\
	\dev\memacs\h\estruct.h\
	\dev\memacs\h\eproto.h\
	\dev\memacs\h\edef.h\
	\dev\memacs\h\elang.h\
	\dev\memacs\h\estruct_orig.h\
	\dev\memacs\h\english.h\
	\dev\memacs\h\frenchis.h\
	\dev\memacs\h\french.h\
	\dev\memacs\h\spanish.h\
	\dev\memacs\h\german.h\
	\dev\memacs\h\dutch.h\
	\dev\memacs\h\platin.h\
	\dev\memacs\h\japan.h\
	\dev\memacs\h\latin.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\screen.obj :  $(SOURCE)  $(DEP_SCREE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\screen.obj :  $(SOURCE)  $(DEP_SCREE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\dev\memacs\src\lock.c
DEP_LOCK_=\
	\dev\memacs\h\estruct.h\
	\dev\memacs\h\eproto.h\
	\dev\memacs\h\edef.h\
	\dev\memacs\h\elang.h\
	\dev\memacs\h\estruct_orig.h\
	\dev\memacs\h\english.h\
	\dev\memacs\h\frenchis.h\
	\dev\memacs\h\french.h\
	\dev\memacs\h\spanish.h\
	\dev\memacs\h\german.h\
	\dev\memacs\h\dutch.h\
	\dev\memacs\h\platin.h\
	\dev\memacs\h\japan.h\
	\dev\memacs\h\latin.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\lock.obj :  $(SOURCE)  $(DEP_LOCK_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\lock.obj :  $(SOURCE)  $(DEP_LOCK_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\dev\memacs\src\line.c
DEP_LINE_=\
	\dev\memacs\h\estruct.h\
	\dev\memacs\h\eproto.h\
	\dev\memacs\h\edef.h\
	\dev\memacs\h\elang.h\
	\dev\memacs\h\estruct_orig.h\
	\dev\memacs\h\english.h\
	\dev\memacs\h\frenchis.h\
	\dev\memacs\h\french.h\
	\dev\memacs\h\spanish.h\
	\dev\memacs\h\german.h\
	\dev\memacs\h\dutch.h\
	\dev\memacs\h\platin.h\
	\dev\memacs\h\japan.h\
	\dev\memacs\h\latin.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\line.obj :  $(SOURCE)  $(DEP_LINE_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\line.obj :  $(SOURCE)  $(DEP_LINE_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\dev\memacs\src\file.c
DEP_FILE_=\
	\dev\memacs\h\estruct.h\
	\dev\memacs\h\eproto.h\
	\dev\memacs\h\edef.h\
	\dev\memacs\h\elang.h\
	\dev\memacs\h\estruct_orig.h\
	\dev\memacs\h\english.h\
	\dev\memacs\h\frenchis.h\
	\dev\memacs\h\french.h\
	\dev\memacs\h\spanish.h\
	\dev\memacs\h\german.h\
	\dev\memacs\h\dutch.h\
	\dev\memacs\h\platin.h\
	\dev\memacs\h\japan.h\
	\dev\memacs\h\latin.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\file.obj :  $(SOURCE)  $(DEP_FILE_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\file.obj :  $(SOURCE)  $(DEP_FILE_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\dev\memacs\src\dolock.c
DEP_DOLOC=\
	\dev\memacs\h\estruct.h\
	\dev\memacs\h\eproto.h\
	\dev\memacs\h\elang.h\
	\dev\memacs\h\estruct_orig.h\
	\dev\memacs\h\english.h\
	\dev\memacs\h\frenchis.h\
	\dev\memacs\h\french.h\
	\dev\memacs\h\spanish.h\
	\dev\memacs\h\german.h\
	\dev\memacs\h\dutch.h\
	\dev\memacs\h\platin.h\
	\dev\memacs\h\japan.h\
	\dev\memacs\h\latin.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\dolock.obj :  $(SOURCE)  $(DEP_DOLOC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\dolock.obj :  $(SOURCE)  $(DEP_DOLOC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\dev\memacs\src\window.c
DEP_WINDO=\
	\dev\memacs\h\estruct.h\
	\dev\memacs\h\eproto.h\
	\dev\memacs\h\edef.h\
	\dev\memacs\h\elang.h\
	\dev\memacs\h\estruct_orig.h\
	\dev\memacs\h\english.h\
	\dev\memacs\h\frenchis.h\
	\dev\memacs\h\french.h\
	\dev\memacs\h\spanish.h\
	\dev\memacs\h\german.h\
	\dev\memacs\h\dutch.h\
	\dev\memacs\h\platin.h\
	\dev\memacs\h\japan.h\
	\dev\memacs\h\latin.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\window.obj :  $(SOURCE)  $(DEP_WINDO) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\window.obj :  $(SOURCE)  $(DEP_WINDO) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
