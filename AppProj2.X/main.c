/*
 * File:   main.c
 * Author: ryanm
 *
 * Created on November 29, 2022, 2:49 PM
 */


#include "xc.h"
#include "IOs.h"
#include "ADC.h"
#include "UART2.h"
#include "RNG.h"
#include "TimeDelay.h"
#pragma config BSS = OFF // Boot segment code protect disabled
#pragma config BWRP = OFF // Boot sengment flash write protection off
#pragma config GCP = OFF // general segment code protecion off
#pragma config GWRP = OFF

// CLOCK CONTROL 
#pragma config IESO = OFF    // 2 Speed Startup disabled
#pragma config FNOSC = FRC  // Start up CLK = 8 MHz
#pragma config FCKSM = CSECMD // Clock switching is enabled, clock monitor disabled
#pragma config SOSCSEL = SOSCLP // Secondary oscillator for Low Power Operation
#pragma config POSCFREQ = MS  //Primary Oscillator/External clk freq betwn 100kHz and 8 MHz. Options: LS, MS, HS
#pragma config OSCIOFNC = ON  //CLKO output disabled on pin 8, use as IO. 
#pragma config POSCMOD = NONE  // Primary oscillator mode is disabled

// WDT
#pragma config FWDTEN = OFF // WDT is off
#pragma config WINDIS = OFF // STANDARD WDT/. Applicable if WDT is on
#pragma config FWPSA = PR32 // WDT is selected uses prescaler of 32
#pragma config WDTPS = PS512 // WDT postscler is 1 if WDT selected

//MCLR/RA5 CONTROL
#pragma config MCLRE = OFF // RA5 pin configured as input, MCLR reset on RA5 diabled

//BOR  - FPOR Register
#pragma config BORV = V18 // LPBOR value=2V is BOR enabled
#pragma config BOREN = BOR0 // BOR controlled using SBOREN bit
#pragma config PWRTEN = OFF // Powerup timer disabled
#pragma config I2C1SEL = PRI // Default location for SCL1/SDA1 pin

//JTAG FICD Register
#pragma config BKBUG = OFF // Background Debugger functions disabled
#pragma config ICS = PGx2 // PGC2 (pin2) & PGD2 (pin3) are used to connect PICKIT3 debugger

// Deep Sleep RTCC WDT
#pragma config DSWDTEN = OFF // Deep Sleep WDT is disabled
#pragma config DSBOREN = OFF // Deep Sleep BOR is disabled
#pragma config RTCOSC = LPRC// RTCC uses LPRC 32kHz for clock
#pragma config DSWDTOSC = LPRC // DeepSleep WDT uses Lo Power RC clk
#pragma config DSWDTPS = DSWDTPS7 // DSWDT postscaler set to 32768 


// GLOBAL VARIABLES
int CN0flag = 0; // button 2 global
int CN1flag = 0; // Button 3 global
int CN30flag = 0; // Button 1 global
int TM1flag = 0; // Timer global (unused?)
int TM3flag = 0; // Timer globals (unused?)
int difficulty = 1; // Difficulty scaler
unsigned int level = 1; // Level the user is on
int leds[3]; // LED blink pattern array
int input[3]; // Array to store user guesses each round
int wrong_guess = 0; // if a guess is incorrect, set to 1
int blink_time = 1000; // Blink time is a variable in ms for how long the LEDs blink (determines difficulty)
int num_guesses = 0; // Used for a while loop to determine how many times the user has guessed
int procEarly = 0; // Used to stop a specific bug
// MACROS
#define Nop() {__asm__ volatile ("nop");}
#define ClrWdt() {__asm__ volatile ("clrwdt");}
#define Sleep() {__asm__ volatile ("pwrsav #0");}   // set sleep mode
#define Idle() {__asm__ volatile ("pwrsav #1");}
#define dsen() {__asm__ volatile ("BSET DSCON, #15");}

void __attribute__((interrupt, no_auto_psv))_T2Interrupt(void){
    IFS0bits.T2IF = 0; // Clear the flag
    T2CONbits.TON = 0; // Stops the timer
    TMR2 = 0; // Sets TMR2 to 0 so it can be run right away.
    TM1flag = 1;
    return;
}

