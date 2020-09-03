/*
 * This is a library module for help file subject lookup.
 *
 * The functions that are available are:
 *
 *	int looktopic(topic, output) char *topic; int (*output)();
 *		Lookup the topic and call output with each line of it.
 *		Returns 1 if the topic could not be found, -1 if the
 *		file could not be accessed.
 *
 *	int lookclose();
 *		Close help files and clean up the rest of the world.
 *
 */

#include <stdio.h>
#include "estruct.h"
#if	LIBHELP
#include "eproto.h"
#include "edef.h"
#include "elang.h"

#if	MSDOS
#include <sys\types.h>
#include <sys\stat.h>
#endif

#if	USG || THEOS || LINUX
#include <sys/stat.h>
#endif

#ifdef	HELPSEP
#undef	HELPSEP
#endif

#define	HELPSEP	'@'			/* marks a new help entry */

#define	LOOK	0			/* d�tail */
#define	INDEX	1			/* sommaire */

extern NOSHARE CONST char *otherpath[];

static	char	*lookfile=0;		/* Help file name ptr. */
static	char	*lookindx=0;		/* Help index file name ptr. */
static	FILE	*lookfp;		/* Our lookfile */
static	char	*lookline;		/* Our help buffer */
static	char	hfname[NFILEN];		/* Help file name place */
static	char	hiname[NFILEN];		/* Help index file name place */
static	char	lookbuff[NFILEN];

/*
 * Structure to speed lookup time.
 */
struct	look	{
	long	l_seek;
	char	*l_name;
};

struct	look	*lread();

void fastlook();

/* Open the help file "name" with access "acs" along the current libpath.
 */
FILE *hfopen(name, acs)
char *name;
char *acs;
{
	CONST char *fullname;
	FILE *fp;

	fp = (FILE *) 0;

	if ((fullname = flook(name, FALSE)) == NULL)
		return NULL;		/* Can't get full name */
	fp = fopen(fullname, acs);	/* Open the file. */
	return fp;			/* return the file ptr */
}

/*
 * Get the "stat" of the help file...
 */
int hstat(name, sb)
char *name;
struct stat *sb;
{
	CONST char *fullname;
	register int s;

	if ((fullname = flook(name, FALSE)) == NULL)
		return -1;	/* could not get the name */

	s = stat(fullname, sb);	/* stat the file. */
	return s;
}

void lookclose()
{
	if (lookfp != NULL)
		fclose(lookfp);
	if (lookline != NULL)
		free(lookline);
	lookfp = (FILE *) 0;
	lookline = NULL;
}

/*
 * program interface to the help file. open file(s) as needed.
 * subsequent calls will not need to search and open them
 * again -- output function takes a string arg and does something
 * with it.  Non-zero return means no help found.
 */

/*
 * Lookup a command in the given file.
 * The format is to look for HELPSEP (@) lines.
 * The optional index-file is provided to speed up
 * the lookup in situations where there is a very
 * large system help file.
 */
int helplookup(com, fp, ind, output)
register char *com;
FILE *fp;
char *ind;
int (*output)(char *str);
{
	if (fp == NULL)
		return (1);
	fseek(fp, 0L, 0);
	fastlook(com, fp, ind);
	while (fgets(lookline, NLINE, fp) != NULL)
		if (lookline[0] == HELPSEP) {
			lookline[strlen(lookline)-1] = '\0';
			if (strcmp(com, lookline+1) == 0) {
				while (fgets(lookline, NLINE, fp) != NULL) {
					if (lookline[0] == HELPSEP)
						break;
					lookline[strlen(lookline)-1] = '\0';
					(*output)(lookline);
				}
				return (0);
			}
		}
	return (1);
}

/*
 * Possibly seek the lookfile to the right place based on an index file.
 * Return non-zero only when it is impossible to find it.
 */
