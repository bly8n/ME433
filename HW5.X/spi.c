#include "spi.h"
#include "nu32dip.h"
#include <math.h>
#define PI 3.1415926
#define num_samples 100
void initSPI(); // initialize SPI1
unsigned char spi_io(unsigned char o);
int main(){
    NU32DIP_Startup();
    int i = 0; 
    initSPI(); // call all init
    // pre-calculate voltage values for sine and triangle waves
    float v_sin[num_samples]; // voltage values for sine wave
    float v_tri[num_samples]; // voltage values for triangle wave

    // calculate voltage values for sine wave
    for (i = 0; i<num_samples; i++) {
        v_sin[i]=1.65+1.65*sin(2*PI*i/num_samples);
    }
    for (i = 0; i<num_samples/2; i++) {
        v_tri[i]=3.3*i/(num_samples/2);
    }
    for (i = num_samples/2; i<num_samples; i++) {
        v_tri[i]=3.3*(num_samples-i)/(num_samples/2);
    }

    // calculate voltage values for triangle wave

    while(1){
        // send voltage values for sine wave
        for (i=0;i<num_samples;i++) {
            // math to make sinewave 0 to 1023
            unsigned short v1 = v_sin[i]*1023/3.3;
            unsigned short v2 = v_tri[i]*1023/3.3;
            unsigned short t_a = 0;
            unsigned short t_b = 0;
            t_a = 0b111 << 12;
            unsigned char a_or_b=0;
            t_a = t_a|(a_or_b<<15);
            //stick in the v<<asfergr
            t_a = t_a|(v1<<2);
            t_b = 0b111 << 12;
            a_or_b=1;
            t_b = t_b|(a_or_b<<15);
            //stick in the v<<asfergr
            t_b = t_b|(v2<<2);
            // send the voltage with SPI
            //output sin wave
            LATAbits.LATA0 = 0; // low CS
            spi_io(t_a>>8);
            spi_io(t_a);
            LATAbits.LATA0=1; // high CS
            //output triangle wave
            LATAbits.LATA0=0; // low CS
            spi_io(t_b>>8);
            spi_io(t_b);
            LATAbits.LATA0=1; // high CS
            _CP0_SET_COUNT(0);
            while (_CP0_GET_COUNT() < 240000/2) {}
        }
    }
}
void initSPI() {
    // Pin B14 has to be SCK1
    ANSELB=0;// Turn off analog pins
    //...
    TRISAbits.TRISA0=0;// Make an output pin for CS
    LATAbits.LATA0=1;//...
    //
    // Set A1 as SDO1
    RPA1Rbits.RPA1R=0b0011;
    // Set B1 as SDI1
    SDI1Rbits.SDI1R=0b0010;

    // setup SPI1
    SPI1CON = 0; // turn off the spi module and reset it
    SPI1BUF; // clear the rx buffer by reading from it
    SPI1BRG = 1000; // 1000 for 24kHz, 1 for 12MHz; // baud rate to 10 MHz [SPI1BRG = (48000000/(2*desired))-1]
    SPI1STATbits.SPIROV = 0; // clear the overflow bit
    SPI1CONbits.CKE = 1; // data changes when clock goes from hi to lo (since CKP is 0)
    SPI1CONbits.MSTEN = 1; // master operation
    SPI1CONbits.ON = 1; // turn on spi 
}


// send a byte via spi and return the response
unsigned char spi_io(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}
// for (i = 0; i < num_samples / 2; i++) {
       // vs_tri[i] = 3.3 * i / (num_samples / 2);
    //}
    //for (i = num_samples / 2; i < num_samples; i++) {
        //vs_tri[i] = 3.3 * (num_samples - i) / (num_samples / 2);
    //}