/*	EVAR.H: Environment and user variable definitions
		for MicroEMACS

		written 1993 by Daniel Lawrence
*/

#define MAXVARS 	512 /* Maximum number of global user variables */

/*	list of recognized environment variables	*/

NOSHARE char *envars[] = {
	"abbell",		/* ring bell on abbreviation expansion? */
	"abcap",		/* match capitalization in expansions */
	"abquick",		/* quick, aggressive expansions enabled? */
	"acount",		/* # of chars until next auto-save */
	"asave",		/* # of chars between auto-saves */
	"bufhook",		/* enter buffer switch hook */
	"cbflags",		/* current buffer flags */
	"cbufname",	 	/* current buffer name */
	"cfname",		/* current file name */
	"class",		/* screen class code */
	"cmdhook",		/* command loop hook */
	"cmode",		/* mode of current buffer */
	"cquote",		/* close quote character */
	"curchar",		/* current character under the cursor */
	"curcol",		/* current column pos of cursor */
	"curline",		/* current line in file */
	"curoffs",		/* current offset in line */
	"curwidth", 		/* current screen width */
	"curwind",		/* current window ordinal on current screen */
	"cwline",		/* current screen line in window */
	"debug",		/* macro debugging */
	"deskcolor",		/* desktop color */
	"diagflag",	 	/* diagonal mouse movements enabled? */
	"discmd",		/* display commands on command line */
	"disinp",		/* display command line input characters */
	"disphigh", 		/* display high bit characters escaped */
	"dispundo",	 	/* display undo depth on command line */
	"exbhook",		/* exit buffer switch hook */
	"exithook", 		/* exiting emacs hook */
	"fcol", 		/* first displayed column in current window */
	"fillcol",		/* current fill column */
	"flicker",		/* flicker suppression */
	"fmtlead",		/* format command lead characters */
	"found",		/* last search status */
	"gflags",		/* global internal emacs flags */
	"gmode",		/* global modes */
	"hardtab",		/* current hard tab size */
	"hilight",		/* region # to hilight (255 to turn off) */
	"hjump",		/* horizontal screen jump size */
	"hscrlbar",	 	/* horizontal scroll bar flag */
	"hscroll",		/* horizontal scrolling flag */
	"isterm",		/* incremental-search terminator character */
	"kill", 		/* kill buffer (read only) */
	"language",		/* language of text messages */
	"lastkey",		/* last keyboard char struck */
	"lastmesg",		/* last string mlwrite()ed */
	"line", 		/* text of current line */
	"lterm",		/* current line terminator for writes */
	"lwidth",		/* width of current line */
	"margin",		/* minimum margin for extended lines */
	"match",		/* last matched magic pattern */
	"mcol", 		/* mark column */
	"mline",		/* mark line */
	"mmove",		/* mouse moves events style */
	"modeflag",		/* Mode lines displayed flag */
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
	"paralead",		/* paragraph leading characters */
	"pending",		/* type ahead pending flag */
	"pid",			/* process id */
	"popflag",		/* pop-up windows active? */
	"popwait",		/* user wait on end of pop-up window? */
	"posflag",		/* display point position on mode line? */
	"progname",		/* returns current prog name - "MicroEMACS" */
	"ram",			/* ram in use by malloc */
	"readhook",		/* read file execution hook */
	"region",		/* current region (read only) */
	"replace",		/* replacement pattern */
	"rval", 		/* child process return value */
	"scrname",		/* current screen name */
	"scrsiz",		/* size of scroll region  */
	"search",		/* search pattern */
	"searchpnt",		/* differing search styles (term point) */
	"seed", 		/* current random number seed */
	"softtab",		/* current soft tab size */
	"sres", 		/* current screen resolution */
	"ssave",		/* safe save flag */
	"sscroll",		/* smooth scrolling flag */
	"status",		/* returns the status of the last command */
	"sterm",		/* search terminator character */
	"syslang",		/* system language */
	"target",		/* target for line moves */
	"time", 		/* date and time */
	"timeflag",		/* display time? */
	"tmpnam",		/* temporary file name */
	"tpause",		/* length to pause for fences matching */
	"undoflag",		/* currently processing undoes */
	"version",		/* current version number */
	"vscrlbar",		/* vertical scroll bar flag */
	"wchars",		/* set of characters legal in words */
	"wline",		/* # of lines in current window */
	"wraphook",		/* wrap word execution hook */
	"writehook",		/* write file hook */
	"xpos", 		/* current mouse X position */
	"yankflag",		/* point placement at yanked/included text */
	"ypos", 		/* current mouse Y position */
};

