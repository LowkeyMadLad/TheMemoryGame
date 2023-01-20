/*
 * File:   ADC.c
 * Author: ryanm
 *
 * Created on November 9, 2022, 7:11 PM
 */

#include "xc.h"

uint16_t do_ADC(void){
    uint16_t ADCvalue;
    AD1CON1bits.ADON = 1; // turn on ADC module
    AD1CON1bits.FORM = 0b00; // Data output format bits
    AD1CON1bits.SSRC = 0b111; // Internal counter ends sampling/ starts conversion
    AD1CON1bits.ASAM = 0b0; // Sampling begins when SAMP is set
    AD1CON1bits.SAMP = 0; // Sets SAMP to 0 so it doesnt start
    AD1CON1bits.DONE = 0; // Makes sure done is set to zero before check
    
    AD1CON2bits.VCFG = 0b000; // Selects AVDD, AVSS (supply voltage to PIC) as Vref
    AD1CON2bits.CSCNA = 0; // Dont scan CH inputs
    AD1CON2bits.SMPI = 0b000; // Interrupts at the completion of each conversion
    AD1CON2bits.BUFM = 0; // one 16 bit word
    AD1CON2bits.ALTS = 0; // Always use mux a input
    
    AD1CON3bits.ADRC = 0; // Use system clock
    AD1CON3bits.SAMC = 0b11111; // Slowest sample time
    AD1CON3bits.ADCS = 0b11111; // Slowest sample time
    
    AD1CHSbits.CH0NA = 0;   //channel 0 negative input is Vr- 
    AD1CHSbits.CH0SA = 0b0101;  //channel 0 positive input is AN5 
    // Start sampling here:
    AD1CON1bits.SAMP = 1; // Starts sampling
    while(AD1CON1bits.DONE == 0){} // Waits until its done
    ADCvalue= ADC1BUF0; // ADC output is stored in ADC1BUF0 as this point
    AD1CON1bits.SAMP=0; //Stop sampling
    AD1CON1bits.ADON=0; //Turn off ADC, ADC value stored in ADC1BUF0;
    return (ADCvalue); //returns 10 bit ADC output stored in ADC1BIF0 to calling function
}