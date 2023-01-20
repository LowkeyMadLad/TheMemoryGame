/* 
 * File:   UART2.h
 * Author: rvyas
 *
 * 
 */

#ifndef UART2_H
#define	UART2_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

void InitUART2(void);
void XmitUART2(char, unsigned int);
uint8_t RxUART2(void);

void __attribute__ ((interrupt, no_auto_psv)) _U2RXInterrupt(void);
void __attribute__ ((interrupt, no_auto_psv)) _U2TXInterrupt(void); 

void Disp2Hex(unsigned int);
void Disp2Hex32(unsigned long int);
void Disp2String(char*);
void Disp2Dec(unsigned int);
void Disp2Input(unsigned int);
void DispDiff(int);


#endif	/* UART2_H */
