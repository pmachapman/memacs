/*
 * The functions in this file implement commands that perform incremental
 * searches in the forward and backward directions.  This "ISearch" command
 * is intended to emulate the same command from the original EMACS
 * implementation (ITS).  Contains references to routines internal to
 * SEARCH.C.
 *
 * REVISION HISTORY:
 *
 *	D. R. Banks 9-May-86
 *	- added ITS EMACSlike ISearch
 *
 *	John M. Gamble 5-Oct-86
 *	- Made iterative search use search.c's scanner() routine.
 *	  This allowed the elimination of bakscan().
 *	- Put isearch constants into estruct.h
 *	- Eliminated the passing of 'status' to scanmore() and
 *	  checknext(), since there were no circumstances where
 *	  it ever equalled FALSE.
 *	Dan Corkill 6-Oct-87
 *      - Changed character loop to terminate with extended characters
 *        (thus arrow keys, and most commands behave intuitively).
 *      - Changed META to be reread rather than simply aborting.
 *      - Conditionalized VMS alternates for ^S and ^Q to only apply
 *        to VMS ports.  (Allowing ^X as a synonym for ^S defeats some
 *        of the benefits of the first change above.)
 *
 *	Jean-Michel Dubois March 2020
 *	- Unicode support
 *
 *	(Further comments are in history.c)
 */

#include <stdio.h>
#include "estruct.h"
#include "eproto.h"
#include "edef.h"
#include "elang.h"

#if	ISRCH

#define CMDBUFLEN	256	/* Length of our command buffer */

/* A couple of "own" variables for re-eat */
/* Hey, BLISS user, these were "GLOBAL", I made them "OWN". */
static int	(*saved_get_char)();	/* Get character routine */
static int	eaten_char = -1;	/* Re-eaten char */

/* A couple more "own" variables for the command string */

static int cmd_buff[CMDBUFLEN];	/* Save the command args here */
static int cmd_offset;	/* Current offset into command buff */
static int cmd_reexecute = -1;	/* > 0 if re-executing command */

/*
 * Subroutine to do incremental reverse search.  It actually uses the
 * same code as the normal incremental search, as both can go both ways.
 */

int PASCAL NEAR risearch(f, n)
int f, n;				/* prefix flag and argument */
{
	register int	status;

	/* Make sure the search doesn't match where we already
	 * are by backing up a character.
	 */
	backchar(TRUE, 1);

	if ((status = isearch(REVERSE)))
		mlerase();		/* If happy, just erase the cmd line  */
	else
		mlwrite(TEXT164);
/*               "[search failed]" */
	return (status);
}

/* Again, but for the forward direction */

int PASCAL NEAR fisearch(f, n)
int f, n;
{
	register int	 status;

	if ((status = isearch(FORWARD)))
		mlerase();		/* If happy, just erase the cmd line  */
	else
		mlwrite(TEXT164);
/*               "[search failed]" */
	return (status);
}

/*
 * Subroutine to do an incremental search.  In general, this works similarly
 * to the older micro-emacs search function, except that the search happens
 * as each character is typed, with the screen and cursor updated with each
 * new search character.
 *
 * While searching forward, each successive character will leave the cursor
 * at the end of the entire matched string.  Typing a Control-S
 * will cause the next occurrence of the string to be searched for (where the
 * next occurrence does NOT overlap the current occurrence).  A Control-R will
 * change to a backwards search, META will terminate the search and Control-G
 * will abort the search.  Rubout will back up to the previous match of the
 * string, or if the starting point is reached first, it will delete the
 * last character from the search string.
 *
 * While searching backward, each successive character will leave the cursor
 * at the beginning of the matched string.  Typing a Control-R will search
 * backward for the next occurrence of the string.  Control-S
 * will revert the search to the forward direction.  In general, the reverse
 * incremental search is just like the forward incremental search inverted.
 *
 * In all cases, if the search fails, the user will be feeped, and the search
 * will stall until the pattern string is edited back into something that
 * exists (or until the search is aborted).
 */

int PASCAL NEAR isearch(dir)

int dir;

