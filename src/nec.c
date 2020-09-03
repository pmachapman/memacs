/*	NEC:	NEC PC-9801vm driver for
		MicroEMACS 4.00
		(C)Copyright 1995 by Daniel M. Lawrence
*/

#define	termdef	1			/* don't define term external */

#include        <stdio.h>
#include	"estruct.h"
#include	"eproto.h"
#include        "edef.h"
#include	"elang.h"

#if     NEC
#include	<dos.h>

#if	PROTO
int fnclabel(int f, int n);
int readparam( int *v);
void dobbnmouse(void);
void docsi( int oh);
void ttputs(char *string);
#else
int fnclabel();
int readparam();
void dobbnmouse();
void docsi();
void ttputs();
#endif

#define NROW    24                      /* Screen size.                 */
#define NCOL    80                      /* Edit if you want to.         */
#define	NPAUSE	100			/* # times thru update to pause */
#define	MARGIN	8			/* size of minimim margin and	*/
#define	SCRSIZ	64			/* scroll size for extended lines */
#define BEL     0x07                    /* BEL character.               */
#define ESC     0x1B                    /* ESC character.               */

/* Forward references.          */
extern int necmove();
extern int neceeol();
extern int neceeop();
extern int necbeep();
extern int necopen();
extern int necrev();
extern int necclose();
extern int neckopen();
extern int neckclose();
extern int neccres();
extern int necparm();
extern int necgetc();
#if	INSDEL
extern int necins();
extern int necdel();
#endif

#if	COLOR
extern int necfcol();
extern int necbcol();

static int cfcolor = -1;	/* current forground color */
static int cbcolor = -1;	/* current background color */
#endif

#if	FLABEL

#define	FSIZE	392		/* save of function key save buffer */

static unsigned char oldkeys[FSIZE];	/* original key save buffer */
static unsigned char curkeys[FSIZE] = {	/* current key save buffer */

	/* function keys F1 - F10 */

	0xfe, ' ', ' ', ' ', ' ', ' ', 0x1d, 59, 0, 0, 0, 0, 0, 0, 0, 0,
	0xfe, ' ', ' ', ' ', ' ', ' ', 0x1d, 60, 0, 0, 0, 0, 0, 0, 0, 0,
	0xfe, ' ', ' ', ' ', ' ', ' ', 0x1d, 61, 0, 0, 0, 0, 0, 0, 0, 0,
	0xfe, ' ', ' ', ' ', ' ', ' ', 0x1d, 62, 0, 0, 0, 0, 0, 0, 0, 0,
	0xfe, ' ', ' ', ' ', ' ', ' ', 0x1d, 63, 0, 0, 0, 0, 0, 0, 0, 0,
	0xfe, ' ', ' ', ' ', ' ', ' ', 0x1d, 64, 0, 0, 0, 0, 0, 0, 0, 0,
	0xfe, ' ', ' ', ' ', ' ', ' ', 0x1d, 65, 0, 0, 0, 0, 0, 0, 0, 0,
	0xfe, ' ', ' ', ' ', ' ', ' ', 0x1d, 66, 0, 0, 0, 0, 0, 0, 0, 0,
	0xfe, ' ', ' ', ' ', ' ', ' ', 0x1d, 67, 0, 0, 0, 0, 0, 0, 0, 0,
	0xfe, ' ', ' ', ' ', ' ', ' ', 0x1d, 68, 0, 0, 0, 0, 0, 0, 0, 0,

	/* function keys F11 - F20 */

	0xfe, ' ', ' ', ' ', ' ', ' ', 0x1d, 84, 0, 0, 0, 0, 0, 0, 0, 0,
	0xfe, ' ', ' ', ' ', ' ', ' ', 0x1d, 85, 0, 0, 0, 0, 0, 0, 0, 0,
	0xfe, ' ', ' ', ' ', ' ', ' ', 0x1d, 86, 0, 0, 0, 0, 0, 0, 0, 0,
	0xfe, ' ', ' ', ' ', ' ', ' ', 0x1d, 87, 0, 0, 0, 0, 0, 0, 0, 0,
	0xfe, ' ', ' ', ' ', ' ', ' ', 0x1d, 88, 0, 0, 0, 0, 0, 0, 0, 0,
	0xfe, ' ', ' ', ' ', ' ', ' ', 0x1d, 89, 0, 0, 0, 0, 0, 0, 0, 0,
	0xfe, ' ', ' ', ' ', ' ', ' ', 0x1d, 90, 0, 0, 0, 0, 0, 0, 0, 0,
	0xfe, ' ', ' ', ' ', ' ', ' ', 0x1d, 91, 0, 0, 0, 0, 0, 0, 0, 0,
	0xfe, ' ', ' ', ' ', ' ', ' ', 0x1d, 92, 0, 0, 0, 0, 0, 0, 0, 0,
	0xfe, ' ', ' ', ' ', ' ', ' ', 0x1d, 93, 0, 0, 0, 0, 0, 0, 0, 0,

	/* other unlabeled keys */

	0x1d, 73, 0, 0, 0, 0,	/* roll up */
	0x1d, 81, 0, 0, 0, 0,	/* roll down */
	0x1d, 82, 0, 0, 0, 0,	/* insert */
	0x1d, 83, 0, 0, 0, 0,	/* delete */
	0x1d, 72, 0, 0, 0, 0,	/* up */
	0x1d, 75, 0, 0, 0, 0,	/* left */
	0x1d, 77, 0, 0, 0, 0,	/* right */
	0x1d, 80, 0, 0, 0, 0,	/* down */
	0x1d, 78, 0, 0, 0, 0,	/* clear */
	12, 0, 0, 0, 0, 0,	/* help */
	0x1d, 71, 0, 0, 0, 0,	/* home */
	0x1d, 0, 0, 0, 0, 0,	/* end */
};

