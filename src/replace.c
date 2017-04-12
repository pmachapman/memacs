/*
 * replace.c
 *
 * Functions, formerly in search.c, which handle the search and replace
 * functionality.
 */

#include <stdio.h>
#include "estruct.h"
#include "eproto.h"
#include "edef.h"
#include "elang.h"

static int	replen;			/* length of replacement string */

/*
 * sreplace -- Search and replace.
 */
int PASCAL NEAR sreplace(f, n)
int f;		/* default flag */
int n;		/* # of repetitions wanted */
{
	return (replaces(FALSE, f, n));
}

/*
 * qreplace -- search and replace with query.
 */
int PASCAL NEAR qreplace(f, n)
int f;		/* default flag */
int n;		/* # of repetitions wanted */
{
	return (replaces(TRUE, f, n));
}

/*
 * replaces -- Search for a string and replace it with another
 *	string.  Query might be enabled (according to kind).
 */
int PASCAL NEAR	replaces(kind, f, n)
int	kind;	/* Query enabled flag */
int	f;	/* default flag */
int	n;	/* # of repetitions wanted */
{
	register int status;	/* success flag on pattern inputs */
	register int nummatch;	/* number of found matches */
	int numsub;		/* number of substitutions */
	int nlflag;		/* last char of search string a <NL>? */
	int nlrepl;		/* was a replace done on the last line? */
	char c;			/* input char for query */
	LINE *origline;		/* original "." position */
	int origoff;		/* and offset (for . query option) */
	LINE *lastline;		/* position of last replace and */
	int lastoff;		/* offset (for 'u' query option) */
	int oldmatchlen;	/* Closure may alter the match length.*/
	static char *oldpatmatch = NULL;	/* allocated memory for un-do.*/

	/*
	 * Don't allow this command if we are
	 * in read only mode.
	 */
	if (curbp->b_mode & MDVIEW)
		return(rdonly());

	/* Check for negative repetitions.
	 */
	if (f && n < 0)
		return(FALSE);

	/* Ask the user for the text of a pattern.
	 */
	if ((status = readpattern(kind? TEXT85: TEXT84, &pat[0], TRUE)) != TRUE)
/*				"Replace" */
/*						"Query replace" */
		return(status);

	/* Ask for the replacement string, and get its length.
	 */
	if ((status = readpattern(TEXT86, &rpat[0], FALSE)) == ABORT)
/*				"with" */
		return(status);

	/* Set up flags so we can make sure not to do a recursive
	 * replace on the last line.
	 */
	nlflag = (pat[matchlen - 1] == '\r');
	nlrepl = FALSE;

	/* Save original . position, reset the number of matches and
	 * substitutions, and scan through the file.
	 */
	origline = curwp->w_dotp;
	origoff = curwp->w_doto;
	numsub = 0;
	nummatch = 0;
	lastline = (LINE *)NULL;
	mmove_flag = FALSE; 	/* disable mouse move events		  */

	while ( (f == FALSE || n > nummatch) &&
		(nlflag == FALSE || nlrepl == FALSE) ) {
		/* Search for the pattern.
		 * If we search with a regular expression,
		 * matchlen is reset to the true length of
		 * the matched string.
		 */
#if	MAGIC
		if (magical && (curwp->w_bufp->b_mode & MDMAGIC)) {
			if (!mcscanner(FORWARD, PTBEG, 1))
				break;
		}
		else
#endif
			if (!scanner(FORWARD, PTBEG, 1))
				break;		/* all done */

		++nummatch;	/* Increment # of matches */

		/* Check if we are on the last line.
		 */
		nlrepl = (lforw(curwp->w_dotp) == curwp->w_bufp->b_linep);

		/* Check for query.
		 */
		if (kind) {
			/* Get the query.
			 */
pprompt:		mlrquery();
qprompt:
			/* Show the proposed place to change, and
			 * update the position on the modeline if needed.
			 */
			if (posflag)
				upmode();
			update(TRUE);
			c = tgetc();			/* and input */
			mlerase();			/* and clear it */

			/* And respond appropriately.
			 */
			switch (c) {
#if	FRENCH
				case 'o':	/* oui, substitute */
				case 'O':
#endif
				case 'y':	/* yes, substitute */
				case 'Y':
				case ' ':
					break;

				case 'n':	/* no, onward */
				case 'N':
					forwchar(FALSE, 1);
					continue;

				case '!':	/* yes/stop asking */
					kind = FALSE;
					break;

				case 'u':	/* undo last and re-prompt */
				case 'U':
					/* Restore old position.
					 */
					if (lastline == (LINE *)NULL) {
						/* There is nothing to undo.
						 */
						TTbeep();
						goto pprompt;
					}
					curwp->w_dotp = lastline;
					curwp->w_doto = lastoff;
					lastline = NULL;
					lastoff = 0;

					/* Delete the new string,
					 * restore the old match.
					 */
					backchar(FALSE, replen);
					status = delins(replen, oldpatmatch, FALSE);
					if (status != TRUE) {
						mmove_flag = TRUE;
						return(status);
					}

					/* Record one less substitution,
					 * backup, save our place, and
					 * reprompt.
					 */
					--numsub;
					backchar(FALSE, oldmatchlen);
					matchlen = oldmatchlen;
					matchline = curwp->w_dotp;
					matchoff  = curwp->w_doto;
					continue;

				case '.':	/* abort! and return */
					/* restore old position */
					curwp->w_dotp = origline;
					curwp->w_doto = origoff;
					curwp->w_flag |= WFMOVE;

				case BELL:	/* abort! and stay */
					mlwrite(TEXT89);
/*						"Aborted!" */
					mmove_flag = TRUE;
					return(FALSE);

				default:	/* bitch and beep */
					TTbeep();

				case '?':	/* help me */
					mlwrite(TEXT90);
/*"(Y)es, (N)o, (!)Do rest, (U)ndo last, (^G)Abort, (.)Abort back, (?)Help: "*/
					goto qprompt;

			}	/* end of switch */
		}	/* end of "if kind" */

		/* if this is the point origin, flag so we a can reset it */
		if (curwp->w_dotp == origline) {
			origline = NULL;
			lastline = lback(curwp->w_dotp);
		}

		/* Delete the sucker, and insert its
		 * replacement.
		 */
#if	MAGIC
		status = delins(matchlen, &rpat[0], rmagical);
#else
		status = delins(matchlen, &rpat[0], FALSE);
#endif
		if (origline == NULL) {
			origline = lforw(lastline);
			origoff = 0;
		}

		if (status != TRUE) {
			mmove_flag = TRUE;
			return(status);
		}

		numsub++;	/* increment # of substitutions */

		/* Save our position, the match length, and the string
		 * matched if we are query-replacing, as we may undo
		 * the replacement. If we are not querying, check to
		 * make sure that we didn't replace an empty string
		 * (possible in MAGIC mode), because we'll infinite loop.
		 */
		if (kind) {
			lastline = curwp->w_dotp;
			lastoff = curwp->w_doto;
			oldmatchlen = matchlen;	/* Save the length for un-do.*/

#if	(TURBO || ZTC) && (DOS16M == 0)
			/* For compilers with reallocs that handle
			 * NULL pointers.
			 */
			if ((oldpatmatch = realloc(oldpatmatch, matchlen + 1)) == NULL) {
				mlabort(TEXT94);
/*					"%%Out of memory" */
				mmove_flag = TRUE;
				return(ABORT);
			}
#else
			if (oldpatmatch != NULL)
				free(oldpatmatch);

			if ((oldpatmatch = malloc(matchlen + 1)) == NULL) {
				mlabort(TEXT94);
/*					"%%Out of memory" */
				mmove_flag = TRUE;
				return(ABORT);
			}
#endif
			strcpy(oldpatmatch, patmatch);
		}
		else if (matchlen == 0) {
			mlwrite(TEXT91);
/*				"Empty string replaced, stopping." */
			mmove_flag = TRUE;
			return(FALSE);
		}
	}

	/* And report the results.
	 */
	mlwrite(TEXT92, numsub);
/*		"%d substitutions" */
	mmove_flag = TRUE;
	return(TRUE);
}

