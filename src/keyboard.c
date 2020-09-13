/*===========================================================================

        KEYBOARD.C for MSDOS and OS/2

        extcode()

        This routine is common to the MSDOS and OS/2 implementations.
        It is used to resolve extended character codes from the keyboard
        into EMACS printable character specifications.

        This implementation can handle the extended AT-style keyboard
        if one is fitted.

        I don't know what happens on an XT but I suspect that it should
        work as if there were no extended keys.

                                Jon Saxton
                                24 Jan 1990

                                Unicode support by Jean-Michel Dubois

===========================================================================*/

#include	<stdio.h>
#include	"estruct.h"

#include	"eproto.h"
#include	"edef.h"
#include	"elang.h"

#if MSDOS | OS2
#if HP150 == 0

int extcode(c)

unsigned c;	/* byte following a zero extended char byte */
		/* High order normally contains 0x00 but may
		   contain 0xE0 if an AT-style keyboard is attached */
{
	/* function keys 1 through 9.  High-order will be 0x00 */
	if (c >= 0x3B && c < 0x44)
		return(SPEC | c - 0x3A + '0');

	/* function key 10 */
	if (c == 0x44)
		return(SPEC | '0');

	/* shifted function keys */
	if (c >= 0x54 && c < 0x5D)
		return(SPEC | SHFT | c - 0x53 + '0');
	if (c == 0x5D)
		return(SPEC | SHFT | '0');

	/* control function keys */
	if (c >= 0x5E && c < 0x67)
		return(SPEC | CTRL | c - 0x5D + '0');
	if (c == 0x67)
		return(SPEC | CTRL | '0');

	/* ALTed function keys */
	if (c >= 0x68 && c < 0x71)
		return(SPEC | ALTD | c - 0x67 + '0');
	if (c == 0x71)
		return(SPEC | ALTD | '0');

	/* ALTed number keys */
	/* This doesn't work for DOS or OS/2.  Using ALT in conjunction with
	   the number keys lets you enter any (decimal) character value
	   you want.  It is therefore commented out.

	   Wrongo joker... alting the top row of numbers works fine DML */
	if (c >= 0x78 && c < 0x81)
		return(ALTD | c - 0x77 + '0');
	if (c == 0x81)
		return(ALTD | '0');

	/* some others as well */
	switch (c) {

case 3:		return(0);			/* null */
case 0x0F:	return(SHFT | CTRL | 'I');	/* backtab */

case 0x10:	return(ALTD | 'Q');
case 0x11:	return(ALTD | 'W');
case 0x12:	return(ALTD | 'E');
case 0x13:	return(ALTD | 'R');
case 0x14:	return(ALTD | 'T');
case 0x15:	return(ALTD | 'Y');
case 0x16:	return(ALTD | 'U');
case 0x17:	return(ALTD | 'I');
case 0x18:	return(ALTD | 'O');
case 0x19:	return(ALTD | 'P');

case 0x1E:	return(ALTD | 'A');
case 0x1F:	return(ALTD | 'S');
case 0x20:	return(ALTD | 'D');
case 0x21:	return(ALTD | 'F');
case 0x22:	return(ALTD | 'G');
case 0x23:	return(ALTD | 'H');
case 0x24:	return(ALTD | 'J');
case 0x25:	return(ALTD | 'K');
case 0x26:	return(ALTD | 'L');

case 0x2C:	return(ALTD | 'Z');
case 0x2D:	return(ALTD | 'X');
case 0x2E:	return(ALTD | 'C');
case 0x2F:	return(ALTD | 'V');
case 0x30:	return(ALTD | 'B');
case 0x31:	return(ALTD | 'N');
case 0x32:	return(ALTD | 'M');

case 0x47:	return(SPEC | '<');	/* home */
case 0x48:	return(SPEC | 'P');	/* cursor up */
case 0x49:	return(SPEC | 'Z');	/* page up */
case 0x4B:	return(SPEC | 'B');	/* cursor left */
case 0x4C:	return(SPEC | 'L');	/* NP 5 */
case 0x4D:	return(SPEC | 'F');	/* cursor right */
case 0x4F:	return(SPEC | '>');	/* end */
case 0x50:	return(SPEC | 'N');	/* cursor down */
case 0x51:	return(SPEC | 'V');	/* page down */
case 0x52:	return(SPEC | 'C');	/* insert */
case 0x53:	return(SPEC | 'D');	/* delete */

case 0x73:	return(SPEC | CTRL | 'B');	/* control left */
case 0x74:	return(SPEC | CTRL | 'F');	/* control right */
case 0x75:	return(SPEC | CTRL | '>');	/* control end */
case 0x76:	return(SPEC | CTRL | 'V');	/* control page down */
case 0x77:	return(SPEC | CTRL | '<');	/* control home */
case 0x84:	return(SPEC | CTRL | 'Z');	/* control page up */
case 0x8D:	return(SPEC | CTRL | 'P');	/* control up */
case 0x8F:	return(SPEC | CTRL | 'L');	/* control NP5 */
case 0x91:	return(SPEC | CTRL | 'N');	/* control down */
case 0x92:	return(SPEC | CTRL | 'C');	/* control grey insert */
case 0x93:	return(SPEC | CTRL | 'D');	/* control grey delete */

case 0x82:	return(ALTD | '-');	/* alt - */
case 0x83:	return(ALTD | '=');	/* alt = */
case 0x27:	return(ALTD | ';');	/* alt ; */
case 0x28:	return(ALTD | '\'');	/* alt ' */
case 0x2B:	return(ALTD | '\\');	/* alt \ */
case 0x1A:	return(ALTD | '[');	/* alt [ */
case 0x1B:	return(ALTD | ']');	/* alt ] */

#if ATKBD | OS2

/* F11 and F12 */

case 0x85:	return(SPEC | '-');	 	/* F11 */
case 0x86:	return(SPEC | '=');		/* F12 */
case 0x87:	return(SPEC | SHFT | '-');	/* shift F11 */
case 0x88:	return(SPEC | SHFT | '=');	/* shift F12 */
case 0x89:	return(SPEC | CTRL | '-');	/* control F11 */
case 0x8A:	return(SPEC | CTRL | '=');	/* control F12 */
case 0x8B:	return(SPEC | ALTD | '-');	/* alt F11 */
case 0x8C:	return(SPEC | ALTD | '=');	/* alt F12 */

/*
   This implementation distinguishes between the cursor controls on the
   number pad and those on the grey keypad if an AT-style keyboard is
   fitted.
*/

case 0xE047:	return(SPEC | 'a');		/* grey home */
case 0xE048:	return(SPEC | 'b');		/* grey cursor up */
case 0xE049:	return(SPEC | 'c');		/* grey page up */
case 0xE04B:	return(SPEC | 'd');		/* grey cursor left */
case 0xE04C:	return(SPEC | 'e');		/* grey center key */
case 0xE04D:	return(SPEC | 'f');		/* grey cursor right */
case 0xE04F:	return(SPEC | 'g');		/* grey end */
case 0xE050:	return(SPEC | 'h');		/* grey cursor down */
case 0xE051:	return(SPEC | 'i');		/* grey page down */
case 0xE052:	return(SPEC | 'j');		/* grey insert */
case 0xE053:	return(SPEC | 'k');		/* grey delete */

case 0xE077:	return(SPEC | CTRL | 'a');	/* control grey home */
case 0xE08D:	return(SPEC | CTRL | 'b');	/* control grey up */
case 0xE084:	return(SPEC | CTRL | 'c');	/* control grey page up */
case 0xE073:	return(SPEC | CTRL | 'd');	/* control grey left */
case 0xE074:	return(SPEC | CTRL | 'f');	/* control grey right */
case 0xE075:	return(SPEC | CTRL | 'g');	/* control grey end */
case 0xE091:	return(SPEC | CTRL | 'h');	/* control grey down */
case 0xE076:	return(SPEC | CTRL | 'i');	/* control grey page down */
case 0xE092:	return(SPEC | CTRL | 'j');	/* control grey insert */
case 0xE093:	return(SPEC | CTRL | 'k');	/* control grey delete */

case 0xE097:	return(SPEC | ALTD | 'a');	/* alt grey home */
case 0xE098:	return(SPEC | ALTD | 'b');	/* alt grey cursor up */
case 0xE099:	return(SPEC | ALTD | 'c');	/* alt grey page up */
case 0xE09B:	return(SPEC | ALTD | 'd');	/* alt grey cursor left */
case 0xE09D:	return(SPEC | ALTD | 'f');	/* alt grey cursor right */
case 0xE09F:	return(SPEC | ALTD | 'g');	/* alt grey end */
case 0xE0A0:	return(SPEC | ALTD | 'h');	/* alt grey cursor down */
case 0xE0A1:	return(SPEC | ALTD | 'i');	/* alt grey page down */
case 0xE0A2:	return(SPEC | ALTD | 'j');	/* alt grey insert */
case 0xE0A3:	return(SPEC | ALTD | 'k');	/* alt grey delete */

case 0x97:	return(SPEC | ALTD | 'a');	/* alt grey home */
case 0x98:	return(SPEC | ALTD | 'b');	/* alt grey cursor up */
case 0x99:	return(SPEC | ALTD | 'c');	/* alt grey page up */
case 0x9B:	return(SPEC | ALTD | 'd');	/* alt grey cursor left */
case 0x9D:	return(SPEC | ALTD | 'f');	/* alt grey cursor right */
case 0x9F:	return(SPEC | ALTD | 'g');	/* alt grey end */
case 0xA0:	return(SPEC | ALTD | 'h');	/* alt grey cursor down */
case 0xA1:	return(SPEC | ALTD | 'i');	/* alt grey page down */
case 0xA2:	return(SPEC | ALTD | 'j');	/* alt grey insert */
case 0xA3:	return(SPEC | ALTD | 'k');	/* alt grey delete */

case 0xA6:	return(SPEC | ALTD | 'l');	/* alt grey enter */
case 0xA4:	return(SPEC | ALTD | '/');	/* alt grey / */
case 0x37:	return(SPEC | ALTD | '*');	/* alt grey * */
case 0x4A:	return(SPEC | ALTD | '-');	/* alt grey - */
case 0x4E:	return(SPEC | ALTD | '+');	/* alt grey + */

case 0x95:	return(SPEC | CTRL | '/');	/* ctrl grey / */
case 0x96:	return(SPEC | CTRL | '*');	/* ctrl grey * */
case 0x8E:	return(SPEC | CTRL | '-');	/* ctrl grey - */
case 0x90:	return(SPEC | CTRL | '+');	/* ctrl grey + */

#endif

	}
/* printf("[ALT %d] ", c); */

	return(ALTD | c);
}