void fastlook(com, fp, ind)
char *com;
FILE *fp;
char *ind;
{
	register struct look *lp;
	FILE *ifp;
	static struct stat sb;
	long htime;

	fstat(fileno(fp), &sb);
	htime = sb.st_mtime;

	if (ind == NULL)			/* No index file? */
		return;
	if (hstat(ind, &sb) < 0)		/* not found ? */
		return;

	if (htime < sb.st_mtime) {
		if ((ifp = hfopen(ind, "rb")) == NULL)
			return;
		while ((lp = lread(ifp)) != NULL)
			if (strcmp(com, lp->l_name) == 0) {
				fseek(fp, lp->l_seek, 0);
				break;
			}
		fclose(ifp);
	}
	return;
}

/*
 * Read in a look structure.  Return NULL
 * on end of file or error.
 */
struct look *lread(fp)
register FILE *fp;
{
	register char *cp;
	register int c;
	static struct look look;
	static char name[50];

	look.l_name = name;
	if (fread(&look.l_seek, sizeof look.l_seek, 1, fp) != 1)
		return (NULL);
	for (cp = name; cp<&name[49]; cp++) {
		if ((c = getc(fp)) == EOF)
			return (NULL);
		if (!(*cp = c))
			break;
	}
	return (&look);
}

/* Return 0 if s1 found in s2 */
static int subcmp(s1, s2)
char *s1;
register char *s2;
{
	register char *st;
	register char *se;

	st = s1;
	for (;;) {
		while (*st != *s2++)
			if (! *s2)
				return 1;
		if (! *st++)
			return 0;
		se = s2;
		while (*st++ == *se++) {
			if (! *st)
				return 0;
			if (! *se)
				return 1;
		}
		st = s1;
	}
}

int doindex(com, fp, output)
register char *com;
FILE *fp;
int (*output)(char *str);
{
	register int t = 0;

	if (fp == NULL)
		return (1);
	fseek(fp, 0L, 0);
	while (fgets(lookline, NLINE, fp) != NULL) {
		if (lookline[0] == HELPSEP) {
			lookline[strlen(lookline)-1] = '\0';
			if (subcmp(com, lookline+1) == 0) {
				if (fgets(lookline, NLINE, fp) == NULL)
					return (t == 0);
				lookline[strlen(lookline)-1] = '\0';
				(*output)(lookline);
				t++;
			}
		}
	}
	return (t == 0);
}

/*
 * Make an index of help for a given topic.
 * Non-zero return means no topics found.
 */

int looktopic(topic, output, mode)
char *topic;
int (*output)(char *str);
int mode;
{
	if (lookfp == NULL)
		if ((lookfp = hfopen(lookfile, "r")) == NULL)
			return -1;
	if (lookline == NULL)
		if ((lookline = malloc(NLINE)) == NULL)
			return -1;
	if (mode == LOOK)
		return(helplookup(topic, lookfp, lookindx, output));
	else
		return(doindex(topic, lookfp, output));
}

/*
 * Service routine for help...
 */
static void addhelp(str)
CONST char *str;
{
	if (! helpbp)
		helpbp = bfind(TEXT294, TRUE, BFINVS|BFHELP);	/* Help buffer	*/
	if (helpbp)
		addline(helpbp, str);
}

/*
 * This routine rebuilds the text in the special secret buffer that holds
 * the help.  It is called by the lookwindow function.	Return TRUE if
 * everything works.  Return FALSE if there is an error (if there is no memory
 * or no help for the topic or file not found.)
 */
int makelook(tag, mode)
char *tag;
int mode;
{
	register int c;

	if ((c = bclear(helpbp)) != TRUE)	/* Blow old text away */
		return (c);
	if (mode == LOOK)
		strcpy(helpbp->b_fname, tag);
	else {
		strcpy(helpbp->b_fname, TEXT293);
		strcat(helpbp->b_fname, tag);
	}


	if ((c = looktopic(tag, addhelp, mode)) < 0)
		mlwrite(TEXT292, lookfile);
	else if (c > 0)
		mlwrite(mode == LOOK ? TEXT290 : TEXT291, tag);

	return (c == 0);		/* All done */
}
/*
 * Put up a help window with the text pointed to by txt (mode == LOOK)
 * or
 * Put up a help window with an index of matching topics (mode == INDEX)
 */

