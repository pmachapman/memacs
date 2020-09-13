#include "estruct.h"

#if THEOS
#include "elang.h"
#include "eproto.h"
#include "edef.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <conio.h>
#include <signal.h>
#include <sys/stat.h>
#include <fdb.h>
#include <peek.h>
#include <scr.h>
#include <sc.h>
#include <ucb.h>
#include <wmapi.h>

char dirpath[NFILEN];

/*
 * Dispatch table.  All the hard fields just point into the terminal I/O code.
 */

int crtopen(),crtclose(),crtkopen(),crtkclose(),crtmove(),
	     crteol(),crteop(),crtclr(),crtbeep(),crtrev(),crtfcol(),crtbcol();
int crtcres();

#define NROW	24			/* Screen size. 		*/
#define NCOL	80			/* Edit if you want to. 	*/
#define	MARGIN	8			/* size of minimim margin and	*/
#define	SCRSIZ	64			/* scroll size for extended lines */
#define	NPAUSE	500			/* # times thru update to pause */

/* The Mouse driver only works with typeahead defined */
#if	MOUSE
#undef	TYPEAH
#define	TYPEAH	1
static int mexist;	/* is the mouse driver installed? */
static short msig;	/* Mouse event */
static char mbuf[6];
static short mbpos = 0;
#endif

TERM term = {
	NROW-1,
	NROW-1,
	NCOL,
	NCOL,
	0, 0,
	MARGIN,
	SCRSIZ,
	NPAUSE,
	&crtopen,
	&crtclose,
	&crtkopen,
	&crtkclose,
	&ttgetc,
	&ttputc,
	&ttflush,
	&crtmove,
	&crteol,
	&crteop,
	&crtclr,
	&crtbeep,
	&crtrev,
	&crtcres
#if	COLOR
	,&crtfcol,
	&crtbcol
#endif
};

#if	COLOR
/*
 * THEOS colors map
 */
int csinfg,csinbg,csirfg,csirbg;
int colormap[NCOLORS] = { 0,4,2,6,1,5,3,7,0+8,4+8,2+8,6+8,1+8,5+8,3+8,7+8 };
int cfcolor;
int cbcolor;
#endif

static int read_raw;

/* used by terminals supporting any national characters or symbols */

#define CTRL_KEYS	29
#define INTER_KEYS	46
#define ISO_KEYS	11

typedef struct TERMINAL_FD {
	char type_code;
	char terminal_name[25];
	near char *attrib[36];
	near char *ctrl_key[CTRL_KEYS];
	near char *graph[26];
	near char *slave[2];
	near char *inter[INTER_KEYS];
	near char *inter_key[INTER_KEYS];
	near char *iso[7];
	near char *iso_key[ISO_KEYS];
} CLASS;

#define	TRM (unsigned short *) ((CLASS *) 0)

#define CONI	26
#define CONO	27

#define	BEL	7
#define RVON	14	/* SO  0x0e reversed video attribute on */
#define RVOFF	15	/* SI  0x0f reversed video attribute off */
#undef	EOL
#define EOS	24	/* CAN 0x18 erase from cursor to end of screen */
#define EOL	23	/* ETB 0x17 erase from cursor to end of line */
#define CLEAR	12
#define PON	4	/* EOT 0x04 protect attribute on (half intensity) */
#define POFF	5	/* ENQ 0x05 protect attribute off */
#define KON     25      /* EM  0x19 cursor on */
#define BON     29      /* GS  0x1d blink attribute on */
#define BOFF    30      /* RS  0x1e blink attribute off */

static struct ucb *ucb;
static unsigned short transeg;
static int next;
static char keybuf[20];
static char term_type;
static int baud;
static char iso[ISO_KEYS] = {
	0x23,0x40,0x5b,0x5c,0x5d,0x5e,0x60,0x7b,0x7c,0x7d,0x7e
};