#endif
#endif

#if BSD || FREEBSD || USG || AIX || AUX || SMOS || HPUX8 || HPUX9 || SUN || XENIX || (AVVION || TERMIOS) || (VMS && SMG) || MPE

#define NKEYSEQ		300		/* Number of keymap entries	*/

typedef struct keyent {			/* Key mapping entry		*/
	struct keyent *samlvl;		/* Character on same level	*/
	struct keyent *nxtlvl;		/* Character on next level	*/
	unsigned char ch;		/* Character			*/
	int code;			/* Resulting keycode		*/
} KEYENT;

/* Needed Prototype */
#if	PROTO
extern int PASCAL NEAR rec_seq(char *buf, char *bufstart, KEYENT *node);
#else
extern int PASCAL NEAR rec_seq();
#endif

/* some globals needed here */
static unsigned char keyseq[256];	/* Prefix escape sequence table	*/
static KEYENT keymap[NKEYSEQ];		/* Key map			*/
static KEYENT *nxtkey = keymap;		/* Next free key entry		*/
static BUFFER *seqbuf;			/* For the pop-up buffer	*/

/*
 * add-keymap "escape sequence" keyname
 */
#if PROTO
int PASCAL NEAR addkeymap(int f, int n)
#else
int PASCAL NEAR addkeymap( f, n)
int f;
int n;
#endif
{
	int c, ec;
	int idx, col;
	char esc_seq[NSTRING];	/* escape sequence to cook */
	char codeseq[NSTRING];	/* fn key name */

	memset(esc_seq, '\0', NSTRING);

	if (clexec == TRUE) {
		if (mlreply(NULL, esc_seq, NSTRING) != TRUE) {
			TTbeep();
			return FALSE;
		}
	}
	else {
		/* get the key sequence */
		mlwrite(": add-keymap ");
		col = strlen(": add-keymap ");
		idx = 0;
		for (;;) {
			c = tgetc();
			if ((ec = ctoec(c)) == abortc) {
				TTbeep();
				return FALSE;
			} else if (c == '\r') {
				break;
			} else if (ec == quotec) {
				c = tgetc();
			}
			esc_seq[idx++] = c;
			movecursor(term.t_nrow, col);	/* Position the cursor	*/
			col += ueechochar(c);
		}

		ostring(" ");
	}

	if (mlreply(NULL, codeseq, NSTRING) != TRUE) {	/* find the key name (e.g., S-FN#) */
		TTbeep();
		return FALSE;
	}

	ec = stock((unsigned char*) codeseq);

	ostring(codeseq);
	return (addkey((unsigned char*) esc_seq, ec));	/* Add to tree */
}

