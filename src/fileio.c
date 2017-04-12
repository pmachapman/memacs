/*  FILEIO.C:   Low level file i/o routines
		MicroEMACS 3.12

 * The routines in this file read and write ASCII files from the disk. All of
 * the knowledge about files are here.
 */

#include        <stdio.h>
#include	"estruct.h"
#include	"eproto.h"
#include        "edef.h"
#include	"elang.h"

#if	AOSVS
#define	fopen	xxfopen
#endif

NOSHARE FILE *ffp;		/* File pointer, all functions. */
static int eofflag;		/* end-of-file flag */

#if	(MSC || TURBO || IC) && MSDOS
#define	FILE_BUFSIZE	4096
char file_buffer[FILE_BUFSIZE];
#endif

/*
 * Open a file for reading.
 */
#if !(VMS & RMSIO)	/* if using RMS under VMS, the code */
			/* is in VMS.C */
PASCAL NEAR ffropen(fn)
char    *fn;
{
	if ((ffp=fopen(fn, "r")) == NULL)
		return(FIOFNF);

#if	(MSC || TURBO || IC) && MSDOS
	/* tell the library to give us a LARGE buffer to speed I/O */
	setvbuf(ffp, file_buffer, _IOFBF, FILE_BUFSIZE);
#endif

#if	WINDOW_MSWIN
	fbusy = FREADING;
#endif
	eofflag = FALSE;
	return(FIOSUC);
}

/*
 * Open a file for writing. Return TRUE if all is well, and FALSE on error
 * (cannot create).
 */
#if	AOSVS == 0
PASCAL NEAR ffwopen(fn, mode)
char    *fn;
char *mode;	/* mode to open file for */
{
	char xmode[6];		/* extended file open mode */

	/* nonstandard line terminators? */
	if (*lterm) {

		/* open in binary mode */
		strcpy(xmode, mode);
		strcat(xmode, "b");
		ffp = fopen(fn, xmode);
	} else {

		/* open in ascii(text) mode */
		ffp = fopen(fn, mode);
	}

	if (ffp == NULL) {
		mlwrite(TEXT155);
/*			"Cannot open file for writing" */
		return(FIOERR);
	}

#if	(MSC || TURBO || IC) && MSDOS
	/* tell the library to give us a LARGE buffer to speed I/O */
	setvbuf(ffp, file_buffer, _IOFBF, FILE_BUFSIZE);
#endif

#if	WINDOW_MSWIN
	fbusy = FWRITING;
#endif
	return(FIOSUC);
}
#endif

/*
 * Close a file. Should look at the status in all systems.
 */
PASCAL NEAR ffclose()
{
	/* free this since we do not need it anymore */
	if (fline) {
		free(fline);
		fline = NULL;
	}

#if	WINDOW_MSWIN
	fbusy = FALSE;
#endif
#if	MSDOS & CTRLZ
	putc(26, ffp);		/* add a ^Z at the end of the file */
#endif
	
#if     V7 | USG | AUX | SMOS | HPUX8 | HPUX9 | SUN | XENIX | BSD | WMCS | VMS | (MSDOS & (LATTICE | MSC | DTL | TURBO | IC | ZTC)) | WINNT | OS2 | (TOS & MWC) | AVIION
        if (fclose(ffp) != FALSE) {
                mlwrite(TEXT156);
/*                      "Error closing file" */
                return(FIOERR);
        }
        return(FIOSUC);
#else
        fclose(ffp);
        return(FIOSUC);
#endif
}

/*
 * Write a line to the already opened file. The "buf" points to the buffer,
 * and the "nbuf" is its length, less the free newline. Return the status.
 * Check only at the newline.
 */
PASCAL NEAR ffputline(buf, nbuf)

char    buf[];
int nbuf;