static short ctl[] = {
	0x50|0x100,	/* down_key */
	0x48|0x100,	/* up_key */
	0x4b|0x100,	/* left_key */
	0x4d|0x100,	/* right_key */
	0x1b,		/* esc_key may be: ESC = ESC ESC ?? */
	0x47|0x100,	/* top_key */
	0x4f|0x100,	/* bottom_key */
	0x53|0x100,	/* delchar_key */
	0x51|0x100,	/* page_fwd_key */
	0x49|0x100,	/* page_bck_key */
	0x3c|0x100,	/* word_right_key */
	0x55|0x100,	/* word_left_key */
	0x3b|0x100,	/* sch_fwd_key */
	0x54|0x100,	/* sch_bck_key */
	0x3d|0x100,	/* again key */
	0x44|0x100,	/* file_key */
	0x3f|0x100,	/* erase_key */
	0x42|0x100,	/* beg_line_key */
	0x5b|0x100,	/* end_line_key */
	0x5a|0x100,	/* case_key */
	0x5d|0x100,	/* save_key */
	0x43|0x100,	/* quit_key */
	0x3e|0x100,	/* find_key */
	0x52|0x100,	/* replace_key */
	0x7f,		/* del_left_key */
	0x41|0x100,	/* transpose_key */
	0x40|0x100,	/* ins_line_key */
	0x59|0x100,	/* del_line_key */
	0x5c|0x100	/* help_key */
};

int crtrev(status)	/* set the reverse video state */
int status;	/* TRUE = reverse video,FALSE = normal video */
{
	putch(status ? RVON : RVOFF);
	return 0;
}

int crtcres()
{
	return TRUE;
}

int spal(char *pal)
{
	return 0;
}

#if	COLOR
int crtfcol(color)
{
	if (cfcolor == color)
		return 0;

	crtcolor(colormap[cfcolor = color] & 7, colormap[cbcolor] & 7,
		csirfg, csirbg);
	putch(colormap[color] < 7 ? PON : POFF);
	if (ispcterm() && _osmajor != 3)
		putch(colormap[cbcolor] > 7 ? BON : BOFF);
	return 0;
}

int crtbcol(color)
{
	struct bytecntl bc;

	if (cbcolor == color)
		return 0;

	crtcolor(colormap[cfcolor] & 7, colormap[cbcolor = color] & 7,
		csirfg, csirbg);
	putch(colormap[cfcolor] < 7 ? PON : POFF);
	if (ispcterm() && _osmajor != 3)
		putch(colormap[cbcolor] > 7 ? BON : BOFF);
}

int getecolor(co)
{
	register int i;

	for (i = 0; i < NCOLORS; i++)
		if (co == colormap[i])
			return i;
	return 0;
}
#endif

void quick_exit()
{
	quit(TRUE, 0);
}

int crtkopen()
{
	signal(SIGQUIT,quick_exit);
	return 0;
}

int crtkclose()
{
	signal(SIGQUIT,SIG_DFL);
	return 0;
}

/*
 * Look in the class for the received sequence.
 */

void scant(unsigned short *ofs, short tblsz, short *exact, short *begin)
{
	register char *sqofs;
	register short i, lseq;

	for (i = 0; i < tblsz; i++, ofs++) {
		if ((sqofs = (char *) peekw(ofs, transeg)) != NULL) {
			lseq = peek(sqofs, transeg);
			if (keybuf[0] == lseq) {
			/* if exists */
				if (*exact < 0
				 && ! farcmp(getds(), keybuf, transeg, sqofs, keybuf[0]+1)) {
					*exact = i;
					if (*begin >= 0)
						break;
				}
			} else if (keybuf[0] < lseq) {
				/* if is beginning of a class sequence */
				if (*begin < 0
				 && ! farcmp(getds(), &keybuf[1], transeg, sqofs+1, keybuf[0])) {
					*begin = i;
					if (*exact >= 0)
						break;
				}
			}
		}
	}
}

/*
 * Map extended chars to function code.
 */

