/*
 * theox.c
 *
 *  Created on: 6 mars 2020
 *      Author: Jean-Michel Dubois
 *
 *      TheoX mode
 *
 *      TheoX mode allows to edit files using TheoX character set.
 *      Keyboard inputs are UTF-8 characters converted to Unicode characters.
 *      Characters are stored in lines converted to TheoX set.
 *      They are converted to Unicode then UTF-8 for output.
 *
 *      THis file contains conversions tables and characters categorization.
 */

#include "estruct.h"

#if	UTF8 && THEOX

#include "eproto.h"
#include "edef.h"

#define _B_ISALPHA	1
#define _B_ISUPPER	2
#define _B_ISLOWER	4
#define _B_ISDIGIT	8
#define _B_ISALNUM	9
#define _B_ISSPACE	16
#define _B_ISSYMBOL	32

typedef struct map {
	wchar_t 	m_wch;
	unsigned char	m_ch;
} Map;

static int compmap(const void* m1, const void* m2)
{
	return ((Map*) m1)->m_wch - ((Map*) m2)->m_wch;
}

static const Map mapTheoX[] = {
	{ 0x00a1, 0xe2 }, { 0x00a2, 0xe3 }, { 0x00a3, 0xe4 }, { 0x00a5, 0xe5 }, { 0x00a7, 0xeb }, { 0x00a9, 0xfc }, { 0x00aa, 0xf0 }, { 0x00ab, 0xbe },
	{ 0x00ae, 0xfd }, { 0x00b0, 0xec }, { 0x00b1, 0xfb }, { 0x00b2, 0xed }, { 0x00b3, 0xee }, { 0x00b5, 0xbc }, { 0x00b6, 0xbd }, { 0x00b9, 0xef },
	{ 0x00ba, 0xf1 }, { 0x00bb, 0xbf }, { 0x00bc, 0xe8 }, { 0x00bd, 0xe9 }, { 0x00bf, 0xe1 }, { 0x00c0, 0x87 }, { 0x00c1, 0x88 }, { 0x00c2, 0x86 },
	{ 0x00c3, 0x89 }, { 0x00c4, 0xc0 }, { 0x00c5, 0xde }, { 0x00c6, 0xdc }, { 0x00c7, 0xd8 }, { 0x00c8, 0x8d }, { 0x00c9, 0xc5 }, { 0x00ca, 0x8c },
	{ 0x00cb, 0x8b }, { 0x00cc, 0x90 }, { 0x00cd, 0x91 }, { 0x00ce, 0x8f }, { 0x00cf, 0x8e }, { 0x00d0, 0xf6 }, { 0x00d1, 0xda }, { 0x00d2, 0x93 },
	{ 0x00d3, 0x94 }, { 0x00d4, 0x92 }, { 0x00d5, 0x95 }, { 0x00d6, 0xce }, { 0x00d7, 0xba }, { 0x00d8, 0x9a }, { 0x00d9, 0x98 }, { 0x00da, 0x99 },
	{ 0x00db, 0x97 }, { 0x00dc, 0xd3 }, { 0x00dd, 0x9e }, { 0x00de, 0xf8 }, { 0x00df, 0xe0 }, { 0x00e0, 0xc3 }, { 0x00e1, 0xc4 }, { 0x00e2, 0xc2 },
	{ 0x00e3, 0x8a }, { 0x00e4, 0xc1 }, { 0x00e5, 0xdf }, { 0x00e6, 0xdd }, { 0x00e7, 0xd9 }, { 0x00e8, 0xc8 }, { 0x00e9, 0xc9 }, { 0x00ea, 0xc7 },
	{ 0x00eb, 0xc6 }, { 0x00ec, 0xcc }, { 0x00ed, 0xcd }, { 0x00ee, 0xcb }, { 0x00ef, 0xca }, { 0x00f0, 0xf7 }, { 0x00f1, 0xdb }, { 0x00f2, 0xd1 },
	{ 0x00f3, 0xd2 }, { 0x00f4, 0xd0 }, { 0x00f5, 0x96 }, { 0x00f6, 0xcf }, { 0x00f7, 0xbb }, { 0x00f8, 0x9b }, { 0x00f9, 0xd6 }, { 0x00fa, 0xd7 },
	{ 0x00fb, 0xd5 }, { 0x00fc, 0xd4 }, { 0x00fd, 0x9f }, { 0x00fe, 0xf9 }, { 0x00fe, 0xfe }, { 0x00ff, 0xea }, { 0x00ff, 0xff }, { 0x0152, 0x9c },
	{ 0x0153, 0x9d }, { 0x0160, 0xf2 }, { 0x0161, 0xf3 }, { 0x0178, 0xfa }, { 0x017d, 0xf4 }, { 0x017e, 0xf5 }, { 0x20a7, 0xe6 }, { 0x20ac, 0xe7 },
	{ 0x2500, 0xa9 }, { 0x2502, 0xaa }, { 0x250c, 0xa0 }, { 0x2510, 0xa1 }, { 0x2514, 0xa3 }, { 0x2518, 0xa2 }, { 0x251c, 0xa5 }, { 0x2524, 0xa6 },
	{ 0x252c, 0xa7 }, { 0x2534, 0xa8 }, { 0x253c, 0xa4 }, { 0x2550, 0xb8 }, { 0x2551, 0xb9 }, { 0x2554, 0xaf }, { 0x2557, 0xb0 }, { 0x255a, 0xb2 },
	{ 0x255d, 0xb1 }, { 0x2560, 0xb4 }, { 0x2563, 0xb5 }, { 0x2566, 0xb6 }, { 0x2569, 0xb7 }, { 0x256c, 0xb3 }, { 0x2588, 0xae }, { 0x2591, 0xab },
	{ 0x2592, 0xac }, { 0x2593, 0xad }
};

