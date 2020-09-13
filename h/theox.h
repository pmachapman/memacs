/*
 * theox.h
 *
 *  Created on: 20 févr. 2020
 *      Author: system
 */

#ifndef H_THEOX_H_
#define H_THEOX_H_

extern unsigned char _b_wchar2theox(unsigned int wch);
extern unsigned int _b_theox2wchar(unsigned char c);
extern VOID theoxputchar(unsigned char c);
extern int _b_tolower(int ch);
extern int _b_toupper(int ch);
extern int _b_islower(int ch);
extern int _b_isupper(int ch);


#endif /* H_THEOX_H_ */
