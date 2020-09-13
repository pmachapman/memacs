/*	Code to handle Abbreviation Expansions
	for MicroEMACS 4.00
	(C)Copyright 1995 by Daniel M. Lawrence
*/

#include	<stdio.h>
#include	"estruct.h"
#include	"eproto.h"
#include	"edef.h"
#include	"elang.h"

VOID PASCAL NEAR ab_save(c)

char c;		/* character to add to current word buffer */

{
	char *s;	/* ptr to cycle chars */

	/* only in ABBREV mode */
	if ((curbp->b_mode & MDABBR) == 0)
		return;

	/* is the buffer full? */
	if (ab_pos == ab_end) {
		/* shift all the letters down one */
		s = ab_word;
		while (s < ab_end) {
			*s = *(s+1);
			++s;
		}
		ab_pos--;
	}

	/* add the character */
	*ab_pos++ = c;
	*ab_pos = 0;
}

VOID PASCAL NEAR ab_expand()

{
	char *exp;	/* expansion of current symbol */
	char c;		/* current character to insert */
	
	/* only in ABBREV mode, never in VIEW mode */
	if ((curbp->b_mode & MDABBR) == 0 ||
	    (curbp->b_mode & MDVIEW) == MDVIEW)
		return;

	/* is the current buffer a symbol in the abbreviation table? */
	if ((exp = ab_lookup(ab_word)) != NULL) {

		/* backwards delete the symbol */
		ldelete(-((long)strlen(ab_word)), FALSE);

		/* and insert its expansion */
		while (*exp) {

			c = *exp++;

			/*
			 * If a space was typed, fill column is defined, the
			 * argument is non-negative, wrap mode is enabled, and
			 * we are now past fill column, perform word wrap.
			 */
			if (c == ' ' && (curwp->w_bufp->b_mode & MDWRAP) &&
			    fillcol > 0 &&
			    getccol(FALSE) > fillcol)
				execkey(&wraphook, FALSE, 1);

			linsert(1, c);
		}

		/* ring the bell */
		if (ab_bell)
			TTbeep();
	}

	/* reset the word buffer */
	ab_pos = ab_word;
	*ab_pos = 0;
}

/* add a new abbreviation */

int PASCAL NEAR add_abbrev(f, n)

int f, n;	/* numeric flag and argument */

{
	register int status;	/* status return */
	char sym_name[MAXSYM+1];/* name of symbol to fetch */
	char value[NSTRING];	/* value to set symbol to */

	/* first get the symbol name */
	status = mlreply(TEXT231, sym_name, MAXSYM + 1);
/*			 "Abbreviation to set: " */
	if (status != TRUE)
		return(status);

	/* get the value for that expansion */
	if (f == TRUE)
		strcpy(value, int_asc(n));
	else {
		status = mlreply(TEXT53, &value[0], NSTRING);
/*				 "Value: " */
		if (status == ABORT)
			return(status);
	}

	/* and add the abbreviation to the list */
	return(ab_insert(sym_name, value));
}

/* Delete a single abbreviation */

int PASCAL NEAR del_abbrev(f, n)

int f, n;	/* numeric flag and argument */

{
	register int status;	/* status return */
	char sym_name[MAXSYM+1];/* name of symbol to fetch */

	/* first get the symbol name */
	status = mlreply(TEXT232, sym_name, MAXSYM + 1);
/*			 "Abbreviation to delete: " */
	if (status != TRUE)
		return(status);

	/* and yank the abbreviation to the list */
	return(ab_delete(sym_name));
}

/* Kill all abbreviations */

int PASCAL NEAR kill_abbrevs(f, n)

int f, n;	/* numeric flag and argument */

{
	/* kill them! */
	return(ab_clean());
}

int PASCAL NEAR desc_abbrevs(f, n)

int f, n;	/* numeric flag and argument */