// Unicode to TheoX character set conversion

unsigned char _b_wchar2theox(unsigned wch)
{
	Map* pMap;

	if (wch < 0x80)
		return (unsigned char) wch;

	if (wch > 0x85 && (pMap = (Map*) bsearch(&wch, mapTheoX, sizeof(mapTheoX) / sizeof(Map), sizeof(Map), compmap)))
		return pMap->m_ch;

	return '?';
}

// table de translation du jeu de caractères TheoX en Unicode en mode normal

const wchar_t _b_awcTheoX[256] = {
//	0	1	2	3	4	5	6	7	8	9	A	B	C	D	E	F
/*0*/	0x0000,	0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,	0x0008, 0x0009, 0x000a, 0x000b, 0x000c, 0x000d, 0x000e, 0x000f,
/*1*/	0x0010,	0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017,	0x0018, 0x0019, 0x001a, 0x001b, 0x001c, 0x001d, 0x001e, 0x001f,
/*2*/	0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,	0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f,
/*3*/	0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,	0x0038, 0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e, 0x003f,
/*4*/	0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,	0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f,
/*5*/	0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,	0x0058, 0x0059, 0x005a, 0x005b, 0x005c, 0x005d, 0x005e, 0x005f,
/*6*/	0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,	0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f,
/*7*/	0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,	0x0078, 0x0079, 0x007a, 0x007b, 0x007c, 0x007d, 0x007e, 0x2302,
//	Â	À	Á	Ã	ã	Ë	Ê	È	Ï	Î
/*8*/	   '?',    '?',    '?',    '?',    '?',    '?', 0x00c2, 0x00c0, 0x00c1, 0x00c3, 0x00e3, 0x00cb, 0x00ca, 0x00c8, 0x00cf, 0x00ce,
//	Ì	Í	Ô	Ò	Ó	Õ	õ	Û	Ù	Ú	Ø	ø	Œ	œ	Ý	ý
/*9*/	0x00cc, 0x00cd, 0x00d4, 0x00d2, 0x00d3, 0x00d5, 0x00f5, 0x00db, 0x00d9, 0x00da, 0x00d8, 0x00f8, 0x0152, 0x0153, 0x00dd, 0x00fd,
//	┌	┐	┘	└	┼	├	┤	┬	┴	─	│	░	▒	▓	█	╔
/*A*/	0x250c, 0x2510, 0x2518, 0x2514, 0x253c, 0x251c, 0x2524, 0x252c, 0x2534, 0x2500, 0x2502, 0x2591, 0x2592, 0x2593, 0x2588, 0x2554,
//	╗	╝	╚	╬	╠	╣	╦	╩	═	║	×	÷	µ	¶	«	»
/*B*/	0x2557, 0x255d, 0x255a, 0x256c, 0x2560, 0x2563, 0x2566, 0x2569,	0x2550, 0x2551, 0x00d7, 0x00f7, 0x00b5, 0x00b6, 0x00ab, 0x00bb,
//	Ä	ä	â	à	á	É	ë	ê	è	é	ï	î	ì	í	Ö	ö
/*C*/	0x00c4, 0x00e4, 0x00e2, 0x00e0, 0x00e1, 0x00c9, 0x00eb, 0x00ea,	0x00e8, 0x00e9, 0x00ef, 0x00ee, 0x00ec, 0x00ed, 0x00d6, 0x00f6,
//	ô	ò	ó	Ü	ü	û	ù	ú	Ç	ç	Ñ	ñ	Æ	æ	Å	å
/*D*/	0x00f4, 0x00f2, 0x00f3, 0x00dc, 0x00fc, 0x00fb, 0x00f9, 0x00fa,	0x00c7, 0x00e7, 0x00d1, 0x00f1, 0x00c6, 0x00e6, 0x00c5, 0x00e5,
//	ß	¿	¡	¢	£	¥	₧	€	¼	½	ÿ	§	°	²	³	¹
/*E*/	0x00df, 0x00bf, 0x00a1, 0x00a2, 0x00a3, 0x00a5, 0x20a7, 0x20ac,	0x00bc, 0x00bd, 0x00ff, 0x00a7, 0x00b0, 0x00b2, 0x00b3, 0x00b9,
//	ª	º	Š	š	Ž	ž	Ð	ð	Þ	þ	Ÿ	±	©	®
/*F*/	0x00aa, 0x00ba, 0x0160, 0x0161, 0x017d, 0x017e, 0x00d0, 0x00f0,	0x00de, 0x00fe, 0x0178, 0x00b1, 0x00a9, 0x00ae, 0x00fe, 0x00ff
//	0	1	2	3	4	5	6	7	8	9	A	B	C	D	E	F
};

