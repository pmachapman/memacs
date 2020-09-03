/*	CHAR.C:	Character handling functions for
		MicroEMACS 4.00
		(C)Copyright 1995 by Daniel Lawrence

		ALL THE CODE HERE IS FOR VARIOUS FORMS OF ASCII AND
		WILL HAVE TO BE MODIFIED FOR EBCDIC

                Unicode support by Jean-Michel Dubois
*/

#include	<stdio.h>
#include	"estruct.h"
#include	"eproto.h"
#include	"edef.h"
#include	"elang.h"

#if	UTF8

/*	is_letter()
		Is the character a letter?  We presume a letter must
	be either in the upper or lower case tables (even if it gets
	translated to itself).
*/

int is_letter(ch)

unsigned int ch;

{
	return(iswalpha(ch));
}

/*	is_lower()
		Is the character a lower case letter?  This looks
	in the lower to uppercase translation table.
*/

int is_lower(ch)

unsigned int ch;

{
	return(iswlower(ch));
}

/*	is_upper()
		Is the character a upper case letter?  This looks
	in the upper to lowercase translation table.
*/

int is_upper(ch)

unsigned int ch;

{
	return(iswupper(ch));
}


/*	chcase()

		Change the case of the current character.
	First check lower and then upper.  If it is not a letter,
	it gets returned unchanged.
*/

unsigned int chcase(ch)
unsigned int ch;
{
	/* translate lower case */
	if (iswlower(ch))
		return(ToWUpper(ch));

	/* translate upper case */
	if (iswupper(ch))
		return(ToWLower(ch));

	/* let the rest pass */
	return(ch);
}

#if	PROTO
unsigned int upperc(unsigned int ch) /* return the upper case equivalant of a character */
#else
int upperc(ch)	/* return the upper case equivalant of a character */

unsigned int ch;	/* character to get uppercase euivalant of */
#endif
{
	if (iswlower(ch))
		return(ToWUpper(ch));
	else
		return(ch);
}

#if	PROTO
unsigned int lowerc(unsigned int ch) /* return the lower case equivalant of a character */
#else
unsigned int lowerc(ch)	/* return the lower case equivalant of a character */

unsigned int ch;	/* character to get lowercase equivalant of */
#endif
{
	if (iswupper(ch))
		return(ToWLower(ch));
	else
		return(ch);
}

#else

/*	is_letter()
		Is the character a letter?  We presume a letter must
	be either in the upper or lower case tables (even if it gets
	translated to itself).
*/

int is_letter(ch)

register char ch;

{
	return(is_upper(ch) || is_lower(ch));
}

/*	is_lower()
		Is the character a lower case letter?  This looks
	in the lower to uppercase translation table.
*/

int is_lower(ch)

register char ch;

{
	return(lowcase[ch & 255] != 0);
}

/*	is_upper()
		Is the character a upper case letter?  This looks
	in the upper to lowercase translation table.
*/

int is_upper(ch)

register char ch;

{
	return(upcase[ch & 255] != 0);
}

/*	chcase()

		Change the case of the current character.
	First check lower and then upper.  If it is not a letter,
	it gets returned unchanged.
*/

unsigned int chcase(ch)
register unsigned int	ch;
{
	/* translate lowercase */
	if (is_lower(ch))
		return(lowcase[ch & 255]);

	/* translate uppercase */
	if (is_upper(ch))
		return(upcase[ch & 255]);

	/* let the rest pass */
	return(ch);
}

#if	PROTO
int upperc(char ch) /* return the upper case equivalant of a character */
#else
int upperc(ch)	/* return the upper case equivalant of a character */

unsigned char ch;	/* character to get uppercase euivalant of */
#endif
{
	if (is_lower(ch))
		return(lowcase[ch & 255]);
	else
		return(ch);
}

#if	PROTO
int lowerc(char ch) /* return the lower case equivalant of a character */
#else
int lowerc(ch)	/* return the lower case equivalant of a character */

unsigned char ch;	/* character to get lowercase equivalant of */
#endif
{
	if (is_upper(ch))
		return(upcase[ch & 255]);
	else
		return(ch);
}

#endif /* UTF8 */

/* change *cp to an upper case character */

int uppercase(cp)

unsigned char *cp;	/* ptr to character to uppercase */

