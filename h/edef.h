/*	EDEF:		Global variable definitions for
			MicroEMACS 3.9

                        written by Daniel Lawrence
                        based on code by Dave G. Conroy,
                        	Steve Wilhite and George Jones

                        Unicode support by Jean-Michel Dubois
*/

#ifdef	maindef

/* for MAIN.C */

/* initialized global definitions */

#if	CURSES

WINDOW* wmenubar;
WINDOW* wmain;
PANEL* pmenu;
PANEL* pmain;
WINDOW* wdrop = NULL;
PANEL* pdrop;
#endif

NOSHARE int DNEAR fillcol = 72; 	/* Current fill column		*/
#if 	UTF8
NOSHARE unsigned int kbdm[NKBDM];	/* Macro			*/
#else
NOSHARE short kbdm[NKBDM];		/* Macro			*/
#endif
NOSHARE char *execstr = NULL;		/* pointer to string to execute */
NOSHARE char golabel[NPAT] = "";	/* current line to go to	*/
NOSHARE char paralead[NPAT] = " \t";	/* paragraph leadin chars	*/
NOSHARE char fmtlead[NPAT] = "";	/* format command leadin chars	*/
NOSHARE char mainbuf[] = "main";	/* name of main buffer		*/
NOSHARE char lterm[NSTRING];		/* line terminators on file write */
NOSHARE unsigned char wordlist[256];	/* characters considered "in words" */
NOSHARE int DNEAR wlflag = FALSE;	/* word list enabled flag	*/
NOSHARE int DNEAR clearflag = TRUE;	/* clear screen on screen change? */
NOSHARE int DNEAR execlevel = 0;	/* execution IF level		*/
NOSHARE int DNEAR eolexist = TRUE;	/* does clear to EOL exist?	*/
NOSHARE int DNEAR revexist = FALSE;	/* does reverse video exist?	*/
NOSHARE int DNEAR exec_error = FALSE;	/* macro execution error pending? */
NOSHARE int DNEAR flickcode = TRUE;	/* do flicker supression?	*/
NOSHARE int DNEAR mouse_move = 1;	/* user allow tracking mouse moves? */
NOSHARE int DNEAR mmove_flag = TRUE;	/* code currently allowing mmoves? */
NOSHARE int DNEAR newscreenflag = FALSE;/* Create new screen on reads? */
NOSHARE int DNEAR overlap = 2;		/* overlap when paging screens */
CONST char *modename[NUMMODES] = { 	/* name of modes		*/
	"WRAP", "CMODE", "SPELL", "EXACT", "VIEW", "OVER",
	"MAGIC", "CRYPT", "ASAVE", "REP", "ABBREV"
#if	JMDEXT
	, "NOBAK", "SLINE", "DLINE"
#if	THEOX
	, "THEOX"			/* support of TheoX character set */
#endif
#endif
	};
#if	JMDEXT
CONST char modecode[] = "WCSEVOMYARBKsl"/* letters to represent modes	*/
#if	THEOX
	"X"
#endif
	;