static const unsigned char _b_theox_ctype[256] = {
	/*   0 */	0,
	/*   1 */	0,
	/*   2 */	0,
	/*   3 */	0,
	/*   4 */	0,
	/*   5 */	0,
	/*   6 */	0,
	/*   7 */	0,
	/*   8 */	0,
	/*   9 */	_B_ISSPACE,
	/*  10 */	0,
	/*  11 */	0,
	/*  12 */	0,
	/*  13 */	_B_ISSPACE,
	/*  14 */	0,
	/*  15 */	0,
	/*  16 */	0,
	/*  17 */	0,
	/*  18 */	0,
	/*  19 */	0,
	/*  20 */	0,
	/*  21 */	0,
	/*  22 */	0,
	/*  23 */	0,
	/*  24 */	0,
	/*  25 */	0,
	/*  26 */	0,
	/*  27 */	0,
	/*  28 */	0,
	/*  29 */	0,
	/*  30 */	0,
	/*  31 */	0,
	/*  32 */	_B_ISSPACE,
	/*  33 */	_B_ISSYMBOL,
	/*  34 */	_B_ISSYMBOL,
	/*  35 */	_B_ISSYMBOL,
	/*  36 */	_B_ISSYMBOL,
	/*  37 */	_B_ISSYMBOL,
	/*  38 */	_B_ISSYMBOL,
	/*  39 */	_B_ISSYMBOL,
	/*  40 */	_B_ISSYMBOL,
	/*  41 */	_B_ISSYMBOL,
	/*  42 */	_B_ISSYMBOL,
	/*  43 */	_B_ISSYMBOL,
	/*  44 */	_B_ISSYMBOL,
	/*  45 */	_B_ISSYMBOL,
	/*  46 */	_B_ISSYMBOL,
	/*  47 */	_B_ISSYMBOL,
	/*  48 */	_B_ISDIGIT,
	/*  49 */	_B_ISDIGIT,
	/*  50 */	_B_ISDIGIT,
	/*  51 */	_B_ISDIGIT,
	/*  52 */	_B_ISDIGIT,
	/*  53 */	_B_ISDIGIT,
	/*  54 */	_B_ISDIGIT,
	/*  55 */	_B_ISDIGIT,
	/*  56 */	_B_ISDIGIT,
	/*  57 */	_B_ISDIGIT,
	/*  58 */	_B_ISSYMBOL,
	/*  59 */	_B_ISSYMBOL,
	/*  60 */	_B_ISSYMBOL,
	/*  61 */	_B_ISSYMBOL,
	/*  62 */	_B_ISSYMBOL,
	/*  63 */	_B_ISSYMBOL,
	/*  64 */	_B_ISSYMBOL,
	/*  65 */	_B_ISALPHA|_B_ISUPPER,
	/*  66 */	_B_ISALPHA|_B_ISUPPER,
	/*  67 */	_B_ISALPHA|_B_ISUPPER,
	/*  68 */	_B_ISALPHA|_B_ISUPPER,
	/*  69 */	_B_ISALPHA|_B_ISUPPER,
	/*  70 */	_B_ISALPHA|_B_ISUPPER,
	/*  71 */	_B_ISALPHA|_B_ISUPPER,
	/*  72 */	_B_ISALPHA|_B_ISUPPER,
	/*  73 */	_B_ISALPHA|_B_ISUPPER,
	/*  74 */	_B_ISALPHA|_B_ISUPPER,
	/*  75 */	_B_ISALPHA|_B_ISUPPER,
	/*  76 */	_B_ISALPHA|_B_ISUPPER,
	/*  77 */	_B_ISALPHA|_B_ISUPPER,
	/*  78 */	_B_ISALPHA|_B_ISUPPER,
	/*  79 */	_B_ISALPHA|_B_ISUPPER,
	/*  80 */	_B_ISALPHA|_B_ISUPPER,
	/*  81 */	_B_ISALPHA|_B_ISUPPER,
	/*  82 */	_B_ISALPHA|_B_ISUPPER,
	/*  83 */	_B_ISALPHA|_B_ISUPPER,
	/*  84 */	_B_ISALPHA|_B_ISUPPER,
	/*  85 */	_B_ISALPHA|_B_ISUPPER,
	/*  86 */	_B_ISALPHA|_B_ISUPPER,
	/*  87 */	_B_ISALPHA|_B_ISUPPER,
	/*  88 */	_B_ISALPHA|_B_ISUPPER,
	/*  89 */	_B_ISALPHA|_B_ISUPPER,
	/*  90 */	_B_ISALPHA|_B_ISUPPER,
	/*  91 */	_B_ISSYMBOL,
	/*  92 */	_B_ISSYMBOL,
	/*  93 */	_B_ISSYMBOL,
	/*  94 */	_B_ISSYMBOL,
	/*  95 */	_B_ISSYMBOL,
	/*  96 */	_B_ISSYMBOL,
	/*  97 */	_B_ISALPHA|_B_ISLOWER,
	/*  98 */	_B_ISALPHA|_B_ISLOWER,
	/*  99 */	_B_ISALPHA|_B_ISLOWER,
	/* 100 */	_B_ISALPHA|_B_ISLOWER,
	/* 101 */	_B_ISALPHA|_B_ISLOWER,
	/* 102 */	_B_ISALPHA|_B_ISLOWER,
	/* 103 */	_B_ISALPHA|_B_ISLOWER,
	/* 104 */	_B_ISALPHA|_B_ISLOWER,
	/* 105 */	_B_ISALPHA|_B_ISLOWER,
	/* 106 */	_B_ISALPHA|_B_ISLOWER,
	/* 107 */	_B_ISALPHA|_B_ISLOWER,
	/* 108 */	_B_ISALPHA|_B_ISLOWER,
	/* 109 */	_B_ISALPHA|_B_ISLOWER,
	/* 110 */	_B_ISALPHA|_B_ISLOWER,
	/* 111 */	_B_ISALPHA|_B_ISLOWER,
	/* 112 */	_B_ISALPHA|_B_ISLOWER,
	/* 113 */	_B_ISALPHA|_B_ISLOWER,
	/* 114 */	_B_ISALPHA|_B_ISLOWER,
	/* 115 */	_B_ISALPHA|_B_ISLOWER,
	/* 116 */	_B_ISALPHA|_B_ISLOWER,
	/* 117 */	_B_ISALPHA|_B_ISLOWER,
	/* 118 */	_B_ISALPHA|_B_ISLOWER,
	/* 119 */	_B_ISALPHA|_B_ISLOWER,
	/* 120 */	_B_ISALPHA|_B_ISLOWER,
	/* 121 */	_B_ISALPHA|_B_ISLOWER,
	/* 122 */	_B_ISALPHA|_B_ISLOWER,
	/* 123 */	_B_ISSYMBOL,
	/* 124 */	_B_ISSYMBOL,
	/* 125 */	_B_ISSYMBOL,
	/* 126 */	_B_ISSYMBOL,
	/* 127 */	0,
	/* 128 */	0,
	/* 129 */	0,
	/* 130 */	0,
	/* 131 */	0,
	/* 132 */	0,
	/* 133 */	0,
	/* 134 Â */	_B_ISALPHA|_B_ISUPPER,
	/* 135 À*/	_B_ISALPHA|_B_ISUPPER,
	/* 136 Á */	_B_ISALPHA|_B_ISUPPER,
	/* 137 Ã */	_B_ISALPHA|_B_ISUPPER,
	/* 138 ã */	_B_ISALPHA|_B_ISLOWER,
	/* 139 Ë */	_B_ISALPHA|_B_ISUPPER,
	/* 140 Ê */	_B_ISALPHA|_B_ISUPPER,
	/* 141 È */	_B_ISALPHA|_B_ISUPPER,
	/* 142 Ï */	_B_ISALPHA|_B_ISUPPER,
	/* 143 Î */	_B_ISALPHA|_B_ISUPPER,
	/* 144 Ì */	_B_ISALPHA|_B_ISUPPER,
	/* 145 Í */	_B_ISALPHA|_B_ISUPPER,
	/* 146 Ô */	_B_ISALPHA|_B_ISUPPER,
	/* 147 Ò */	_B_ISALPHA|_B_ISUPPER,
	/* 148 Ó */	_B_ISALPHA|_B_ISUPPER,
	/* 149 Õ */	_B_ISALPHA|_B_ISUPPER,
	/* 150 õ */	_B_ISALPHA|_B_ISLOWER,
	/* 151 Û */	_B_ISALPHA|_B_ISUPPER,
	/* 152 Ù*/	_B_ISALPHA|_B_ISUPPER,
	/* 153 Ú*/	_B_ISALPHA|_B_ISUPPER,
	/* 154 Ø */	_B_ISALPHA|_B_ISUPPER,
	/* 155 ø */	_B_ISALPHA|_B_ISLOWER,
	/* 156 Œ */	_B_ISALPHA|_B_ISUPPER,
	/* 157 œ */	_B_ISALPHA|_B_ISLOWER,
	/* 158 Ý */	_B_ISALPHA|_B_ISUPPER,
	/* 159 ý */	_B_ISALPHA|_B_ISLOWER,
	/* 160 ┌ */	0,
	/* 161 ┐ */	0,
	/* 162 ┘ */	0,
	/* 163 └ */	0,
	/* 164 ┼ */	0,
	/* 165 ├ */	0,
	/* 166 ┤ */	0,
	/* 167 ┬ */	0,
	/* 168 ┴ */	0,
	/* 169 ─ */	0,
	/* 170 │ */	0,
	/* 171 ░ */	0,
	/* 172 ▒ */	0,
	/* 173 ▓ */	0,
	/* 174 █ */	0,
	/* 175 ╔ */	0,
	/* 176 ╗ */	0,
	/* 177 ╝ */	0,
	/* 178 ╚ */	0,
	/* 179 ╬ */	0,
	/* 180 ╠ */	0,
	/* 181 ╣ */	0,
	/* 182 ╦ */	0,
	/* 183 ╩ */	0,
	/* 184 ═ */	0,
	/* 185 ║ */	0,
	/* 186 × */	_B_ISSYMBOL,
	/* 187 ÷ */	_B_ISSYMBOL,
	/* 188 µ */	_B_ISSYMBOL,
	/* 189 ¶ */	_B_ISSYMBOL,
	/* 190 « */	_B_ISSYMBOL,
	/* 191 » */	_B_ISSYMBOL,
	/* 192 Ä */	_B_ISALPHA|_B_ISUPPER,
	/* 193 ä */	_B_ISALPHA|_B_ISLOWER,
	/* 194 â */	_B_ISALPHA|_B_ISLOWER,
	/* 195 à */	_B_ISALPHA|_B_ISLOWER,
	/* 196 á */	_B_ISALPHA|_B_ISLOWER,
	/* 197 É */	_B_ISALPHA|_B_ISUPPER,
	/* 198 ë */	_B_ISALPHA|_B_ISLOWER,
	/* 199 ê */	_B_ISALPHA|_B_ISLOWER,
	/* 200 è */	_B_ISALPHA|_B_ISLOWER,
	/* 201 é */	_B_ISALPHA|_B_ISLOWER,
	/* 202 ï */	_B_ISALPHA|_B_ISLOWER,
	/* 203 î */	_B_ISALPHA|_B_ISLOWER,
	/* 204 ì */	_B_ISALPHA|_B_ISLOWER,
	/* 205 í */	_B_ISALPHA|_B_ISLOWER,
	/* 206 Ö */	_B_ISALPHA|_B_ISUPPER,
	/* 207 ö */	_B_ISALPHA|_B_ISLOWER,
	/* 208 ô */	_B_ISALPHA|_B_ISLOWER,
	/* 209 ò */	_B_ISALPHA|_B_ISLOWER,
	/* 210 ó */	_B_ISALPHA|_B_ISLOWER,
	/* 211 Ü */	_B_ISALPHA|_B_ISUPPER,
	/* 212 ü */	_B_ISALPHA|_B_ISLOWER,
	/* 213 û */	_B_ISALPHA|_B_ISLOWER,
	/* 214 ù */	_B_ISALPHA|_B_ISLOWER,
	/* 215 ú */	_B_ISALPHA|_B_ISLOWER,
	/* 216 Ç */	_B_ISALPHA|_B_ISUPPER,
	/* 217 ç */	_B_ISALPHA|_B_ISLOWER,
	/* 218 Ñ */	_B_ISALPHA|_B_ISUPPER,
	/* 219 ñ */	_B_ISALPHA|_B_ISLOWER,
	/* 220 Æ */	_B_ISALPHA|_B_ISUPPER,
	/* 221 æ */	_B_ISALPHA|_B_ISLOWER,
	/* 222 Å */	_B_ISALPHA|_B_ISUPPER,
	/* 223 å */	_B_ISALPHA|_B_ISLOWER,
	/* 224 ß */	_B_ISALPHA,
	/* 225 ¿ */	_B_ISSYMBOL,
	/* 226 ¡ */	_B_ISSYMBOL,
	/* 227 ¢ */	_B_ISSYMBOL,
	/* 228 £ */	_B_ISSYMBOL,
	/* 229 ¥ */	_B_ISSYMBOL,
	/* 230 ₧ */	_B_ISSYMBOL,
	/* 231 € */	_B_ISSYMBOL,
	/* 232 ¼ */	_B_ISSYMBOL,
	/* 233 ½ */	_B_ISSYMBOL,
	/* 234 ÿ */	_B_ISSYMBOL,
	/* 235 § */	_B_ISSYMBOL,
	/* 236 ° */	_B_ISSYMBOL,
	/* 237 ² */	_B_ISSYMBOL,
	/* 238 ³ */	_B_ISSYMBOL,
	/* 239 ¹ */	_B_ISSYMBOL,
	/* 240 ª */	_B_ISSYMBOL,
	/* 241 º */	_B_ISSYMBOL,
	/* 242 Š */	_B_ISALPHA|_B_ISUPPER,
	/* 243 š */	_B_ISALPHA|_B_ISLOWER,
	/* 244 Ž */	_B_ISALPHA|_B_ISUPPER,
	/* 245 ž */	_B_ISALPHA|_B_ISLOWER,
	/* 246 Ð */	_B_ISALPHA|_B_ISUPPER,
	/* 247 ð */	_B_ISALPHA|_B_ISLOWER,
	/* 248 Þ */	_B_ISALPHA|_B_ISUPPER,
	/* 249 þ */	_B_ISALPHA|_B_ISLOWER,
	/* 250 Ÿ */	_B_ISALPHA|_B_ISUPPER,
	/* 251 ± */	_B_ISSYMBOL,
	/* 252 © */	_B_ISSYMBOL,
	/* 253 ® */	_B_ISSYMBOL,
	/* 254 . */	0,
	/* 255 . */	0
};

