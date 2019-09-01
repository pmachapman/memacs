/*
 * The functions in this file implement commands that search in the forward
 * and backward directions.
 *
 * (History comments formerly here have been moved to history.c)
 */

#include <stdio.h>
#include "estruct.h"
#include "eproto.h"
#include "edef.h"
#include "elang.h"

#define SEARCH_HIGHLIGHT	10	/* mark # used to highlight */
#define MAGIC_JUMP_TABLES	1	/* Jump tables in MAGIC mode */

#if MAGIC
static int	group_count;
static int	group_len[MAXGROUPS];
static REGION	group_reg[MAXGROUPS];

#endif

#if     WINDOW_MSWIN
static int  o = 0;	/* For longop() calls.*/
#endif

/*
 * forwsearch -- Search forward.  Get a search string from the user, and
 *	search for the string.  If found, reset the "." to be just after
 *	the match string, and (perhaps) repaint the display.
 */
#if PROTO
int PASCAL NEAR forwsearch(int f, int n)
#else
int PASCAL NEAR forwsearch( f, n)
int f;
int n;
#endif
{
	register int	status;

	/* If n is negative, search backwards.
	 * Otherwise proceed by asking for the search string.
	 */
	if (n < 0)
		return (backsearch(f, -n));

	/* Ask the user for the text of a pattern.  If the response is
	 * TRUE (responses other than FALSE are possible), search for
	 * pattern for up to n times, as long as the pattern is there
	 * to be found.
	 */
	if ((status = readpattern(TEXT78, (char *)&pat[0], TRUE)) == TRUE)
		status = forwhunt(f, n);
/*				"Search" */

	return (status);
}

/*
 * forwhunt -- Search forward for a previously acquired search string.
 *	If found, reset the "." to be just after the match string,
 *	and (perhaps) repaint the display.
 */
#if PROTO
int PASCAL NEAR forwhunt(int f, int n)
#else
int PASCAL NEAR forwhunt( f, n)
int f;
int n;
#endif
{
	register int	spoint = PTEND;
	register int	status;

	if (n < 0)		/* search backwards */
		return (backhunt(f, -n));

	/* Make sure a pattern exists, or that we didn't switch
	 * into MAGIC mode after we entered the pattern.
	 */
	if (pat[0] == '\0') {
		mlwrite(TEXT80);
/*			"No pattern set" */
		return FALSE;
	}

#if MAGIC
	if ((curwp->w_bufp->b_mode & MDMAGIC) && mcpat[0].mc_type == MCNIL) {
		if (!mcstr())
			return FALSE;
	}
#endif

	/*
	 * Do one extra search to get us past our current
	 * match, if the search type has us at the start
	 * of a match, instead of after a match.
	 */
	if (searchtype == SRBEGIN) {
		spoint = PTBEG;
		if (lastflag & CFSRCH)
			n = (n > 2)? (n + 1): 2;
	}

#if MAGIC
#if 0
	if (magical && (curwp->w_bufp->b_mode & MDMAGIC))
		status = mcscanner(&mcpat[0], FORWARD, spoint, n);
	else
		status = mcscanner(&mcdeltapat[0], FORWARD, spoint, n);
#else
	status = mcscanner((magical && (curwp->w_bufp->b_mode & MDMAGIC))?
				&mcpat[0]: &mcdeltapat[0], FORWARD, spoint, n);
#endif
#else
	status = scanner(FORWARD, spoint, n);
#endif

	/* Complain if not there.
	 */
	if (status == FALSE)
		mlwrite(TEXT79);
/*			"Not found" */

	thisflag |= CFSRCH;
	return (status);
}

/*
 * backsearch -- Reverse search.  Get a search string from the user, and
 *	search, starting at "." and proceeding toward the front of the buffer.
 *	If found "." is left pointing at the first character of the pattern
 *	(the last character that was matched).
 */
#if PROTO
int PASCAL NEAR backsearch(int f, int n)
#else
int PASCAL NEAR backsearch( f, n)
int f;
int n;
#endif
{
	register int	status;

	/* If n is negative, search forwards.
	 * Otherwise proceed by asking for the search string.
	 */
	if (n < 0)
		return (forwsearch(f, -n));

	/* Ask the user for the text of a pattern.  If the response is
	 * TRUE (responses other than FALSE are possible), search for
	 * pattern for up to n times, as long as the pattern is there
	 * to be found.
	 */
	if ((status = readpattern(TEXT81, (char *)&pat[0], TRUE)) == TRUE)
		status = backhunt(f, n);
/*				"Reverse search" */

	return (status);
}

/*
 * backhunt -- Reverse search for a previously acquired search string,
 *	starting at "." and proceeding toward the front of the buffer.
 *	If found "." is left pointing at the first character of the pattern
 *	(the last character that was matched).
 */
#if PROTO
int PASCAL NEAR backhunt(int f, int n)
#else
int PASCAL NEAR backhunt( f, n)
int f;
int n;
#endif
{
	register int	spoint = PTBEG;
	register int	status;

	if (n < 0)
		return (forwhunt(f, -n));

	/* Make sure a pattern exists, or that we didn't switch
	 * into MAGIC mode after we entered the pattern.
	 */
	if (tap[0] == '\0') {
		mlwrite(TEXT80);
/*			"No pattern set" */
		return FALSE;
	}

#if MAGIC
	if ((curwp->w_bufp->b_mode & MDMAGIC) && tapcm[0].mc_type == MCNIL) {
		if (!mcstr())
			return FALSE;
	}
#endif

	/*
	 * Do one extra search to get us past our current
	 * match, if the search type has us at the start
	 * of a match, instead of after a match.
	 */
	if (searchtype == SREND) {
		spoint = PTEND;
		if (lastflag & CFSRCH)
			n = (n > 2)? (n + 1): 2;
	}

#if MAGIC
#if 0
	if (magical && (curwp->w_bufp->b_mode & MDMAGIC))
		status = mcscanner(&tapcm[0], REVERSE, spoint, n);
	else
		status = mcscanner(&tapatledcm[0], REVERSE, spoint, n);
#else
	status = mcscanner((magical && (curwp->w_bufp->b_mode & MDMAGIC))?
				&tapcm[0]: &tapatledcm[0], REVERSE, spoint, n);
#endif
#else
	status = scanner(REVERSE, spoint, n);
#endif

	/* Complain if not there.
	 */
	if (status == FALSE)
		mlwrite(TEXT79);
/*			"Not found" */

	thisflag |= CFSRCH;
	return (status);
}

