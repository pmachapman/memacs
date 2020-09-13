/*
 *	MicroEMACS 5.00
 *		written by Daniel M. Lawrence
 *		based on code by Dave G. Conroy.
 *
 *	(C)Copyright 1988-2009 by Daniel M. Lawrence
 *	MicroEMACS 5.00 can be copied and distributed freely for any
 *	non-commercial purposes. MicroEMACS 5.00 can only be incorporated
 *	into commercial software with the permission of the current author.
 *
 *      Unicode support by Jean-Michel Dubois
 *
 * This file contains the main driving routine, and some keyboard processing
 * code, for the MicroEMACS screen editor.
 *
 */

#include <stdio.h>

/* make global definitions not external */
#define maindef

#include "estruct.h"	/* global structures and defines */
#include "eproto.h"	/* variable prototype definitions */
#include "efunc.h"	/* function declarations and name table */
#include "edef.h"	/* global definitions */
#include "elang.h"	/* human language definitions */
#include "ebind.h"	/* default key bindings */

/* for many different systems, increase the default stack space */

#if	MSDOS && MSC
#if	_MSC_VER < 700
unsigned _stackavail = 40000;
#endif
#endif

#if	MSDOS && LATTICE
unsigned _stack = 20000;
#endif

#if	MSDOS && ZTC
int	_okbigbuf = 0;		/* Only allocate memory when needed.*/
int	_stack = 20000; 	/* Reset the ol' stack size.*/
#endif

#if	TOS && MWC
long _stksize = 20000L; 	/* reset stack size (must be even) */
#endif

#if	MSDOS && TURBO
extern unsigned int _stklen = 10000;
#endif

/*	make VMS happy...	*/

#if	VMS
#include	<ssdef.h>
#define GOOD	(SS$_NORMAL)
#endif

#ifndef GOOD
#define GOOD	0
#endif

/*
 * Systems that handle window size changes via signals.
 */
#if HANDLE_WINCH
#include <signal.h>
#endif

#if	THEOS
# ifndef MAINWA_BUG
#  pragma wild argv
# endif
#pragma prog 5.0,0,0,0,"Port by Jean-Michel Dubois"
#endif

/*
	This is the primary entry point that is used by command line
	invocation, and by applications that link with microemacs in
	such a way that each invocation of Emacs is a fresh environment.

	There is another entry point in VMS.C that is used when
	microemacs is "linked" (In quotes, because it is a run-time link
	rather than a link-time link.) with applications that wish Emacs
	to preserve it's context across invocations.  (For example,
	EMACS.RC is only executed once per invocation of the
	application, instead of once per invocation of Emacs.)

	Note that re-entering an Emacs that is saved in a kept
	subprocess would require a similar entrypoint.
 */

#if	CALLED
int emacs(argc, argv)
#else
#if	XVT
int called_main(argc, argv)
#else
int main(argc, argv)
#endif
#endif

int argc;			/* # of arguments */
char *argv[];			/* argument strings */

{
	register int status = 0;
#if	UTF8
	char* p = setlocale(LC_CTYPE, NULL);
#endif
#if defined(THEOS) && defined(MAINWA_BUG)
	extern int _setargv(int*, char***);
	_setargv(&argc, &argv);
#endif
#if HANDLE_WINCH
	signal(SIGWINCH,winch_changed);
#endif
#if	UTF8
	/* save LC_CTYPE locale */
	if (p) {
		size_t l = strlen(p);
		l = min(l, NSTRING - 1);
		strncpy(locale, p, l);
		locale[l] = '\0';
	}
	else
		p = "C";


	if (! strstr(p, ".UTF-8")) {
		/* set French locale */
		setlocale(LC_CTYPE, "");
	}
#endif
	/* the room mechanism would deallocate undo info no failure....
	   its not set up yet, so make sure it doesn't try until the
	   editor is initialized */
	bheadp = (BUFFER *) NULL;

	/* Initialize the editor */
	eexitflag = FALSE;
#if	DYNMSGS
	msinit();		/* load dynamic messages */
#endif
#if	!WINDOW_MSWIN
	vtinit();			/* Terminal */
#endif

	if (eexitflag)
		goto abortrun;
	edinit(mainbuf);	/* Buffers, windows, screens */
	ab_init();		/* initialize the abbreviation behavior */
	varinit();		/* user variables */
#if ! UTF8
	initchars();		/* character set definitions */
#endif
#if MAGIC
	mcdeltapat[0].mc_type = tapatledcm[0].mc_type = JMPTABLE;
	mcdeltapat[0].u.jmptable = &deltapat;
	tapatledcm[0].u.jmptable = &tapatled;
	mcdeltapat[1].mc_type = tapatledcm[1].mc_type = MCNIL;
#endif

	/* Process the command line and let the user edit */
#if	VMS
	expandargs(&argc, &argv);	/* expand VMS wildcards.*/
#endif
	dcline(argc, argv, TRUE);
edit:
	status = editloop();
abortrun:

	/* execute the macro the user had bound to $exithook */
	eexitflag = FALSE;
	execkey(&exithook, FALSE, 1);
	if ((gflags & GFEXIT) == 0) {
		eexitflag = FALSE;
		goto edit;
	}
	eexitflag = TRUE;

	/* close things down */
	vttidy();
#if	CLEAN
	clean();
#endif
#if	UTF8
	setlocale(LC_CTYPE, locale);	/* restore LC_CTYPE locale */
#endif
#if	CALLED
	return(status);
#else
	exit(status);
#endif
}