void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void){
    if (PORTAbits.RA2 == 0){ // If button 1 is pressed
        CN30flag = 1; // Global
        IFS1bits.CNIF = 0; // clear IF flag
    } else if(PORTAbits.RA4 == 0){ // If button 2 is pressed
        CN0flag = 1; // user defined global variable used as flag
        IFS1bits.CNIF = 0; // clear IF flag
    } else if (PORTBbits.RB4 == 0){ // If button 3 is pressed
        CN1flag = 1; // Global
        IFS1bits.CNIF = 0; // clear IF flag
    }
    
    IFS1bits.CNIF = 0; // Clear flag always
    return;
    
}

void flushButtonPresses(){
    // This just clears any button presses that happened so that the next button
    // press can be recorded correctly
    if(CN30flag == 1){ 
        CN30flag = 0;
    }
    if(CN0flag == 1){ 
        CN0flag = 0;
    }
    if(CN1flag == 1){ 
        CN1flag = 0;
    }
    IFS1bits.CNIF = 0; // Clear flag
}

int buttonPressed() {
    // This function reads the global CN flags and stores the value in val to return
    int val= 0;
    if(CN30flag == 1){ 
        CN30flag = 0;
        val = 1;
    }
    if(CN0flag == 1){ 
        CN0flag = 0;
        val = 2;
    }
    if(CN1flag == 1){ 
        CN1flag = 0;
        val = 3;
    }
    return val;
}

void startScreen(){
    int adcVal; // Value from ADC
    int prevDiff = 0; // Previous value storage
    Disp2String("Please select the difficulty using the potentiometer on the breadboard.");
    XmitUART2(10,1); // New line
    XmitUART2(13,1); // Carriage Return
    Disp2String("(Press button 3 to start!)");
    XmitUART2(10,1); 
    flushButtonPresses();
    while(1){
        
        if(buttonPressed() == 3){ // Waits for button 3 to be pressed
            break;
        }
        
        // This grabs the ADC value and sets the difficulty accordingly
        adcVal = do_ADC();
        if(adcVal <= 205){difficulty = 1;}
        else if(adcVal > 205 && adcVal <= 410){difficulty = 2;}
        else if(adcVal > 410 && adcVal <= 615){difficulty = 3;}
        else if(adcVal > 615 && adcVal <= 820){difficulty = 4;}
        else if(adcVal > 820){difficulty = 5;}        
        
        // Display only if the difficulty is different
        if(prevDiff != difficulty){
            DispDiff(difficulty);
            prevDiff = difficulty;
        }
    }
    
}
   
void get_seq(){
    for(int i = 0; i < 3; i++){
        leds[i] = rand_num(); // Get random number, store in LED's
    }
}

int check_Pattern(int index){
    if (leds[index] == input[index]){ // Compare if the inputs are the same
        return 1;
    }
    return 0;
}

void displayLEDs(){
    // This function blinks the respective LED for a given blink time
    for(int i = 0; i < 3; i++){
        if(leds[i] == 1){
            LATBbits.LATB8 = 1; // Turn on LED 1
            delay_ms(blink_time);
        }
        else if(leds[i] == 2){
            LATBbits.LATB12 = 1; // Turn on LED 2
            delay_ms(blink_time);
        }
        else if(leds[i] == 3){
            LATBbits.LATB13 = 1; // Turn on LED 3
            delay_ms(blink_time);
        }
        // Turn off all LED's
        LATBbits.LATB8 = 0; 
        LATBbits.LATB12 = 0;
        LATBbits.LATB13 = 0;
        delay_ms((int)blink_time/2);
    }
    XmitUART2(32,50); // Space character 50 times (clear whats there)
    XmitUART2(13,1); // carriage return
    Disp2String("Your turn to guess!");
    XmitUART2(10,1); // New Line
    XmitUART2(13,1); // carriage return
//    lineCount++;
}