/*
 * list-keymappings
 */
#if PROTO
int PASCAL NEAR listkeymaps(int f, int n)
#else
int PASCAL NEAR listkeymaps( f, n)
int f;
int n;
#endif
{
	char outseq[NSTRING];	/* output buffer for key sequence */

	/*
	 * Get a buffer for it.
	 */
	seqbuf = bfind("Key sequence list", TRUE, BFINVS);
/*		   "Key sequence list" */

	if (seqbuf == NULL || bclear(seqbuf) == FALSE) {
		mlwrite("Cannot display key sequences list");
/*			"Cannot display key sequences list" */
		return(FALSE);
	}

	/*
	 * Build the list, pop it if all went well.
	 */
	*outseq = '"';
	if (rec_seq(outseq + 1, outseq, keymap) == TRUE) {
		wpopup(seqbuf);
		mlerase();
		return(TRUE);
	}
	return FALSE;
}

/*
 * recursively track through the tree, finding the escape sequences
 * and their function name equivalents.
 */
#if PROTO
int PASCAL NEAR rec_seq(char *buf, char *bufstart, KEYENT *node)
#else
int PASCAL NEAR rec_seq( buf, bufstart, node)
char *buf;
char *bufstart;
KEYENT *node;
#endif
{
	if (node == NULL)
		return TRUE;

	*buf = node->ch;

	if (node->nxtlvl == NULL) {
		*(buf + 1) = '"';
		*(buf + 2) = '\0';
		pad(bufstart, 20);
		cmdstr(node->code, bufstart + 20);
		if (addline(seqbuf, bufstart) != TRUE)
			return FALSE;
	}
	else if (rec_seq(buf + 1, bufstart, node->nxtlvl) != TRUE)
		return FALSE;

	return (rec_seq(buf, bufstart, node->samlvl));
}

