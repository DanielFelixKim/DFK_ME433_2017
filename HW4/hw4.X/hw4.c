#include<math.h>
#include"PIC32.h"

#define CS LATBbits.LATB8       // chip select pin

unsigned char sinwave[100]; //Sin wave
unsigned char triwave[100]; //Triangle wave

// send a byte via spi and return the response
unsigned char spi_io(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}

void initSPI1(){
    TRISBbits.TRISB8 = 0; 
    CS = 1; //Don't read
    RPA1Rbits.RPA1R = 0b0011; //SDO set to Pin B13
    SPI1CON = 0;              // turn off the spi module and reset it
    SPI1BUF;                  // clear the rx buffer by reading from it
    SPI1BRG = 79999;            // baud rate to 12 MHz [SPI4BRG = (48000000/(2*desired))-1]
    SPI1STATbits.SPIROV = 0;  // clear the overflow bit
    SPI1CONbits.CKE = 1;      // data changes when clock goes from hi to lo (since CKP is 0)
    SPI1CONbits.MSTEN = 1;    // master operation
    SPI1CONbits.ON = 1;       // turn on spi 4
}

void setVoltage(char channel, unsigned char voltage){
    unsigned short send_command = 0x0000;
    send_command |= (channel << 15);
    send_command |= (0b111 << 14);
    send_command |= (voltage << 4);
    CS = 0;
    spi_io(send_command >> 8);
    spi_io(send_command);
    CS = 1;
}

void waves(){
    int i;
    for(i = 0; i < 100; i++){
        sinwave[i] = (unsigned char) 255.0 * ((sin((i/100.0)*2.0*3.14159) + 1.0)/2.0);
        triwave[i] = (unsigned char) 255.0*(i/99.0);
    }
}



main(void){
    initSPI1();
    waves();
    
    while(1){
        int ii;
        for (ii = 0; ii < 100 ; ii++){
            setVoltage(0,sinwave[ii]);
            setVoltage(0,triwave[ii]);
        _CP0_SET_COUNT(0);
        
        while(_CP0_GET_COUNT() < 2400){
           ;
        }
        }
    }
}