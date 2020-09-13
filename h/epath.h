/*	EPATH:	This file contains certain info needed to locate the
		MicroEMACS files on a system dependent basis.

									*/

/*	possible names and paths of help files under different OSs	*/

NOSHARE CONST char *pathname[] =

#if	AMIGA
{
	".emacsrc",
	"emacs.hlp",
	"",
	"c:",
	":t/",
	":s/"
};
#if	JMDEXT
NOSHARE CONST char *otherpath[] =
{
	"emacs.msg",
	"error.cmd",
};
#endif
#endif

#if	TOS
{
	"emacs.rc",
	"emacs.hlp",
	"\\",
	"\\bin\\",
	"\\util\\",
	""
};
#if	JMDEXT
NOSHARE CONST char *otherpath[] =
{
	"emacs.msg",
	"error.cmd",
};
#endif
#endif

#if	FINDER
{
	"emacs.rc",
	"emacs.hlp",
	"/bin",
	"/sys/public",
	""
};
#if	JMDEXT
NOSHARE CONST char *otherpath[] =
{
	"emacs.msg",
	"error.cmd",
};
#endif
#endif

#if	MSDOS || WINNT || WINXP
{
#if WINDOW_MSWIN || WINDOW_MSWIN32
	"mewin.cmd",
#else
	"emacs.rc",
#endif
	"emacs.hlp",
	"\\sys\\public\\",
	"\\usr\\bin\\",
	"\\bin\\",
	"\\",
	""
};
#if	JMDEXT
NOSHARE CONST char *otherpath[] =
{
	"emacs.msg",
	"error.cmd",
};
#endif
#endif

#if OS2
{
        "emacs.rc",
        "emacs.hlp",
        "C:\\OS2\\SYSTEM\\",
        "C:\\OS2\\DLL\\",
        "C:\\OS2\\BIN\\",
        "C:\\OS2\\",
        "\\",
        ""
};
#if	JMDEXT
NOSHARE CONST char *otherpath[] =
{
	"emacs.msg",
	"error.cmd",
};
#endif
#endif

#if	V7 | BSD | FREEBSD | USG | AIX | SMOS | HPUX8 | HPUX9 | XENIX | SUN | AVIION
{
	".emacsrc",
	"emacs.hlp",
#if	JMDEXT
	"/usr/local/share/uemacs/",
	"/opt/lib/uemacs/",
	"/usr/lib/uemacs/",
#endif
	"/usr/local/",
	"/usr/lib/",
	""
};
#if	JMDEXT
NOSHARE CONST char *otherpath[] =
{
	"emacs.msg",
	"error.cmd",
};
#endif
#endif

#if	VMS
{
	"emacs.rc",
	"emacs.hlp",
	"MICROEMACS$LIB:",
	"SYS$LOGIN:",
	""
};
#if	JMDEXT
NOSHARE CONST char *otherpath[] =
{
	"emacs.msg",
	"error.cmd",
};
#endif
#endif

#if WMCS
{
	"emacs.rc",
	"emacs.hlp",
	"",
	"sys$disk/syslib.users/"
};
#if	JMDEXT
NOSHARE CONST char *otherpath[] =
{
	"emacs.msg",
	"error.cmd",
};
#endif
#endif

#if AOSVS
/*
    NOTE: you must use the Unix style pathnames here!
*/
{
    "emacs.rc",
    "emacs.hlp",
    "",
    "/macros/",
    "/help/"
};
#if	JMDEXT
NOSHARE CONST char *otherpath[] =
{
	"emacs.msg",
	"error.cmd",
};
#endif
#endif

#if MPE
{
	"emacsrc",
	"emacshlp",
	".pub",
	".pub.sys",
	""
};
#if	JMDEXT
NOSHARE CONST char *otherpath[] =
{
	"emacsmsg",
	"errorcmd",
};
#endif
#endif /* MPE */

#if	THEOS
{
	"emacs.rc",
	"system.help386 ._me",
	"/usr/local/lib/emacs/",
	""
};
#if	JMDEXT
NOSHARE CONST char *otherpath[] =
{
	"emacs.msg",
	"error.cmd",
};
#endif
#endif

#define	NPNAMES	(sizeof(pathname)/sizeof(char *))