#else
CONST char modecode[] = "WCSEVOMYARB";	/* letters to represent modes	*/
#endif
NOSHARE int DNEAR numfunc = NFUNCS;	/* number of bindable functions */
#if	MDTHEOX
NOSHARE int DNEAR gmode = 0;		/* global editor mode		*/
#else
NOSHARE int DNEAR gmode = 0;		/* global editor mode		*/
#endif
NOSHARE int DNEAR gflags = GFREAD;	/* global control flag		*/
NOSHARE int DNEAR gfcolor = 7;		/* global forgrnd color (white) */
NOSHARE int DNEAR gbcolor = 0;		/* global backgrnd color (black)*/
NOSHARE int DNEAR deskcolor = 0;	/* desktop background color	*/
NOSHARE int DNEAR gasave = 256; 	/* global ASAVE size		*/
NOSHARE int DNEAR gacount = 256;	/* count until next ASAVE	*/
NOSHARE int DNEAR sgarbf = TRUE; 	/* TRUE if screen is garbage	*/
NOSHARE int DNEAR mpresf = FALSE;	/* TRUE if message in last line */
NOSHARE int DNEAR clexec = FALSE;	/* command line execution flag	*/
NOSHARE int DNEAR mstore = FALSE;	/* storing text to macro flag	*/
NOSHARE int DNEAR discmd = TRUE; 	/* display command flag 	*/
NOSHARE int DNEAR disinp = TRUE; 	/* display input characters	*/
NOSHARE int DNEAR modeflag = TRUE;	/* display modelines flag	*/
NOSHARE int DNEAR timeflag = FALSE;	/* display time			*/
NOSHARE int DNEAR undoflag = TRUE;	/* processing undo commands	*/
NOSHARE OBJECT obj;			/* scratch undo object		*/
NOSHARE int DNEAR undoing = FALSE;	/* currently undoing a command? */
NOSHARE char DNEAR lasttime[6] = "";	/* last time string displayed	*/
NOSHARE int DNEAR popflag = TRUE;	/* pop-up windows enabled?	*/
NOSHARE int DNEAR popwait = TRUE;	/* user wait on pops enabled?	*/
NOSHARE int DNEAR posflag = FALSE;	/* display point position	*/
NOSHARE int cpending = FALSE;		/* input character pending?	*/
NOSHARE int charpending;		/* character pushed back	*/
NOSHARE int DNEAR sscroll = FALSE;	/* smooth scrolling enabled flag*/
NOSHARE int DNEAR hscroll = TRUE;	/* horizontal scrolling flag	*/
NOSHARE int DNEAR hscrollbar = TRUE;	/* horizontal scroll bar flag	*/
NOSHARE int DNEAR vscrollbar = TRUE;	/* vertical scroll bar flag */
NOSHARE int DNEAR hjump = 1;		/* horizontal jump size 	*/
NOSHARE int DNEAR ssave = TRUE; 	/* safe save flag		*/
NOSHARE struct BUFFER *bstore = NULL;	/* buffer to store macro text to*/
NOSHARE int DNEAR vtrow = 0;		/* Row location of SW cursor	*/
NOSHARE int DNEAR vtcol = 0;		/* Column location of SW cursor */
NOSHARE int DNEAR ttrow = HUGENUM; 	/* Row location of HW cursor	*/
NOSHARE int DNEAR ttcol = HUGENUM; 	/* Column location of HW cursor */
NOSHARE int DNEAR lbound = 0;		/* leftmost column of current line
					   being displayed		*/
NOSHARE int DNEAR taboff = 0;		/* tab offset for display	*/
NOSHARE int DNEAR tabsize = 8;		/* current hard tab size	*/
NOSHARE int DNEAR stabsize = 0;		/* current soft tab size (0: use hard tabs)  */
NOSHARE int DNEAR reptc = CTRL | 'U';	/* current universal repeat char*/
NOSHARE int DNEAR abortc = CTRL | 'G';	/* current abort command char	*/
NOSHARE int DNEAR sterm = CTRL | '[';	/* search terminating character */
NOSHARE int DNEAR isterm = CTRL | '[';	/* incremental-search terminating char */
NOSHARE int DNEAR searchtype = SRNORM;	/* current search style		*/
NOSHARE int DNEAR yankflag = FALSE;	/* current yank style		*/

NOSHARE int DNEAR prefix = 0;		/* currently pending prefix bits */
NOSHARE int DNEAR prenum = 0;		/*     "       "     numeric arg */
NOSHARE int DNEAR predef = TRUE;	/*     "       "     default flag */

NOSHARE int DNEAR quotec = CTRL | 'Q';	/* quote char during mlreply() */
NOSHARE CONST char *cname[] = {		/* names of colors		*/
	"BLACK", "RED", "GREEN", "YELLOW", "BLUE",
	"MAGENTA", "CYAN", "GREY",
	"GRAY", "LRED", "LGREEN", "LYELLOW", "LBLUE",
	"LMAGENTA", "LCYAN", "WHITE"};