int lookwindow(tag, mode)
char *tag;
int mode;
{
	register LINE	*lp;
	register int	s;
	register int	numlines;

	if (mode == INDEX || curwp->w_bufp != helpbp)
		savewnd(TRUE, 0);

	initlook(NULL);

	if ((s = makelook(tag, mode)) != TRUE)
		return (s);

	lp = lforw(helpbp->b_linep);

	/* start counting lines */
	numlines = 0;
	while (lp != helpbp->b_linep) {
		++numlines;
		lp = lforw(lp);
	}

	if (helpbp->b_nwnd == 0) {		/* Not on screen yet. */
		if (wpopup(helpbp) == FALSE)
			return (FALSE);
		if (popflag)
			return (TRUE);
	} else {
		while (helpbp->b_nwnd > 1)
			zaphelp(0, 0);		/* Delete windows */
	}
	while (curwp->w_bufp != helpbp)
		nextwind(FALSE, 1);
	helpbp->b_flag |= BFINVS|BFHELP;
	resize(TRUE, numlines);
	if (mode == LOOK)
		restwnd(TRUE, 0);
	lookclose();
	return (TRUE);
}

/*
 * On GEMDOS and MSDOS, lookfiles specified on the command line
 * are <name>.HLP for the main help, and <name>.IDX for the index file,
 * if any.
 */

VOID initlook(lf)
char *lf;
{
	extern char *getenv();

	if (lookfile == NULL) {
		if ((lookfile=lf)==NULL && (lookfile=getenv("HELP"))==NULL) {
			lookfile = (char *) otherpath[1];
			lookindx = (char *) otherpath[2];
		} else {
#if	! THEOS
			strcat(hfname, otherpath[3]);
			lookfile = hfname;
			strcat(hiname, otherpath[4]);
			lookindx = hiname;
#else
			strcpy(hfname, lookfile);
			lookfile = strcat(hfname, otherpath[3]);
			strcpy(hiname, hfname);
			lookindx = strcat(hiname, otherpath[4]);
#endif
		}
	}
}

int prompthelp(int mode)
{
	register int	s;

	if ((s=mlreply(mode == LOOK ? TEXT288 : TEXT289, lookbuff, NFILEN)) != TRUE)
		return s;
	mlwrite(mode == LOOK ? TEXT286 : TEXT287, lookbuff);
	return lookwindow(lookbuff, mode);
}

int promptlook(int f, int n)
{
	return prompthelp(LOOK);
}

int hlpindex(int f, int n)
{
	return prompthelp(INDEX);
}

/*
 * Lookup the current word.
 * Error if you try and move past the end of the buffer.
 * Bound to "M-M-".  Count is passed to lookup routine.
 */
int lookupword(int f, int n)
{
	char topic[128];
	register char *cp;
	struct LINE *dotp;
	int	doto;

	doto = curwp->w_doto;
	dotp = curwp->w_dotp;
	while (! inword())
		if (! backchar(FALSE, 1))
			break;
	while (inword())
		if (! backchar(FALSE, 1))
			break;
	while (! inword())
		if (! forwchar(FALSE, 1))
			break;
	cp = topic;
	while (inword()) {
		*cp++ = lgetc(curwp->w_dotp, curwp->w_doto);
		if (! forwchar(FALSE, 1))
			break;
	}
	*cp = '\0';

	curwp->w_doto = doto;
	curwp->w_dotp = dotp;

	if (topic == NULL || ! *topic) {
		mlwrite(TEXT285);
		return FALSE;
	}
	mlwrite(TEXT286, topic);
	return lookwindow(topic, LOOK);
}

#else
VOID hlphello()	/* dummy function */
{
}
#endif	/* LIBHELP */
