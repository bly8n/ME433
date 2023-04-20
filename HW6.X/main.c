#include "i2c_master_noint.h"
#include "nu32dip.h"
int read_from_pin(unsigned char address, unsigned char reg);
void set_pin(unsigned char address, unsigned char reg, unsigned char value);
void blink(int iterations, int time_ms);
int main(void){
    NU32DIP_Startup();
    i2c_master_setup();//init the i2c pins
    INT1Rbits.INT1R=0b0010; //set B0 pin to INT
    set_pin(0b01000000,0x00,0b01111111); //GP7 is an output pin and GP0 is input 
    while(1){
    //blink the green LED
    blink(1,200);
    //blink GP7
    unsigned char button_press = read_from_pin(0b01000000,0x09);
    if(button_press){
      set_pin(0b01000000,0x0A,0b10000000); //turn on GP7
    }
    else{
      set_pin(0b01000000,0x0A,0b00000000); //turn off GP7
    }
    }
}
void set_pin(unsigned char address, unsigned char reg, unsigned char value){
   i2c_master_start();
   i2c_master_send(address);//0b01000000// or 0b0100001 for read from the chip  
   i2c_master_send(reg);//0x0A
   i2c_master_send(value);//send the value to turn on GP7// 0b10000000  //turn off 0b00000000
   i2c_master_stop();//send stop bit 
}
int read_from_pin(unsigned char address, unsigned char reg){
  i2c_master_start();//send start bit
  i2c_master_send(address);//send address with write bit
  i2c_master_send(reg);//send GPIO reg you want to read from
  i2c_master_restart();//restart
  i2c_master_send(address|0b00000001);//send address with read bit
  unsigned char r = i2c_master_recv();
  i2c_master_ack(1);//send the ACK bit
  i2c_master_stop();//send the stop bit
  return (r & 0b00000001);
}
void blink(int iterations, int time_ms){
	int i;
	unsigned int t;
	for (i=0; i< iterations; i++){
		NU32DIP_GREEN = 0; // on
		t = _CP0_GET_COUNT(); // should really check for overflow here
		// the core timer ticks at half the SYSCLK, so 24000000 times per second
		// so each millisecond is 24000 ticks
		// wait half in each delay
		while(_CP0_GET_COUNT() < t + 12000*time_ms){}
		
		NU32DIP_GREEN = 1; // off
		t = _CP0_GET_COUNT(); // should really check for overflow here
		while(_CP0_GET_COUNT() < t + 12000*time_ms){}
	}
}