#if MAGIC
/*
 * mcscanner -- Search for a meta-pattern in either direction.  If found,
 *	reset the "." to be at the start or just after the match string,
 *	and (perhaps) repaint the display.
 */
#if PROTO
int PASCAL NEAR mcscanner(MC *mcpatrn, int direct, int beg_or_end, int repeats)
#else
int PASCAL NEAR mcscanner( mcpatrn, direct, beg_or_end, repeats)
MC *mcpatrn;
int direct;
int beg_or_end;
int repeats;
#endif
{
	LINE	*curline;	/* current line during scan */
	int	curoff;		/* position within current line */
	DELTA	*tbl;		/* structure with jump info */
	int	patlenadd;
	int	jump;

	/* If we are going in reverse, then the 'end' is actually
	 * the beginning of the pattern.  Toggle it.
	 */
	beg_or_end ^= direct;

#if MAGIC_JUMP_TABLES
	if (mcpatrn->mc_type == JMPTABLE) {
		tbl = mcpatrn->u.jmptable;
		jump = tbl->jump;
		patlenadd = tbl->patlen;
		mcpatrn++;
	}
	else
		tbl = NULL;
#endif

	/* Setup local scan pointers to global ".".
	 */
	curline = curwp->w_dotp;
	curoff  = curwp->w_doto;

	/* Scan each character until we hit the head link record.
	 */
	while (!boundry(curline, curoff, direct)) {
		/* Save the current position in case we need to
		 * restore it on a match, and initialize matchlen to
		 * zero in case we are doing a search for replacement.
		 */
		matchline = curline;
		matchoff = curoff;
		matchlen = 0;

#if MAGIC_JUMP_TABLES
		/*
		 * If the first thing to look for is a constant string
		 * that has been made into a jump table, use the fast
		 * scan routines.  If that fails, then we have nothing
		 * to match and return immediately.
		 */
		if (tbl != NULL) {
			if (!fbound(tbl, patlenadd, &curline, &curoff, direct)) {
				do
				{
					if (direct == FORWARD)
						movelocalpoint(-patlenadd, &curoff, &curline);
					else
						movelocalpoint(patlenadd + 1, &curoff, &curline);

					matchline = curline;
					matchoff = curoff;

					if ((matchlen = liteq(&curline, &curoff, direct, tbl->patrn)) > 0)
						break;
				} while (!fbound(tbl, jump, &curline, &curoff, direct));
			}

			if (matchlen == 0)
				return FALSE;
		}
#endif

#if     WINDOW_MSWIN
		/* to lower overhead, only 1/100 calls to longop */
		if (--o < 0) {
			longop(TRUE);
			o = 100;
		}
#endif
		if (amatch(mcpatrn, direct, &curline, &curoff)) {
			/* A SUCCESSFULL MATCH!!!
			 * Flag that we have moved,
			 * reset the global "." pointers.
			 */
			curwp->w_markp[SEARCH_HIGHLIGHT] = matchline;
			curwp->w_marko[SEARCH_HIGHLIGHT] = matchoff;
			curwp->w_markp[SEARCH_HIGHLIGHT+1] = curline;
			curwp->w_marko[SEARCH_HIGHLIGHT+1] = curoff;

			curwp->w_flag |= WFMOVE;
			if (beg_or_end == PTEND)	/* at end of string */
			{
				curwp->w_dotp = curline;
				curwp->w_doto = curoff;
			}
			else		/* at beginning of string */
			{
				curwp->w_dotp = matchline;
				curwp->w_doto = matchoff;
			}

			/* If we're heading in reverse, set the "match"
			 * pointers to the start of the string, for savematch().
			 */
			if (direct == REVERSE) {
				matchline = curline;
				matchoff = curoff;
			}

			if (savematch() == ABORT)
				return ABORT;

			/* Continue scanning if we haven't found
			 * the nth match.
			 */
			if (--repeats <= 0)
				return TRUE;
		}

		/* Advance the cursor.
		 */
		nextch(&curline, &curoff, direct);
	}

	return FALSE;	/* We could not find a match.*/
}

/*
 * amatch -- Search for a meta-pattern in either direction.  Update
 *	the passed-in LINE and offset values if successful.
 *	Based on the recursive routine amatch() (for "anchored match")
 *	in Kernighan & Plauger's "Software Tools".
 */
#if PROTO
int PASCAL NEAR	amatch(MC *mcptr, int direct, LINE **pcwline, int *pcwoff)
#else
int PASCAL NEAR	amatch( mcptr, direct, pcwline, pcwoff)
MC *mcptr;
int direct;
LINE **pcwline;
int *pcwoff;
#endif
{
	LINE	*curline;	/* current line during scan */
	int	curoff;		/* position within current line */
	int	pre_matchlen;	/* matchlen before a recursive amatch() call.*/
	int	cl_matchlen;	/* number of chars matched in a local closure.*/
	int	cl_min;		/* minimum number of chars matched in closure.*/
	int	cl_type;	/* Which closure type?.*/

	/* Set up local scan pointers to ".",
	 * and set up our local character counting
	 * variable, which can correct matchlen on
	 * a failed partial match.
	 */
	curline = *pcwline;
	curoff = *pcwoff;
	cl_matchlen = 0;

	/*
	 * Loop through the meta-pattern, laughing all the way.
	 */
	while (mcptr->mc_type != MCNIL) {
		/* Is the current meta-character modified
		 * by a closure?
		 */
		if (cl_type = (mcptr->mc_type & ALLCLOS)) {

			/* Minimum number of characters that may
			 * match is 0 or 1.
			 */
			cl_min = (cl_type == CLOSURE_1);

			if (cl_type == ZEROONE)
			{
				if (mceq(nextch(&curline, &curoff, direct), mcptr))
				{
					nextch(&curline, &curoff, direct);
					cl_matchlen++;
				}
			}
			else
			{
				/* Match as many characters as possible
				 * against the current meta-character.
				 */
				while (mceq(nextch(&curline, &curoff, direct), mcptr))
					cl_matchlen++;
			}

			/* We are now at the character that made us
			 * fail.  Try to match the rest of the pattern.
			 * Shrink the closure by one for each failure.
			 */
			mcptr++;
			matchlen += cl_matchlen;

			for (;;)
			{
				if (cl_matchlen < cl_min) {
					matchlen -= cl_matchlen;
					return FALSE;
				}

				nextch(&curline, &curoff, direct ^ REVERSE);
				pre_matchlen = matchlen;
				if (amatch(mcptr, direct, &curline, &curoff))
					goto success;
				matchlen = pre_matchlen - 1;
				cl_matchlen--;
			}
		}
		else if (mcptr->mc_type == GRPBEG) {
			group_reg[mcptr->u.group_no].r_offset = curoff;
			group_reg[mcptr->u.group_no].r_linep = curline;
			group_reg[mcptr->u.group_no].r_size = (direct == FORWARD)? -matchlen: matchlen;
		}
		else if (mcptr->mc_type == GRPEND) {
			group_len[mcptr->u.group_no] = (direct == FORWARD)? matchlen: -matchlen;
		}
		else if (mcptr->mc_type == BOL) {
			if (curoff != 0)
				return FALSE;
		}
		else if (mcptr->mc_type == EOL) {
			if (curoff != lused(curline))
				return FALSE;
		}
		else if (mcptr->mc_type == BOWRD) {
			if (!isinword(lgetc(curline, curoff)))
				return FALSE;

			if (curoff != 0 &&
			     isinword(lgetc(curline, curoff - 1)))
				return FALSE;
		}
		else if (mcptr->mc_type == EOWRD) {
			if (isinword(lgetc(curline, curoff)))
				return FALSE;

			if (curoff == 0 ||
			     !isinword(lgetc(curline, curoff - 1)))
				return FALSE;
		}
		else if (mcptr->mc_type == LITSTRING) {
			if ((pre_matchlen = liteq(&curline, &curoff, direct, mcptr->u.lstring)) == 0)
				return FALSE;
			matchlen += pre_matchlen;
		}
		else
		{
			/* A character to compare against
			 * the meta-character equal function.
			 * If we still match, increment the length.
			 */
			if (!mceq(nextch(&curline, &curoff, direct), mcptr))
				return FALSE;

			matchlen++;
		}

		mcptr++;
	}			/* End of mcptr loop.*/

	/* A SUCCESSFULL MATCH!!!
	 * Reset the "." pointers.
	 */
success:
	*pcwline = curline;
	*pcwoff  = curoff;
	return (TRUE);
}
#else

