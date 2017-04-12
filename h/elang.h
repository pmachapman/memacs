/*	ELANG.H:	Language specific definitions for
			MicroEMACS 3.12
			(C)opyright 1993 by Daniel Lawrence
*/

#if	ENGLISH
#include	"english.h"
#define	LANGUAGE "english"
#endif
#if	FRENCH
#if BSD || USG || SMOS || HPUX8 || HPUX9 || SUN || XENIX || AVIION
#include	"frenchis.h"	/* ISO version */
#else
#include	"french.h"	/* IBM-PC char set */
#endif
#define	LANGUAGE "french"
#endif
#if	SPANISH
#include	"spanish.h"
#define	LANGUAGE "spanish"
#endif
#if	GERMAN
#include	"german.h"
#define	LANGUAGE "german"
#endif
#if	DUTCH
#include	"dutch.h"
#define	LANGUAGE "dutch"
#endif
#if	PLATIN
#include	"platin.h"
#define	LANGUAGE "pig latin"
#endif
#if	JAPAN
#include	"japan.h"
#define	LANGUAGE "japaneese"
#endif
#if	LATIN
#include	"latin.h"
#define	LANGUAGE "latin"
#endif
