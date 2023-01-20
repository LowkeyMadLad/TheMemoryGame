/*
 * File:   IOs.c
 * Author: ryanm
 *
 * Created on November 29, 2022, 2:49 PM
 */

#include "xc.h"

void IOinit(){
    AD1PCFG = 0xFFDF; // Turn all analog pins as digital except AN5 (pin 8)
    AD1CSSL = 0x0020; // Only take analog input from pin 8
    TRISAbits.TRISA3 = 1; // Sets RA3-Pin 8 as input Analog 
    
    TRISAbits.TRISA2 = 1; // Sets RA2-Pin 7 as Power Button 1
    TRISAbits.TRISA4 = 1; // Sets RA4-Pin 10 as Power Button 2
    TRISBbits.TRISB4 = 1; // Sets RB4-Pin 9 as Power Button 3
    
    CNPD2bits.CN30PDE = 0;	// Disables the pull down on CN30 (RA2) Pin 7 
    CNPD1bits.CN0PDE = 0;	// Disables the pull down on CN0 (RA4) Pin 10
    CNPD1bits.CN1PDE = 0; 	// Disables the pull down on CN1 (RB4) Pin 9
    
    CNPU1bits.CN0PUE = 1;	// Sets the pull up on CN0 (RA4) pin 10
    CNPU1bits.CN1PUE = 1;	// Sets the pull up on CN1 (RB4) pin 9
    CNPU2bits.CN30PUE = 1;	// Sets the pull up on CN30 (RA2) pin 7
    
    TRISBbits.TRISB8 = 0; // Sets RB8-Pin 12 as output (LED 1)  
    TRISBbits.TRISB12 = 0; // Sets RB12-Pin 15 as output (LED 2) 
    TRISBbits.TRISB13 = 0; // Sets RB13-Pin 16 as output (LED 3) 
    
    CNEN1bits.CN0IE = 1;
    CNEN1bits.CN1IE = 1;
    CNEN2bits.CN30IE = 1; // Enables CN30
    
    IPC4bits.CNIP = 6; // Interrupt Priority
    IFS1bits.CNIF = 0; // Clear flag
    IEC1bits.CNIE = 1; // Enable Interrupt
    return;
}