/*
 * scanner -- Search for a pattern in either direction.  If found,
 *	reset the "." to be at the start or just after the match string,
 *	and (perhaps) repaint the display.
 *	Fast version using simplified version of Boyer and Moore
 *	Software-Practice and Experience, vol 10, 501-506 (1980)
 */
#if PROTO
int PASCAL NEAR scanner(int direct, int beg_or_end, int repeats)
#else
int PASCAL NEAR scanner( direct, beg_or_end, repeats)
int direct;
int beg_or_end;
int repeats;
#endif
{
	DELTA	*tbl;			/* structure holding the jump info */
	char	*patrn;			/* string to scan for */
	LINE	*curline;		/* current line during scan */
	int	curoff;			/* position within current line */
	int	jump;			/* next offset */
	int	patlenadd;

	/* If we are going in reverse, then the 'end' is actually
	 * the beginning of the pattern.  Toggle it.
	 */
	beg_or_end ^= direct;

	/* Another directional problem: if we are searching
	 * forwards, use the pattern and the jump size in
	 * deltapat.  Otherwise, use the reversed pattern
	 * pattern and the jump size in tapatled.
	 */
	if (direct == FORWARD) {
		tbl = &deltapat;
		patrn = deltapat.patrn;
		patlenadd = deltapat.patlen;
		jump = deltapat.jump;
	}
	else
	{
		tbl = &tapatled;
		patrn = tapatled.patrn;
		patlenadd = tapatled.patlen;
		jump = tapatled.jump;
	}

	/* Set up local pointers to global ".".
	 */
	curline = curwp->w_dotp;
	curoff = curwp->w_doto;

	/* Scan each character until we hit the head link record.
	 * Get the character resolving newlines, offset
	 * by the pattern length, i.e. the last character of the
	 * potential match.
	 */
	if (!fbound(tbl, patlenadd, &curline, &curoff, direct)) {
		do
		{
			/* Set up the scanning pointers, and save
			 * the current position in case we match
			 * the search string at this point.
			 */
			if (direct == FORWARD)
				movelocalpoint(-patlenadd, &curoff, &curline);
			else
				movelocalpoint(patlenadd + 1, &curoff, &curline);

			matchline = curline;
			matchoff  = curoff;

			if (liteq(&curline, &curoff, direct, patrn) == 0)
				goto fail;

			/* A SUCCESSFULL MATCH!!!
			 * Flag that we have moved and reset the
			 * global "." pointers.
			 */
			curwp->w_markp[SEARCH_HIGHLIGHT] = matchline;
			curwp->w_marko[SEARCH_HIGHLIGHT] = matchoff;
			curwp->w_markp[SEARCH_HIGHLIGHT+1] = curline;
			curwp->w_marko[SEARCH_HIGHLIGHT+1] = curoff;

			curwp->w_flag |= WFMOVE;
			if (beg_or_end == PTEND)	/* at end of string */
			{
				curwp->w_dotp = curline;
				curwp->w_doto = curoff;
			}
			else		/* at beginning of string */
			{
				curwp->w_dotp = matchline;
				curwp->w_doto = matchoff;
			}

			/* If we're heading in reverse, set the "match"
			 * pointers to the start of the string, for savematch().
			 */
			if (direct == REVERSE) {
				matchline = curline;
				matchoff = curoff;
			}

			matchlen = patlenadd + 1;
			if (savematch() == ABORT)
				return ABORT;

			/* Continue scanning if we haven't found
			 * the nth match.
			 */
			if (--repeats <= 0)
				return (TRUE);

fail:;			/* continue to search */
		} while (!fbound(tbl, jump, &curline, &curoff, direct));
	}

	return FALSE;	/* We could not find a match */
}
#endif

/*
 * fbound -- Return information depending on whether we have hit a boundry
 *	(and may therefore search no further) or if a trailing character
 *	of the search string has been found.  See boundry() for search
 *	restrictions.
 */
#if PROTO
int PASCAL NEAR	fbound(DELTA *tbl, int jump, LINE **pcurline, int *pcuroff, int dir)
#else
int PASCAL NEAR	fbound( tbl, jump, pcurline, pcuroff, dir)
DELTA *tbl;
int jump;
LINE **pcurline;
int *pcuroff;
int dir;
#endif
{
	int	curoff;
	LINE	*curline;

	curline = *pcurline;
	curoff = *pcuroff;

	if (dir == FORWARD) {
		while (jump != 0) {
#if WINDOW_MSWIN
			/* to lower overhead, only 1/100 calls to longop */
			if (--o < 0) {
				longop(TRUE);
				o = 100;
			}
#endif
			if (movelocalpoint(jump, &curoff, &curline))
				return (TRUE);	/* hit end of buffer */

			if (curoff == lused(curline))
				jump = tbl->delta[(int) '\r'];
			else
				jump = tbl->delta[(int) lgetc(curline, curoff)];
		}
	}
	else			/* Reverse.*/
	{
		jump++;		/* allow for offset in reverse */
		while (jump != 0) {
#if WINDOW_MSWIN
			/* to lower overhead, only 1/100 calls to longop */
			if (--o < 0) {
				longop(TRUE);
				o = 100;
			}
#endif
			if (movelocalpoint(-jump, &curoff, &curline))
				return (TRUE);	/* hit end of buffer */

			if (curoff == lused(curline))
				jump = tbl->delta[(int) '\r'];
			else
				jump = tbl->delta[(int) lgetc(curline, curoff)];
		}
	}

	*pcurline = curline;
	*pcuroff = curoff;
	return FALSE;
}