{
	register BUFFER *abbbuf;/* buffer to put abbreviation list into */
	register ABBREV *cur_node;/* ptr to current abbreviation */
	char outseq[NSTRING];	/* output buffer for keystroke sequence */

	/* and get a buffer for it */
	abbbuf = bfind(TEXT234, TRUE, BFINVS);
/*		   "Abbreviation list" */
	if (abbbuf == NULL || bclear(abbbuf) == FALSE) {
		mlwrite(TEXT235);
/*			"Can not display abbreviation list" */
		return(FALSE);
	}

	/* let us know this is in progress */
	mlwrite(TEXT233);
/*		"[Building Abbreviation list]" */

	/* build the abbreviation list */
	cur_node = ab_head;
	while (cur_node != (ABBREV *)NULL) {

		/* add in the abbreviation symbol name */
		strcpy(outseq, cur_node->ab_sym);
		pad(outseq, 20);

		/* add it's expansion */
		strncat(outseq, cur_node->ab_exp, NSTRING - 20);
		outseq[NSTRING - 1] = 0;

		/* and add it as a line into the buffer */
		if (addline(abbbuf, outseq) != TRUE)
			return(FALSE);

		cur_node = cur_node->ab_next;
	}

	/* display the list */
	wpopup(abbbuf);
	mlerase();	/* clear the mode line */
	return(TRUE);
}

/* insert a list of all the current abbreviations into the current buffer */

int PASCAL NEAR ins_abbrevs(f, n)

int f, n;	/* numeric flag and argument */

{
	register ABBREV *cur_node;/* ptr to current abbreviation */

	/* insert the abbreviation list in the current buffer */
	cur_node = ab_head;
	while (cur_node != (ABBREV *)NULL) {

		/* insert the abbreviation symbol as a line */
		if (addline(curbp, cur_node->ab_sym) != TRUE)
			return(FALSE);

		/* now a line with the expansion */
		if (addline(curbp, cur_node->ab_exp) != TRUE)
			return(FALSE);

		cur_node = cur_node->ab_next;
	}

	return(TRUE);
}

int PASCAL NEAR def_abbrevs(f, n)

int f,n;	/* prefix flag and argument */

{
	register BUFFER *bp;	/* ptr to buffer to dump */
	register LINE *lp;	/* ptr to current line in our buffer */
	register int llength;	/* length of the current line being examined */
	char cur_sym[MAXSYM+1];	/* current symbol being defined */
	char cur_exp[NSTRING];	/* current expansion */

	/* get the buffer to load abbreviations from */
	bp = getdefb();
	bp = getcbuf(TEXT236, bp ? bp->b_bname : mainbuf, TRUE);
/*		     "Define Abbreviations from buffer" */
	if (bp == NULL)
		return(ABORT);

	/* step through the buffer */
	lp = lforw(bp->b_linep);
	while (lp != bp->b_linep) {

		/* get a symbol name */
		llength = lused(lp);
		if (llength > MAXSYM)
			llength = MAXSYM;
		strncpy(cur_sym, ltext(lp), llength);
		cur_sym[llength] = 0;

		/* advance to the next line in the buffer */
		lp = lforw(lp);
		if (lp == bp->b_linep)
			break;

		/* and an expansion for that symbol */
		llength = lused(lp);
		if (llength > MAXSYM)
			llength = MAXSYM;
		strncpy(cur_exp, ltext(lp), llength);
		cur_exp[llength] = 0;

		/* add it to the current abbreviation list */
		ab_insert(cur_sym, cur_exp);

		/* on to the next pair */
		lp = lforw(lp);
	}

	return(TRUE);
}

VOID PASCAL NEAR ab_init()

{
	ab_head = (ABBREV *)NULL; /* abbreviation list empty */
 	ab_bell = FALSE;	/* no ringing please! */
	ab_cap = FALSE;		/* don't match capitalization on expansion */
	ab_quick = FALSE;	/* no aggressive expansion */
	ab_pos = ab_word;	/* no word accumulated yet */
	ab_end = &ab_word[NSTRING - 1];	/* ptr to detect end of this buffer */
}

