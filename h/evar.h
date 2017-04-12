/*	EVAR.H: Environment and user variable definitions
		for MicroEMACS

		written 1993 by Daniel Lawrence
*/

#define MAXVARS 	512	/* Maximum number of global user variables */

/*	list of recognized environment variables	*/

NOSHARE char *envars[] = {
	"abbell",		/* ring bell on abbreviation expansion? */
	"abcap",		/* match capitolization in expansions */
	"abquick",		/* quick, aggressive expansions enabled? */
	"acount",		/* # of chars until next auto-save */
	"asave",		/* # of chars between auto-saves */
	"bufhook",		/* enter buffer switch hook */
	"cbflags",		/* current buffer flags */
	"cbufname",		/* current buffer name */
	"cfname",		/* current file name */
	"cmdhook",		/* command loop hook */
	"cmode",		/* mode of current buffer */
	"cquote",		/* close quote character */
	"curchar",		/* current character under the cursor */
	"curcol",		/* current column pos of cursor */
	"curline",		/* current line in file */
	"curwidth",		/* current screen width */
	"curwind",		/* current window ordinal on current screen */
	"cwline",		/* current screen line in window */
	"debug",		/* macro debugging */
	"deskcolor",		/* desktop color */
	"diagflag",		/* diagonal mouse movements enabled? */
	"discmd",		/* display commands on command line */
	"disinp",		/* display command line input characters */
	"disphigh",		/* display high bit characters escaped */
	"dispundo",		/* display undo depth on command line */
	"exbhook",		/* exit buffer switch hook */
	"exithook",		/* exiting emacs hook */
	"fcol",			/* first displayed column in curent window */
	"fillcol",		/* current fill column */
	"flicker",		/* flicker supression */
	"fmtlead",		/* format command lead characters */
	"gflags",		/* global internal emacs flags */
	"gmode",		/* global modes */
	"hardtab",		/* current hard tab size */
	"hilight",		/* region # to hilight (255 to turn off) */
	"hjump",		/* horizontal screen jump size */
	"hscrlbar",		/* horizontal scroll bar flag */
	"hscroll",		/* horizontal scrolling flag */
	"isterm",		/* incremental-search terminator character */
	"kill", 		/* kill buffer (read only) */
	"language",		/* language of text messages */
	"lastkey",		/* last keyboard char struck */
	"lastmesg",		/* last string mlwrite()ed */
	"line", 		/* text of current line */
	"lterm",		/* current line terminator for writes */
	"lwidth",		/* width of current line */
	"match",		/* last matched magic pattern */
	"mmove",		/* mouse moves events style */
	"modeflag",		/* Modelines displayed flag */
	"msflag",		/* activate mouse? */
	"newscreen",		/* new screen with new buffer? */
	"numwind",		/* number of windows on current screen */
	"oquote",		/* open quote character */
	"orgcol",		/* screen origin column */
	"orgrow",		/* screen origin row */
	"os",			/* what Operating System? */
	"overlap",		/* line overlap when paging */
	"pagelen",		/* number of lines used by editor */
	"palette",		/* current palette string */
	"paralead",		/* paragraph leadin characters */
	"pending",		/* type ahead pending flag */
	"popflag",		/* pop-up windows active? */
	"popwait",		/* user wait on end of pop-up window? */
	"posflag",		/* display point position on modeline? */
	"progname",		/* returns current prog name - "MicroEMACS" */
	"ram",			/* ram in use by malloc */
	"readhook",		/* read file execution hook */
	"region",		/* current region (read only) */
	"replace",		/* replacement pattern */
	"rval", 		/* child process return value */
	"scrname",		/* current screen name */
	"search",		/* search pattern */
	"searchpnt",		/* differing search styles (term point) */
	"seed", 		/* current random number seed */
	"softtab",		/* current soft tab size */
	"sres", 		/* current screen resolution */
	"ssave",		/* safe save flag */
	"sscroll",		/* smooth scrolling flag */
	"status",		/* returns the status of the last command */
	"sterm",		/* search terminator character */
	"target",		/* target for line moves */
	"time",			/* date and time */
	"timeflag",		/* display time? */
	"tpause",		/* length to pause for paren matching */
	"undoflag",		/* currently processing undos */
	"version",		/* current version number */
	"vscrlbar",		/* vertical scroll bar flag */
	"wchars",		/* set of characters legal in words */
	"wline",		/* # of lines in current window */
	"wraphook",		/* wrap word execution hook */
	"writehook",		/* write file hook */
	"xpos", 		/* current mouse X position */
	"yankflag",		/* point placement at yanked/included text */
	"ypos"	 		/* current mouse Y position */
};