/*
 * movelocalpoint -- Take the passed-in offset and LINE pointers, and
 *	adjust them by n characters.  If boundry is hit, leave the pointers
 *	alone and return TRUE.  Otherwise all went well, and return FALSE.
 */
#if PROTO
int PASCAL NEAR movelocalpoint(int n, int *pcuroff, LINE **pcurline)
#else
int PASCAL NEAR movelocalpoint( n, pcuroff, pcurline)
int n;
int *pcuroff;
LINE **pcurline;
#endif
{
	register int spare;
	register int curoff;
	register LINE *curline;

	curline = *pcurline;
	curoff = *pcuroff + n;

	if (n > 0) {
		if (curline == curbp->b_linep)
			return TRUE;		/* hit end of buffer */

		while ((spare = curoff - lused(curline)) > 0) {
			curline = lforw(curline);/* skip to next line */
			curoff = spare - 1;
			if (curline == curbp->b_linep)
				return (TRUE);	/* hit end of buffer */
		}
	}
	else {
		while (curoff < 0) {
			curline = lback(curline);	/* skip back a line */
			curoff += lused(curline) + 1;
			if (curline == curbp->b_linep)
				return (TRUE);	/* hit end of buffer */
		}
	}
	*pcurline = curline;
	*pcuroff = curoff;
	return FALSE;
}

/*
 * make_delta -- Create the delta tables.
 */
#if PROTO
VOID make_delta(char *pstring, DELTA *tbl)
#else
VOID make_delta( pstring, tbl)
char *pstring;
DELTA *tbl;
#endif
{
	int	j, jump_by, ch;

	strcpy(tbl->patrn, pstring);

	jump_by = (int)strlen(pstring);

	for (j = 0; j < HICHAR; j++)
		tbl->delta[j] = jump_by;

	jump_by -= 1;

	/* Now put in the characters contained
	 * in the pattern, duplicating the CASE.
	 */
	for (j = 0; j < jump_by; j++) {
		ch = *pstring++;
		if (is_letter(ch))
			tbl->delta[(unsigned char) chcase(ch)] = jump_by - j;
		tbl->delta[ch] = jump_by - j;
	}

	/* The last character (left over from the loop above) will
	 * have the pattern length, unless there are duplicates of
	 * it.  Get the number to jump from the delta array, and
	 * overwrite with zeroes in delta duplicating the CASE.
	 */
	ch = *pstring;
	tbl->patlen = jump_by;
	tbl->jump = jump_by + tbl->delta[ch];

	if (is_letter(ch))
		tbl->delta[(unsigned char) chcase(ch)] = 0;
	tbl->delta[ch] = 0;
}

/*
 * setjtable -- Settting up search delta forward and delta backward
 *	tables.  The reverse search string and string lengths are
 *	set here, for table initialization and for substitution
 *	purposes.  The default for any character to jump is the
 *	pattern length.
 */
VOID PASCAL NEAR setjtable()
{
	make_delta(pat, &deltapat);
	make_delta(strrev(strcpy((char *)tap, (char *)pat)), &tapatled);
}

/*
 * eq -- Compare two characters.  The "bc" comes from the buffer, "pc"
 *	from the pattern.  If we are not in EXACT mode, fold out the case.
 */
int PASCAL NEAR eq(bc, pc)
register unsigned char bc;
register unsigned char pc;
{
	if ((curwp->w_bufp->b_mode & MDEXACT) == 0) {
		if (is_lower(bc))
			bc = chcase(bc);

		if (is_lower(pc))
			pc = chcase(pc);
	}

	return (bc == pc);
}

/*
 * readpattern -- Read a pattern.  Stash it in apat.  If it is the
 *	search string (which means that the global variable pat[]
 *	has been passed in), create the reverse pattern and the magic
 *	pattern, assuming we are in MAGIC mode (and #defined that way).
 *
 *	Apat is not updated if the user types in an empty line.  If
 *	the user typed an empty line, and there is no old pattern, it is
 *	an error.  Display the old pattern, in the style of Jeff Lomicka.
 *	There is some do-it-yourself control expansion.  Change to using
 *	<META> to delimit the end-of-pattern to allow <NL>s in the search
 *	string.
 */
#if PROTO
int PASCAL NEAR readpattern(char *prompt, char apat[], int srch)
#else
int PASCAL NEAR readpattern( prompt, apat, srch)
char *prompt;
char apat[];
int srch;
#endif
{
	register int	status;
	char		tpat[NPAT+20];

	mlprompt(prompt, apat, sterm);

	/* Read a pattern.  Either we get one,
	 * or we just get the META charater, and use the previous pattern.
	 * Then, if it's the search string, create the delta tables.
	 * *Then*, make the meta-pattern, if we are defined that way.
	 */
	if ((status = nextarg(NULL, tpat, NPAT, sterm)) == TRUE) {
		lastflag &= ~CFSRCH;
		strcpy(apat, tpat);

		if (srch)
			setjtable();
	}
	else if (status == FALSE && apat[0] != 0)	/* Old one */
		status = TRUE;

#if MAGIC
	/* Only make the meta-pattern if in magic mode, since the
	 * pattern in question might have an invalid meta combination.
	 */
	if (status == TRUE)
		if ((curwp->w_bufp->b_mode & MDMAGIC) == 0) {
			mcclear();
			rmcclear();
		}
		else
			status = srch? mcstr(): rmcstr();
#endif
	return (status);
}

/*
 * savematch -- We found the pattern?  Let's save it away.
 */