/*
 * mlrquery -- The prompt for query-replace-string.
 */
VOID PASCAL NEAR mlrquery()
{
	register int	tcol;
#if	MAGIC
	register RMC	*rmcptr;
#endif

	mlwrite(TEXT87);
/*		"Replace '" */

	tcol = echostring(patmatch, strlen(TEXT87), NPAT/2);

	mlputs(TEXT88);
/*		"' with '" */
	tcol += strlen(TEXT88);

#if	MAGIC
	if (rmagical && (curwp->w_bufp->b_mode & MDMAGIC)) {
		rmcptr = &rmcpat[0];

		while (rmcptr->mc_type != MCNIL && tcol < NPAT - 8) {
			if (rmcptr->mc_type == LITSTRING)
				tcol = echostring(rmcptr->u.rstr, tcol, NPAT - 8);
			else if (rmcptr->mc_type == DITTO)
				tcol = echostring(patmatch, tcol, NPAT - 8);
			else
				tcol = echostring(grpmatch[rmcptr->u.group_no],
					tcol, NPAT - 8);
			rmcptr++;
		}
	}
	else
#endif
		echostring(rpat, tcol, NPAT - 8);

	mlputs("'? ");
}

/*
 * delins -- Delete a specified length from the current point
 *	then either insert the string directly, or make use of
 *	replacement meta-array.
 */