extern union REGS rg;		/* cpu register for use of DOS calls */
#endif

/*
 * Standard terminal interface dispatch table. Most of the fields point into
 * "termio" code.
 */
NOSHARE TERM term    = {
	NROW-1,
        NROW-1,
        NCOL,
        NCOL,
	0, 0,
	MARGIN,
	SCRSIZ,
	NPAUSE,
        necopen,
        necclose,
	neckopen,
	neckclose,
        necgetc,
        ttputc,
        ttflush,
        necmove,
        neceeol,
        neceeop,
        neceeop,
        necbeep,
	necrev,
	neccres
#if	COLOR
	, necfcol,
	necbcol
#endif
#if	INSDEL
	, necins,
	necdel
#endif
};

#if	COLOR
necfcol(color)		/* set the current output color */

int color;	/* color to set */

{
	if (color == cfcolor)
		return;
	ttputc(ESC);
	ttputc('[');
	necparm((7 & color)+30);
	ttputc('m');
	cfcolor = color;
}

necbcol(color)		/* set the current background color */

int color;	/* color to set */

{
	if (color == cbcolor)
		return;
#if	1
	ttputc(ESC);
	ttputc('[');
	necparm((7 & color)+40);
	ttputc('m');
#endif
        cbcolor = color;
}
#endif

#if	INSDEL
necins(lines)		/* insert some screen lines */

int lines;	/* number of lines to insert */

{
	if (lines < 1)
		return;
	ttputc(ESC);
	ttputc('[');
	necparm(lines);
	ttputc('L');
}

necdel(lines)		/* delete some screen lines */

int lines;	/* number of lines to delete */

{
	if (lines < 1)
		return;
	ttputc(ESC);
	ttputc('[');
	necparm(lines);
	ttputc('M');
}
#endif

necmove(row, col)
{
        ttputc(ESC);
        ttputc('[');
        necparm(row+1);
        ttputc(';');
        necparm(col+1);
        ttputc('H');
}