NOSHARE int kill_index;			/* current index into kill ring */
NOSHARE KILL *kbufp[NRING];		/* current kill buffer chunk pointer*/
NOSHARE KILL *kbufh[NRING];		/* kill buffer header pointer	*/
NOSHARE	int kskip[NRING];		/* # of bytes to skip in 1st kill chunk */
NOSHARE int kused[NRING];		/* # of bytes used in last kill chunk*/
NOSHARE EWINDOW *swindow = NULL; 	/* saved window pointer 	*/
NOSHARE int cryptflag = FALSE;		/* currently encrypting?	*/
NOSHARE int oldcrypt = FALSE;		/* using old(broken) encryption? */
#if	UTF8
NOSHARE unsigned int *kbdptr;			/* current position in keyboard buf */
NOSHARE unsigned int *kbdend = &kbdm[0];/* ptr to end of the keyboard */
#else
NOSHARE short *kbdptr;			/* current position in keyboard buf */
#if	THEOSC
NOSHARE short *kbdend = kbdm;		/* ptr to end of the keyboard */
#else
NOSHARE short *kbdend = &kbdm[0];	/* ptr to end of the keyboard */
#endif
#endif
NOSHARE int DNEAR kbdmode = STOP;	/* current keyboard macro mode	*/
NOSHARE int DNEAR kbdrep = 0;		/* number of repetitions	*/
NOSHARE int DNEAR restflag = FALSE;	/* restricted use?		*/
NOSHARE int DNEAR lastkey = 0;		/* last keystoke		*/
NOSHARE long DNEAR seed = 1L;		/* random number seed		*/
NOSHARE long envram = 0l;		/* # of bytes current in use by malloc */
NOSHARE long access_time = 0L;		/* counter of buffer access	*/
NOSHARE int DNEAR macbug = FALSE;	/* macro debugging flag		*/
NOSHARE int DNEAR mouseflag = TRUE;	/* use the mouse?		*/
NOSHARE int DNEAR diagflag = FALSE;	/* diagonal mouse movements?	*/
CONST char errorm[] = "ERROR";		/* error literal		*/
CONST char truem[] = "TRUE";		/* true literal 		*/
CONST char falsem[] = "FALSE";		/* false litereal		*/
NOSHARE int DNEAR cmdstatus = TRUE;	/* last command status		*/
NOSHARE int schstatus = TRUE;		/* last search status jmd	*/
NOSHARE char palstr[49] = "";		/* palette string		*/
NOSHARE char lastmesg[NSTRING] = ""; 	/* last message posted		*/
NOSHARE char rval[NSTRING] = "0";	/* result of last procedure/sub	*/
NOSHARE char *lastptr = NULL;		/* ptr to lastmesg[]		*/
NOSHARE int DNEAR saveflag = 0; 	/* Flags, saved with the $target var */
NOSHARE char *fline = NULL;		/* dynamic return line		*/
NOSHARE int DNEAR flen = 0;		/* current length of fline	*/
NOSHARE int DNEAR eexitflag = FALSE;	/* EMACS exit flag		*/
NOSHARE int DNEAR eexitval = 0; 	/* and the exit return value	*/
NOSHARE int xpos = 0;		/* current column mouse is positioned to*/
NOSHARE int ypos = 0;		/* current screen row	     "		*/
NOSHARE int nclicks = 0;	/* cleared on any non-mouse event	*/
NOSHARE int disphigh = FALSE;	/* display high bit chars escaped	*/
NOSHARE int dispundo = FALSE;	/* display undo stack depth on comand line */
NOSHARE int defferupdate = FALSE;	/* if TRUE, update(TRUE) should be
					called before yielding to another
					Windows application */
NOSHARE int notquiescent = 1;		/* <=0 only when getkey called
					directly by editloop () */
NOSHARE int fbusy = FALSE;		/* indicates file activity if FREADING
					or FWRITING. Used by abort mechanism */
NOSHARE int hilite = 10;		/* current region to highlight (255 if none) */

/* uninitialized global definitions */