int extcode(c)
{
	/* function keys 1 through 9.  High-order will be 0x00 */
	if (c >= 0xC0 && c < 0xC9)
		return(SPEC | c - 0xBF + '0');
	/* function key 10 */
	if (c == 0xC9)
		return(SPEC | '0');

	/* shifted function keys */
	if (c >= 0xD0 && c < 0xD9)
		return(SPEC | SHFT | c - 0xCF + '0');
	if (c == 0xD9)
		return(SPEC | SHFT | '0');

	/* control function keys */
	if (c >= 0xE0 && c < 0xE8)
		return(SPEC | CTRL | c - 0xDF + '0');
	if (c == 0xE9)
		return(SPEC | CTRL | '0');

	/* ALTed function keys */
	if (c >= 0xF0 && c < 0xF8)
		return(SPEC | ALTD | c - 0xEF + '0');
	if (c == 0xF9)
		return(SPEC | ALTD | '0');

	switch (c) {
	case 0x6E:	return(SHFT | CTRL | 'I');	/* backtab */
	case 0x47:	return(SPEC | '<');	/* home */
	case 0x48:	return(SPEC | 'P');	/* cursor up */
	case 0x49:	return(SPEC | 'Z');	/* page up */
	case 0x4B:	return(SPEC | 'B');	/* cursor left */
	case 0x4D:	return(SPEC | 'F');	/* cursor right */
	case 0x4F:	return(SPEC | '>');	/* end */
	case 0x50:	return(SPEC | 'N');	/* cursor down */
	case 0x51:	return(SPEC | 'V');	/* page down */
	case 0x52:	return(SPEC | 'C');	/* insert */
	case 0x53:	return(SPEC | 'D');	/* delete */

	case 0x0b:	return(SPEC | CTRL | 'B');	/* control left */
	case 0x0d:	return(SPEC | CTRL | 'F');	/* control right */
	case 0x0f:	return(SPEC | CTRL | '>');	/* control end */
	case 0x11:	return(SPEC | CTRL | 'V');	/* control page down */
	case 0x07:	return(SPEC | CTRL | '<');	/* control home */
	case 0x09:	return(SPEC | CTRL | 'Z');	/* control page up */
	case 0x08:	return(SPEC | CTRL | 'P');	/* control up */
	case 0x10:	return(SPEC | CTRL | 'N');	/* control down */
	case 0x12:	return(SPEC | CTRL | 'C');	/* control grey insert */
	case 0x13:	return(SPEC | CTRL | 'D');	/* control grey delete */

	/* F11 and F12 */

	case 0xCA:	return(SPEC | '-');	 	/* F11 */
	case 0xCB:	return(SPEC | '=');		/* F12 */
	case 0xDA:	return(SPEC | SHFT | '-');	/* shift F11 */
	case 0xDB:	return(SPEC | SHFT | '=');	/* shift F12 */
	case 0xEA:	return(SPEC | CTRL | '-');	/* control F11 */
	case 0xEB:	return(SPEC | CTRL | '=');	/* control F12 */
	case 0xFA:	return(SPEC | ALTD | '-');	/* alt F11 */
	case 0xFB:	return(SPEC | ALTD | '=');	/* alt F12 */
	}
}

#if	MOUSE

int checkmouse()
{
	short mbut;	/* State of mouse buttons */
	int mcol;	/* x position of mouse */
	int mrow;	/* y position of mouse */
	long mtime;
	static short obut;	/* State of mouse buttons */

	wMouseRead(&mbut, &mcol, &mrow, &mtime);

	if (mbut == 0) {
		mbuf[4] = MOUS>>8;
		mbuf[3] = mcol;
		mbuf[2] = mrow;
		mbuf[1] = (obut & wMOUSE_LEFT_DOWN) ? 'b' : 'd';
		mbpos = 4;
		obut = mbut;
		return TRUE;
	} else if (mbut & (wMOUSE_LEFT_DRAG|wMOUSE_RIGHT_DRAG)) {
		/* generate a mouse movement */
		if (((mouse_move == 1) && (mmove_flag == TRUE)) ||
		    (mouse_move == 2)) {
			mbuf[4] = MOUS>>8;
			mbuf[3] = mcol;
			mbuf[2] = mrow;
			mbuf[1] = 'm';
			mbpos = 4;
			obut = mbut;
			return TRUE;
		}
	} else if (mbut & (wMOUSE_LEFT_DOWN|wMOUSE_RIGHT_DOWN)) {
		mbuf[4] = MOUS>>8;
		mbuf[3] = mcol;
		mbuf[2] = mrow;
		mbuf[1] = (mbut & wMOUSE_LEFT_DOWN) ? 'a' : 'c';
		mbpos = 4;
		obut = mbut;
		return TRUE;
	}
	obut = mbut;
	return FALSE;
}
#endif

/*
 * Read raw from terminal.
 */