{
	int 		status;	/* Search status */
	int 		col;	/* prompt column */
	register int	cpos;	/* character number in search string  */
	register int	c;	/* current input character */
	register int	expc;	/* function expanded input char 	  */
	char		pat_save[NPAT];	/* Saved copy of the old pattern str  */
	LINE		*curline;	/* Current line on entry		  */
	int 		curoff;	/* Current offset on entry		  */
	int 		init_direction;	/* The initial search direction 	  */
	KEYTAB		*ktp;	/* The command bound to the key 	  */
	register int (PASCAL NEAR *kfunc)();	/* ptr to the requested function to bind to */

	/* Set up the starting conditions */

	cmd_reexecute = -1;	/* We're not re-executing (yet?) */
	cmd_offset = 0;		/* Start at the beginning of cmd_buff */
	cmd_buff[0] = '\0';	/* Reset the command buffer */
	init_direction = dir;	/* Save the initial search direction */
	mmove_flag = FALSE;	/* disable mouse move events */

	/*
	 * Save the current search string and line position, in case
	 * we bounce out if isearch.  Unmake the meta-character array,
	 * so that it will get re-made automatically with the (maybe)
	 * new search string on a MAGIC mode search.
	 */
	bytecopy(pat_save, (char *) pat, NPAT);	/* Save the old pattern string */
	curline = curwp->w_dotp;	/* Save the current line pointer */
	curoff = curwp->w_doto;	/* Save the current offset */
#if MAGIC
	mcclear();
#endif


start_over:				/* This is a good place to start a re-execution: */

	/*
	 * Ask the user for the text of a pattern,
	 * and remember the column.
	 */
	col = (clexec) ? 0 : mlprompt(TEXT165, (char *) pat, isterm);
/*				"ISearch: " */

	cpos = 0;			/* Start afresh 		  */
	status = TRUE;		/* Assume everything's cool   */

	for (;;)
	{				/* ISearch per character loop */

		/*
		 * Check for special characters first.
		 * That is, a control or ^X or FN or mouse function.
		 * Most cases here change the search.
		 */
		c = ectoc(expc = get_char());

		if (expc == isterm)
		{			/* Want to quit searching?	  */
			setjtable();	/* Update jump tables...	  */
			mmove_flag = TRUE;
			return (TRUE);	/* Quit searching now		  */
		}

		if (expc == abortc)	/* If abort search request	  */
			break;		/* Quit searching		  */

		if (expc == quotec)			/* Quote character? 	  */
			c = ectoc(expc = get_char());	/* Get the next char		  */
		else if ((expc > CMSK || expc == 0) &&
			(c != '\t' && c != RET_CHAR))
		{

			kfunc = ((ktp = getbind(expc)) == NULL) ? NULL : ktp->k_ptr.fp;

			if (kfunc == forwsearch || kfunc == forwhunt ||
				kfunc == fisearch || kfunc == backsearch ||
				kfunc == backhunt || kfunc == risearch)
			{
				dir = (kfunc == backsearch ||
					kfunc == backhunt ||
					kfunc == risearch) ? REVERSE : FORWARD;

				/*
				 * if cpos == 0 then we are either just starting
				 * or starting over.  Use the original pattern
				 * in pat, which has either not been changed or
				 * has just been restored. Find the length and
				 * re-echo the string.
				 */
				if (cpos == 0)
					{
					cpos = strlen((char *) pat);
					col = echostring((char *) pat, col, NPAT / 2);
					}

				status = scanmore(dir);
				continue;
			}
			else if (kfunc == backdel)
			{
				/*
				 * If there's nothing to delete, just exit.
				 */
				if (cmd_offset <= 1)
				{
					mmove_flag = TRUE;
					return (TRUE);
				}

				/* Back up over the Rubout and the character
				 * it's rubbing out.  If it is a quoted
				 * char, rub the quote char out too.
				 */
				cmd_offset -= 2;
				if (cmd_offset > 0 &&
					cmd_buff[cmd_offset - 1] == quotec)
					cmd_offset--;
				cmd_buff[cmd_offset] = '\0';

				/*
				 * Reset everything - line and offset,
				 * search direction and pattern and
				 * jump tables, start the whole mess
				 * over (cmd_reexecute = 0) and
				 * let it take care of itself.
				 */
				curwp->w_dotp = curline;
				curwp->w_doto = curoff;
				dir = init_direction;
				bytecopy((char *) pat, pat_save, NPAT);
				setjtable();
				cmd_reexecute = 0;
				goto start_over;
			}

			/*
			 * Presumably the key was a command key,
			 * yet strangely uninteresting...
			 */
			reeat(expc);	/* Re-eat the char		  */
			setjtable();
			mmove_flag = TRUE;
			return (TRUE);	/* And return the last status */
		}

		/*
		 * I guess we got something to search for, so put the
		 * character in the buffer and search for it.
		 */
		pat[cpos++] = c;

		/*
		 * Too many characters in the string?  Yup.  Complain
		 * about it, and restore the old search string and
		 * its jump tables.
		 */
		if (cpos >= NPAT)
		{
			mlwrite(TEXT166);
/*				"? Search string too long" */
			bytecopy((char *) pat, pat_save, NPAT);
			setjtable();
			mmove_flag = TRUE;
			return (FALSE);	/* Return an error, but stay. */
		}

		pat[cpos] = 0;	/* null terminate the buffer  */
#if	COLOR
		/* set up the proper colors for the command line */
		TTforg(gfcolor);
		TTbacg(gbcolor);
#endif
		movecursor(term.t_nrow, col);	/* Position the cursor	*/
		col += ueechochar(c);	/* Echo the character		  */
		if (!status)	/* If we lost last time 	  */
			TTbeep();	/* Feep again		*/
		else 			/* Otherwise, we must have won*/
			status = checknext(c, dir);	/* See if still matches or find next */
	}

	curwp->w_dotp = curline;	/* Reset the line pointer		*/
	curwp->w_doto = curoff;	/*   and the offset to original value	*/
	curwp->w_flag |= WFMOVE;	/* Say we've moved			*/
	update(FALSE);		/* And force an update			*/
	mmove_flag = TRUE;
	return (FALSE);
}