NOSHARE int oquote;		/* open quote variable */
NOSHARE int cquote;		/* close quote variable */
NOSHARE int DNEAR currow;	/* Cursor row			*/
NOSHARE int DNEAR curcol;	/* Cursor column		*/
NOSHARE int DNEAR thisflag;	/* Flags, this command		*/
NOSHARE int DNEAR lastflag;	/* Flags, last command		*/
NOSHARE int DNEAR curgoal;	/* Goal for C-P, C-N		*/
NOSHARE EWINDOW *curwp; 	/* Current window		*/
NOSHARE BUFFER *curbp; 		/* Current buffer		*/
NOSHARE EWINDOW *wheadp;	/* Head of list of windows	*/
NOSHARE BUFFER *bheadp;		/* Head of list of buffers 	*/
#if	LIBHELP
NOSHARE BUFFER *helpbp;		/* Help buffer			*/
#endif
NOSHARE UTABLE *uv_head;	/* head of list of user variables */
NOSHARE UTABLE *uv_global;	/* global variable table */
NOSHARE ABBREV *ab_head;	/* head of the abbreviation list */
NOSHARE int DNEAR ab_bell;	/* are we ringing the bell on completion? */
NOSHARE int DNEAR ab_cap;	/* match capatilization on expansion? */
NOSHARE int DNEAR ab_quick;	/* aggressive completion enabled? */
NOSHARE char ab_word[NSTRING];	/* current word being typed */
NOSHARE char *ab_pos;		/* current place in ab_word */
NOSHARE char *ab_end;		/* ptr to physical end of ab_word */
NOSHARE ESCREEN *first_screen;	/* Head and current screen in list */
NOSHARE BUFFER *blistp;		/* Buffer for C-X C-B		*/
NOSHARE BUFFER *ulistp;		/* Buffer for C-X U		*/
NOSHARE BUFFER *slistp;		/* Buffer for A-B		*/

NOSHARE char sres[NBUFN];	/* current screen resolution	*/
NOSHARE char os[NBUFN];		/* what OS are we running under */
#if	UTF8
NOSHARE char locale[NSTRING];	/* LC_CTYPE locale backup	*/
#else
NOSHARE	char lowcase[HICHAR];	/* lower casing map		*/
NOSHARE	char upcase[HICHAR];	/* upper casing map		*/
#endif

NOSHARE unsigned char pat[NPAT];	/* Search pattern		*/
NOSHARE unsigned char tap[NPAT];	/* Reversed pattern array.	*/
NOSHARE unsigned char rpat[NPAT];	/* replacement pattern		*/

/*	Various "Hook" execution variables	*/

NOSHARE KEYTAB readhook;	/* executed on all file reads */
NOSHARE KEYTAB wraphook;	/* executed when wrapping text */
NOSHARE KEYTAB cmdhook;		/* executed before looking for a command */
NOSHARE KEYTAB writehook;	/* executed on all file writes */
NOSHARE KEYTAB exbhook;		/* executed when exiting a buffer */
NOSHARE KEYTAB bufhook;		/* executed when entering a buffer */
NOSHARE	KEYTAB exithook;	/* executed when exiting emacs */

/* The variables matchline and matchoff hold the line
 * and offset position of the *start* of the match.
 * The variable patmatch holds the string that satisfies
 * the search command.
 */
NOSHARE int	matchlen;
NOSHARE int	matchoff;
NOSHARE LINE	*matchline;
NOSHARE char *patmatch = NULL;

#if	MAGIC
/*
 * The variables magical and rmagical determine if there
 * were actual metacharacters in the search and replace strings -
 * if not, then we don't have to use the slower MAGIC mode
 * search functions.
 */
NOSHARE short int DNEAR magical = FALSE;
NOSHARE short int DNEAR rmagical = FALSE;

NOSHARE MC mcpat[NPAT]; 	/* the magic pattern		*/
NOSHARE MC tapcm[NPAT]; 	/* the reversed magic pattern	*/
NOSHARE MC mcdeltapat[2]; 	/* the no-magic pattern		*/
NOSHARE MC tapatledcm[2]; 	/* the reversed no-magic pattern*/
NOSHARE RMC rmcpat[NPAT];	/* the replacement magic array	*/
NOSHARE char *grpmatch[MAXGROUPS];	/* holds groups found in search */

#endif

DELTA	deltapat;	/* Forward pattern delta structure.*/
DELTA	tapatled;	/* Reverse pattern delta structure.*/

