#include "nu32dip.h" // constants, functions for startup and UART
#include <stdio.h>
#include <math.h>
#define PI 3.1415926
//void blink(int, int); // blink the LEDs function

int main(void) {
  char message[100];
  TRISAbits.TRISA4 = 1;
  //TRISBbits.TRISB4 = 0;
  //TRISBbits.TRISB5 = 0;
  NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init
  //NU32DIP_WriteUART1("Hello!\r\n");
  while (1) {
    while (NU32DIP_USER == 0) {  // wait for button press
      int i=0;
      for (i=0;i<100;i++){
        sprintf(message,"%f\r\n",1.65 * sin(i * 2 * PI / 100) + 1.65);
        NU32DIP_WriteUART1(message);
        _CP0_SET_COUNT(0);
        while (_CP0_GET_COUNT() < 240000) {}
      }
      while (NU32DIP_USER == 0) {}  // wait for button release
    }
  }
}