/*
 * This hack will search for the next occurrence of <pat> in the buffer,
 * either forward or backward.  If we can't find any more matches, "point"
 * is left where it was before.  If we do find a match, "point" will be at
 * the end of the matched string for forward searches and at the beginning
 * of the matched string for reverse searches.
 */

int PASCAL NEAR scanmore(dir)
int dir;				/* direction to search		*/
{
	register int	status;	/* search status		*/

	setjtable();		/* Set up fast search arrays	*/

#if MAGIC
	if (dir == FORWARD)
		status = mcscanner(&mcdeltapat[0], dir, PTEND, 1);
	else
		status = mcscanner(&tapatledcm[0], dir, PTBEG, 1);
#else
	status = scanner(dir, (dir == REVERSE) ? PTBEG : PTEND, 1);
#endif

	if (!status)
		TTbeep();		/* Feep if search fails       */

	return (status);
}

/*
 * Trivial routine to insure that the next character in the search
 * string is still true to whatever we're pointing to in the buffer.
 * This routine will not attempt to move the "point" if the match
 * fails, although it will implicitly move the "point" if we're
 * forward searching, and find a match, since that's the way forward
 * isearch works.  If we are reverse searching we compare all
 * characters in the pattern string from "point" to the new end.
 *
 * If the compare fails, we return FALSE and call scanmore or something.
 */