int PASCAL NEAR savematch()
{
	register int	j;
	REGION		tmpreg;

	if ((patmatch = reroom(patmatch, matchlen + 1)) == NULL) {
		mlabort(TEXT94);
/*			"%%Out of memory" */
		return ABORT;
	}

	tmpreg.r_offset = matchoff;
	tmpreg.r_linep = matchline;
	tmpreg.r_size = matchlen;

#if MAGIC == 0
	regtostr(patmatch, &tmpreg);
#else
	/*
	 * Save the groups.
	 */
	grpmatch[0] = regtostr(patmatch, &tmpreg);

	for (j = 1; j <= group_count; j++)
	{
		group_reg[j].r_size += group_len[j];

		if ((grpmatch[j] = reroom(grpmatch[j], group_reg[j].r_size + 1)) == NULL) {
			mlabort(TEXT94);
/*			"%%Out of memory" */
			return ABORT;
		}
		regtostr(grpmatch[j], &group_reg[j]);
	}
#endif
	return TRUE;
}

/*
 * boundry -- Return information depending on whether we may search no
 *	further.  Beginning of file and end of file are the obvious
 *	cases, but we may want to add further optional boundry restrictions
 *	in future, a' la VMS EDT.  At the moment, just return (TRUE) or
 *	FALSE depending on if a boundry is hit (ouch).
 */
#if PROTO
int PASCAL NEAR boundry(LINE *curline, int curoff, int dir)
#else
int PASCAL NEAR boundry( curline, curoff, dir)
LINE *curline;
int curoff;
int dir;
#endif
{
	register int	border;

	if (dir == FORWARD) {
		border = (curoff == lused(curline)) &&
			 (lforw(curline) == curbp->b_linep);
	}
	else
	{
		border = (curoff == 0) &&
			 (lback(curline) == curbp->b_linep);
	}
	return (border);
}

/*
 * nextch -- retrieve the next/previous character in the buffer,
 *	and advance/retreat the point.
 *	The order in which this is done is significant, and depends
 *	upon the direction of the search.  Forward searches look at
 *	the current character and move, reverse searches move and
 *	look at the character.
 */
#if PROTO
int PASCAL NEAR nextch(LINE **pcurline, int *pcuroff, int dir)
#else
int PASCAL NEAR nextch( pcurline, pcuroff, dir)
LINE **pcurline;
int *pcuroff;
int dir;
#endif
{
	register LINE	*curline;
	register int	curoff;
	register int	c;

	curline = *pcurline;
	curoff = *pcuroff;

	if (dir == FORWARD) {
		if (curoff == lused(curline))		/* if at EOL */
		{
			curline = lforw(curline);	/* skip to next line */
			curoff = 0;
			c = '\r';			/* and return a <NL> */
		}
		else
			c = lgetc(curline, curoff++);	/* get the char */
	}
	else			/* Reverse.*/
	{
		if (curoff == 0) {
			curline = lback(curline);
			curoff = lused(curline);
			c = '\r';
		}
		else
			c = lgetc(curline, --curoff);
	}
	*pcurline = curline;
	*pcuroff = curoff;

	return (c);
}

/*
 * liteq -- compare the string versus the current characters in the line.
 *	Returns 0 (no match) or the number of characters matched.
 */
#if PROTO
int PASCAL NEAR liteq(LINE **curline, int *curpos, int direct, char *lstring)
#else
int PASCAL NEAR liteq( curline, curpos, direct, lstring)
LINE **curline;
int *curpos;
int direct;
char *lstring;
#endif
{
	LINE	*scanline = *curline;
	int	scanpos = *curpos;
	register int	c;
	register int	count = 0;

	while ((c = (unsigned char)(*lstring++)) != '\0') {
		if (!eq(c, nextch(&scanline, &scanpos, direct)))
			return 0;
		count++;
	}

	*curline = scanline;
	*curpos = scanpos;
	return count;
}

#if MAGIC
/*
 * mcstr -- Set up the 'magic' array.  The closure symbol is taken as
 *	a literal character when (1) it is the first character in the
 *	pattern, and (2) when preceded by a symbol that does not allow
 *	closure, such as beginning or end of line symbol, or another
 *	closure symbol.
 *
 *	Coding comment (jmg):  yes, i know i have gotos that are, strictly
 *	speaking, unnecessary.  But right now we are so cramped for
 *	code space that i will grab what i can in order to remain
 *	within the 64K limit.  C compilers actually do very little
 *	in the way of optimizing - they expect you to do that.
 */
