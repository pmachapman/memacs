/*	UTF8:		Unicode UTF-8 prototypes and declarations
			MicroEMACS 4.00

                        written by Jean-Michel Dubois
*/

#ifndef UTF8_H
#define UTF8_H

unsigned utf8_to_unicode(const char *line, unsigned index, unsigned len, unsigned int *res);
unsigned unicode_to_utf8(unsigned int c, char *utf8);

static inline int is_beginning_utf8(unsigned char c)
{
	return (c & 0xc0) != 0x80;
}

static inline int is_multibyte_utf8(unsigned char c)
{
	return (c & 0xc0) == 0xc0;
}

#endif