int getkbd()
{
	int c;
	struct bytecntl bc;

	if (read_raw) {
		bc.c_cmd = READ_RAW;
		return _ioctl(stdin, &bc);
	}

	pokewucb(0,&ucb->transeg);
	c = getch();
	pokewucb(transeg,&ucb->transeg);

	return c;
}

int kbdget()
{
#if	MOUSE
	int c;

	if (mbpos)
		return mbuf[mbpos--];

	for (;;) {
		c = getkbd();

		if (mexist && msig) {
			msig = 0;

			if (checkmouse())
				return 0;
		} else
			return c;
	}
#else
	return getkbd();
#endif
}

/*
 * Read keyboard on Ascii terminal.
 */

int asgetc()
{
	short c;
	short n;
	short ectrl, einter, eiso;	/* exact string */
	short begin;			/* beginning of string */

	if (! keybuf[0]) {
		next = 0;
next:
		ectrl = einter = eiso = begin = -1;
		/* read raw */
		keybuf[1+keybuf[0]++] = kbdget();
		/* look for extended key */
		scant(TRM->ctrl_key, CTRL_KEYS, &ectrl, &begin);
		/* if international class and sequence is not a ctrl key */
		if (term_type == '\xfd' && ectrl < 0 && begin < 0) {
			/* look for international character */
			scant(TRM->inter_key, INTER_KEYS, &einter, &begin);
			/* if not, look for iso key */
			if (einter < 0 && begin < 0)
				scant(TRM->iso_key, ISO_KEYS, &eiso, &begin);
		}

		/* is received sequence the beginning of a class sequence ? */
		if (begin >= 0) {
			/* yes, and main term keyboard grab next character */
			if (baud == 0 || keybuf[0] > 1)
				goto next;
			/* wait a little on a serial terminal */
			_sleep(10L);
			/* if a character exists, grab it */
			if (conrdy())
				goto next;
		}
		/* map received sequence ... */
		if (ectrl >= 0) {
			/* to control key */
			if (ctl[ectrl] & 0x100) {
				/* sequence 0xff xx */
				keybuf[0] = 0;
				return ctl[ectrl] & 0xff;
			}
			keybuf[0] = 0;
			return ctl[ectrl];
		} else if (einter >= 0) {
			/* to international character */
			keybuf[0] = 0;
			return (einter + 192);
		} else if (eiso >= 0) {
			/* to iso character */
			keybuf[0] = 0;
			return (iso[eiso]);
		}
	}

	keybuf[0]--;
	return (keybuf[++next]);
}

/*
 * Read keyboard on PcTerm terminal.
 */

int pcgetc()
{
	int c;

	if ((c = kbdget()) == 0xff)
		return extcode(kbdget());
	else if (c > 0x80)
		return c;

	return c;
}

/*
 * Read keyboard.
 */

int ttgetc()
{
	return (ispcterm()) ? pcgetc() : asgetc();
}

int crtputc(c)
{
	putch(c);
	return 0;
}

int crtmove(row,col)
{
	at(col,row);
	return 0;
}

int crteol()
{
#if	COLOR
	crtcolor(colormap[cfcolor] & 7, colormap[cbcolor] & 7, csirfg, csirbg);
	putch(colormap[cfcolor] < 7 ? PON : POFF);
	if (ispcterm() && _osmajor != 3)
		putch(colormap[cbcolor] > 7 ? BON : BOFF);
#endif
	putch(EOL);
	return 0;
}

int crteop()
{
#if	COLOR
	crtcolor(colormap[gfcolor] & 7,colormap[gbcolor] & 7,csirfg,csirbg);
	putch(colormap[gfcolor] < 7 ? PON : POFF);
	if (ispcterm() && _osmajor != 3)
		putch(colormap[gbcolor] > 7 ? BON : BOFF);
#endif
	putch(EOS);
	return 0;
}

int crtclr()
{
#if	COLOR
	crtcolor(csinfg, csinbg, csirfg, csirbg);
#endif
	putch(CLEAR);
}

int crtbeep()
{
	putch(BEL);
	return 0;
}