neceeol()
{
        ttputc(ESC);
        ttputc('[');
        ttputc('K');
}

neceeop()
{
#if	COLOR
	necfcol(gfcolor);
	necbcol(gbcolor);
#endif
        ttputc(ESC);
        ttputc('[');
        ttputc('J');
}

necrev(state)		/* change reverse video state */

int state;	/* TRUE = reverse, FALSE = normal */

{
#if	COLOR
	int ftmp, btmp;		/* temporaries for colors */
#endif

	ttputc(ESC);
	ttputc('[');
	ttputc(state ? '7': '0');
	ttputc('m');
#if	COLOR & 0
	if (state == FALSE) {
		ftmp = cfcolor;
		btmp = cbcolor;
		cfcolor = -1;
		cbcolor = -1;
		necfcol(ftmp);
		necbcol(btmp);
	}
#endif
}

neccres()	/* change screen resolution */

{
	return(TRUE);
}

spal(char *dummy)		/* change pallette settings */

{
	/* none for now */
}

necbeep()
{
        ttputc(BEL);
        ttflush();
}

necparm(n)
register int    n;
{
        register int q,r;

        q = n/10;
        if (q != 0) {
		r = q/10;
		if (r != 0) {
			ttputc((r%10)+'0');
		}
		ttputc((q%10) + '0');
        }
        ttputc((n%10) + '0');
}

necopen()
{
	strcpy(sres, "NORMAL");
	revexist = TRUE;
        ttopen();
	ttputc(ESC);
	ttputc(')');
	ttputc('0');
#if	FLABEL
	getkeys(oldkeys);	/* save original function keys */
#endif
}

necclose()

{
#if	COLOR
	necfcol(7);
	necbcol(0);
#endif
	ttclose();
}

neckopen()	/* open the keyboard (a noop here) */

{
#if	FLABEL
	setkeys(curkeys);	/* and reset them to new values */
#endif
}

neckclose()	/* close the keyboard (a noop here) */

{
#if	FLABEL
	setkeys(oldkeys);	/* restore the original function key block */
#endif
}

necgetc()

{
	return(ttgetc());
}

#if	FLABEL
int fnclabel(f, n)		/* label a function key */

int f,n;	/* default flag, numeric argument [unused] */

{
	char *ptr;	/* ptr into function key string */
	char lbl[6];	/* returned label contents */
	int status;
	int index;

	/* must have a numeric argument */
	if (f == FALSE) {
		mlwrite(TEXT159);
/*                      "%Need function key number" */
		return(FALSE);
	}

	/* and it must be a legal key number */
	if (n < 1 || n > 10) {
		mlwrite(TEXT50);
/*                      "%Function key number out of range" */
		return(FALSE);
	}

	/* get the string to send */
	status = mlreply(TEXT51, lbl, 6);
/*                       "Label contents: " */
	if (status != TRUE)
		return(status);

	/* pad the label out */
	for (index=0; index < 5; index++) {
		if (lbl[index] == 0)
			break;
	}
	for (; index < 5; index++)
		lbl[index] = ' ';
	lbl[5] = 0;

	/* label the key! */
	ptr = &curkeys[(n - 1) * 16 + 1];
	movmem(lbl, ptr, 5);
	setkeys(curkeys);
	return(TRUE);
}

getkeys(keys)	/* read the function key labels and definitions */

unsigned char *keys;	/* function key block */

{
	rg.h.cl = 0x0c;
	rg.x.ax = 0;
	rg.x.dx = (int)keys;
	int86(0xdc, &rg, &rg);
}

setkeys(keys)	/* reset the function key labels and definitions */

unsigned char *keys;	/* function key block */

{
	rg.h.cl = 0x0d;
	rg.x.ax = 0;
	rg.x.dx = (int)keys;
	int86(0xdc, &rg, &rg);
}
#endif
#else
nechello()
{
}
#endif