/* table de translation de TheoX en minuscules */

unsigned char _thxlcase[] = {
//        0    1    2    3     4    5    6    7     8    9    A    B     C    D    E    F

// 8      .    .    .    .     .    .    Â    À     Á    Ã    ã    Ë     Ê    È    Ï    Î
	0x80,0x81,0x82,0x83, 0x84,0x85,0xc2,0xc3, 0xc4,0x8a,0x8a,0xc6, 0xc7,0xc8,0xca,0xcb,
// 9      Ì    Í    Ô    Ò     Ó    Õ    õ    Û     Ù    Ú    Ø    ø     Œ    œ    Ý    ý
	0xcc,0xcd,0xd0,0xd1, 0xd2,0x9a,0x9a,0xd5, 0xd6,0xd7,0x9b,0x9b, 0x9d,0x9d,0x9f,0x9f,
// A      ┌    ┐    ┘    └     ┼    ├    ┤    ┬     ┴    ─    │    ░     ▒    ▓    █    ╔
	0xa0,0xa1,0xa2,0xa3, 0xa4,0xa5,0xa6,0xa7, 0xa8,0xa9,0xaa,0xab, 0xac,0xad,0xae,0xaf,
// B      ╗    ╝    ╚    ╬     ╠    ╣    ╦    ╩     ═    ║    ×    ÷     µ    ¶    «    »
	0xb0,0xb1,0xb2,0xb3, 0xb4,0xb5,0xb6,0xb7, 0xb8,0xb9,0xba,0xbb, 0xbc,0xbd,0xbe,0xbf,
// C      Ä    ä    â    à     á    É    ë    ê     è    é    ï    î     ì    í    Ö    ö
	0xc1,0xc1,0xc2,0xc3, 0xc4,0xc9,0xc6,0xc7, 0xc8,0xc9,0xca,0xcb, 0xcc,0xcd,0xcf,0xcf,
// D      ô    ò    ó    Ü     ü    û    ù    ú     Ç    ç    Ñ    ñ     Æ    æ    Å    å
	0xd0,0xd1,0xd2,0xd4, 0xd4,0xd5,0xd6,0xd7, 0xd9,0xd9,0xdb,0xdb, 0xdd,0xdd,0xdf,0xdf,
// E      ß    ¿    ¡    ¢     £    ¥    ₧    €     ¼    ½    ÿ    §     °    ²    ³    ¹
	0xe0,0xe1,0xe2,0xe3, 0xe4,0xe5,0xe6,0xe7, 0xe8,0xe9,0xea,0xeb, 0xec,0xed,0xee,0xef,
// F      ª    º    Š    š     Ž    ž    Ð    ð     Þ    þ    Ÿ    ±     ©    ®
	0xf0,0xf1,0xf3,0xf3, 0xf5,0xf5,0xf7,0xf7, 0xf9,0xf9,0xaa,0xfb, 0xfc,0xfd,0xfe,0xff
};