/* directive name table:
	This holds the names of all the directives....	*/

CONST char *dname[] = {
	"if", "else", "endif",
	"goto", "return", "endm",
	"while", "endwhile", "break",
	"force"
};
/* directive lengths		*/
CONST short int dname_len[NUMDIRS] = {2, 4, 5, 4, 6, 4, 5, 7, 5, 5};

/*	var needed for macro debugging output	*/
NOSHARE char outline[NSTRING];	/* global string to hold debug line text */

/*
 * System message notification (at the moment, VMS only).
 */
#if	VMS
NOSHARE char brdcstbuf[1024];		/* Broadcast messages */
NOSHARE int pending_msg = FALSE;	/* Flag - have we notified user yet.*/
#endif

#if	HANDLE_WINCH
int winch_flag=0;			/* Window size changed flag */
#endif

#else

/* for all the other .C files */

/* initialized global external declarations */
#if	CURSES
extern WINDOW* wmenubar;
extern WINDOW* wmain;
extern PANEL* pmenu;
extern PANEL* pmain;
extern WINDOW* wdrop;
extern PANEL* pdrop;
#endif


NOSHARE extern int DNEAR fillcol;	/* Current fill column		*/
#if	UTF8
NOSHARE extern unsigned int kbdm[DUMMYSZ];	/* Holds keyboard macro data	*/
#else
NOSHARE extern short kbdm[DUMMYSZ];	/* Holds keyboard macro data	*/
#endif
NOSHARE extern char *execstr;		/* pointer to string to execute */
NOSHARE extern char golabel[DUMMYSZ];	/* current line to go to	*/
NOSHARE extern char paralead[DUMMYSZ];	/* paragraph leadin chars	*/
NOSHARE extern char fmtlead[DUMMYSZ];	/* format command leadin chars	*/
NOSHARE extern char mainbuf[DUMMYSZ];	/* name of main buffer		*/
NOSHARE extern char lterm[DUMMYSZ];	/* line terminators on file write */
NOSHARE extern unsigned char wordlist[DUMMYSZ];/* characters considered "in words" */
NOSHARE extern int DNEAR wlflag;	/* word list enabled flag	*/
NOSHARE extern int DNEAR clearflag;	/* clear screen on screen change? */
NOSHARE extern int DNEAR execlevel;	/* execution IF level		*/
NOSHARE extern int DNEAR eolexist;	/* does clear to EOL exist?	*/
NOSHARE extern int DNEAR revexist;	/* does reverse video exist?	*/
NOSHARE extern int DNEAR exec_error;	/* macro execution error pending? */
NOSHARE extern int DNEAR flickcode;	/* do flicker supression?	*/
NOSHARE extern int DNEAR mouse_move;	/* user allow tracking mouse moves? */
NOSHARE extern int DNEAR mmove_flag;	/* code currently allowing mmoves? */
NOSHARE extern int DNEAR newscreenflag;	/* Create new screen on reads? */
NOSHARE extern int DNEAR overlap;	/* overlap when paging screens */
CONST extern char *modename[DUMMYSZ];	/* text names of modes		*/
CONST extern char modecode[DUMMYSZ];	/* letters to represent modes	*/
NOSHARE extern int DNEAR numfunc;	/* number of bindable functions */
NOSHARE extern KEYTAB keytab[DUMMYSZ];	/* key bind to functions table	*/
NOSHARE extern NBIND names[DUMMYSZ];	/* name to function table	*/
NOSHARE extern int DNEAR gmode;		/* global editor mode		*/
NOSHARE extern int DNEAR gflags;	/* global control flag		*/
NOSHARE extern int DNEAR gfcolor;	/* global forgrnd color (white) */
NOSHARE extern int DNEAR gbcolor;	/* global backgrnd color (black)*/
NOSHARE extern int DNEAR deskcolor;	/* desktop background color	*/
NOSHARE extern int DNEAR gasave;	/* global ASAVE size		*/
NOSHARE extern int DNEAR gacount;	/* count until next ASAVE	*/
NOSHARE extern int DNEAR sgarbf;	/* State of screen unknown	*/
NOSHARE extern int DNEAR mpresf;	/* Stuff in message line	*/
NOSHARE extern int DNEAR clexec;	/* command line execution flag	*/
NOSHARE extern int DNEAR mstore;	/* storing text to macro flag	*/
NOSHARE extern int DNEAR discmd;	/* display command flag 	*/
NOSHARE extern int DNEAR disinp;	/* display input characters	*/
NOSHARE extern int DNEAR modeflag;	/* display modelines flag	*/
NOSHARE extern int DNEAR timeflag;	/* display time			*/
NOSHARE extern int DNEAR undoflag;	/* processing undo commands	*/
NOSHARE extern OBJECT obj;		/* scratch undo object		*/
NOSHARE extern int DNEAR undoing;	/* currently undoing a command? */
NOSHARE extern char DNEAR lasttime[DUMMYSZ];/* last time string displayed*/
NOSHARE extern int DNEAR popflag;	/* pop-up windows enabled?	*/
NOSHARE extern int DNEAR popwait;	/* user wait on pops enabled?	*/
NOSHARE extern int DNEAR posflag;	/* display point position	*/
NOSHARE extern int cpending;		/* input character pending?	*/
NOSHARE extern int charpending;		/* character pushed back	*/
NOSHARE extern int DNEAR sscroll;	/* smooth scrolling enabled flag*/
NOSHARE extern int DNEAR hscroll;	/* horizontal scrolling flag	*/
NOSHARE extern int DNEAR hscrollbar;	/* horizontal scroll bar flag	*/
NOSHARE extern int DNEAR vscrollbar;	/* vertical scroll bar flag */
NOSHARE extern int DNEAR hjump;		/* horizontal jump size 	*/
NOSHARE extern int DNEAR ssave;		/* safe save flag		*/
NOSHARE extern struct BUFFER *bstore;	/* buffer to store macro text to*/
NOSHARE extern int DNEAR vtrow;		/* Row location of SW cursor	*/
NOSHARE extern int DNEAR vtcol;		/* Column location of SW cursor */
NOSHARE extern int DNEAR ttrow;		/* Row location of HW cursor	*/
NOSHARE extern int DNEAR ttcol;		/* Column location of HW cursor */
NOSHARE extern int DNEAR lbound;	/* leftmost column of current line
					   being displayed		*/
