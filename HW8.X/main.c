#include "nu32dip.h" // constants, functions for startup and UART
#include "i2c_master_noint.h"
#include "font.h"
#include <stdio.h>
#include "ssd1306.h"
#include "mpu6050.h"
void drawChar(char letter, char x, char y);
void drawString(char *m, char x, char y);
void blink(int, int);
int main(void) {
    NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init
    i2c_master_setup();
    init_mpu6050();
    ssd1306_setup();
    ssd1306_clear();
    unsigned char d[14];
	// floats to store the data
	float az;
    int i=0;
    //for (i = 0; i < 100; i++) {
        //char m[100];
        //sprintf(m, "my var=%d",i);
        //drawString(m,10,10);
        //ssd1306_update();
    //}
    char a[200];
    char m[100];
    unsigned char who;
	// print whoami
    who = whoami();
	// if whoami is not 0x68, stuck in loop with LEDs on
    if (who !=0x68){
        while(1){blink(1,500);}
    }
    while (1) {
		// use core timer for exactly 100Hz loop
        _CP0_SET_COUNT(0);
        blink(1, 100);
        sprintf(m, "my var=%d",i);
        drawString(m,10,0);
        // read IMU
        burst_read_mpu6050(d);
		// convert data
        az=conv_zXL(d);
        // print out the data
        sprintf(a,"az=%f\r\n",az);
        drawString(a,10,10);
        //ssd1306_update();
        while (_CP0_GET_COUNT() < 48000000 / 2 / 100) {
        }
        float fps = 24000000.0 / _CP0_GET_COUNT();
        //char fps_message[200];
        //sprintf(a, "az=%f\r\n)",az);
        sprintf(a, "fps =%f\r\n",fps);
        drawString(a, 10, 20);
        ssd1306_update();
        i++;
    }
}
void drawChar(char letter, char x, char y){
     int i, j;
     for (i = 0; i < 5; i++) {
         unsigned char column = ASCII[letter -0x20][i];
         for (j = 0; j < 8; j++) {
             ssd1306_drawPixel(x + i, y + j, (column>>j)&0b1);
         }
     }
}
void drawString(char *m, char x, char y){
  int k=0;
  while (m[k]!=0){
     drawChar(m[k],x+5*k,y);
     k++;
  }
}
void blink(int iterations, int time_ms) {
    int i;
    unsigned int t;
    for (i = 0; i < iterations; i++) {
        NU32DIP_GREEN = 0; // on
        NU32DIP_YELLOW = 1; // off
        t = _CP0_GET_COUNT(); // should really check for overflow here
        // the core timer ticks at half the SYSCLK, so 24000000 times per second
        // so each millisecond is 24000 ticks
        // wait half in each delay
        while (_CP0_GET_COUNT() < t + 12000 * time_ms) {
        }

        NU32DIP_GREEN = 1; // off
        NU32DIP_YELLOW = 0; // on
        t = _CP0_GET_COUNT(); // should really check for overflow here
        while (_CP0_GET_COUNT() < t + 12000 * time_ms) {
        }
    }
}