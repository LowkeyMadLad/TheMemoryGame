#include "xc.h"
#include "TimeDelay.h"
//int timeUP = 0;

//void __attribute__((interrupt, no_auto_psv))_T2Interrupt(void){
//    timeUP = 1;
//    IFS0bits.T2IF = 0; // Clear the flag
//    T2CONbits.TON = 0; // Stops the timer
//    TMR2 = 0; // Sets TMR2 to 0 so it can be run right away.
//    return;
//}

//extern int TM1flag;

void delay_ms(int time_ms){
    
    INTCON1bits.NSTDIS = 1; // Disables nested interrupts
    IPC1bits.T2IP = 4; // Sets T2 Interrupt Priority
    
    T2CONbits.TSIDL = 0; // Continue while cpu in idle
    T2CONbits.TCS = 0; // Use internal clock
    T2CONbits.TCKPS = 0b11; // Pre-scale of 256
    IEC0bits.T2IE = 1; // Enables the interrupt for T2
    
    PR2 =(int) (time_ms)/(0.000064*(1000)); // Time/(2*pre-scale/f-clock)(1000)
    T2CONbits.TON = 1; // Turn on timer

    Idle(); // Set CPU Idle
    return;
}

//
//void start_TM3(){
//    INTCON1bits.NSTDIS = 1; // Disables nested interrupts
//    IPC2bits.T3IP = 4; // Sets T2 Interrupt Priority
//    
//    T3CONbits.TSIDL = 0; // Continue while cpu in idle
//    T3CONbits.TCS = 0; // Use internal clock
//    T3CONbits.TCKPS = 0b11; // Pre-scale of 256
//    IEC0bits.T3IE = 1; // Enables the interrupt for T2
//    
//    PR3 = (int) (1000)/(0.000064*(1000)); // Time/(2*pre-scale/f-clock)(1000)
//    T3CONbits.TON = 1; // Turn on timer
//    return;
//}