#if	CLEAN
/*
	On some primitive operation systems, and when emacs is used as
	a subprogram to a larger project, emacs needs to de-alloc its
	own used memory, otherwise we just exit.
 */

VOID PASCAL NEAR clean()

{
	register BUFFER *bp;	/* buffer list pointer */
	register ESCREEN *scrp;	/* ptr to screen to dump */

	/* first clean up the screens */
	scrp = first_screen;
	while (scrp) {
		first_screen = scrp->s_next_screen;
		free_screen(scrp);
		scrp = first_screen;
	}
	wheadp = NULL;

	/* then the buffers */
	bp = bheadp;
	while (bp) {
		bp->b_nwnd = 0;
		bp->b_flag = 0; /* don't say anything about a changed buffer! */
		zotbuf(bp);
		bp = bheadp;
	}

	/* and the kill buffers */
	clear_ring(FALSE, 0);

	/* clear some search variables */
#if	MAGIC
	mcclear();
	rmcclear();
#endif
	if (patmatch != NULL) {
		free(patmatch);
		patmatch = NULL;
	}

	/* dump the abbreviation list */
	ab_clean();

	/* dealloc the user variables */
	varclean(uv_head);

	/* and the video buffers */
	vtfree();
}
#endif

/*	Process a command line.   May be called any time.	*/

VOID PASCAL NEAR dcline(argc, argv, firstflag)

int argc;
char *argv[];
int firstflag;			/* is this the first time in? */