int PASCAL NEAR mcstr()
{
	MC	*mcptr, *rtpcm;
	DELTA	*tbl;
	char	*patptr;
	int	pchr;
	int	status = TRUE;
	int	does_closure = FALSE;
	int	mj = 0;
	int	group_stack[MAXGROUPS];
	int	stacklevel = 0;

	/* If we had metacharacters in the MC array previously,
	 * free up any bitmaps that may have been allocated, and
	 * reset magical.
	 */
	if (magical)
		mcclear();

	mcptr = &mcpat[0];
	patptr = (char *)&pat[0];

	while ((pchr = *patptr) && status) {
		switch (pchr) {
			case MC_CCL:
				status = cclmake(&patptr, mcptr);
				magical = TRUE;
				does_closure = TRUE;
				break;

			case MC_BOL:
				/* If the BOL character isn't the
				 * first in the pattern, we assume
				 * it's a literal instead.
				 */
				if (mj != 0)
					goto litcase;

				mcptr->mc_type = BOL;
				magical = TRUE;
				break;

			case MC_EOL:
				/* If the EOL character isn't the
				 * last in the pattern, we assume
				 * it's a literal instead.
				 */
				if (*(patptr + 1) != '\0')
					goto litcase;

				mcptr->mc_type = EOL;
				magical = TRUE;
				break;

			case MC_ANY:
				mcptr->mc_type = ANY;
				magical = TRUE;
				does_closure = TRUE;
				break;

			case MC_CLOSURE:
			case MC_CLOSURE_1:
			case MC_ZEROONE:

				/* Does the closure symbol mean closure here?
				 * If so, back up to the previous element
				 * and indicate it is enclosed.
				 */
				if (does_closure == FALSE)
					goto litcase;
				mj--;
				mcptr--;
				if (pchr == MC_CLOSURE)
					mcptr->mc_type |= CLOSURE;
				else if (pchr == MC_CLOSURE_1)
					mcptr->mc_type |= CLOSURE_1;
				else
					mcptr->mc_type |= ZEROONE;

				magical = TRUE;
				does_closure = FALSE;
				break;

			case MC_ESC:

				/* No break between here and LITCHAR if
				 * the next character is to be taken literally.
				 */
				magical = TRUE;
				pchr = *++patptr;
				if (pchr == MC_GRPBEG) {
					/* Start of a group.  Indicate it, and
					 * set magical.
					 */
					if (++group_count < MAXGROUPS) {
						mcptr->mc_type = GRPBEG;
						mcptr->u.group_no = group_count;
						group_stack[stacklevel++] = group_count;
						does_closure = FALSE;
					}
					else
					{
						mlwrite(TEXT221);
/*							"Too many groups" */
						status = FALSE;
					}
					break;
				}
				else if (pchr == MC_GRPEND) {
					/* If we've no groups to close, assume
					 * a literal character.  Otherwise,
					 * indicate the end of a group.
					 */
					if (stacklevel > 0) {
						mcptr->u.group_no = group_stack[--stacklevel];
						mcptr->mc_type = GRPEND;
						does_closure = FALSE;
						break;
					}
				}
				else if (pchr == MC_BOWRD) {
					mcptr->mc_type = BOWRD;
					does_closure = FALSE;
					break;
				}
				else if (pchr == MC_EOWRD) {
					mcptr->mc_type = EOWRD;
					does_closure = FALSE;
					break;
				}
				else if (pchr == '\0') {
					pchr = '\\';
					--patptr;
				}
			default:
litcase:
				status = litmake(&patptr, mcptr);
				does_closure = TRUE;
				break;
		}		/* End of switch.*/
		mcptr++;
		patptr++;
		mj++;
	}		/* End of while.*/


	/* Close off the meta-string, then set up the reverse array,
	 * if the status is good.
	 *
	 * If the status is not good, free up any bitmaps or strings,
	 * and make mcpat[0].mc_type MCNIL.
	 *
	 * Please note the structure assignment - your compiler may
	 * not like that.
	 */
	mcptr->mc_type = MCNIL;

	if (stacklevel) {
		status = FALSE;
		mlwrite(TEXT222);
/*			"Group not ended"	*/
	}

	if (status) {
		rtpcm = &tapcm[0];
		while (--mj >= 0) {
			*rtpcm = *--mcptr;

			if (rtpcm->mc_type == LITSTRING) {
			   	if ((rtpcm->u.lstring = copystr(mcptr->u.lstring)) == NULL) {
			      		status = FALSE;
			      		break;
			   	}
			   	strrev(rtpcm->u.lstring);
			}

			rtpcm++;
		}
		rtpcm->mc_type = MCNIL;
	}

	if (status) {
#if MAGIC_JUMP_TABLES
		/*
		 * Now see if we can use the fast jump tables instead
		 * of a brute-force string search, if the first or
		 * last meta-character types are strings.
		 */
		if (mcpat[0].mc_type == LITSTRING)
		{
			if ((tbl = (DELTA *) room(sizeof(DELTA))) != NULL)
			{
				make_delta(mcpat[0].u.lstring, tbl);
				free(mcpat[0].u.lstring);
				mcpat[0].u.jmptable = tbl;
				mcpat[0].mc_type = JMPTABLE;
			}
		}
		if (tapcm[0].mc_type == LITSTRING)
		{
			if ((tbl = (DELTA *) room(sizeof(DELTA))) != NULL)
			{
				make_delta(tapcm[0].u.lstring, tbl);
				free(tapcm[0].u.lstring);
				tapcm[0].u.jmptable = tbl;
				tapcm[0].mc_type = JMPTABLE;
			}
		}
#endif
	}
	else
		mcclear();

#if DEBUG_SEARCH
	mc_list(0,0);
#endif
	return (status);
}

/*
 * mcclear -- Free up any CCL bitmaps, and MCNIL the MC search arrays.
 */
VOID PASCAL NEAR mcclear()
{
	register MC	*mcptr;
	register int	j;

	/*
	 * Free up any memory allocated for the meta-characters:
	 * bitmaps, strings, or the DELTA jmptables.
	 */
	mcptr = &mcpat[0];
	while (mcptr->mc_type != MCNIL) {
		if ((mcptr->mc_type == CCL) || (mcptr->mc_type == NCCL))
			free(mcptr->u.cclmap);
		else if (mcptr->mc_type == LITSTRING)
			free(mcptr->u.lstring);
		else if (mcptr->mc_type == JMPTABLE)
			free(mcptr->u.jmptable);
		mcptr++;
	}

	/*
	 * Do the same for the reverse pattern, with the exception of
	 * of the bitmaps.  The reverse pattern simply 'borrowed'
	 * the forward pattern's bitmaps, which by now have been freed.
	 */
	mcptr = &tapcm[0];
	while (mcptr->mc_type != MCNIL) {
		if (mcptr->mc_type == LITSTRING)
			free(mcptr->u.lstring);
		else if (mcptr->mc_type == JMPTABLE)
			free(mcptr->u.jmptable);
		mcptr++;
	}
	mcpat[0].mc_type = tapcm[0].mc_type = MCNIL;

	/*
	 * Remember that grpmatch[0] == patmatch.
	 */
	for (j = 0; j < MAXGROUPS; j++) {
		if (grpmatch[j] != NULL) {
			free(grpmatch[j]);
			grpmatch[j] = NULL;
		}
	}
	patmatch = NULL;
	group_count = 0;
	magical = FALSE;
}

/*
 * mceq -- meta-character equality with a character.  In Kernighan & Plauger's
 *	Software Tools, this is the function omatch(), but i felt there were
 *	too many functions with the 'match' name already.
 */
#if PROTO
int PASCAL NEAR	mceq(unsigned char bc, MC *mt)
#else
int PASCAL NEAR	mceq( bc, mt)
unsigned char bc;
MC *mt;
#endif
{
	register int result;

	switch (mt->mc_type & MASKCLO) {
		case LITCHAR:
			result = (unsigned char) eq(bc, (unsigned char) mt->u.lchar);
			break;

		case ANY:
			result = (bc != '\r');
			break;

		case CCL:
			if (!(result = biteq(bc, mt->u.cclmap))) {
				if ((curwp->w_bufp->b_mode & MDEXACT) == 0 &&
				    (is_letter(bc)))
					result = biteq(chcase(bc), mt->u.cclmap);
			}
			break;

		case NCCL:
			result = !biteq(bc, mt->u.cclmap);

			if ((curwp->w_bufp->b_mode & MDEXACT) == 0 &&
			    (is_letter(bc)))
				result &= !biteq(chcase(bc), mt->u.cclmap);

			break;

		default:
			mlwrite(TEXT95, mt->mc_type);
/*				"%%mceq: what is %d?" */
			result = FALSE;
			break;
	}	/* End of switch.*/

	return (result);
}

/*
 * cclmake -- create the bitmap for the character class.
 *	ppatptr is left pointing to the end-of-character-class character,
 *	so that a loop may automatically increment with safety.
 */
