/* 
 * File:   TimeDelay.h
 * Author: user
 *
 * Created on November 19, 2022, 7:53 PM
 */


#ifndef TIMEDELAY_H
#define	TIMEDELAY_H

#include <xc.h> // include processor files - each processor file is guarded.  

void delay_ms(int time_ms); // Runs the timer delay
//void start_TM3();


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