{
	register BUFFER *bp;	/* temp buffer pointer */
	register int	firstfile;	/* first file flag */
	register int	carg;	/* current arg to scan */
	register int	startflag;	/* startup executed flag */
	BUFFER *firstbp = NULL;	/* ptr to first buffer in cmd line */
	int viewflag;		/* are we starting in view mode? */
	int gotoflag;		/* do we need to goto a line at start? */
	int gline;			/* if so, what line? */
	int gchar;			/* and what character? */
	int searchflag;		/* Do we need to search at start? */
	int errflag;		/* C error processing? */
	VDESC vd;			/* variable num/type */
	char bname[NBUFN];	/* buffer name of file to read */
#if	MSDOS | OS2
	unsigned char *scan;	/* scan pointer for filenames */
#endif

#if	CRYPT
	int cryptflag;		/* encrypting on the way in? */
	char ekey[NPAT];	/* startup encryption key */
#endif
#if	BACKUP
	int nobackflag;		/* no backup ? */
#endif
	viewflag = FALSE;	/* view mode defaults off in command line */
	gotoflag = FALSE;	/* set to off to begin with */
	gline = 1;
	gchar = 1;			/* line and character to go to */
	searchflag = FALSE;	/* set to off to begin with */
	firstfile = TRUE;	/* no file to edit yet */
	startflag = FALSE;	/* startup file not executed yet */
	errflag = FALSE;	/* not doing C error parsing */
	exec_error = FALSE;	/* no macro error pending */
#if	CRYPT
	cryptflag = FALSE;	/* no encryption by default */
#endif
#if	BACKUP
	nobackflag = FALSE;	/* backup files by default */
#endif
	disphigh = FALSE;	/* don't escape high bit characters */
	lterm[0] = 0;		/* standard line terminators */

	/* Parse a command line */
	for (carg = 1;  carg < argc;  ++carg) {

		/* Process Switches */
		if (argv[carg][0] ==
#if WMCS
				':'
#else
				'-'
#endif
		) {
			/* Process Startup macroes */
			switch (argv[carg][1]) {

				case 'c':	/* -c for changeable file */
				case 'C':
					viewflag = FALSE;
					break;
				case 'e':	/* -e process error file */
				case 'E':
					errflag = TRUE;
					break;
				case 'g':	/* -g for initial goto line */
				case 'G':
					gotoflag = TRUE;
					gline = asc_int(&argv[carg][2]);
					break;
#if	LIBHELP
				case 'h':	/* Alternate help */
					if (! argv[carg][2]) {
						if (++carg == argc) {
							continue;
						}
						initlook(argv[carg]);
					} else
						initlook(&argv[carg][2]);
					break;
#endif
				case 'i':	/* -i<var> <value> set an initial */
				case 'I':	/* value for a variable */
					bytecopy(bname, &argv[carg][2], NVSIZE);
					findvar(bname, &vd, NVSIZE + 1, VT_GLOBAL);
					if (vd.v_type == -1) {
						mlwrite(TEXT52, bname);
						/*							"%%No such variable as '%s'" */
						break;
					}
					svar(&vd, argv[++carg]);
					break;
#if	CRYPT
				case 'k':	/* -k<key> for code key */
				case 'K':
					cryptflag = TRUE;
					strcpy(ekey, &argv[carg][2]);
					break;
#endif
#if	BACKUP
				case 'n':	/* -n for No Backup File */
					nobackflag = TRUE;
					break;
#endif
				case 'p':	/* -p for initial goto char position */
				case 'P':
					gotoflag = TRUE;
					gchar = asc_int(&argv[carg][2]);
					break;
				case 'r':	/* -r restrictive use */
				case 'R':
					restflag = TRUE;
					break;
				case 's':	/* -s for initial search string */
				case 'S':
					searchflag = TRUE;
					bytecopy((char *) pat, &argv[carg][2], NPAT);
					setjtable();
					break;
				case 'v':	/* -v for View File */
				case 'V':
					viewflag = TRUE;
					break;
				default:	/* unknown switch */
					/* ignore this for now */
					break;
			}

		} else if (argv[carg][0] == '+') {

			/* +<line num> */
			gotoflag = TRUE;
			gline = asc_int(&argv[carg][1]);

		} else if (argv[carg][0] == '@') {

			/* Process Startup macroes */
			if (startup(&argv[carg][1]) == TRUE)
				/* don't execute emacs.rc */
				startflag = TRUE;

#if WINDOW_MSWIN32
		} else if ((argv[carg][0] != ' ') ||
				(argv[carg][1] != '\0')) {
			/* WinNT PDK2 causes spurious space arguments */
#else
		} else {
#endif
			/* Process an input file */
#if	(MSDOS|ST520|THEOS)
			strlwr(argv[carg]);
#endif
#if	MSDOS | OS2
			/* change forward slashes to back */
			scan = (unsigned char *) argv[carg];
			while (*scan) {
				if (*scan == '/')
					*scan = DIRSEPCHAR;
				++scan;
			}
#endif
			/* set up a buffer for this file */
			makename(bname, argv[carg]);
			unqname(bname);

			/* set this to inactive */
			bp = bfind(bname, TRUE, 0);
			strcpy(bp->b_fname, argv[carg]);
#if	WINDOW_MSWIN
			fullpathname (bp->b_fname, NFILEN);
#endif
			bp->b_active = FALSE;
			if (firstfile) {
				firstbp = bp;
				firstfile = FALSE;
			}

			/* set the modes appropriatly */
			if (viewflag)
				bp->b_mode |= MDVIEW;
#if	CRYPT
			if (cryptflag) {
				bp->b_mode |= MDCRYPT;
				ecrypt((char *) NULL, 0);
				ecrypt(ekey, strlen(ekey));
				bytecopy(bp->b_key, ekey, NPAT);
			}
#endif
#if	BACKUP
			if (nobackflag)
				bp->b_mode |= MDNOBAK;
#endif
		}
	}

	/* if we are C error parsing... run it! */
	if (errflag) {
		if (startup("error.cmd") == TRUE)
			startflag = TRUE;
	}

	/* if invoked with no other startup files,
	   run the system startup file here */
	if (firstflag && startflag == FALSE)
		startup("");

	/* if there are any files to read, read the first one! */
	if (firstflag) {
		bp = bfind(mainbuf, FALSE, 0);
		if (firstfile == FALSE && (gflags & GFREAD)) {
			swbuffer(firstbp);
			curbp->b_mode |= gmode;
			update(TRUE);
			mlwrite(lastmesg);
			zotbuf(bp);
		} else
			bp->b_mode |= gmode;
	} else {
		swbuffer(firstbp);
		curbp->b_mode |= gmode;
		update(TRUE);
		mlwrite(lastmesg);
	}

	/* Deal with startup gotos and searches */
	if (gotoflag && searchflag) {
		update(FALSE);
		mlwrite(TEXT101);
		/*			"[Can not search and goto at the same time!]" */
	} else if (gotoflag) {
		if ((gotoline(TRUE, gline) == FALSE) ||
				(forwchar(TRUE, gchar - 1) == FALSE)) {
			update(FALSE);
			mlwrite(TEXT102);
			/*				"[Bogus goto argument]" */
		}
	} else if (searchflag) {
		if (forwhunt(FALSE, 0) == FALSE)
			update(FALSE);
	}
}