{
#if	UTF8
	/* translate upper case */
	if (is_multibyte_utf8(*cp)) {
		char utf8[6];
		size_t len = strlen((char*) cp);
		unsigned int wc;
		unsigned int bytes = utf8_to_unicode((char*) cp, 0, len, &wc);

		if (iswlower(wc)) {
			wc = ToWUpper(wc);
			/* it's a in place conversion, so do it only if the converted
			 * character has the same number of bytes as the original. */
			if (unicode_to_utf8(wc, utf8) == bytes)
				memcpy(cp, utf8, bytes);
		}

		return bytes;
	}

	if (islower(*cp))
		*cp = toupper(*cp);
#else
	/* translate uppercase */
	if (is_lower(*cp))
		*cp = lowcase[*cp & 255];
#endif
	return 1;
}

/* change *cp to an lower case character */

int lowercase(cp)

unsigned char* cp;	/* ptr to character to lowercase */

{
#if	UTF8
	/* translate lower case */
	if (is_multibyte_utf8(*cp)) {
		char utf8[6];
		size_t len = strlen((char*) cp);
		unsigned int wc;
		unsigned int bytes = utf8_to_unicode((char*) cp, 0, len, &wc);

		if (iswupper(wc)) {
			wc = ToWLower(wc);
			/* it's a in place conversion, so do it only if the converted
			 * character has the same number of bytes as the original. */
			if (unicode_to_utf8(wc, utf8) == bytes)
				memcpy(cp, utf8, bytes);
		}

		return bytes;
	}

	if (isupper(*cp))
		*cp = tolower(*cp);
#else
	/* translate lowercase */
	if (is_upper(*cp))
		*cp = upcase[*cp & 255];
#endif
	return 1;
}

#if ! UTF8

VOID initchars()	/* initialize the character upper/lower case tables */