/*
 *  addkey  -  Add key to key map
 *
 *  Adds a new escape sequence to the sequence table.
 *  I am not going to try to explain this table to you in detail.
 *  However, in short, it creates a tree which can easily be traversed
 *  to see if input is in a sequence which can be translated to a
 *  function key (arrows and find/select/do etc. are treated like
 *  function keys).  If the sequence is ambiguous or duplicated,
 *  it is silently ignored.
 *
 *  Replaces code in SMG.C, MPE.C, POSIX.C, and UNIX.C
 *  Nothing returned
 *
 *  seq - character sequence
 *  fn  - Resulting keycode
 */
#if PROTO
int PASCAL NEAR addkey(unsigned char * seq, int fn)
#else
int PASCAL NEAR addkey( seq, fn)
unsigned char * seq;
int fn;
#endif
{
	int first;
	KEYENT *cur = NULL, *nxtcur;

	/* Skip on null sequences or single character sequences. */
	if (seq == NULL || strlen((CONST char*) seq) < 2)
		return FALSE;


	/* If no keys defined, go directly to insert mode */
	first = 1;
	if (nxtkey != keymap) {

		/* Start at top of key map */
		cur = keymap;

		/* Loop until matches are exhausted */
		while (*seq) {

			/* Do we match current character */
			if (*seq == cur->ch) {

				/* Advance to next level */
				seq++;
				cur = cur->nxtlvl;
				first = 0;
			} else {

				/* Try next character on same level */
				nxtcur = cur->samlvl;

				/* Stop if no more */
				if (nxtcur)
					cur = nxtcur;
				else
					break;
			}
		}
	}

	/* Check for room in keymap */
	if (strlen((CONST char*) seq) > NKEYSEQ - (nxtkey - keymap)) {
		mlwrite("No more room for key entries.");
		return FALSE;
	}

	/* If first character in sequence is inserted, add to prefix table */
	if (first)
		keyseq[(unsigned char) *seq] = 1;

	/* If characters are left over, insert them into list */
	for (first = 1; *seq; first = 0) {

		/* Make new entry */
		nxtkey->ch = *seq++;
		nxtkey->code = fn;

		/* If root, nothing to do */
		if (nxtkey != keymap) {

			/* Set first to samlvl, others to nxtlvl */
			if (first)
				cur->samlvl = nxtkey;
			else
				cur->nxtlvl = nxtkey;
		}

		/* Advance to next key */
		cur = nxtkey++;
	}
	return TRUE;
}

#if	!CURSES

/*
 * Cook input characters, using the key sequences stored by addkey().
 *
 * To use, we need a grabwait(), grabnowait(), qin() and qrep() function.
 */
#define TIMEOUT	255
VOID cook()
{
#if	UTF8
	register unsigned int ch;
#else
	register unsigned char ch;
#endif
	KEYENT *cur;

	qin(ch = grabwait());	/* Get first character untimed */

	/*
	 * Skip if the key isn't a special leading escape sequence.
	 */
	if (keyseq[ch] == 0) {
		/*
		 * But if it is a '\0', make it a (0/1/32).
		 */
		if (ch == 0) {
			qin(CTRL >> SHIFTPFX);	/* control */
			qin(32);		/* space */
		}
		return;
	}

	/* Start at root of keymap */
	cur = keymap;

	/* Loop until keymap exhausts */
	while (cur) {

		/* Did we find a matching character */
		if (cur->ch == ch) {

			/* Is this the end */
			if (cur->nxtlvl == NULL) {
				/* Replace all characters with a new sequence */
				qrep(cur->code);
				return;
			} else {
				/* Advance to next level */
				cur = cur->nxtlvl;

				/* Get next character, timed */
				ch = grabnowait();
				if (ch == TIMEOUT)
					return;

				/* Queue character */
				qin(ch);
			}
		} else
			/* Try next character on same level */
			cur = cur->samlvl;
	}
}

#endif

#endif