#if	WINDOW_MSWIN
#define GETBASEKEY getbasekey
static int PASCAL NEAR getbasekey()

{
	register int c;

	notquiescent = -1;  /* will be <= 0 only if get_key() is called
			   directly from editloop(). This is used to
			   restrict some windows-specific actions
			   (menus, sizing, etc...) when not called from
			   the lowest level of the editor */
	c = get_key();
	notquiescent = 1;
	return c;
}
#else
#define GETBASEKEY get_key
#endif

/*
	This is called to let the user edit something.	Note that if you
	arrange to be able to call this from a macro, you will have
	invented the "recursive-edit" function.
 */

int PASCAL NEAR editloop()

{
	register int c;		/* command character */
	register int f;		/* default flag */
	register int n;		/* numeric repeat count */
	register int mflag;	/* negative flag on repeat */
	register int basec;	/* c stripped of meta character */
	register int oldflag;	/* old last flag value */
	char time[6];		/* current display time */

	/* setup to process commands */
	lastflag = 0;		/* Fake last flags.	*/

loop:
	/* if a macro error is pending, wait for a character */
	if (exec_error) {
#if	WINDOW_MSWIN
		mlhistory();
#else
		mlforce(TEXT227);
/*			"\n--- Press any key to Continue ---" */
		tgetc();
#endif
		sgarbf = TRUE;
		update(FALSE);
		mlferase();
		exec_error = FALSE;
	}

	/* if we were called as a subroutine and want to leave, do so */
	if (eexitflag)
		return(eexitval);

	/* execute the "command" macro...normally null */
	oldflag = lastflag;	/* preserve lastflag through this */
	execkey(&cmdhook, FALSE, 1);
	lastflag = oldflag;

	/* Notify user of any intercepted system messages.
	 * VMS only right now.
	 */
#if	VMS
	if (pending_msg) {
		pending_msg = FALSE;
		echostring(brdcstbuf, 0, NSTRING);
	}
#endif

	/* update time on the bottom modeline? */
	if (timeflag)
#if TYPEAH || WINDOW_MSWIN
		if (!typahead())
#endif
		{
			getdtime(time);
			if (strcmp(lasttime, time) != 0)
				upmode();
		}

	/* update position on current modeline? */
	if (posflag)
#if TYPEAH || WINDOW_MSWIN
		if (!typahead())
#endif
			upmode();

	/*
	 * Did our window get resized?
	 */
#if HANDLE_WINCH
	if (winch_flag) winch_new_size();
#endif
	/* Fix up the screen	*/
	update(FALSE);

	/* get the next command from the keyboard */
	discmd = TRUE;
	disinp = TRUE;
	c = GETBASEKEY();

	/* if there is something on the command line, clear it */
	if (mpresf != FALSE) {
		mlerase();
		update(FALSE);
	}

	/* override the arguments if prefixed */
	if (prefix) {
		if (is_lower(c & CMSK))
			c = (c & ~CMSK) | upperc(c & CMSK);
		c |= prefix;
		f = predef;
		n = prenum;
		prefix = 0;
	} else {
		f = FALSE;
		n = 1;
	}

	/* do META-# processing if needed */

	basec = c & ~META;	/* strip meta char off if there */
	if ((c & META) && ((basec >= '0' && basec <= '9') || basec == '-') &&
	    (getbind(c) == NULL)) {
		f = TRUE;		/* there is a # arg */
		n = 0;			/* start with a zero default */
		mflag = 1;		/* current minus flag */
		c = basec;		/* strip the META */
		while ((c >= '0' && c <= '9') || (c == '-')) {
			if (c == '-') {
				/* already hit a minus or digit? */
				if ((mflag == -1) || (n != 0))
					break;
				mflag = -1;
			} else {
				n = n * 10 + (c - '0');
			}
			if ((n == 0) && (mflag == -1))	/* lonely - */
				mlwrite("Arg:");
			else
				mlwrite("Arg: %d", n * mflag);

			c = GETBASEKEY();	/* get the next key */
		}
		n = n * mflag;	/* figure in the sign */
	}

	/* do ^U repeat argument processing */

	if (c == reptc) { /* ^U, start argument   */
		f = TRUE;
		n = 4;			/* with argument of 4 */
		mflag = 0;		/* that can be discarded. */
		mlwrite("Arg: 4");
		while (((c = GETBASEKEY()) >= '0' && c <= '9') ||
				c == reptc || c == '-') {
			if (c == reptc)
				if ((n > 0) == ((n * 4) > 0))
					n = n * 4;
				else
					n = 1;
			/*
			 * If dash, and start of argument string, set arg.
			 * to -1.  Otherwise, insert it.
			 */
			else if (c == '-') {
				if (mflag)
					break;
				n = 0;
				mflag = -1;
			}

			/*
			 * If first digit entered, replace previous argument
			 * with digit and set sign.  Otherwise, append to arg.
			 */
			else {
				if (!mflag) {
					n = 0;
					mflag = 1;
				}
				n = 10 * n + c - '0';
			}
			mlwrite("Arg: %d", (mflag >= 0) ? n : (n ? -n : -1));
		}

		/*
		 * Make arguments preceded by a minus sign negative and change
		 * the special argument "^U -" to an effective "^U -1".
		 */
		if (mflag == -1) {
			if (n == 0)
				n++;
			n = -n;
		}
	}
#if	MDSLINE
	if (! (c & ~CMSK) && (curbp->b_mode & (MDSLINE|MDDLINE)) && (c =='+' || iswdigit(c)))
		c = tosgraph(c);
#endif
	/* and execute the command */
#if	CURSES
	if (wdrop != NULL && ! (c & MOUS))
		 menukey(c);
	else
#endif
	execute(c, f, n);
	goto loop;
}