int crtopen()
{
	/* No echo, ... */
	conmask("nm0");
	/* get screen size */
	term.t_mrow = term.t_nrow = getpl(CONO)-1;
	term.t_mcol = term.t_ncol = getll(CONO);
	/* open keyboard */
	crtkopen();
	keybuf[0] = '\0';
	/* get and save some terminal parameters */
	ucb = getucb(CONI);
	transeg = peekwnuc(&ucb->transeg);
	term_type = peek(&((CLASS *)0)->type_code,transeg);
	baud = peeknuc(&ucb->baud);
	read_raw = _osmajor != 3;
	ttopen();
#if	COLOR
	/* initialize colors */
	csinfg = peekscr(&SCR->csi_color[0]) & 0x0f;
	csinbg = peekscr(&SCR->csi_color[1]) & 0x0f;
	csirfg = peekscr(&SCR->csi_color[2]) & 0x0f;
	csirbg = peekscr(&SCR->csi_color[3]) & 0x0f;
	gfcolor = cfcolor = getecolor(csinfg);
	gbcolor = cbcolor = getecolor(csinbg);
#endif
	return 0;
}

int crtclose()
{
	crtkclose();
	ttflush();
	ttclose();
	return 0;
}

/** Open terminal device **/
int ttopen()
{
	/* on all screens we are not sure of the initial position
	   of the cursor					*/
	ttrow = 999;
	ttcol = 999;
	cfcolor = cbcolor = -1;
	wIntensity(-1);
	strcpy(os, "THEOS");
	putch(KON);
#if	MOUSE
	if (HasMouse()) {
		mexist = TRUE;
		wMouseEnable(&msig,
			wMOUSE_LEFT_DOWN | wMOUSE_LEFT_DRAG |
			wMOUSE_LEFT_DOWN | wMOUSE_LEFT_DRAG |
			wMOUSE_RIGHT_DOWN | wMOUSE_RIGHT_DRAG |
			wMOUSE_RIGHT_DOWN | wMOUSE_RIGHT_DRAG
			);
		msig = 0;
		mbpos = 0;
	} else
		mexist = FALSE;
#endif
	return 0;
}

/** Close terminal device **/
int ttclose()
{
	struct bytecntl bc;

#if	MOUSE
	wMouseDisable();
#endif
	wIntensity(0);
#if	COLOR
	crtcolor(csinfg, csinbg, csirfg, csirbg);
#endif
	putch(CLEAR);
	putch(KON);
	return 0;
}

/*
 * Write a character to the display. On VMS, terminal output is buffered, and
 * we just put the characters in the big array, after checking for overflow.
 * On MS-DOS use the very very raw console output routine.
 */
int ttputc(int c)
{
	putch(c);
	return 0;
}

/*
 * Flush terminal buffer. Does real work where the terminal output is buffered
 * up. A no-operation on systems where byte at a time terminal I/O is done.
 */
int ttflush()
{
	return 0;
}

#if	TYPEAH
/* typahead:	Check to see if any characters are already in the
		keyboard buffer
*/

int typahead()
{
	return conrdy();
}
#endif	/* TYPEAH */

/** Get time of day **/
char * timeset()
{
	long int buf; /* Should be time_t */
	char * sp, * cp;

	char * ctime();

	/* Get system time */
	time(&buf);

	/* Pass system time to converter */
	sp = ctime(&buf);

	/* Eat newline character */
	for (cp = sp; *cp; cp++)
		if (*cp == '\n') {
			*cp = '\0';
			break;
		}
	return(sp);
}

/** Callout to system to perform command **/
int callout(cmd)
char * cmd;				/* Command to execute		*/
{
	int status;

	/* Close down */
	crtmove(term.t_nrow, 0);
	ttflush();
	crtkclose();
	ttclose();

	/* Do command */
	status = system(cmd) == 0;

	/* Restart system */
        sgarbf = TRUE;
	crtkopen();
	if (ttopen()) {
		puts("** Error reopening terminal device **");
		exit(1);
	}

	/* Success */
        return(status);
}

/** Create subshell **/
int spawncli(f, n)
int f;					/* Flags			*/
int n;					/* Argument count		*/
{
	char * sh;

	/* Don't allow this command if restricted */
	if (restflag)
		return(resterr());

	/* Get shell path */
	sh = getenv("SHELL");
	if (!sh)
		sh = "SHELL";

	/* Do shell */
	return(callout(sh));
}