#define NEVARS	sizeof(envars) / sizeof(char *)

/*	and its preprocessor definitions	*/

#define EVABBELL	0
#define EVABCAP 	1
#define EVABQUICK	2
#define EVACOUNT	3
#define EVASAVE 	4
#define EVBUFHOOK	5
#define EVCBFLAGS	6
#define EVCBUFNAME	7
#define EVCFNAME	8
#define EVCLASS 	9
#define EVCMDHK 	10
#define EVCMODE 	11
#define EVCQUOTE	12
#define EVCURCHAR	13
#define EVCURCOL	14
#define EVCURLINE	15
#define EVCUROFFS	16
#define EVCURWIDTH	17
#define EVCURWIND	18
#define EVCWLINE	19
#define EVDEBUG 	20
#define EVDESKCLR	21
#define EVDIAGFLAG	22
#define EVDISCMD	23
#define EVDISINP	24
#define EVDISPHIGH	25
#define EVDISPUNDO	26
#define EVEXBHOOK	27
#define EVEXITHOOK	28
#define EVFCOL		29
#define EVFILLCOL	30
#define EVFLICKER	31
#define EVFMTLEAD	32
#define EVFOUND 	33
#define EVGFLAGS	34
#define EVGMODE 	35
#define EVHARDTAB	36
#define EVHILITE	37
#define EVHJUMP 	38
#define EVHSCRLBAR	39
#define EVHSCROLL	40
#define EVISTERM	41
#define EVKILL		42
#define EVLANG		43
#define EVLASTKEY	44
#define EVLASTMESG	45
#define EVLINE		46
#define EVLTERM 	47
#define EVLWIDTH	48
#define EVMARGIN	49
#define EVMATCH 	50
#define EVMCOL		51
#define EVMLINE 	52
#define EVMMOVE 	53
#define EVMODEFLAG	54
#define EVMSFLAG	55
#define EVNEWSCRN	56
#define EVNUMWIND	57
#define EVOQUOTE	58
#define EVORGCOL	59
#define EVORGROW	60
#define EVOS		61
#define EVOVERLAP	62
#define EVPAGELEN	63
#define EVPALETTE	64
#define EVPARALEAD	65
#define EVPENDING	66
#define EVPID		67
#define EVPOPFLAG	68
#define EVPOPWAIT	69
#define EVPOSFLAG	70
#define EVPROGNAME	71
#define EVRAM		72
#define EVREADHK	73
#define EVREGION	74
#define EVREPLACE	75
#define EVRVAL		76
#define EVSCRNAME	77
#define EVSCRSIZ	78
#define EVSEARCH	79
#define EVSEARCHPNT	80
#define EVSEED		81
#define EVSOFTTAB	82
#define EVSRES		83
#define EVSSAVE 	84
#define EVSSCROLL	85
#define EVSTATUS	86
#define EVSTERM 	87
#define EVSYSLANG	88
#define EVTARGET	89
#define EVTIME		90
#define EVTIMEFLAG	91
#define EVTMPNAM	92
#define EVTPAUSE	93
#define EVUNDOFLAG	94
#define EVVERSION	95
#define EVVSCRLBAR	96
#define EVWCHARS	97
#define EVWLINE 	98
#define EVWRAPHK	99
#define EVWRITEHK	100
#define EVXPOS		101
#define EVYANKFLAG	102
#define EVYPOS		103

/*	list of recognized user functions	*/

typedef struct UFUNC {
	char *f_name;  /* name of function */
	int f_type; /* 1 = monamic, 2 = dynamic */
} UFUNC;

#define NILNAMIC	0
#define MONAMIC 	1
#define DYNAMIC 	2
#define TRINAMIC	3