/*
 * Initialize all of the buffers, windows and screens. The buffer name is
 * passed down as an argument, because the main routine may have been told
 * to read in a file by default, and we want the buffer name to be right.
 */

int PASCAL NEAR edinit(bname)

char bname[];			/* name of buffer to initialize */

{
	register BUFFER *bp;
	register int index;

	/* the quote characters are LANGUAGE SPECIFIC
	   so they need to be set here instead of in the header file */
	oquote = OQUOTE_CHAR;
	cquote = CQUOTE_CHAR;

	/* init the kill ring */
	for (index = 0;  index < NRING;  index++) {
		kbufp[index] = (KILL *) NULL;
		kbufh[index] = (KILL *) NULL;
		kskip[index] = 0;
		kused[index] = KBLOCK;
	}
	kill_index = 0;

	/* initialize some important globals */

	readhook.k_ptr.fp = nullproc;	/* set internal hooks to OFF */
	readhook.k_type = BINDFNC;
	wraphook.k_ptr.fp = wrapword;
	wraphook.k_type = BINDFNC;
	cmdhook.k_ptr.fp = nullproc;
	cmdhook.k_type = BINDFNC;
	writehook.k_ptr.fp = nullproc;
	writehook.k_type = BINDFNC;
	bufhook.k_ptr.fp = nullproc;
	bufhook.k_type = BINDFNC;
	exbhook.k_ptr.fp = nullproc;
	exbhook.k_type = BINDFNC;
	exithook.k_ptr.fp = nullproc;
	exithook.k_type = BINDFNC;

	/* allocate the first buffer */
	bp = bfind(bname, TRUE, 0);	/* First buffer 	*/
	blistp = bfind("[Buffers]", TRUE, BFINVS);	/* Buffer list buffer	*/
	slistp = bfind("[Screens]", TRUE, BFINVS);	/* screen list buffer	*/
	ulistp = bfind("[Undos]", TRUE, BFINVS);	/* undo list buffer	*/
#if	LIBHELP
	helpbp = bfind(TEXT294, TRUE, BFINVS|BFHELP);	/* Help buffer	*/
	bfind(TEXT295, TRUE, BFINVS);			/* Error buffer */
	bfind(TEXT296, TRUE, BFINVS);			/* Fkeys buffer	*/
#endif
	if (bp == NULL || blistp == NULL
#if	LIBHELP
			|| helpbp == NULL
#endif
	)
		meexit(1);

	/* and allocate the default screen */
	first_screen = (ESCREEN *) NULL;
	init_screen("MAIN", bp);
	if (first_screen == (ESCREEN *) NULL)
		return(meexit(1));

	/* set the current default screen/buffer/window */
	curbp = bp;
	curwp = wheadp = first_screen->s_cur_window = first_screen->s_first_window;
	return(TRUE);
}

/*
 * This is the general command execution routine. It handles the fake binding
 * of all the keys to "self-insert". It also clears out the "thisflag" word,
 * and arranges to move it to the "lastflag", so that the next command can
 * look at it. Return the status of command.
 */

int PASCAL NEAR execute(c, f, n)

int c;					/* key to execute */
int f;					/* prefix argument flag */
int n;					/* prefix value */