{
	register int index;	/* index into tables */

	/* all of both tables to zero */
	for (index = 0; index < HICHAR; index++) {
		lowcase[index] = 0;
		upcase[index] = 0;
	}

	/* lower to upper, upper to lower */
	for (index = 'a'; index <= 'z'; index++) {
		lowcase[index] = index ^ DIFCASE;
		upcase[index ^ DIFCASE] = index;
	}
#if BSD || FREEBSD || USG || AIX || AUX || SMOS || HPUX8 || HPUX9 || SUN || XENIX || AVIION
	/* and for those international characters! */
	for (index = (unsigned char)'\340';
	     index <= (unsigned char)'\375'; index++) {
		lowcase[index] = index ^ DIFCASE;
		upcase[index ^ DIFCASE] = index;
	}
#endif /*  BSD || FREEBSD || USG || AIX || AUX || SMOS || HPUX8 || HPUX9 || SUN || XENIX || AVIION */

#if	MSDOS
	/* setup various extended IBM-PC characters */
	upcase[0x80]  = 0x87;	/* C with a cedilla */
	lowcase[0x81] = 0x9a;	/* U with an umlaut */
	lowcase[0x82] = 0x90;	/* E with an acute accent */
	lowcase[0x83] = 0x83;	/* A with a circumflex */
	lowcase[0x84] = 0x8e;	/* A with an umlaut */
	lowcase[0x85] = 0x85;	/* A with a grave accent */
	lowcase[0x86] = 0x8f;	/* A with a circle */
	lowcase[0x87] = 0x80;	/* C with a cedilla */
	lowcase[0x88] = 0x88;	/* E with a circumflex */
	lowcase[0x89] = 0x89;	/* E with an umlaut */
	lowcase[0x8a] = 0x8a;	/* E with a grave accent */
	lowcase[0x8b] = 0x8b;	/* I with an umlaut */
	lowcase[0x8c] = 0x8c;	/* I with a circumflex */
	lowcase[0x8d] = 0x8d;	/* I with a grave accent */
	upcase[0x8e]  = 0x84;	/* A with an umlaut */
	upcase[0x8f]  = 0x86;	/* A with a circle */
	upcase[0x90]  = 0x82;	/* E with an acute accent */
	lowcase[0x91] = 0x92;	/* AE diphthong */
	upcase[0x92]  = 0x91;	/* AE diphthong */
	lowcase[0x93] = 0x93;	/* O with a circumflex */
	lowcase[0x94] = 0x99;	/* O with an umlaut */
	lowcase[0x95] = 0x95;	/* O with an acute accent */
	lowcase[0x96] = 0x96;	/* u with a circumflex */
	lowcase[0x97] = 0x97;	/* U with an grave accent */
	lowcase[0x98] = 0x98;	/* y with an umlaut */
	upcase[0x99]  = 0x94;	/* O with an umlaut */
	upcase[0x9a]  = 0x81;	/* U with an umlaut */
	lowcase[0xa0] = 0xa0;	/* a with an acute accent */
	lowcase[0xa1] = 0xa1;	/* i with an acute accent */
	lowcase[0xa2] = 0xa2;	/* o with an acute accent */
	lowcase[0xa3] = 0xa3;	/* u with an acute accent */
	lowcase[0xa4] = 0xa5;	/* n with a tilde */
	upcase[0xa5]  = 0xa4;	/* N with a tilde */
#endif /* MSDOS */
#if	VMS
	/* setup DEC Multinational Character Set */
	upcase[ 192]  = 224;	/* A with a grave accent */
	upcase[ 193]  = 225;	/* A with an acute accent */
	upcase[ 194]  = 226;	/* A with a circumflex */
	upcase[ 195]  = 227;	/* A with a tilde */
	upcase[ 196]  = 228;	/* A with an umlaut */
	upcase[ 197]  = 229;	/* A with a ring */
	upcase[ 198]  = 230;	/* AE diphthong */
	upcase[ 199]  = 231;	/* C with a cedilla */
	upcase[ 200]  = 232;	/* E with a grave accent */
	upcase[ 201]  = 233;	/* E with an acute accent */
	upcase[ 202]  = 234;	/* E with circumflex */
	upcase[ 203]  = 235;	/* E with an umlaut */
	upcase[ 204]  = 236;	/* I with a grave accent */
	upcase[ 205]  = 237;	/* I with an acute accent */
	upcase[ 206]  = 238;	/* I with circumflex */
	upcase[ 207]  = 239;	/* I with an umlaut */
	upcase[ 209]  = 241;	/* N with a tilde */
	upcase[ 210]  = 242;	/* O with a grave accent */
	upcase[ 211]  = 243;	/* O with an acute accent */
	upcase[ 212]  = 244;	/* O with circumflex */
	upcase[ 213]  = 245;	/* O with a tilde */
	upcase[ 214]  = 246;	/* O with an umlaut */
	upcase[ 215]  = 247;	/* OE ligature */
	upcase[ 216]  = 248;	/* O with a slash */
	upcase[ 217]  = 249;	/* U with a grave accent */
	upcase[ 218]  = 250;	/* U with an acute accent */
	upcase[ 219]  = 251;	/* U with circumflex */
	upcase[ 220]  = 252;	/* U with an umlaut */
	upcase[ 221]  = 253;	/* Y with an umlaut */

	lowcase[ 223]  = 223;	/* German lowercase sharp s */

	lowcase[ 224]  = 192;	/* a with a grave accent */
	lowcase[ 225]  = 193;	/* a with an acute accent */
	lowcase[ 226]  = 194;	/* a with a circumflex */
	lowcase[ 227]  = 195;	/* a with a tilde */
	lowcase[ 228]  = 196;	/* a with an umlaut */
	lowcase[ 229]  = 197;	/* a with a ring */
	lowcase[ 230]  = 198;	/* ae diphthong */
	lowcase[ 231]  = 199;	/* c with a cedilla */
	lowcase[ 232]  = 200;	/* e with a grave accent */
	lowcase[ 233]  = 201;	/* e with an acute accent */
	lowcase[ 234]  = 202;	/* e with circumflex */
	lowcase[ 235]  = 203;	/* e with an umlaut */
	lowcase[ 236]  = 204;	/* i with a grave accent */
	lowcase[ 237]  = 205;	/* i with an acute accent */
	lowcase[ 238]  = 206;	/* i with circumflex */
	lowcase[ 239]  = 207;	/* i with an umlaut */
	lowcase[ 241]  = 209;	/* n with a tilde */
	lowcase[ 242]  = 210;	/* o with a grave accent */
	lowcase[ 243]  = 211;	/* o with an acute accent */
	lowcase[ 244]  = 212;	/* o with circumflex */
	lowcase[ 245]  = 213;	/* o with a tilde */
	lowcase[ 246]  = 214;	/* o with an umlaut */
	lowcase[ 247]  = 215;	/* oe ligature */
	lowcase[ 248]  = 216;	/* o with a slash */
	lowcase[ 249]  = 217;	/* u with a grave accent */
	lowcase[ 250]  = 218;	/* u with an acute accent */
	lowcase[ 251]  = 219;	/* u with circumflex */
	lowcase[ 252]  = 220;	/* u with an umlaut */
	lowcase[ 253]  = 221;	/* y with an umlaut */

#endif /* VMS */
#if	THEOS
	upcase[192] = 193;	/* A with an umlaut */
	lowcase[193] = 192;	/* a with an umlaut */
	lowcase[194] = 194;	/* a with a circumflex */
	lowcase[195] = 195;	/* a with a grave accent */
	lowcase[196] = 196;	/* a with a acute accent */
	upcase[197]  = 201;	/* E with a acute accent */
	lowcase[198] = 198;	/* e with an umlaut */
	lowcase[199] = 199;
	lowcase[200] = 200;
	lowcase[201] = 197;
	lowcase[202] = 202;
	lowcase[203] = 203;
	lowcase[204] = 204;
	lowcase[205] = 205;
	upcase[206]  = 207;
	lowcase[207] = 206;
	lowcase[208] = 208;
	lowcase[209] = 209;
	lowcase[210] = 210;
	upcase[211] = 212;
	lowcase[212] = 211;
	lowcase[213] = 213;
	lowcase[214] = 214;
	lowcase[215] = 215;
	upcase[216]  = 217;
	lowcase[217] = 216;
	upcase[218]  = 219;
	lowcase[219] = 218;
	upcase[220]  = 221;
	lowcase[221] = 220;
	upcase[222]  = 223;
	lowcase[223] = 222;
#endif /* THEOS */
}

