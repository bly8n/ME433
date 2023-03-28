#include "nu32dip.h" 
#define NUMSAMPS 1000 // number of points in waveform
#define PLOTPTS 200 // number of data points to plot
#define DECIMATION 10 // plot every 10th point
static volatile int Waveform[NUMSAMPS]; // waveform
static volatile int ADCarray[PLOTPTS];  // measured values to plot
static volatile int REFarray[PLOTPTS];  // reference values to plot
static volatile int StoringData = 0;    // if this flag = 1, currently storing
                                        // plot data
static volatile float Kp = 0, Ki = 0;   // control gains

void makeWaveform();
unsigned int adc_sample_convert(int pin);
void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Controller(void) { // _TIMER_2_VECTOR = 8
  static int counter = 0; // initialize counter once
  static int plotind = 0; // index for data arrays; counts up to PLOTPTS
  static int decctr = 0; // counts to store data one every DECIMATION
  static int eint=0;
  unsigned int adcval = adc_sample_convert(1); //
  int e = -adcval+ Waveform[counter];
  
  eint = eint+e;
  if (eint > 1000){
    eint=1000;
  }
  if (eint < -1000){
    eint=-1000;
  }
  float u = Kp*e + Ki*eint;
  float unew = u + 50.0;
  if (unew > 100.0) {
    unew = 100.0;
  } 
  else if (unew < 0.0) { 
    unew = 0.0;
  }
  OC1RS = (unsigned int) ((unew/100.0) * PR3);
  if (StoringData) {
     decctr++;
     if (decctr == DECIMATION) { // after DECIMATION control loops,
       decctr = 0; // reset decimation counter
       ADCarray[plotind] = adcval; // store data in global arrays
       REFarray[plotind] = Waveform[counter];
       plotind++; // increment plot data index
     }
     if (plotind == PLOTPTS) { // if max number of plot points plot is reached,
       plotind = 0; // reset the plot index
       StoringData = 0; // tell main data is ready to be sent to MATLAB
     }
  }
  counter++; // add one to counter every time ISR is entered
  if (counter == NUMSAMPS) {
    counter = 0; // roll the counter over when needed
  }
     IFS0bits.T2IF = 0; // insert line to clear interrupt flag
}

void makeWaveform() {
  int i = 0, center = 1023/2, A = 1023/4; // square wave, fill in center value and //1023amplitude
  for (i = 0; i < NUMSAMPS; ++i) {
    if ( i < NUMSAMPS/2) {
     Waveform[i] = center + A;
    } 
    else { 
     Waveform[i] = center - A;
    }
  }
}


int main(void) {
  NU32DIP_Startup();          // cache on, interrupts on, LED/button init, UART init
  makeWaveform();
  RPB7Rbits.RPB7R=0b0101;      //use pin B7 for OC1
  T3CONbits.TCKPS = 0;     // Timer3 prescaler N=4 (1:4)
  PR3 = 2400-1;              // period = (PR3+1) * N * 12.5 ns = 100 us, 20 kHz
  TMR3 = 0;                // initial TMR3 count is 0
  OC1CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
  OC1CONbits.OCTSEL=1;
  OC1RS = 0;             // duty cycle = OC1RS/(PR3+1) = 25%
  OC1R = 0;              // initialize before turning OC1 on; afterward it is read-only
  T3CONbits.ON = 1;        // turn on Timer2
  OC1CONbits.ON = 1;       // turn on OC1
  //AD1PCFGbits.PCFG14 = 0;                 // AN14 is an adc pin
  ANSELAbits.ANSA1 =0;                     //read voltage on AN1
  AD1CON3bits.ADCS = 1;                   // ADC clock period is Tad = 2*(ADCS+1)*Tpb =
                                          //                           2*3*12.5ns = 75ns
  AD1CON1bits.ADON = 1; 
   
  __builtin_disable_interrupts(); // INT step 2: disable interrupts at CPU
  // INT step 3: setup peripheral
  PR2 = 48000-1; // set period register
  TMR2 = 0; // initialize count to 0
  T2CONbits.TCKPS = 0; // set prescaler to 256
  T2CONbits.ON = 1; // turn on Timer1
  IPC2bits.T2IP = 5; // INT step 4: priority
  IPC2bits.T2IS = 0; // subpriority
  IFS0bits.T2IF = 0; // INT step 5: clear interrupt flag
  IEC0bits.T2IE = 1; // INT step 6: enable interrupt
  __builtin_enable_interrupts(); // INT step 7: enable interrupts at CPU
  char message[100]; // message to and from MATLAB
  float kptemp = 0, kitemp = 0; // temporary local gains
  int i = 0; // plot data loop counter
  //_CP0_SET_COUNT(0);       // delay 4 seconds to see the 25% duty cycle on a 'scope
  //while(_CP0_GET_COUNT() < 4 * 24000000) {
    //;
  //}
  //OC1RS = 1200;            // set duty cycle to 50%
  while (1) {
    NU32DIP_ReadUART1(message, sizeof(message)); // wait for a message from MATLAB
    sscanf(message, "%f %f" , &kptemp, &kitemp);
    __builtin_disable_interrupts(); // keep ISR disabled as briefly as possible
    Kp = kptemp; // copy local variables to globals used by ISR
    Ki = kitemp;
    __builtin_enable_interrupts(); // only 2 simple C commands while ISRs disabled
    StoringData = 1; // message to ISR to start storing data
    while (StoringData) { // wait until ISR says data storing is done
      ; // do nothing 
    }
    for (i=0; i<PLOTPTS; i++) { // send plot data to MATLAB
      // when first number sent = 1, MATLAB knows we’re done
    sprintf(message, "%d %d %d\r\n", PLOTPTS-i, ADCarray[i], REFarray[i]);
    NU32DIP_WriteUART1(message);
    }
  }
  return 0;
}

unsigned int adc_sample_convert(int pin) { // sample & convert the value on the given 
                                           // adc pin the pin should be configured as an 
                                           // analog input in AD1PCFG
    unsigned int elapsed = 0, finish_time = 0;
    AD1CHSbits.CH0SA = pin;                // connect chosen pin to MUXA for sampling
    AD1CON1bits.SAMP = 1;                  // start sampling
    elapsed = _CP0_GET_COUNT();
    finish_time = elapsed + 10;
    while (_CP0_GET_COUNT() < finish_time) { 
      ;                                   // sample for more than 250 ns
    }
    AD1CON1bits.SAMP = 0;                 // stop sampling and start converting
    while (!AD1CON1bits.DONE) {
      ;                                   // wait for the conversion process to finish
    }
    return ADC1BUF0;                      // read the buffer with the result
}