{
	register int status;
	KEYTAB *key;		/* key entry to execute */
#if	DBCS
	int schar;		/* second key in 2 byte sequence */
#endif

#if	WINDOW_MSWIN
	/* if it is a menu command, go for it... */
	if ((c & MENU) == MENU) {
		thisflag = 0;
		status = execmenu(f, n);
		lastflag = thisflag;
		return(status);
	}
#endif
	/* if the keystroke is a bound function...do it */
	key = getbind(c);
	if (key != NULL) {

		/* before a command, we attempt to expand abbreviations */
		ab_expand();

		/* Don't reset the function type flags on a prefix */
		if ((key->k_type == BINDFNC) &&
				((key->k_ptr.fp == uemeta) || (key->k_ptr.fp == cex)))
			status = execkey(key, f, n);
		else {
			thisflag = 0;
			status = execkey(key, f, n);
			lastflag = thisflag;
		}

		return(status);
	}

	/* since the keystroke is not a command, */
	if (isinword(c))
		/* in a word, we save it */
		ab_save(c);
	else
		/* not in a word, we attempt an expansion */
		ab_expand();

	/*
	 * If a space was typed, fill column is defined, the argument is non-
	 * negative, wrap mode is enabled, and we are now past fill column,
	 * and we are not read-only, perform word wrap.
	 */
	if (c == ' '
	 && (curwp->w_bufp->b_mode & MDWRAP)
	 && fillcol > 0
	 && n >= 0
	 && getccol(FALSE) > fillcol
	 && (curwp->w_bufp->b_mode & MDVIEW) == FALSE)
		execkey(&wraphook, FALSE, 1);

	if ((c >= 0x20 && c <= CMSK)) { /* Self inserting.	*/
		if (n <= 0) {		/* Fenceposts.		*/
			lastflag = 0;
			return(n < 0 ? FALSE : TRUE);
		}
		thisflag = 0;	/* For the future.	*/


#if	DBCS
		/* Get the second half of a double-byte character.*/
		if (is2char(c))
			schar = get_key();
#endif

		/* replace or overwrite mode, not at the end of a string */
		if ((curwp->w_bufp->b_mode & (MDREPL | MDOVER)) &&
				curwp->w_doto < lused(curwp->w_dotp)) {
			do {
				/* if we are in replace mode, or
				   (next char is not a tab or we are at a tab stop) */
				if ((curwp->w_bufp->b_mode & MDREPL)
				 || ((lgetc(curwp->w_dotp, curwp->w_doto) != '\t'
				 || tabsize == 0)
				 || getccol(FALSE) % tabsize == (tabsize - 1))) {

					/* make sure the cursor gets back to
					   the right place on an undo */
					undo_insert(OP_CPOS, 0L, obj);
#if	UTF8
					ldelchar(1L, FALSE);
#else
					ldelete(1L, FALSE);
#endif
				}

				/* do the appropriate insertion */
				if (c == '}' && (curbp->b_mode & MDCMOD) != 0)
					status = insbrace(1, c);
				else if (c == '#' && (curbp->b_mode & MDCMOD) != 0)
					status = inspound();
				else {
					status = linsert(1, c);
#if	DBCS
					/* Insert the second half of a double-byte character.*/
					if (is2char(c))
						status = linsert(1, schar);
#endif
				}
			} while (--n > 0 && curwp->w_doto < lused(curwp->w_dotp) && status == TRUE);
		}

		/* do the appropriate insertion */
		if (n > 0) {
			if (c == '}' && (curbp->b_mode & MDCMOD) != 0)
				status = insbrace(n, c);
			else if (c == '#' && (curbp->b_mode & MDCMOD) != 0)
				status = inspound();
#if	DBCS
			else if (is2char(c)) {
				status = TRUE;
				while (n--) {
					if (linsert(1, c) == FALSE)
						status = FALSE;
					if (linsert(1, schar) == FALSE)
						status = FALSE;
				}
			}
#endif
			else
				status = linsert(n, c);
		}

		/* In ABBREV mode, if we are doing aggressive expansion and
		   the current buffer is a symbol in the abbreviation table */
		if (((curbp->b_mode & MDABBR) != 0) &&
			(ab_quick && (ab_lookup(ab_word) != NULL)))
			ab_expand();

		/* check for CMODE fence matching */
		if ((c == '}' || c == ')' || c == ']') &&
			(curbp->b_mode & MDCMOD) != 0)
			fmatch(c);

		/* check auto-save mode */
		if (curbp->b_mode & MDASAVE)

			if (--gacount == 0) {

				/* and save the file if needed */
				upscreen(FALSE, 0);
				filesave(FALSE, 0);
				gacount = gasave;
			}

		lastflag = thisflag;
		return(status);
	}
	TTbeep();
	mlwrite(TEXT19);	/* complain		*/
/*		"[Key not bound]" */
	lastflag = 0;		/* Fake last flags.	*/
	return(FALSE);
}

/*
	Fancy quit command, as implemented by Norm. If the any buffer
has changed do a write on that buffer and exit emacs, otherwise simply
exit.
 */

