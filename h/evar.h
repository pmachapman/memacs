/*	EVAR.H: Environment and user variable definitions
		for MicroEMACS

		written 1993 by Daniel Lawrence
*/

/*	structure to hold user variables and their definitions	*/

typedef struct UVAR {
	char u_name[NVSIZE + 1];	       /* name of user variable */
	char *u_value;				/* value (string) */
} UVAR;

/*	current user variables (This structure will probably change)	*/

#define MAXVARS 	512

UVAR NOSHARE uv[MAXVARS + 1];	/* user variables */

/*	list of recognized environment variables	*/

NOSHARE char *envars[] = {
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
	"exbhook",		/* exit buffer switch hook */
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

#define EVACOUNT	0
#define EVASAVE 	1
#define	EVBUFHOOK	2
#define EVCBFLAGS	3
#define EVCBUFNAME	4
#define EVCFNAME	5
#define EVCMDHK 	6
#define EVCMODE 	7
#define EVCURCHAR	8
#define EVCURCOL	9
#define EVCURLINE	10
#define EVCURWIDTH	11
#define	EVCURWIND	12
#define EVCWLINE	13
#define EVDEBUG         14
#define	EVDESKCLR	15
#define EVDIAGFLAG      16
#define EVDISCMD        17
#define EVDISINP        18
#define	EVDISPHIGH	19
#define EVEXBHOOK       20
#define EVFCOL		21
#define EVFILLCOL	22
#define EVFLICKER	23
#define	EVFMTLEAD	24
#define EVGFLAGS	25
#define EVGMODE 	26
#define	EVHARDTAB	27
#define	EVHILITE	28
#define EVHJUMP		29
#define EVHSCRLBAR      30
#define EVHSCROLL	31
#define EVISTERM	32
#define EVKILL          33
#define EVLANG          34
#define EVLASTKEY       35
#define EVLASTMESG      36
#define EVLINE          37
#define	EVLTERM		38
#define EVLWIDTH        39
#define EVMATCH         40
#define EVMMOVE		41
#define EVMODEFLAG      42
#define EVMSFLAG        43
#define	EVNUMWIND	44
#define	EVORGCOL	45
#define	EVORGROW	46
#define	EVOS		47
#define	EVOVERLAP	48
#define EVPAGELEN       49
#define EVPALETTE       50
#define	EVPARALEAD	51
#define EVPENDING       52
#define	EVPOPFLAG	53
#define	EVPOSFLAG	54
#define EVPROGNAME      55
#define EVRAM           56
#define EVREADHK        57
#define	EVREGION	58
#define EVREPLACE       59
#define EVRVAL          60
#define EVSCRNAME	61
#define EVSEARCH        62
#define EVSEARCHPNT	63
#define EVSEED          64
#define EVSOFTTAB	65
#define EVSRES          66
#define EVSSAVE         67
#define EVSSCROLL	68
#define EVSTATUS	69
#define EVSTERM 	70
#define EVTARGET	71
#define EVTIME		72
#define EVTIMEFLAG	73
#define EVTPAUSE	74
#define EVVERSION	75
#define EVVSCRLBAR      76
#define	EVWCHARS	77
#define EVWLINE 	78
#define EVWRAPHK	79
#define	EVWRITEHK	80
#define EVXPOS		81
#define	EVYANKFLAG	82
#define EVYPOS		83

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
	"abs", MONAMIC, 	/* absolute value of a number */
	"add", DYNAMIC,		/* add two numbers together */
	"and", DYNAMIC, 	/* logical and */
	"asc", MONAMIC, 	/* char to integer conversion */
	"ban", DYNAMIC, 	/* bitwise and	 9-10-87  jwm */
	"bin", MONAMIC, 	/* loopup what function name is bound to a key */
	"bno", MONAMIC, 	/* bitwise not */
	"bor", DYNAMIC, 	/* bitwise or	 9-10-87  jwm */
	"bxo", DYNAMIC, 	/* bitwise xor	 9-10-87  jwm */
	"cat", DYNAMIC, 	/* concatinate string */
	"chr", MONAMIC, 	/* integer to char conversion */
	"div", DYNAMIC, 	/* division */
	"env", MONAMIC, 	/* retrieve a system environment var */
	"equ", DYNAMIC, 	/* logical equality check */
	"exi", MONAMIC, 	/* check if a file exists */
	"fin", MONAMIC, 	/* look for a file on the path... */
	"gre", DYNAMIC, 	/* logical greater than */
	"gro", MONAMIC,		/* return group match in MAGIC mode */
	"gtc", NILNAMIC,	/* get 1 emacs command */
	"gtk", NILNAMIC,	/* get 1 charater */
	"ind", MONAMIC, 	/* evaluate indirect value */
	"isn", MONAMIC,		/* is the arg a number? */
	"lef", DYNAMIC, 	/* left string(string, len) */
	"len", MONAMIC, 	/* string length */
	"les", DYNAMIC, 	/* logical less than */
	"low", MONAMIC, 	/* lower case string */
	"mid", TRINAMIC,	/* mid string(string, pos, len) */
	"mod", DYNAMIC, 	/* mod */
	"neg", MONAMIC, 	/* negate */
	"not", MONAMIC, 	/* logical not */
	"or",  DYNAMIC, 	/* logical or */
	"rev", MONAMIC,		/* reverse */
	"rig", DYNAMIC, 	/* right string(string, pos) */
	"rnd", MONAMIC, 	/* get a random number */
	"seq", DYNAMIC, 	/* string logical equality check */
	"sgr", DYNAMIC, 	/* string logical greater than */
	"sin", DYNAMIC, 	/* find the index of one string in another */
	"sle", DYNAMIC, 	/* string logical less than */
	"slo", DYNAMIC,		/* set lower to upper char translation */
	"sub", DYNAMIC, 	/* subtraction */
	"sup", DYNAMIC,		/* set upper to lower char translation */
	"tim", DYNAMIC, 	/* multiplication */
	"tri", MONAMIC,		/* trim whitespace off the end of a string */
	"tru", MONAMIC, 	/* Truth of the universe logical test */
	"upp", MONAMIC, 	/* uppercase string */
	"xla", TRINAMIC		/* XLATE character string translation */
};

#define NFUNCS	sizeof(funcs) / sizeof(UFUNC)

/*	and its preprocesor definitions 	*/

#define UFABS		0
#define UFADD		1
#define UFAND		2
#define UFASCII 	3
#define UFBAND		4
#define UFBIND		5
#define UFBNOT		6
#define UFBOR		7
#define UFBXOR		8
#define UFCAT		9
#define UFCHR		10
#define UFDIV		11
#define UFENV		12
#define UFEQUAL 	13
#define UFEXIST 	14
#define UFFIND		15
#define UFGREATER	16
#define UFGROUP		17
#define UFGTCMD 	18
#define UFGTKEY 	19
#define UFIND		20
#define	UFISNUM		21
#define UFLEFT		22
#define UFLENGTH	23
#define UFLESS		24
#define UFLOWER 	25
#define UFMID		26
#define UFMOD		27
#define UFNEG		28
#define UFNOT		29
#define UFOR		30
#define UFREVERSE	31
#define UFRIGHT 	32
#define UFRND		33
#define UFSEQUAL	34
#define UFSGREAT	35
#define UFSINDEX	36
#define UFSLESS 	37
#define	UFSLOWER	38
#define UFSUB		39
#define	UFSUPPER	40
#define UFTIMES 	41
#define	UFTRIM		42
#define UFTRUTH 	43
#define UFUPPER 	44
#define UFXLATE 	45