/* ab_insert:	Insert a <sym> in the abbreviation list defined as
		<expansion>
*/

int PASCAL NEAR ab_insert(sym, expansion)

char *sym;		/* symbol to expand */
char *expansion;	/* string to expand to */

{
	ABBREV *new_node;	/* pointer to the newly allocated node */
	ABBREV *cur_node;	/* pointer to travel down list */

	/* nothing longer than MAXSYM please */
	if (strlen(sym) > MAXSYM)
		sym[MAXSYM + 1] = 0;

	/* is this already defined? */
	if (ab_lookup(sym) != NULL)
		ab_delete(sym);

	/* allocate a new node to hold abbreviation */
	new_node = (ABBREV *)room(sizeof(ABBREV)+strlen(expansion)+1);
	if (new_node == NULL)
		return(FALSE);

	/* copy data to that node */
	strcpy(new_node->ab_sym, sym);
	strcpy(new_node->ab_exp, expansion);

	/* do we have an empty list */
	if (ab_head == NULL) {

		ab_head = new_node;
		new_node->ab_next = NULL;

	} else {

		/* does our new node go before the first */
		if (strcmp(sym, ab_head->ab_sym) < 0) {

			/* insert the node before the first node */
			new_node->ab_next = ab_head;
			ab_head = new_node;

		} else {

			/* search for the right place to insert */
			cur_node = ab_head;
			while (cur_node->ab_next != NULL) {

				if (strcmp(sym, cur_node->ab_next->ab_sym) < 0) {

					/* insert after cur_node */
					new_node->ab_next = cur_node->ab_next;
					cur_node->ab_next = new_node;
					return(TRUE);
				}

				/* step to the next node */
				cur_node = cur_node->ab_next;
			}

			/* insert after the last node */
			cur_node->ab_next = new_node;
			new_node->ab_next = NULL;
		}
	}

	return(TRUE);
}

/* ab_lookup:	look up and return the expansion of <sym>.
		Return a NULL if it is not in the list
*/

char *PASCAL NEAR ab_lookup(sym)

char *sym;	/* name of the symbol to look up */

{

	ABBREV *cur_node;	/* ptr to look through list */

	/* starting at the head, step through the list */
	cur_node = ab_head;
	while (cur_node != NULL) {

		/* if there is a match, return the expansion */
		if (strcmp(sym,cur_node->ab_sym) == 0) {
			return(cur_node->ab_exp);
		}
		cur_node=cur_node->ab_next;
	}

	/* at the end, return NULL */
	return(NULL);
}

/* ab_delete:	Delete <sym> from the abbreviation list */

int PASCAL NEAR ab_delete(sym)

char *sym;

{

	ABBREV *cur_node,*previous;	/* ptr to look through list */

	/* start at beginning */
	previous=NULL;
	cur_node=ab_head;

	/* step through the list */
	while(cur_node!=NULL) {

		/* if there is a match, delete the node */
		if (previous == NULL && strcmp(sym,cur_node->ab_sym) == 0) {

			/*important: resets our head pointer*/
			ab_head=cur_node->ab_next;
			free(cur_node);
			return(TRUE);

		} else if (strcmp(sym,cur_node->ab_sym) == 0
			    && cur_node != NULL) {
			previous->ab_next=NULL;
			free(cur_node);
			return(TRUE);
		}

		/*makes sure our previous pointer steps with cur_node*/
		previous = cur_node;
		cur_node = cur_node->ab_next;
	}

	/* at the end, no match to delete */
	return(FALSE);
}

int PASCAL NEAR ab_clean()

{

	ABBREV *cur_node;	/* ptr to look through list */
	ABBREV *next;		/* ptr to next abbreviation */

	/* start at the beginning, */
	cur_node = ab_head;

	/* cycle through the list */
	while (cur_node != (ABBREV *)NULL) {
		next = cur_node->ab_next;
		free(cur_node);
		cur_node = next;
	}

	/* and re-init the list */
	ab_head = (ABBREV *)NULL;
	return(TRUE);
}