void introMessage(){
    Disp2String("Clear Buffer");
    XmitUART2(8,15); // Backspace
    Disp2String("Hello and welcome to The Memory Game designed by:");
    XmitUART2(10,1); // new line character
    XmitUART2(13,1); // Carriage return
    Disp2String("Ryan Mailhiot");
    XmitUART2(10,1); // new line character
    XmitUART2(13,1); // Carriage return
    Disp2String("Kartik Sharma");
    XmitUART2(10,1); // new line character
    XmitUART2(13,1); // Carriage return
    Disp2String("Danny Picazo");
    XmitUART2(10,1); // new line character
    XmitUART2(13,1); // Carriage return
}

void displayRoundStart(){
    DispDiff(difficulty);
    XmitUART2(10,1); // new line character
    Disp2String("Current Level: ");
    Disp2Dec(level); 
    XmitUART2(10,1); // new line character
    XmitUART2(13,1); // carriage return
    Disp2String("Press Any Button to Continue");
    XmitUART2(13,1); // carriage return

    flushButtonPresses(); // Flushes all inputs
    while(!buttonPressed()){ // Waits until any button is pressed
        Idle();
    }
    flushButtonPresses();
    XmitUART2(32,50); // Blank a line
    XmitUART2(13,1); // carriage return
    Disp2String("Get Ready! Look at LED's");
    XmitUART2(13,1); // carriage return
    blink_time = (int) (2000/(difficulty*level)); // Set blink time on the LEDs
    if (blink_time < 50){ // Failsafe so the user will still see the blink
        blink_time = 50;
    }
    IFS1bits.CNIF = 0;
    Sleep(); // Sleep for 512 ms to get the user ready
    if(RCONbits.WDTO == 1){ // random sleep check
        Nop();
    }
    //RCONbits.SWDTEN = 0;
}

void UserGuesses(){
    int button;
    num_guesses = 0; // Number of correct user guesses
    wrong_guess = 0; // Only set if a wrong input is implemented
    while(num_guesses < 3){
        flushButtonPresses(); // sets all button presses to 0
        input[num_guesses] = 0;
        while(1){ // While loop to get button press
            button = buttonPressed();
            if(button != 0){
                break;
            }
            Idle();           
        } // End of INPUT while  
        input[num_guesses] = button;
        Disp2Input(button);
        if(input[num_guesses] == 0){
            XmitUART2(10,1); // new line
            XmitUART2(13,1); // carriage return
            Disp2String("Button Input did not read correctly. Try again");
            continue;
        }
        if(check_Pattern(num_guesses) == 1){ // Button matches
            num_guesses++;
            continue;
        } else {
            wrong_guess = 1;
            break;
        }
        
    } // End of Guesses while loop.
    
}
void restart(){
    int button;
    wrong_guess = 0;
    Disp2String("WRONG GUESS!");
    XmitUART2(10,1);
    XmitUART2(13,1);
    Disp2String("Do you wish to restart?");
    XmitUART2(10,1);
    XmitUART2(13,1);
    Disp2String("(Press button 1 for continue)");
    XmitUART2(10,1);
    XmitUART2(13,1);
    flushButtonPresses();
    level = 1;
    while(1){
        button = buttonPressed();
        if(button == 1){
            break;
        }
        Idle();
    } 
    while(1){ // force WDT
        Nop();
    }
}
void clearTerm(){
    XmitUART2(8,150);
//    lineCount = 0;
}

int main(void){// enters State 1
    IOinit(); // Initializes all input output
    InitUART2();  // initializes UART settings
    introMessage(); // Displays 
    startScreen(); // State 2
    RCONbits.SWDTEN = 1; //Enables sleep
    
    while(1){
        if(procEarly == 0){
            displayRoundStart(); // state 3, Displays info on round start
        } else {
            procEarly = 0;
        }
        Sleep();
        if(RCONbits.WDTO == 1){ // If the sleep was proc'd
            get_seq(); // state 4, Get a random LED sequence, store in 
            displayLEDs(); // State 5, Display LED to user
            UserGuesses(); // state 6, User gets guesses
            if(wrong_guess == 1){
                restart(); // State 7, Restarts the game
            } else {
                level++; // Back to state 3
            }
        } else {
            procEarly = 1; // Failsafe for random bug
        }
        
    }

}