NOSHARE UFUNC funcs[] = {
	{ "abbrev", MONAMIC },	/* look up abbreviation */
	{ "abs", MONAMIC }, 	/* absolute value of a number */
	{ "add", DYNAMIC },	/* add two numbers together */
	{ "and", DYNAMIC }, 	/* logical and */
	{ "ascii", MONAMIC },	/* char to integer conversion */
	{ "band", DYNAMIC },	/* bitwise and	 9-10-87  jwm */
	{ "bind", MONAMIC },	/* loopup what function name is bound to a key */
	{ "bnot", MONAMIC },	/* bitwise not */
	{ "bor", DYNAMIC }, 	/* bitwise or	 9-10-87  jwm */
	{ "bxor", DYNAMIC },	/* bitwise xor	 9-10-87  jwm */
	{ "call", MONAMIC },	/* call a procedure */
	{ "cat", DYNAMIC }, 	/* concatenate string */
	{ "chr", MONAMIC }, 	/* integer to char conversion */
	{ "divide", DYNAMIC },	/* division */
	{ "env", MONAMIC }, 	/* retrieve a system environment var */
	{ "equal", DYNAMIC },	/* logical equality check */
	{ "exist", MONAMIC },	/* check if a file exists */
	{ "find", MONAMIC },	/* look for a file on the path... */
	{ "greater", DYNAMIC }, /* logical greater than */
	{ "group", MONAMIC },	/* return group match in MAGIC mode */
	{ "gtc", NILNAMIC },	/* get 1 emacs command */
	{ "gtk", NILNAMIC },	/* get 1 character */
	{ "indirect", MONAMIC },/* evaluate indirect value */
	{ "isnum", MONAMIC },	/* is the arg a number? */
	{ "left", DYNAMIC },	/* left string(string, len) */
	{ "length", MONAMIC },	/* string length */
	{ "less", DYNAMIC },	/* logical less than */
	{ "lower", MONAMIC },	/* lower case string */
	{ "mid", TRINAMIC },	/* mid string(string, pos, len) */
	{ "mkcol", MONAMIC },	/* column position of a mark */
	{ "mkline", MONAMIC },	/* line number of a mark */
	{ "modulo", DYNAMIC },	/* mod */
	{ "negate", MONAMIC },	/* negate */
	{ "not", MONAMIC }, 	/* logical not */
	{ "or",  DYNAMIC }, 	/* logical or */
	{ "reverse", MONAMIC }, /* reverse */
	{ "right", DYNAMIC },	/* right string(string, pos) */
	{ "rnd", MONAMIC }, 	/* get a random number */
	{ "sequal", DYNAMIC },	/* string logical equality check */
	{ "sgreater", DYNAMIC },/* string logical greater than */
	{ "sindex", DYNAMIC },	/* find the index of one string in another */
	{ "sless", DYNAMIC },	/* string logical less than */
	{ "slower", DYNAMIC },	/* set lower to upper char translation */
	{ "srindex", DYNAMIC }, /* find the last index of one string in another */
	{ "subtract", DYNAMIC },/* subtraction */
	{ "supper", DYNAMIC },	/* set upper to lower char translation */
	{ "times", DYNAMIC },	/* multiplication */
	{ "trim", MONAMIC },	/* trim whitespace off the end of a string */
	{ "true", MONAMIC },	/* Truth of the universe logical test */
	{ "upper", MONAMIC },	/* uppercase string */
	{ "xlate", TRINAMIC },	/* XLATE character string translation */
};

#define NFUNCS	sizeof(funcs) / sizeof(UFUNC)

/*	and its preprocessor definitions	*/

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
#define UFGROUP 	19
#define UFGTCMD 	20
#define UFGTKEY 	21
#define UFIND		22
#define UFISNUM 	23
#define UFLEFT		24
#define UFLENGTH	25
#define UFLESS		26
#define UFLOWER 	27
#define UFMID		28
#define UFMKCOL 	29
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
#define UFSRINDEX	43
#define UFSUB		44
#define UFSUPPER	45
#define UFTIMES 	46
#define UFTRIM		47
#define UFTRUTH 	48
#define UFUPPER 	49
#define UFXLATE 	50