/*	Set a character in the lowercase map */

int setlower(ch, val)

char *ch;	/* ptr to character to set */
char *val;	/* value to set it to */

{
	return(lowcase[*ch & 255] = *val & 255);
}

/*	Set a character in the uppercase map */

int setupper(ch, val)

char *ch;	/* ptr to character to set */
char *val;	/* value to set it to */

{
	return(upcase[*ch & 255] = *val & 255);
}

#if (ZTC | TURBO | MSC) == 0
/*
 * strrev -- Reverse string in place.  Code here for those compilers
 *	that do not have the function in their own library.
 */
char *strrev(our_str)
char *our_str;
{
	register char	*beg_str, *end_str;
	register char	the_char;

	end_str = beg_str = our_str;
	end_str += strlen(beg_str);

	do {
		the_char = *--end_str;
		*end_str = *beg_str;
		*beg_str++ = the_char;
	} while (end_str > beg_str);

	return(our_str);
}
#endif /* (ZTC | TURBO | MSC) == 0 */

#else

/*	Set a character in the lowercase map */

int setlower(ch, val)

char *ch;	/* ptr to character to set */
char *val;	/* value to set it to */

{
	unsigned int c;
	unsigned int bytes;
	char utf8[6];

	if (is_multibyte_utf8(*ch) && utf8_to_unicode(ch, 0, strlen(ch), &c) > 1 && iswupper(c)) {
		c = ToWUpper(c);
		bytes = unicode_to_utf8(c, utf8);
		memcpy(ch, utf8, bytes);
		return c;
	}

	return (*ch = toupper(*ch));
}

/*	Set a character in the uppercase map */

int setupper(ch, val)

char *ch;	/* ptr to character to set */
char *val;	/* value to set it to */

{
	unsigned int c;
	unsigned int bytes;
	char utf8[6];

	if (is_multibyte_utf8(*ch) && utf8_to_unicode(ch, 0, strlen(ch), &c) > 1 && iswlower(c)) {
		c = ToWLower(c);
		bytes = unicode_to_utf8(c, utf8);
		memcpy(ch, utf8, bytes);
		return c;
	}

	return (*ch = toupper(*ch));
}

/* strrev -- Reverse UTF-8 string in place. */

