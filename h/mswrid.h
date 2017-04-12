/* This file provides ID definitions for the resource scripts
   for support of operation under the Microsoft Windows environment on
   an IBM-PC or compatible computer.

   Must be compiled with the Resource Compiler or Borland C++ 2.0
   or MSC 6.0 or later versions
    
   It should not be compiled if the WINDOW_MSWIN symbol is not set */

/* system menus extra items */
#define SC_OPTIMIZE     0x1000
   
/* ABOUT dialog box control IDs */
#define ID_PROGVER      200
#define ID_MOREABOUT    205
#define ID_ABOUTBAR     206

/* FILENAME dialog box control IDs */
#define ID_FILENAME     210
#define ID_PATH         215
#define ID_FILES        220
#define ID_DIRECTORIES  225

/* MODES dialog box control IDs */
#define ID_WRAP         281
#define ID_CMODE        282
#define ID_SPELL        283     /* not implemented yet */
#define ID_EXACT        284
#define ID_VIEW         285
#define ID_OVER         286
#define ID_REP          287
#define ID_MAGIC        288
#define ID_CRYPT        289
#define ID_ASAVE        290

/* FONTS dialog box control IDs */
#define ID_FONT         300
#define ID_FONTSIZE     320
#define ID_BOLD         331
#define ID_ANSI         341
#define ID_1ST_CHARSET  ID_ANSI
#define ID_OEM          342
#define ID_LAST_CHARSET ID_OEM
#define ID_MAXCOLUMNS   351
#define ID_MAXROWS      352
#define ID_SAMPLE       361
#define ID_SAVEFONT     370

/* MLHIST dialog box control IDs */
#define ID_HIST         390

/* menu codes (the bulk of them is in mswmenu.h) */
#define IDM_FIRSTCHILD	0xE000	/* for MDI window control list */