#if PROTO
int PASCAL NEAR	cclmake(char **ppatptr, MC *mcptr)
#else
int PASCAL NEAR	cclmake( ppatptr, mcptr)
char **ppatptr;
MC *mcptr;
#endif
{
	EBITMAP		bmap;
	register char	*patptr;
	register int	pchr, ochr;

	if ((bmap = (EBITMAP) room(BMAPSIZE)) == NULL) {
		mlabort(TEXT94);
/*			"%%Out of memory" */
		mcptr->mc_type = MCNIL;
		return FALSE;
	}

	memset(bmap, 0, BMAPSIZE);

	mcptr->u.cclmap = bmap;
	patptr = *ppatptr;
	ochr = MC_CCL;

	/*
	 * Test the initial character(s) in ccl for
	 * special cases - negate ccl, or an end ccl
	 * character as a first character.  Anything
	 * else gets set in the bitmap.
	 */
	if (*++patptr == MC_NCCL) {
		patptr++;
		mcptr->mc_type = NCCL;
	}
	else
		mcptr->mc_type = CCL;

	if ((pchr = *patptr) == MC_ECCL) {
		mlwrite(TEXT96);
/*			"%%No characters in character class" */
		free(bmap);
		return FALSE;
	}

	while (pchr != MC_ECCL && pchr != '\0') {
		switch (pchr) {
			/* Range character loses its meaning if it is
			 * the first or last character in the class.  We
			 * also treat it as un-ordinary if the order is
			 * wrong, e.g. "z-a".
			 */
			case MC_RCCL:
				pchr = *(patptr + 1);
				if (ochr == MC_CCL || pchr == MC_ECCL ||
				    ochr > pchr)
					setbit(MC_RCCL, bmap);
				else
				{
					do {
						setbit(++ochr, bmap);
					} while (ochr < pchr);
					patptr++;
				}
				break;

			/* Note: no break between case MC_ESC and the default.
			 */
			case MC_ESC:
				pchr = *++patptr;
			default:
				setbit(pchr, bmap);
				break;
		}
		ochr = pchr;
		pchr = *++patptr;
	}

	*ppatptr = patptr;

	if (pchr == '\0') {
		mlwrite(TEXT97);
/*			"%%Character class not ended" */
		free(bmap);
		return FALSE;
	}
	return TRUE;
}

/*
 * litmake -- create the literal string from the collection of characters.
 *	If there is only one character in the collection, then no memory
 *	needs to be allocated.
 *
 *	Please Note:  If new meta-characters are added (see estruct.h) then
 *	you will also need to update this function!
 */
#if PROTO
int PASCAL NEAR	litmake(char **ppatptr, MC *mcptr)
#else
int PASCAL NEAR	litmake( ppatptr, mcptr)
char **ppatptr;
MC *mcptr;
#endif
{
	char	collect[NPAT + 1];
	int	collect_len;
	register int	pchr;
	register char	*patptr;

	/*
	 * The reason this function was called was because a literal
	 * character was encountered, so collect it immediately.
	 */
	collect[0] = *(patptr = *ppatptr);
	collect_len = 1;

	/*
	 * Now loop through the pattern, collecting characters until
	 * we run into a meta-character.
	 */
	while (pchr = *++patptr)
	{
		/*
		 * If the current character is a closure character,
		 * then the previous character cannot be part of the
		 * collected string (it will be modified by closure).
		 * Back up one, if it is not solo.
		 */
		if (pchr == MC_CLOSURE || pchr == MC_CLOSURE_1 ||
		    pchr == MC_ZEROONE)
		{
			if (collect_len > 1) {
				collect_len--;
				patptr--;
			}
			break;
		}

		/*
		 * The single-character meta-characters...
		 */
		if (pchr == MC_ANY || pchr == MC_CCL ||
                    pchr == MC_BOL || pchr == MC_EOL)
                	break;

		/*
		 * See if an escaped character is part of a meta-character
		 * or not.  If not, then advance the pointer to collect the
		 * next character, if there is a next character.
		 */
		if (pchr == MC_ESC) {
			pchr = *(patptr + 1);

			if (pchr == MC_GRPBEG || pchr == MC_GRPEND ||
			    pchr == MC_BOWRD || pchr == MC_EOWRD)
				break;

			if (pchr != '\0')
				patptr++;
			magical = TRUE;
		}

		collect[collect_len++] = *patptr;
	}

	/*
	 * Finished collecting characters, so either make a string out
	 * of them, or a simple character.
	 */
	if (collect_len == 1) {
		mcptr->u.lchar = collect[0];
		mcptr->mc_type = LITCHAR;
	}
	else
	{
		collect[collect_len] = '\0';
		if ((mcptr->u.lstring = copystr(collect)) == NULL)
			mcptr->mc_type = MCNIL;
		else
			mcptr->mc_type = LITSTRING;
	}
	/*
	 * Back up one so that the calling function will
	 * increment onto the character we halted on.
	 */
	*ppatptr = patptr -1;
	return (mcptr->mc_type != MCNIL);
}

/*
 * biteq -- is the character in the bitmap?
 */
#if PROTO
int PASCAL NEAR	biteq(int bc, EBITMAP cclmap)
#else
int PASCAL NEAR	biteq( bc, cclmap)
int bc;
EBITMAP cclmap;
#endif
{
	if ((unsigned)bc >= HICHAR)
		return FALSE;

	return ( (*(cclmap + (bc >> 3)) & BIT(bc & 7))? TRUE: FALSE );
}

/*
 * setbit -- Set a bit (ON only) in the bitmap.
 */
#if PROTO
VOID PASCAL NEAR setbit(int bc, EBITMAP cclmap)
#else
VOID PASCAL NEAR setbit( bc, cclmap)
int bc;
EBITMAP cclmap;
#endif
{
	if ((unsigned)bc < HICHAR)
		*(cclmap + (bc >> 3)) |= BIT(bc & 7);
}
#endif

#if DEBUG_SEARCH