int PASCAL NEAR checknext(chr, dir)
int chr;				/* Next char to look for	*/
int dir;				/* Search direction 		*/
{
	LINE *curline;		/* current line during scan	*/
	int curoff;			/* position within current line	*/
	register char *patrn;	/* The entire search string (incl chr) */
	register int sts;	/* how well things go		*/

	/* setup the local scan pointer to current "." */

	curline = curwp->w_dotp;	/* Get the current line structure */
	curoff = curwp->w_doto;	/* Get the offset within that line */

	if (dir == FORWARD)
		{				/* If searching forward		*/
		if ((sts = !boundry(curline, curoff, FORWARD)))
			{
			/* If it's what we're looking for, set the point
			 * and say that we've moved.
			 */
			if ((sts = eq(nextch(&curline, &curoff, FORWARD), chr)))
				{
				curwp->w_dotp = curline;
				curwp->w_doto = curoff;
				curwp->w_flag |= WFMOVE;
				}
			}
		}
	else {				/* Else, reverse search check. */
		patrn = (char *) pat;
#if	UTF8
		unsigned int wc;
		size_t len = strlen(patrn);
#endif
		while (*patrn)
			{			/* Loop for all characters in patrn   */
#if	UTF8
			int bytes = utf8_to_unicode(patrn, 0, len, &wc);

			if ((sts = !boundry(curline, curoff, FORWARD)) == FALSE ||
				(sts = weq(nextch(&curline, &curoff, FORWARD), *patrn)) == FALSE)
				break;	/* Nope, just punt it then */
			patrn += bytes;
			len -= bytes;
#else
			if ((sts = !boundry(curline, curoff, FORWARD)) == FALSE ||
				(sts = eq(nextch(&curline, &curoff, FORWARD), *patrn)) == FALSE)
				break;	/* Nope, just punt it then */
			patrn++;
#endif
			}
		}

	/*
	 * If the 'next' character didn't fit in the pattern,
	 * let's go search for it somewhere else.
	 */
	if (sts == FALSE)
		sts = scanmore(dir);

	return (sts);		/* And return the status */
}

/*
 * Routine to get the next character or extended character from the input
 * stream.  If we're reading from the real terminal, force a screen update
 * before we get the char.  Otherwise, we must be re-executing the command
 * string, so just return the next character.
 */

int PASCAL NEAR get_char()
{
	int	c;
	KEYTAB	*key;

	/* See if we're re-executing: */

	if (cmd_reexecute >= 0)	/* Is there an offset?		*/
		if ((c = cmd_buff[cmd_reexecute++]) != 0)
			return (c);	/* Yes, return any character	*/

	/* We're not re-executing (or aren't any more).  Try for a real char
	 */
	cmd_reexecute = -1;	/* Say we're in real mode again	*/
	update(FALSE);		/* Pretty up the screen		*/
	if (posflag)
		upmode();		/* and the modeline, if need be.*/
	if (cmd_offset >= CMDBUFLEN - 1)
		{				/* If we're getting too big ...	*/
		mlwrite(TEXT167);	/* Complain loudly and bitterly	*/
/*               "? command too long" */
		return (isterm);	/* And force a quit		*/
		}

	/*
	 * If $isterm != meta character, then create the extended
	 * character as getcmd() does.
	 */
	if ((c = get_key()) == isterm)
		return (isterm);
	if ((key = getbind(c)) != NULL)
		{
		if (key->k_ptr.fp == cex || key->k_ptr.fp == uemeta)
			{
			c = get_key();
#if	SMOS
			c = upperc(c&CMSK) | (c & ~CMSK); /* Force to upper */
#elif UTF8
			c = ToWUpper(c);
#else
			c = upperc(c) | (c & ~CMSK);	/* Force to upper */
#endif
			c |= (key->k_ptr.fp == cex) ? CTLX : META;
			}
		}

	cmd_buff[cmd_offset++] = c;	/* Save the char for next time	*/
	cmd_buff[cmd_offset] = '\0';	/* And terminate the buffer	*/
	return (c);			/* Return the character		*/
}

/*
 * Hacky routine to re-eat a character.  This will save the character to be
 * re-eaten by redirecting the input call to a routine here.  Hack, etc.
 *
 * Come here on the next term.t_getchar call:
 */

int PASCAL NEAR uneat()
{
	int c;

	term.t_getchar = saved_get_char;	/* restore the routine address	*/
	c = eaten_char;		/* Get the re-eaten char	*/
	eaten_char = -1;	/* Clear the old char		*/
	return (c);			/* and return the last char	*/
}

VOID PASCAL NEAR reeat(c)
int	c;
{
	if (eaten_char != -1)	/* If we've already been here	*/
		return;			/* Don't do it again		*/
	eaten_char = c;		/* Else, save the char for later*/
	saved_get_char = term.t_getchar;	/* Save the char get routine	*/
	term.t_getchar = uneat;	/* Replace it with ours		*/
}

#else
int PASCAL NEAR isearch(dir)
int dir;
{
}
#endif