#define NEVARS	sizeof(envars) / sizeof(char *)

/*	and its preprocesor definitions 	*/

#define EVABBELL	0
#define EVABCAP		1
#define EVABQUICK	2
#define EVACOUNT	3
#define EVASAVE 	4
#define EVBUFHOOK	5
#define EVCBFLAGS	6
#define EVCBUFNAME	7
#define EVCFNAME	8
#define EVCMDHK 	9
#define EVCMODE 	10
#define	EVCQUOTE	11
#define EVCURCHAR	12
#define EVCURCOL	13
#define EVCURLINE	14
#define EVCURWIDTH	15
#define EVCURWIND	16
#define EVCWLINE	17
#define EVDEBUG         18
#define EVDESKCLR	19
#define EVDIAGFLAG      20
#define EVDISCMD        21
#define EVDISINP        22
#define EVDISPHIGH	23
#define EVDISPUNDO	24
#define EVEXBHOOK       25
#define EVEXITHOOK	26
#define EVFCOL		27
#define EVFILLCOL	28
#define EVFLICKER	29
#define EVFMTLEAD	30
#define EVGFLAGS	31
#define EVGMODE 	32
#define EVHARDTAB	33
#define EVHILITE	34
#define EVHJUMP		35
#define EVHSCRLBAR      36
#define EVHSCROLL	37
#define EVISTERM	38
#define EVKILL          39
#define EVLANG          40
#define EVLASTKEY       41
#define EVLASTMESG      42
#define EVLINE          43
#define EVLTERM		44
#define EVLWIDTH        45
#define EVMATCH         46
#define EVMMOVE		47
#define EVMODEFLAG      48
#define EVMSFLAG        49
#define EVNEWSCRN	50
#define EVNUMWIND	51
#define	EVOQUOTE	52
#define EVORGCOL	53
#define EVORGROW	54
#define EVOS		55
#define EVOVERLAP	56
#define EVPAGELEN       57
#define EVPALETTE       58
#define EVPARALEAD	59
#define EVPENDING       60
#define EVPOPFLAG	61
#define EVPOPWAIT	62
#define EVPOSFLAG	63
#define EVPROGNAME      64
#define EVRAM           65
#define EVREADHK        66
#define EVREGION	67
#define EVREPLACE       68
#define EVRVAL          69
#define EVSCRNAME	70
#define EVSEARCH        71
#define EVSEARCHPNT	72
#define EVSEED          73
#define EVSOFTTAB	74
#define EVSRES          75
#define EVSSAVE         76
#define EVSSCROLL	77
#define EVSTATUS	78
#define EVSTERM 	79
#define EVTARGET	80
#define EVTIME		81
#define EVTIMEFLAG	82
#define EVTPAUSE	83
#define EVUNDOFLAG	84
#define EVVERSION	85
#define EVVSCRLBAR      86
#define EVWCHARS	87
#define EVWLINE 	88
#define EVWRAPHK	89
#define EVWRITEHK	90
#define EVXPOS		91
#define EVYANKFLAG	92
#define EVYPOS		93

/*	list of recognized user functions	*/

typedef struct UFUNC {
	char *f_name;  /* name of function */
	int f_type;	/* 1 = monamic, 2 = dynamic */
} UFUNC;

#define NILNAMIC	0
#define MONAMIC 	1
#define DYNAMIC 	2
#define TRINAMIC	3