/** Spawn a command **/
int spawn(f, n)
int f;					/* Flags			*/
int n;					/* Argument count		*/
{
	char line[NLINE];
	int s;

	/* Don't allow this command if restricted */
	if (restflag)
		return(resterr());

	/* Get command line */
	s = (mlreply("!", line, NLINE) == TRUE);
	if (!s)
		return(s);

	/* Perform the command */
	s = callout(line);

	/* if we are interactive, pause here */
	if (clexec == FALSE) {
	        mlwrite("[End]");
		ttflush();
		ttgetc();
        }
        return(s);
}

/** Execute program **/
int execprg(f, n)
int f;					/* Flags			*/
int n;					/* Argument count		*/
{
	/* Same as spawn */
	return(spawn(f, n));
}

/** Pipe output of program to buffer **/
int pipecmd(f, n)
int f;					/* Flags			*/
int n;					/* Argument count		*/
{
	char line[NLINE];
	int s;
	BUFFER * bp;
	EWINDOW * wp;
	static char bname[] = "command";
	char filnam[NSTRING];

	/* Don't allow this command if restricted */
	if (restflag)
		return(resterr());

	/* Get pipe-in command */
	s = (mlreply("@", line, NLINE) == TRUE);
	if (!s)
		return(s);

	tmpnam(filnam);

	/* Get rid of the command output buffer if it exists */
	if ((bp=bfind(bname, FALSE, 0)) != FALSE) {
		/* Try to make sure we are off screen */
		wp = wheadp;
		while (wp) {
			if (wp->w_bufp == bp) {
				onlywind(FALSE, 1);
				break;
			}
			wp = wp->w_wndp;
		}
		if (!zotbuf(bp))
			return(0);
	}

	/* Add output specification */
	strcat(line, ">");
	strcat(line, filnam);

	/* Do command */
	s = callout(line);
	if (!s)
		return(s);

	/* Split the current window to make room for the command output */
	if (!splitwind(FALSE, 1))
		return(0);

	/* ...and read the stuff in */
	if (!getfile(filnam, FALSE))
		return(0);

	/* Make this window in VIEW mode, update all mode lines */
	curwp->w_bufp->b_mode |= MDVIEW;
	wp = wheadp;
	while (wp) {
		wp->w_flag |= WFMODE;
		wp = wp->w_wndp;
	}

	/* ...and get rid of the temporary file */
	unlink(filnam);
	return(1);
}

/** Filter buffer through command **/
#undef filter
int mefilter(f, n)
int f;					/* Flags			*/
int n;					/* Argument count		*/
{
	char line[NLINE], tmpname[NFILEN];
	int s;
	BUFFER * bp;
	static char bname1[] = "fltinp";
	char filnam1[NSTRING];
	char filnam2[NSTRING];

	tmpnam(filnam1);
	tmpnam(filnam2);

	/* Don't allow this command if restricted */
	if (restflag)
		return(resterr());

	/* Don't allow filtering of VIEW mode buffer */
	if (curbp->b_mode & MDVIEW)
		return(rdonly());

	/* Get the filter name and its args */
	s = (mlreply("#", line, NLINE) == TRUE);
	if (!s)
		return(s);

	/* Setup the proper file names */
	bp = curbp;
	strcpy(tmpname, bp->b_fname);	/* Save the original name */
	strcpy(bp->b_fname, bname1);	/* Set it to our new one */

	/* Write it out, checking for errors */
	if (!writeout(filnam1, "w")) {
		mlwrite("[Cannot write filter file]");
		strcpy(bp->b_fname, tmpname);
		return(0);
	}

	/* Setup input and output */
	strcat(line," <");
	strcat(line,filnam1);
	strcat(line," >");
	strcat(line,filnam2);

	/* Perform command */
	s = callout(line);

	/* If successful, read in file */
	if (s) {
		s = readin(filnam2, FALSE);
		if (s)
			/* Mark buffer as changed */
			bp->b_flag |= BFCHG;
	}


	/* Reset file name */
	strcpy(bp->b_fname, tmpname);

	/* and get rid of the temporary file */
	unlink(filnam1);
	unlink(filnam2);

	/* Show status */
	if (!s)
		mlwrite("[Execution failed]");
	return(s);
}