#if PROTO
int PASCAL NEAR mc_list(int f, int n)
#else
int PASCAL NEAR mc_list( f, n)
int f;
int n;
#endif
{
	MC	*mcptr;
	BUFFER *patbuf; 	/* buffer containing pattern list */
	char pline[NPAT*2]; 	/* text buffer to hold current line */
	char cstr[2];		/* to turn single characters into strings.*/
	int status;		/* return status from subcommands */
	int j;

	/* prepare and clear the buffer holding the meta-character list */
	patbuf = bfind("[Debug Search Metacharacters]", TRUE, BFINVS);
	if (patbuf == NULL) {
		mlwrite(TEXT137);
/*		"Cannot create buffer" */
		return FALSE;
	}

	patbuf->b_mode |= MDVIEW;
	if ((status = bclear(patbuf)) != TRUE) 	/* Blow old text away	*/
		return(status);

	/* add in the header text */
	strcpy(pline, "         Pattern = \"");
	strcat(pline, pat);
	strcat(pline, "\"  (");
	strcat(pline, int_asc(deltapat.jump));
	strcat(pline, ", ");
	strcat(pline, int_asc(deltapat.patlen));
	strcat(pline, ")");

	/* scan through the regular expression pattern */
	cstr[0] = cstr[1] = '\0';
	mcptr = &mcpat[0];
	for (j = 0; j < 2; j++)
	{
		if (addline(patbuf, " ") == FALSE
		|| addline(patbuf, pline) == FALSE
		|| addline(patbuf,  "-----------------------") == FALSE
		|| addline(patbuf, "Closure         MC Type") == FALSE
		|| addline(patbuf,  "-----------------------") == FALSE)
			return(FALSE);

		while (mcptr->mc_type != MCNIL) {

			if ((mcptr->mc_type) & CLOSURE)
				strcpy(pline, "Zero to many    ");
			else if ((mcptr->mc_type) & CLOSURE_1)
				strcpy(pline, "One to many     ");
			else if ((mcptr->mc_type) & ZEROONE)
				strcpy(pline, "Optional        ");
			else
				strcpy(pline, "                ");

			/* next, the meta-character type */
			mctype_cat(pline, (mcptr->mc_type) & MASKCLO);

			/* and some additional information */
			switch ((mcptr->mc_type) & MASKCLO) {
				case JMPTABLE:
					strcat(pline, "\"");
					strcat(pline, mcptr->u.jmptable->patrn);
					strcat(pline, "\"  (");
					strcat(pline, int_asc(mcptr->u.jmptable->jump));
					strcat(pline, ", ");
					strcat(pline, int_asc(mcptr->u.jmptable->patlen));
					strcat(pline, ")");
					break;

				case LITSTRING:
					strcat(pline, "\"");
					strcat(pline, mcptr->u.lstring);
					strcat(pline, "\"");
					break;

				case LITCHAR:
					cstr[0] =  mcptr->u.lchar;
					strcat(pline, "\"");
					strcat(pline, cstr);
					strcat(pline, "\"");
					break;

				case GRPBEG:
					cstr[0] = mcptr->u.group_no + '0';
					strcat(pline, cstr);
					break;

				case GRPEND:
					cstr[0] = mcptr->u.group_no + '0';
					strcat(pline, cstr);
					break;
			}

			/* terminate and add the built line into the buffer */
			if (addline(patbuf, pline) == FALSE)
				return(FALSE);

			mcptr++;
		}
		/* add in the header text */
		strcpy(pline, " Reverse Pattern = \"");
		strcat(pline, tap);
		strcat(pline, "\"  (");
		strcat(pline, int_asc(tapatled.jump));
		strcat(pline, ", ");
		strcat(pline, int_asc(tapatled.patlen));
		strcat(pline, ")");

		mcptr = &tapcm[0];
	}
	return(wpopup(patbuf));
}

#if PROTO
int PASCAL NEAR rmc_list(int f, int n)
#else
int PASCAL NEAR rmc_list( f, n)
int f;
int n;
#endif
{
	RMC	*rmcptr;
	BUFFER *patbuf; 	/* buffer containing pattern list */
	char pline[NPAT+32]; 	/* text buffer to hold current line */
	char cstr[2];		/* to turn single characters into strings.*/
	int status;		/* return status from subcommands */

	/* prepare and clear the buffer holding the meta-character list */
	patbuf = bfind("[Debug Replace Metacharacters]", TRUE, BFINVS);
	if (patbuf == NULL) {
		mlwrite(TEXT137);
/*		"Cannot create buffer" */
		return FALSE;
	}

	patbuf->b_mode |= MDVIEW;
	if ((status = bclear(patbuf)) != TRUE) 	/* Blow old text away	*/
		return(status);

	/* add in the header text */
	strcpy(pline, "Replacement Pattern = \"");
	strcat(pline, rpat);
	strcat(pline, "\"");

	/* scan through the regular expression pattern */
	cstr[0] = cstr[1] = '\0';
	rmcptr = &rmcpat[0];

	if (addline(patbuf, " ") == FALSE
	|| addline(patbuf, pline) == FALSE
	|| addline(patbuf,  "-----------------------") == FALSE
	|| addline(patbuf,  "RMC Type") == FALSE
	|| addline(patbuf,  "-----------------------") == FALSE)
		return(FALSE);

	while (rmcptr->mc_type != MCNIL) {
		mctype_cat(pline, rmcptr->mc_type);

		/* and some additional information */
		switch (rmcptr->mc_type) {
			case LITSTRING:
				strcat(pline, "\"");
				strcat(pline, rmcptr->u.rstr);
				strcat(pline, "\"");
				break;

			case GROUP:
				cstr[0] = rmcptr->u.group_no + '0';
				strcat(pline, cstr);
				break;
		}

		/* terminate and add the built line into the buffer */
		if (addline(patbuf, pline) == FALSE)
			return(FALSE);

		rmcptr++;
	}
	return(wpopup(patbuf));
}

#if PROTO
VOID PASCAL NEAR mctype_cat(char pline[], int mc_type)
#else
VOID PASCAL NEAR mctype_cat( pline, mc_type)
char pline[];
int mc_type;
#endif
{
	switch (mc_type) {
		case JMPTABLE:
			strcat(pline, "JMPTABLE   ");
			break;

		case LITSTRING:
			strcat(pline, "LITSTRING  ");
			break;

		case LITCHAR:
			strcat(pline, "LITCHAR    ");
			break;

		case ANY:
			strcat(pline, "ANY        ");
			break;

		case CCL:
			strcat(pline, "CCL        ");
			break;

		case NCCL:
			strcat(pline, "NCCL       ");
			break;

		case BOL:
			strcat(pline, "BOL        ");
			break;

		case EOL:
			strcat(pline, "EOL        ");
			break;

		case BOWRD:
			strcat(pline, "BOWORD     ");
			break;

		case EOWRD:
			strcat(pline, "EOWORD     ");
			break;

		case GRPBEG:
			strcat(pline, "GRPBEG     ");
			break;

		case GRPEND:
			strcat(pline, "GRPEND     ");
			break;

		case GROUP:
			strcat(pline, "GROUP      ");
			break;

		case DITTO:
			strcat(pline, "DITTO      ");
			break;

		default:
			strcat(pline, "Unknown type");
			break;
	}
}
#endif