NOSHARE extern int DNEAR taboff;	/* tab offset for display	*/
NOSHARE extern int DNEAR tabsize;	/* current hard tab size	*/
NOSHARE extern int DNEAR stabsize;	/* current soft tab size (0: use hard tabs)  */
NOSHARE extern int DNEAR reptc;		/* current universal repeat char*/
NOSHARE extern int DNEAR abortc;	/* current abort command char	*/
NOSHARE extern int DNEAR sterm;		/* search terminating character */
NOSHARE extern int DNEAR isterm;	/* incremental-search terminating char */
NOSHARE extern int DNEAR searchtype;	/* current search style		*/
NOSHARE extern int DNEAR yankflag;	/* current yank style		*/

NOSHARE extern int DNEAR prefix;	/* currently pending prefix bits */
NOSHARE extern int DNEAR prenum;	/*     "       "     numeric arg */
NOSHARE extern int DNEAR predef;	/*     "       "     default flag */

NOSHARE extern int DNEAR quotec;	/* quote char during mlreply() */
NOSHARE extern CONST char *cname[DUMMYSZ];/* names of colors		*/

NOSHARE extern int kill_index;		/* current index into kill ring */
NOSHARE extern KILL *kbufp[DUMMYSZ];	/* current kill buffer chunk pointer */
NOSHARE extern KILL *kbufh[DUMMYSZ];	/* kill buffer header pointer	*/
NOSHARE	extern int kskip[DUMMYSZ];	/* # of bytes to skip in 1st kill chunk */
NOSHARE extern int kused[DUMMYSZ];	/* # of bytes used in kill buffer*/
NOSHARE extern EWINDOW *swindow; 	/* saved window pointer 	*/
NOSHARE extern int cryptflag;		/* currently encrypting?	*/
NOSHARE extern int oldcrypt;		/* using old(broken) encryption? */
#if	UTF8
NOSHARE extern unsigned int *kbdptr;	/* current position in keyboard buf */
NOSHARE extern unsigned int *kbdend;	/* ptr to end of the keyboard */
#else
NOSHARE extern short *kbdptr;		/* current position in keyboard buf */
NOSHARE extern short *kbdend;		/* ptr to end of the keyboard */
#endif
NOSHARE extern int kbdmode;		/* current keyboard macro mode	*/
NOSHARE extern int kbdrep;		/* number of repetitions	*/
NOSHARE extern int restflag;		/* restricted use?		*/
NOSHARE extern int lastkey;		/* last keystoke		*/
NOSHARE extern long seed;		/* random number seed		*/
NOSHARE extern long envram;		/* # of bytes current in use by malloc */
NOSHARE extern long access_time;	/* counter of buffer access	*/
NOSHARE extern int DNEAR macbug;	/* macro debugging flag		*/
NOSHARE extern int DNEAR mouseflag;	/* use the mouse?		*/
NOSHARE extern int DNEAR diagflag;	/* diagonal mouse movements?	*/
CONST extern char errorm[DUMMYSZ];	/* error literal		*/
CONST extern char truem[DUMMYSZ];	/* true literal 		*/
CONST extern char falsem[DUMMYSZ];	/* false litereal		*/
NOSHARE extern int DNEAR cmdstatus;	/* last command status		*/
NOSHARE extern int schstatus;		/* last search status jmd	*/
NOSHARE extern char palstr[DUMMYSZ];	/* palette string		*/
NOSHARE extern char lastmesg[DUMMYSZ];	/* last message posted		*/
NOSHARE extern char rval[DUMMYSZ];	/* result of last procedure	*/
NOSHARE extern char *lastptr;		/* ptr to lastmesg[]		*/
NOSHARE extern int DNEAR saveflag;	/* Flags, saved with the $target var */
NOSHARE extern char *fline; 		/* dynamic return line */
NOSHARE extern int DNEAR flen;		/* current length of fline */
NOSHARE extern int DNEAR eexitflag;	/* EMACS exit flag */
NOSHARE extern int DNEAR eexitval;	/* and the exit return value */
NOSHARE extern int xpos;		/* current column mouse is positioned to */
NOSHARE extern int ypos;		/* current screen row	     "	 */
NOSHARE extern int nclicks;		/* cleared on any non-mouse event*/
NOSHARE extern int disphigh;		/* display high bit chars escaped*/
NOSHARE extern int dispundo;	/* display undo stack depth on comand line */
NOSHARE extern int defferupdate;	/* if TRUE, update(TRUE) should be
					called before yielding to another
					Windows application */