{
        register int i;		/* index into line to write */
	register char *lptr;	/* ptr into the line terminator */
#if	CRYPT
	char c;		/* character to translate */

	if (cryptflag) {
	        for (i = 0; i < nbuf; ++i) {
			c = buf[i];
			crypt(&c, 1);
			putc(c, ffp);
		}
	} else
	        for (i = 0; i < nbuf; ++i)
        	        putc(buf[i], ffp);
#else
        for (i = 0; i < nbuf; ++i)
                putc(buf[i], ffp);
#endif

	/* write out the appropriate line terminator(s) */
	if (*lterm) {
		lptr = &lterm[0];
		while (*lptr)
			putc(*lptr++, ffp);
	} else {
	        putc('\n', ffp);
	}

	/* check for write errors */
        if (ferror(ffp)) {
                mlwrite(TEXT157);
/*                      "Write I/O error" */
                return(FIOERR);
        }

#if	WINDOW_MSWIN
	{
	    static int	o = 0;
	    if (--o < 0) {
		longop(TRUE);
		o = 10;    /* to lower overhead, only 10% calls to longop */
	    }
	}
#endif
        return(FIOSUC);
}

/*
 * Read a line from a file, and store the bytes in the supplied buffer. The
 * "nbuf" is the length of the buffer. Complain about long lines and lines
 * at the end of the file that don't have a newline present. Check for I/O
 * errors too. Return status.
 */
PASCAL NEAR ffgetline(nbytes)

int *nbytes;

{
        register int c;		/* current character read */
        register int i;		/* current index into fline */

	/* if we are at the end...return it */
	if (eofflag)
		return(FIOEOF);

	/* dump fline if it ended up too big */
	if (flen > NSTRING && fline != NULL) {
		free(fline);
		fline = NULL;
	}

	/* if we don't have an fline, allocate one */
	if (fline == NULL)
		if ((fline = malloc(flen = NSTRING)) == NULL)
			return(FIOMEM);

	/* read the line in */
        i = 0;
        while ((c = getc(ffp)) != EOF && c != '\n') {
                fline[i++] = c;
		/* if it's longer, get more room */
                if (i >= flen) {
#if	MSDOS
			if (flen >= 16636)
				return(FIOMEM);
#endif
			flen *= 2;
			if ((fline = realloc(fline, flen)) == NULL) {
			    return(FIOMEM);
			}
		}
	}

	/* dump any extra line terminators at the end */
	while (i > 0 && (fline[i-1] == 10 || fline[i-1] == 13))
		i--;

	/* and save the length for our caller... */
	*nbytes = i;

	/* we should be ready to dump leading terminators too - ADD THIS DAN */

	/* test for any errors that may have occured */
        if (c == EOF) {
                if (ferror(ffp)) {
                        mlwrite(TEXT158);
/*                              "File read error" */
                        return(FIOERR);
                }

                if (i != 0)
			eofflag = TRUE;
		else
			return(FIOEOF);
        }

	/* terminate and decrypt the string */
        fline[i] = 0;
#if	CRYPT
	if (cryptflag)
		crypt(fline, strlen(fline));
#endif
        return(FIOSUC);
}
#endif

int PASCAL NEAR fexist(fname)	/* does <fname> exist on disk? */

char *fname;		/* file to check for existance */

{
	FILE *fp;

	/* try to open the file for reading */
	fp = fopen(fname, "r");

	/* if it fails, just return false! */
	if (fp == NULL)
		return(FALSE);

	/* otherwise, close it and report true */
	fclose(fp);
	return(TRUE);
}

#if	AZTEC & MSDOS
/*	a1getc:		Get an ascii char from the file input stream
			but DO NOT strip the high bit
*/

#undef	getc

int a1getc(fp)

FILE *fp;

{
	int c;		/* translated character */

	c = getc(fp);	/* get the character */

	/* if its a <LF> char, throw it out  */
	while (c == 10)
		c = getc(fp);

	/* if its a <RETURN> char, change it to a LF */
	if (c == '\r')
		c = '\n';

	/* if its a ^Z, its an EOF */
	if (c == 26)
		c = EOF;

	return(c);
}
#endif
