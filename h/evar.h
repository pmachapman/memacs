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
#define EVCURCHAR	11
#define EVCURCOL	12
#define EVCURLINE	13
#define EVCURWIDTH	14
#define EVCURWIND	15
#define EVCWLINE	16
#define EVDEBUG         17
#define EVDESKCLR	18
#define EVDIAGFLAG      19
#define EVDISCMD        20
#define EVDISINP        21
#define EVDISPHIGH	22
#define EVDISPUNDO	23
#define EVEXBHOOK       24
#define EVEXITHOOK	25
#define EVFCOL		26
#define EVFILLCOL	27
#define EVFLICKER	28
#define EVFMTLEAD	29
#define EVGFLAGS	30
#define EVGMODE 	31
#define EVHARDTAB	32
#define EVHILITE	33
#define EVHJUMP		34
#define EVHSCRLBAR      35
#define EVHSCROLL	36
#define EVISTERM	37
#define EVKILL          38
#define EVLANG          39
#define EVLASTKEY       40
#define EVLASTMESG      41
#define EVLINE          42
#define EVLTERM		43
#define EVLWIDTH        44
#define EVMATCH         45
#define EVMMOVE		46
#define EVMODEFLAG      47
#define EVMSFLAG        48
#define EVNEWSCRN	49
#define EVNUMWIND	50
#define EVORGCOL	51
#define EVORGROW	52
#define EVOS		53
#define EVOVERLAP	54
#define EVPAGELEN       55
#define EVPALETTE       56
#define EVPARALEAD	57
#define EVPENDING       58
#define EVPOPFLAG	59
#define EVPOPWAIT	60
#define EVPOSFLAG	61
#define EVPROGNAME      62
#define EVRAM           63
#define EVREADHK        64
#define EVREGION	65
#define EVREPLACE       66
#define EVRVAL          67
#define EVSCRNAME	68
#define EVSEARCH        69
#define EVSEARCHPNT	70
#define EVSEED          71
#define EVSOFTTAB	72
#define EVSRES          73
#define EVSSAVE         74
#define EVSSCROLL	75
#define EVSTATUS	76
#define EVSTERM 	77
#define EVTARGET	78
#define EVTIME		79
#define EVTIMEFLAG	80
#define EVTPAUSE	81
#define EVUNDOFLAG	82
#define EVVERSION	83
#define EVVSCRLBAR      84
#define EVWCHARS	85
#define EVWLINE 	86
#define EVWRAPHK	87
#define EVWRITEHK	88
#define EVXPOS		89
#define EVYANKFLAG	90
#define EVYPOS		91

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