/** Get first filename from pattern **/
char *getffile(fspec)
char *fspec;				/* Filename specification	*/
{
	char* p, *q;
	char path[NFILEN];

	/* First parse the file path off the file spec */
	strcpy(path, fspec);
	p = strrchr(path, '/');

	if (p == NULL) {
		p = path;
		dirpath[0] = '\0';
	} else {
		char c = p[1];
		p[1] = '\0';
		strcpy(dirpath, path);
		p[1] = c;
	}

	/* Check for an extension */
	p = strchr(p, '.');

	if (p == NULL)
		strcat(path, "*.*");
	else {
		strcat(path, "*");
	}

	dirclose();

	if (diropen(path))
		return NULL;

	return(getnfile());
}

/** Get next filename from pattern **/
char *getnfile()
{
	int index;
	char* dp;
	char* p;
	struct stat fstat;
	char rbuf[NFILEN];
	char pbuf[NFILEN];
	char disk[3];

	/* ...and call for the next file */
	do {
		dp = dirread();
		if (!dp)
			return(NULL);

	} while (stat(dp, &fstat) ||
		((fstat.st_mode & S_IFMT) & (S_IFREG | S_IFDIR | S_IFLIB)) == 0);

	/* Return the next file name! */
	strcpy(rbuf, dp);
	p = strrchr(dp, '/');
	if (p) {
		strcpy(rbuf, dirpath);
		strcat(rbuf, p + 1);
	}

	/* save disk name */
	p = strchr(rbuf, ':');
	strcpy(disk, p);

	/* if this entry is a directory name, say so */
	if ((fstat.st_mode & S_IFMT) == S_IFDIR)
		strcat(strcpy(p, DIRSEPSTR), disk);
	/* if this entry is a library name, say so */
	else if ((fstat.st_mode & S_IFMT) == S_IFLIB)
		strcat(strcpy(p, "."), disk);

	strlwr(rbuf);
	return(rbuf);
}

/* Retourne un pointeur sur le nom du disque */

char *drive(char *s)
{
	if ((s = strchr(s, ':')) != NULL) {	/* seek to fd */
		if (s[1] && s[1] != '*')
			s[2] = '\0';
		else {			/* if fd == *, set fd to search */
//			s[0] = '\0';
//			s = NULL;
			s[2] = *getenv("SEARCH");
		}
	}
	return s;
}

/* Ajoute le nom de la bibliotheque courante si le nom de fichier ne
   comporte pas de point.
*/

int addlname(fname)
char *fname;
{
	char buf[256];
	struct fdb *fdb;
	short lub;
	char lname[40];
	register char *dl, *df;
	char *getlib(), *strchr();

	df = drive(fname);

	/* si fn est un membre et qu'il existe une biblioth�que par d�faut,
	   qu'il n'y a pas contradiction entre les noms de disque du fichier
	   et de la biblioth�que, concat�ner le nom de la biblioth�que, celui
	   du fichier et �ventuellement celui du disque */
	if (! strchr(fname, '.')) {
		strcpy(lname, getlib());
		if (*lname) {
			if (!(dl = drive(lname)) || df == NULL || dl[1]==df[1]) {
				if (df == NULL && dl != NULL)
					strcat(fname, dl);
				if (dl != NULL)
					*dl = '\0';
				strcat(lname, ".");
				strcat(lname, fname);
				strcpy(fname, lname);
			}
		} else if (df = strchr(fname, ':')) {
			strcpy(lname, df);
			strcpy(df, ".");
			strcat(fname, lname);
		} else
			strcat(fname, ".");
	}

	if (! strchr(fname, ':'))
		strcat(fname, ":*");
	df = strchr(fname, ':') + 1;
	strlwr(fname);
	if (fdb = _locate(fname, buf, &lub)) {
		*df = lub + 'a';
		if (! (fdb->filestat & _FDB_STAT_STREAM)) {
			mlwrite(TEXT297, fname);
			return (FALSE);
		}
	} else {
		strcpy(lname, fname);
		if ((dl = strchr(lname, '.')) && (dl = strchr(dl+1,'.'))) {
			strcpy(dl, df - 1);
			fdb = _locate(lname, buf, &lub);
			*df = lub + 'a';
			if (! fdb || fdb->filestat != _FDB_STAT_LIBRARY) {
				mlwrite(TEXT299, lname);
				return (FALSE);
			}
		} else if (*df == '*') {
			char cwd[256];
			getcwd(cwd, 256);
			dl = strchr(cwd, ':');
			*df = dl ? tolower(dl[1]) : 's';
		}
	}
	return (TRUE);
}

