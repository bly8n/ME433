#include "nu32dip.h"
int main() {
  //char buffer[BUF_SIZE];
  NU32DIP_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  TRISBbits.TRISB6 = 0;    // Set B6 as digital output pin
  TRISBbits.TRISB10 = 0;   //Set B10 as digital output pin
  RPB7Rbits.RPB7R=0b0101;      //use pin B7 for OC1
  RPB8Rbits.RPB8R=0b0101;      //use pin B8 for OC2
  RPB9Rbits.RPB9R=0b0101;      //use pin B9 for OC3
  //timer2 for motor 1 and motor 2
  T2CONbits.TCKPS = 0;     // Timer2 prescaler N=1 (1:4)
  PR2 = 2400-1;              // period = (PR3+1) * N * 12.5 ns = 100 us, 20 kHz, PWM
  TMR2 = 0;                // initial TMR2 count is 0
  //OC1
  OC1CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
  OC1CONbits.OCTSEL=0;
  OC1RS = 1200;             // duty cycle = OC1RS/(PR2+1) = 25%
  OC1R = 1200;  
  //OC2
  OC2CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
  OC2CONbits.OCTSEL=0;
  OC2RS = 1200;             // duty cycle = OC1RS/(PR2+1) = 25%
  OC2R = 1200;  
  // initialize before turning OC1 on; afterward it is read-only
  T2CONbits.ON = 1;        // turn on Timer2
  OC1CONbits.ON = 1;       // turn on OC1
  OC2CONbits.ON = 1;       // turn on OC1
  //Timer 3 for servo motor
  T3CONbits.TCKPS = 4;     //Timer3 prescaler N=16 (1:16)
  PR3 = 60000-1;            //50hz
  TMR3 = 0;
  //OC3
  OC3CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
  OC3CONbits.OCTSEL=1;
  OC3RS = 4500;             // start from 1.5ms
  OC3R = 4500;
  T3CONbits.ON = 1;        // turn on Timer3
  OC3CONbits.ON = 1;       //turn on OC3
  NU32DIP_GREEN = 0;      // turn off the LEDs
  NU32DIP_YELLOW = 0;
  //5250 45degree, 6750 135degree
   while(1) {
    for(OC3RS =4500 ; OC3RS <= 6750; OC3RS++) { 
      _CP0_SET_COUNT(0); 
      while(_CP0_GET_COUNT() < 48000) {};  // wait for 1ms
    }
    for(OC3RS = 6750; OC3RS >= 5250; OC3RS--) { 
      _CP0_SET_COUNT(0);
      while(_CP0_GET_COUNT() < 48000) {};  // wait for 1ms
    }
  }
}