int PASCAL NEAR delins(dlength, instr, use_rmc)
int	dlength;
char	*instr;
int	use_rmc;
{
	register int	status;
	register char	*rstr;
#if	MAGIC
	register RMC	*rmcptr;
#endif

	replen = 0;

	/* Zap what we gotta,
	 * and insert its replacement.
	 */
	if ((status = ldelete((long) dlength, FALSE)) != TRUE)
		mlwrite(TEXT93);
/*			"%%ERROR while deleting" */
	else
#if	MAGIC
		if (use_rmc && (curwp->w_bufp->b_mode & MDMAGIC)) {
			rmcptr = &rmcpat[0];
			while (rmcptr->mc_type != MCNIL && status == TRUE) {
				if (rmcptr->mc_type == LITSTRING)
					status = linstr(rstr = rmcptr->u.rstr);
				else if (rmcptr->mc_type == DITTO)
					status = linstr(rstr = patmatch);
				else
					status = linstr(rstr = fixnull(grpmatch[rmcptr->u.group_no]));
				replen += strlen(rstr);
				rmcptr++;
			}
		}
		else
#endif
		{
			status = linstr(instr);
			replen = strlen(instr);
		}

	return (status);
}

/*
 * rmcstr -- Set up the replacement 'magic' array.  Note that if there
 *	are no meta-characters encountered in the replacement string,
 *	the array is never actually created - we will just use the
 *	character array rpat[] as the replacement string.
 */
int PASCAL NEAR rmcstr()
{
	RMC	*rmcptr;
	char	*patptr;
	int	pchr;
	int	status = TRUE;
	int	mj;

	patptr = (char *)&rpat[0];
	rmcptr = &rmcpat[0];
	mj = 0;
	rmagical = FALSE;

	while (*patptr && status == TRUE) {
		switch (*patptr) {
			case MC_DITTO:

				/* If there were non-magical characters in
				 * the string before reaching this character
				 * plunk it in the replacement array before
				 * processing the current meta-character.
				 */
				if (mj != 0) {
					rmcptr->mc_type = LITSTRING;
					if ((rmcptr->u.rstr = malloc(mj + 1)) == NULL) {
						mlabort(TEXT94);
/*							"%%Out of memory" */
						status = FALSE;
						break;
					}
					bytecopy(rmcptr->u.rstr, patptr - mj, mj);
					rmcptr++;
					mj = 0;
				}
				rmcptr->mc_type = DITTO;
				rmcptr++;
				rmagical = TRUE;
				break;

			case MC_ESC:
				pchr = *(patptr + 1);	/* peek at next char.*/
				if (pchr <= '9' && pchr >= '1') {
					if (mj != 0) {
						rmcptr->mc_type = LITSTRING;
						if ((rmcptr->u.rstr = malloc(mj + 1)) == NULL) {
							mlabort(TEXT94);
/*							"%%Out of memory" */
							status = FALSE;
							break;
						}
						bytecopy(rmcptr->u.rstr, patptr - mj, mj);
						rmcptr++;
						mj = 0;
					}
					rmcptr->mc_type = GROUP;
					rmcptr->u.group_no = pchr - '0';
					patptr++;
				}
				else
				{
					rmcptr->mc_type = LITSTRING;

					/* We malloc mj plus two here, instead
					 * of one, because we have to count the
					 * current character.
					 */
					if ((rmcptr->u.rstr = malloc(mj + 2)) == NULL) {
						mlabort(TEXT94);
/*						"%%Out of memory" */
						status = FALSE;
						break;
					}

					bytecopy(rmcptr->u.rstr, patptr - mj, mj + 1);

					/* If MC_ESC is not the last character
					 * in the string, find out what it is
					 * escaping, and overwrite the last
					 * character with it.
					 */
					if (pchr != '\0') {
						*((rmcptr->u.rstr) + mj) = pchr;
						patptr++;
					}
					mj = 0;
				}

				rmcptr++;
				rmagical = TRUE;
				break;

			default:
				mj++;
		}
		patptr++;
	}

	if (rmagical && mj > 0) {
		rmcptr->mc_type = LITSTRING;
		if ((rmcptr->u.rstr = malloc(mj + 1)) == NULL) {
			mlabort(TEXT94);
/*				"%%Out of memory" */
			status = FALSE;
		}
		bytecopy(rmcptr->u.rstr, patptr - mj, mj);
		rmcptr++;
	}

	rmcptr->mc_type = MCNIL;
	return (status);
}

/*
 * rmcclear -- Free up any strings, and MCNIL the RMC array.
 */
VOID PASCAL NEAR rmcclear()
{
	register RMC	*rmcptr;

	rmcptr = &rmcpat[0];

	while (rmcptr->mc_type != MCNIL) {
		if (rmcptr->mc_type == LITSTRING)
			free(rmcptr->u.rstr);
		rmcptr++;
	}

	rmcpat[0].mc_type = MCNIL;
}