char *strrev(char *str)
{
    /* this assumes that str is valid UTF-8 */
    char    *scanl, *scanr, *scanr2, c;

    /* first reverse the string */
    for (scanl = str, scanr = str + strlen(str); scanl < scanr; ) {
        c = *scanl;
        *scanl++ = *--scanr;
        *scanr = c;
    }

    /* then scan all bytes and reverse each multibyte character */
    for (scanl = scanr = str; (c = *scanr++); ) {
        if ((c & 0x80) == 0) // ASCII char
            scanl = scanr;
        else if (is_multibyte_utf8(c)) { // start of multibyte
            scanr2 = scanr;

            switch (scanr - scanl) {
			case 4:
				c = *scanl;
				*scanl++ = *--scanr;
				*scanr = c;
				// no break
			case 3:
				// no break
			case 2:
				c = *scanl;
				*scanl++ = *--scanr;
				*scanr = c;
            }

            scanr = scanl = scanr2;
        }
    }

    return str;
}

#endif /* UTF8 */

#if	DBCS
/* is this character a 2 byte character prefix code? */

int is2byte(sp, cp)

char *sp;	/* ptr to beginning of string containing character to test */
char *cp;	/* ptr to charactor to test */

{
	register char *cc;	/* pointer to current character */

	cc = sp;
	while (*cc) {
		if (cc > cp)
			return(FALSE);
		if (cc == cp)
			return(is2char(*cp));
		if (is2char(*cc))
			++cc;
		++cc;
	}
	return(FALSE);
}
#endif

#if	MDSLINE
unsigned int theoxtosgraph(unsigned int c)
{
	if (curbp->b_mode & MDSLINE) {
		switch (c) {
			case '7': return 0x00A0;
			case '8': return 0x00A7;
			case '9': return 0x00A1;
			case '4': return 0x00A5;
			case '5': return 0x00A4;
			case '6': return 0x00A6;
			case '1': return 0x00A3;
			case '2': return 0x00A8;
			case '3': return 0x00A2;
			case '+': return 0x00AA;
			case '0': return 0x00A9;
		}
	} else if (curbp->b_mode & MDDLINE) {
		switch (c) {
			case '7': return 0x00AF;
			case '8': return 0x00B6;
			case '9': return 0x00B0;
			case '4': return 0x00B4;
			case '5': return 0x00B3;
			case '6': return 0x00B5;
			case '1': return 0x00B2;
			case '2': return 0x00B7;
			case '3': return 0x00B1;
			case '+': return 0x00B9;
			case '0': return 0x00B8;
		}
	}

	return c;
}

unsigned int tosgraph(unsigned int c)
{
#if	UTF8
#if	THEOX
	if (curbp->b_mode & MDTHEOX)
		return theoxtosgraph(c);
	else {
#endif
		if (curbp->b_mode & MDSLINE) {
			switch (c & CMSK) {		// single line box
				case '7': return 0x250c;    // upper left corner
				case '8': return 0x252c;    // upper intersection
				case '9': return 0x2510;    // upper right corner
				case '4': return 0x251c;    // left intersection
				case '5': return 0x253c;    // four ways
				case '6': return 0x2524;    // right intersection
				case '1': return 0x2514;    // lower left corner
				case '2': return 0x2534;    // lower intersection
				case '3': return 0x2518;    // lower right corner
				case '+': return 0x2502;    // vertical
				case '0': return 0x2500;    // horizontal
			}
		} else if (curbp->b_mode & MDDLINE) {   /* double line box */
			switch (c & CMSK) {
				case '7': return 0x2554;    // upper left corner
				case '8': return 0x2566;    // upper intersection
				case '9': return 0x2557;    // upper right corner
				case '4': return 0x2560;    // left intersection
				case '5': return 0x256c;    // four ways
				case '6': return 0x2563;    // right intersection
				case '1': return 0x255a;    // lower left corner
				case '2': return 0x2569;    // lower intersection
				case '3': return 0x255d;    // lower right corner
				case '+': return 0x2551;    // vertical
				case '0': return 0x2550;    // horizontal
			}
		}
#if	THEOX
	}
#endif
#elif THEOS
	return theoxtosgraph(c);
#endif

	return c;
}

#endif