int PASCAL NEAR quickexit(f, n)

int f, n;				/* prefix flag and argument */

{
	register BUFFER *bp;	/* scanning pointer to buffers */
	register BUFFER *oldcb;	/* original current buffer */
	register int status;

	oldcb = curbp;		/* save in case we fail */

#if	TIPC
	mlwrite("\n\n");
#endif
	bp = bheadp;
	while (bp != NULL) {

		if ((bp->b_flag & BFCHG) != 0	/* Changed.		*/
				&& (bp->b_flag & BFINVS) == 0) { /* Real. */
			curbp = bp;	/* make that buffer cur */
			mlwrite(TEXT103, bp->b_fname);
			/*				"[Saving %s]" */
			mlwrite("\n");
			if ((status = filesave(f, n)) != TRUE) {
				curbp = oldcb;	/* restore curbp */
				return(status);
			}
		}
		bp = bp->b_bufp;	/* on to the next buffer */
	}
	quit(f, n);			/* conditionally quit	*/
	return(TRUE);
}

/*
 * Quit command. If an argument, always quit. Otherwise confirm if a buffer
 * has been changed and not written out. Normally bound to "C-X C-C".
 */

int PASCAL NEAR quit(f, n)

int f, n;				/* prefix flag and argument */
{
	register int status;	/* return status */

	if (f != FALSE		/* Argument forces it.	*/
	    || anycb() == FALSE	/* All buffers clean or user says it's OK. */
	    || (status = mlyesno(TEXT104)) == TRUE) {
/*			     "Modified buffers exist. Leave anyway" */
#if	FILOCK
		if (lockrel() != TRUE) {
			TTputc('\n');
			TTputc(RET_CHAR);
			TTclose();
			TTkclose();
			status = meexit(1);
		}
#endif
		if (f)
			status = meexit(n);
		else
			status = meexit(GOOD);
	}
	mlerase();
	return(status);
}

int PASCAL NEAR meexit(status)
int status;				/* return status of emacs */
{
	eexitflag = TRUE;	/* flag a program exit */
	gflags |= GFEXIT;
	eexitval = status;

	/* and now.. we leave and let the main loop kill us */
	return(TRUE);
}

/*
 * Begin a keyboard macro.
 * Error if not at the top level in keyboard processing. Set up variables and
 * return.
 */

int PASCAL NEAR ctlxlp(f, n)

int f, n;				/* prefix flag and argument */

{
	if (kbdmode != STOP) {
		mlwrite(TEXT105);
		/*			"%%Macro already active" */
		return(FALSE);
	}
	mlwrite(TEXT106);
	/*		"[Start macro]" */
	kbdptr = &kbdm[0];
	kbdend = kbdptr;
	kbdmode = RECORD;
	return(TRUE);
}

/*
 * End keyboard macro. Check for the same limit conditions as the above
 * routine. Set up the variables and return to the caller.
 */

int PASCAL NEAR ctlxrp(f, n)

int f, n;				/* prefix flag and argument */

{
	if (kbdmode == STOP) {
		mlwrite(TEXT107);
		/*			"%%Macro not active" */
		return(FALSE);
	}
	if (kbdmode == RECORD) {
		mlwrite(TEXT108);
		/*			"[End macro]" */
		kbdmode = STOP;
	}
	return(TRUE);
}

/*
 * Execute a macro.
 * The command argument is the number of times to loop. Quit as soon as a
 * command gets an error. Return TRUE if all ok, else FALSE.
 */

int PASCAL NEAR ctlxe(f, n)

int f, n;				/* prefix flag and argument */

{
	if (kbdmode != STOP) {
		mlwrite(TEXT105);
		/*			"%%Macro already active" */
		return(FALSE);
	}
	if (n <= 0)
		return(TRUE);
	kbdrep = n;			/* remember how many times to execute */
	kbdmode = PLAY;		/* start us in play mode */
	kbdptr = &kbdm[0];	/*    at the beginning */
	return(TRUE);
}

/*
 * Abort.
 * Beep the beeper. Kill off any keyboard macro, etc., that is in progress.
 * Sometimes called as a routine, to do general aborting of stuff.
 */

int PASCAL NEAR ctrlg(f, n)

int f, n;				/* prefix flag and argument */

{
	TTbeep();
	kbdmode = STOP;
	mlwrite(TEXT8);
	/*		"[Aborted]" */
	return(ABORT);
}

/* tell the user that this command is illegal while we are in
   VIEW (read-only) mode				*/

int PASCAL NEAR rdonly()

{
	TTbeep();
	mlwrite(TEXT109);
	/*		"[Key illegal in VIEW mode]" */
	return(FALSE);
}

int PASCAL NEAR resterr()