/* table de translation de TheoX en majuscules */

unsigned char _thxucase[] = {
//        0    1    2    3     4    5    6    7     8    9    A    B     C    D    E    F

// 8      .    .    .    .     .    .    Â    À     Á    Ã    ã    Ë     Ê    È    Ï    Î
	0x80,0x81,0x82,0x83, 0x84,0x85,0x86,0x87, 0x88,0x89,0x89,0x8b, 0x8c,0x8d,0x8e,0x8f,
// 9      Ì    Í    Ô    Ò     Ó    Õ    õ    Û     Ù    Ú    Ø    ø     Œ    œ    Ý    ý
	0x90,0x91,0x92,0x93, 0x94,0x95,0x95,0x97, 0x98,0x99,0x9a,0x9a, 0x9c,0x9c,0x9e,0x9e,
// A      ┌    ┐    ┘    └     ┼    ├    ┤    ┬     ┴    ─    │    ░     ▒    ▓    █    ╔
	0xa0,0xa1,0xa2,0xa3, 0xa4,0xa5,0xa6,0xa7, 0xa8,0xa9,0xaa,0xab, 0xac,0xad,0xae,0xaf,
// B      ╗    ╝    ╚    ╬     ╠    ╣    ╦    ╩     ═    ║    ×    ÷     µ    ¶    «    »
	0xb0,0xb1,0xb2,0xb3, 0xb4,0xb5,0xb6,0xb7, 0xb8,0xb9,0xba,0xbb, 0xbc,0xbd,0xbe,0xbf,
// C      Ä    ä    â    à     á    É    ë    ê     è    é    ï    î     ì    í    Ö    ö
	0xc1,0xc1,0x86,0x87, 0x88,0xc5,0x8b,0x8c, 0x8d,0xc5,0x8e,0x8f, 0x90,0x91,0xce,0xce,
// D      ô    ò    ó    Ü     ü    û    ù    ú     Ç    ç    Ñ    ñ     Æ    æ    Å    å
	0x92,0x93,0x94,0xd3, 0xd3,0x97,0x98,0x99, 0xd8,0xd8,0xda,0xda, 0xdc,0xdc,0xde,0xde,
// E      ß    ¿    ¡    ¢     £    ¥    ₧    €     ¼    ½    ÿ    §     °    ²    ³    ¹
	0xe0,0xe1,0xe2,0xe3, 0xe4,0xe5,0xe6,0xe7, 0xe8,0xe9,0xfa,0xeb, 0xec,0xed,0xee,0xef,
// F      ª    º    Š    š     Ž    ž    Ð    ð     Þ    þ    Ÿ    ±     ©    ®
	0xf0,0xf1,0xf2,0xf2, 0xf4,0xf4,0xf6,0xf6, 0xf8,0xf8,0xfa,0xfb, 0xfc,0xfd,0xfe,0xff
};