NOSHARE UFUNC funcs[] = {
	"abbrev", MONAMIC,	/* look up abbreviation */
	"abs", MONAMIC, 	/* absolute value of a number */
	"add", DYNAMIC,		/* add two numbers together */
	"and", DYNAMIC, 	/* logical and */
	"ascii", MONAMIC, 	/* char to integer conversion */
	"band", DYNAMIC, 	/* bitwise and	 9-10-87  jwm */
	"bind", MONAMIC, 	/* loopup what function name is bound to a key */
	"bnot", MONAMIC, 	/* bitwise not */
	"bor", DYNAMIC, 	/* bitwise or	 9-10-87  jwm */
	"bxor", DYNAMIC, 	/* bitwise xor	 9-10-87  jwm */
	"call", MONAMIC,	/* call a procedure */
	"cat", DYNAMIC, 	/* concatenate string */
	"chr", MONAMIC, 	/* integer to char conversion */
	"divide", DYNAMIC, 	/* division */
	"env", MONAMIC, 	/* retrieve a system environment var */
	"equal", DYNAMIC, 	/* logical equality check */
	"exist", MONAMIC, 	/* check if a file exists */
	"find", MONAMIC, 	/* look for a file on the path... */
	"greater", DYNAMIC, 	/* logical greater than */
	"group", MONAMIC,	/* return group match in MAGIC mode */
	"gtc", NILNAMIC,	/* get 1 emacs command */
	"gtk", NILNAMIC,	/* get 1 charater */
	"indirect", MONAMIC, 	/* evaluate indirect value */
	"isnum", MONAMIC,	/* is the arg a number? */
	"left", DYNAMIC, 	/* left string(string, len) */
	"length", MONAMIC, 	/* string length */
	"less", DYNAMIC, 	/* logical less than */
	"lower", MONAMIC, 	/* lower case string */
	"mid", TRINAMIC,	/* mid string(string, pos, len) */
	"mkcol", MONAMIC,	/* column position of a mark */
	"mkline", MONAMIC,	/* line number of a mark */
	"modulo", DYNAMIC, 	/* mod */
	"negate", MONAMIC, 	/* negate */
	"not", MONAMIC, 	/* logical not */
	"or",  DYNAMIC, 	/* logical or */
	"reverse", MONAMIC,	/* reverse */
	"right", DYNAMIC, 	/* right string(string, pos) */
	"rnd", MONAMIC, 	/* get a random number */
	"sequal", DYNAMIC, 	/* string logical equality check */
	"sgreater", DYNAMIC, 	/* string logical greater than */
	"sindex", DYNAMIC, 	/* find the index of one string in another */
	"sless", DYNAMIC, 	/* string logical less than */
	"slower", DYNAMIC,	/* set lower to upper char translation */
	"subtract", DYNAMIC, 	/* subtraction */
	"supper", DYNAMIC,	/* set upper to lower char translation */
	"times", DYNAMIC, 	/* multiplication */
	"trim", MONAMIC,	/* trim whitespace off the end of a string */
	"true", MONAMIC, 	/* Truth of the universe logical test */
	"upper", MONAMIC, 	/* uppercase string */
	"xlate", TRINAMIC	/* XLATE character string translation */
};

#define NFUNCS	sizeof(funcs) / sizeof(UFUNC)

/*	and its preprocesor definitions 	*/

#define UFABBREV	0
#define UFABS		1
#define UFADD		2
#define UFAND		3
#define UFASCII 	4
#define UFBAND		5
#define UFBIND		6
#define UFBNOT		7
#define UFBOR		8
#define UFBXOR		9
#define UFCALL		10
#define UFCAT		11
#define UFCHR		12
#define UFDIV		13
#define UFENV		14
#define UFEQUAL 	15
#define UFEXIST 	16
#define UFFIND		17
#define UFGREATER	18
#define UFGROUP		19
#define UFGTCMD 	20
#define UFGTKEY 	21
#define UFIND		22
#define UFISNUM		23
#define UFLEFT		24
#define UFLENGTH	25
#define UFLESS		26
#define UFLOWER 	27
#define UFMID		28
#define UFMKCOL		29
#define UFMKLINE	30
#define UFMOD		31
#define UFNEG		32
#define UFNOT		33
#define UFOR		34
#define UFREVERSE	35
#define UFRIGHT 	36
#define UFRND		37
#define UFSEQUAL	38
#define UFSGREAT	39
#define UFSINDEX	40
#define UFSLESS 	41
#define UFSLOWER	42
#define UFSUB		43
#define UFSUPPER	44
#define UFTIMES 	45
#define UFTRIM		46
#define UFTRUTH 	47
#define UFUPPER 	48
#define UFXLATE 	49