#if BACKUP && THEOS40

/*
 * Retourne l'octet d'etat d'un fichier ou 0 s'il n'existe pas
 */

int filestat(s)
char *s;
{
	struct fdb *fdb;
	char buf[256];
	short lub;

	if (fdb = (struct fdb *) _locate(s,buf,&lub))
		return (fdb->filestat);
	else
		return 0;
}

/* Renomme le fichier en backup */

int renback(fn, lname)
char *fn, *lname;
{
	register int fs;

	if (fs = filestat(fn)) {
		if (fs & _FDB_STAT_STREAM)
			rename(fn, lname);
		else {
			mlwrite(TEXT297, fn);
			return (FALSE);
		}
	}
	return(TRUE);
}

int unlback(lname, bname, dname)
char *lname,*bname,*dname;
{
	register char *p;

	if (filestat(lname) & _FDB_STAT_LIBRARY) {
		if ((p = strchr(lname, ':')) == NULL)
			p = strend(lname);
		*p = '.';
		strcpy(p+1, bname);
		unlink(strcat(lname, dname));
		return(TRUE);
	}
	return(FALSE);
}

void backname(ft, fn)
char *ft;
char *fn;
{
	char	lname[NFILEN];	/* nom complet fn.ft.mn:d */
	char	dname[9];	/* :d */
	register char *p;
	register char *bname;	/* pointeur sur fn ou mn */
	register int fs;	/* organisation du fichier */
	extern char *cuserid();

	/* extract disk name and remove it */
	if (p = strchr(strcpy(ft, fn), ':')) {
		strcpy(dname, p);
		*p = '\0';
	} else
		*dname = '\0';

	/* look for first dot in name */
	p = strchr(ft, '.');
	/* look for last one */
	bname = strrchr(ft, '.');

	if (p == NULL) {
		/* no dot, add BACKUP and disk name */
		strcat(strcat(ft, ".BACKUP"), dname);
	} else if (p == bname) {
		/* if only one dot, cut extension, add BACKUP and disk name */
		strcat(strcpy(p + 1, "BACKUP"), dname);
	} else {
		strcpy(lname, ft);
		strcat(strcat(strchr(lname, '.'), ".BACKLIB"), dname);
		if (filestat(lname) & _FDB_STAT_LIBRARY) {
			....
		}
	} else
		bname = ft;

	cuserid(lname);
	strcat(strcat(lname,".BACKLIB"),dname);

	if (unlback(lname,bname,dname) == FALSE) {
		strcat(strcat(strcpy(lname,bname),".BACKUP"),dname);
		if (filestat(lname) & _FDB_STAT_LIBRARY)
			return(FALSE);
		unlink(lname);
	}
}

int fileback(fn)
char *fn;
{
	char	lname[NFILEN];	/* nom complet fn.ft.mn:d */
	char	fback[NFILEN];	/* copie de travail */
	char	dname[9];	/* :d */
	register char *p;
	register char *bname;	/* pointeur sur fn ou mn */
	register int fs;	/* organisation du fichier */
	extern char *cuserid();

	if ((curbp->b_mode & MDNOBAK) == 0) {
		if (p = strchr(strcpy(fback,fn),':')) {
			strcpy(dname,p);
			*p = '\0';
		} else
			dname[0] = '\0';

		p = strchr(fback,'.');

		if (p != NULL && p == (bname = strrchr(fback,'.'))) {
			bname = fback;
			*p++ = '\0';
		} else if (bname != (char *) 0) {
			strcpy(lname,fback);
			strcat(strcat(strchr(lname,'.'),".BACKLIB"),dname);
			bname++;
			if (unlback(lname,bname,dname) != FALSE)
				return renback(fn,lname);
		} else
			bname = fback;

		cuserid(lname);
		strcat(strcat(lname,".BACKLIB"),dname);

		if (unlback(lname,bname,dname) == FALSE) {
			strcat(strcat(strcpy(lname,bname),".BACKUP"),dname);
			if (filestat(lname) & _FDB_STAT_LIBRARY)
				return(FALSE);
			unlink(lname);
		}

		return renback(fn,lname);
	}
	return (TRUE);
}
#endif	/* BACKUP */
#endif