int _b_islower(int ch)
{
	return (_b_theox_ctype[ch & 0xff] & _B_ISLOWER) != 0;
}

int _b_isupper(int ch)
{
	return (_b_theox_ctype[ch & 0xff] & _B_ISUPPER) != 0;
}

int _b_isalpha(int ch)
{
	return (_b_theox_ctype[ch & 0xff] & _B_ISALPHA) != 0;
}

int _b_isalnum(int ch)
{
	return (_b_theox_ctype[ch & 0xff] & (_B_ISALPHA|_B_ISDIGIT)) != 0;
}

int _b_isdigit(int ch)
{
	return (_b_theox_ctype[ch & 0xff] & _B_ISDIGIT) != 0;
}

int _b_isspace(int ch)
{
	return (_b_theox_ctype[ch & 0xff] & _B_ISSPACE) != 0;
}

int _b_issymbol(int ch)
{
	return (_b_theox_ctype[ch & 0xff] & _B_ISSYMBOL) != 0;
}

int _b_isprint(int ch)
{
	return (_b_theox_ctype[ch & 0xff] & (_B_ISALNUM|_B_ISSYMBOL)) != 0;
}

int _b_tolower(int ch)
{
	return (_b_theox_ctype[ch &= 0xff] & _B_ISUPPER) ? (ch & 0x80 ? _thxlcase[ch & 0x7f] : ch | 32) : ch;
}

int _b_toupper(int ch)
{
	return (_b_theox_ctype[ch &= 0xff] & _B_ISLOWER) ? (ch & 0x80 ? _thxucase[ch & 0x7f] : ch & ~32) : ch;
}

VOID theoxputchar(unsigned char c)
{
	char utf8[6];

	if ((c & 0x80) && (curbp->b_mode & MDTHEOX)) {
		unsigned wc = _b_awcTheoX[c];
		unsigned bytes = unicode_to_utf8(wc, utf8);
		unsigned i;

		for (i = 0; i < bytes; ++i)
#if	WINNT || WINXP
			putchar(utf8[i]);
#else
			TTputc(c);
#endif
	} else
#if	WINNT || WINXP
		putchar(c);
#else
		TTputc(c);
#endif
}

unsigned int _b_theox2wchar(unsigned char c)
{
	return _b_awcTheoX[c];
}
#endif /* UTF8 && THEOX */