NOSHARE extern int notquiescent;	/* <=0 only when getkey called
					directly by editloop () */
NOSHARE extern int fbusy;		/* indicates file activity if FREADING
					or FWRITING. Used by abort mechanism */
NOSHARE extern int hilite;		/* current region to highlight (255 if none) */

/* uninitialized global external declarations */

NOSHARE extern int oquote;		/* open quote variable */
NOSHARE extern int cquote;		/* close quote variable */
NOSHARE extern int DNEAR currow;	/* Cursor row			*/
NOSHARE extern int DNEAR curcol;	/* Cursor column		*/
NOSHARE extern int DNEAR thisflag;	/* Flags, this command		*/
NOSHARE extern int DNEAR lastflag;	/* Flags, last command		*/
NOSHARE extern int DNEAR curgoal;	/* Goal for C-P, C-N		*/
NOSHARE extern EWINDOW *curwp; 		/* Current window		*/
NOSHARE extern BUFFER *curbp; 		/* Current buffer		*/
NOSHARE extern EWINDOW *wheadp;		/* Head of list of windows	*/
NOSHARE extern BUFFER *bheadp;		/* Head of list of buffers	*/
#if	LIBHELP
NOSHARE extern BUFFER *helpbp;		/* Help buffer			*/
#endif
NOSHARE extern ABBREV *ab_head;		/* head of the abbreviation list */
NOSHARE extern UTABLE *uv_head;		/* head of list of user variables */
NOSHARE extern UTABLE *uv_global;	/* global variable table */
NOSHARE extern int DNEAR ab_bell;	/* are we ringing the bell on completion? */
NOSHARE extern int DNEAR ab_cap;	/* match capatilization on expansion? */
NOSHARE extern int DNEAR ab_quick;	/* aggressive completion enabled? */
NOSHARE extern char ab_word[DUMMYSZ];	/* current word being typed */
NOSHARE extern char *ab_pos;		/* current place in ab_word */
NOSHARE extern char *ab_end;		/* ptr to physical end of ab_word */
NOSHARE extern ESCREEN *first_screen;	/* Head and current screen in list */
NOSHARE extern BUFFER *blistp;		/* Buffer for C-X C-B		*/
NOSHARE extern BUFFER *ulistp;		/* Buffer for C-X U		*/
NOSHARE extern BUFFER *slistp;		/* Buffer for A-B		*/