{
	TTbeep();
	mlwrite(TEXT110);
	/*		"[That command is RESTRICTED]" */
	return(FALSE);
}

int PASCAL NEAR nullproc(f, n)	/* user function that does NOTHING */

int n, f;	/* yes, these are default and never used.. but MUST be here */

{
	return(TRUE);
}

int PASCAL NEAR uemeta(f, n)	/* set META prefixing pending */

int f, n;				/* prefix flag and argument */

{
	prefix |= META;
	prenum = n;
	predef = f;
	return(TRUE);
}

int PASCAL NEAR cex(f, n)	/* set ^X prefixing pending */

int f, n;				/* prefix flag and argument */

{
	prefix |= CTLX;
	prenum = n;
	predef = f;
	return(TRUE);
}

int PASCAL NEAR unarg()	/* dummy function for binding to universal-argument */
{
	return(TRUE);
}

#if	JMDEXT
int notavail()
{
	mlwrite(TEXT284);
	return (FALSE);
}
#else
int notavail()
{
	mlwrite("Not available");
	return (FALSE);
}
#endif
/*	bytecopy:	copy a string...with length restrictions
			ALWAYS null terminate
 */

char *PASCAL NEAR bytecopy(dst, src, maxlen)

char *dst;				/* destination of copied string */
CONST char *src;			/* source */
int maxlen;				/* maximum length */

{
	char *dptr;			/* ptr into dst */

	dptr = dst;
	while ((maxlen-- > 0) && *src)
		*dptr++ = *src++;
	*dptr = 0;
	return(dst);
}

/*	copystr:	make another copy of the argument

 */

char *PASCAL NEAR copystr(sp)

char *sp;				/* string to copy */

{
	char *dp;			/* copy of string */

	/* make room! */
	dp = room(strlen(sp) + 1);
	if (dp == NULL)
		return(NULL);
	strcpy(dp, sp);
	return(dp);
}

/*****		Compiler specific Library functions	****/

#if	RAMSIZE
/*	These routines will allow me to track memory usage by placing
	a layer on top of the standard system malloc() and free() calls.
	with this code defined, the environment variable, $RAM, will
	report on the number of bytes allocated via malloc.

	with RAMSHOW defined, the number is also posted on the
	end of the bottom mode line and is updated whenever it is changed.
 */

#undef	malloc
#undef	free

#if 	VMS & OPTMEM		/* these routines are faster! */
#define	malloc	VAXC$MALLOC_OPT
#define free	VAXC$FREE_OPT
#endif

char *Eallocate(nbytes)	/* allocate nbytes and track */

unsigned nbytes;	/* # of bytes to allocate */

{
	char *mp;	/* ptr returned from malloc */
	char *malloc();
	FILE *track;	/* malloc track file */

	mp = malloc(nbytes);

#if	RAMTRCK
	track = fopen("emacs.log", "a");
	fprintf(track, "Allocating %u bytes at %u:%u\n", nbytes,
			FP_SEG(mp), FP_OFF(mp));
	fclose(track);
#endif

	if (mp) {
#if	MSC
		envram += nbytes;
#else
		envram += 1;
#endif
#if	RAMSHOW
		dspram();
#endif
	}

	return(mp);
}

Erelease(mp)	/* release malloced memory and track */

char *mp;	/* chunk of RAM to release */

{
	unsigned *lp;	/* ptr to the long containing the block size */
#if	RAMTRCK
	FILE *track;	/* malloc track file */

	track = fopen("emacs.log", "a");
	fprintf(track, "Freeing %u:%u\n",
			FP_SEG(mp), FP_OFF(mp));
	fclose(track);
#endif

	if (mp) {
		/* update amount of ram currently malloced */
#if	MSC
		lp = ((unsigned *)mp) - 1;
		envram -= (long)*lp - 2;
#else
		envram -= 1;
#endif
		free(mp);
#if	RAMSHOW
		dspram();
#endif
	}
}

#if	RAMSHOW
dspram()	/* display the amount of RAM currently malloced */

{
	char mbuf[20];
	char *sp;
	FILE *track;	/* malloc track file */

	TTmove(term.t_nrow - 0, 70);
#if	COLOR
	TTforg(7);
	TTbacg(0);
#endif
	sprintf(mbuf, "[%lu]", envram);
	sp = &mbuf[0];
	while (*sp)
		TTputc(*sp++);
	TTmove(term.t_nrow, 0);
	movecursor(term.t_nrow, 0);
#if	RAMTRCK & LOGFLG
	track = fopen("emacs.log", "a");
	fprintf(track, "Total allocation at %lu bytes\n", envram);
	fprintf(track, "Stack space at %u bytes\n", stackavail());
	fclose(track);
#endif
}
#endif
#endif