NOSHARE extern char sres[NBUFN];	/* current screen resolution	*/
NOSHARE extern char os[NBUFN];		/* what OS are we running under */
#if	UTF8
NOSHARE char locale[NSTRING];		/* LC_CTYPE locale backup	*/
#else
NOSHARE	extern char lowcase[HICHAR];	/* lower casing map		*/
NOSHARE	extern char upcase[HICHAR];	/* upper casing map		*/
#endif
NOSHARE extern unsigned char pat[DUMMYSZ]; /* Search pattern		*/
NOSHARE extern unsigned char tap[DUMMYSZ]; /* Reversed pattern array.	*/
NOSHARE extern unsigned char rpat[DUMMYSZ]; /* replacement pattern	*/

/*	Various "Hook" execution variables	*/

NOSHARE extern KEYTAB readhook;		/* executed on all file reads */
NOSHARE extern KEYTAB wraphook;		/* executed when wrapping text */
NOSHARE extern KEYTAB cmdhook;		/* executed before looking for a cmd */
NOSHARE extern KEYTAB writehook;	/* executed on all file writes */
NOSHARE extern KEYTAB exbhook;		/* executed when exiting a buffer */
NOSHARE extern KEYTAB bufhook;		/* executed when entering a buffer */
NOSHARE	extern KEYTAB exithook;		/* executed when exiting emacs */

NOSHARE extern int matchlen;
NOSHARE extern int matchoff;
NOSHARE extern LINE *matchline;
NOSHARE extern char *patmatch;

#if	MAGIC
NOSHARE extern short int magical;
NOSHARE extern short int rmagical;
NOSHARE extern MC mcpat[NPAT];		/* the magic pattern		*/
NOSHARE extern MC tapcm[NPAT];		/* the reversed magic pattern	*/
NOSHARE extern MC mcdeltapat[2]; 	/* the no-magic pattern		*/
NOSHARE extern MC tapatledcm[2]; 	/* the reversed no-magic pattern*/
NOSHARE extern RMC rmcpat[NPAT];	/* the replacement magic array	*/
NOSHARE extern char *grpmatch[MAXGROUPS];	/* holds groups found in search */
#endif

NOSHARE extern DELTA deltapat;	/* Forward pattern delta structure.*/
NOSHARE extern DELTA tapatled;	/* Reverse pattern delta structure.*/

CONST extern char *dname[DUMMYSZ];	/* directive name table 	*/
CONST extern short int dname_len[NUMDIRS]; /* directive lengths		*/

/*	var needed for macro debugging output	*/
NOSHARE extern char outline[DUMMYSZ];	/* global string to hold debug line text */

/*
 * System message notification (at the moment, VMS only).
 */
#if	VMS
NOSHARE extern char brdcstbuf[1024];		/* Broadcast messages */
NOSHARE extern int pending_msg;		/* Flag - have we notified user yet.*/
#endif

#if	HANDLE_WINCH
NOSHARE extern int winch_flag;		/* Window size changed flag */
#endif

#endif

/* terminal table defined only in TERM.C */

#ifndef termdef
NOSHARE extern TERM	term;		/* Terminal information.	*/
#endif
