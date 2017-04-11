#include<math.h>
#include"PIC32.h"

#define CS LATBbits.LATB8       // chip select pin

unsigned char sinwave[300]; //Sin wave
unsigned char triwave[300]; //Triangle wave

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
    SDI1Rbits.SDI1R = 0b0100; // SDI 
    RPB13Rbits.RPB13R = 0b0011; //SDO set to Pin B13
    SPI1CON = 0;              // turn off the spi module and reset it
    SPI1BUF;                  // clear the rx buffer by reading from it
    SPI1BRG = 0x1;            // baud rate to 12 MHz [SPI4BRG = (48000000/(2*desired))-1]
    SPI1STATbits.SPIROV = 0;  // clear the overflow bit
    SPI1CONbits.MODE32 = 0;
    SPI1CONbits.MODE16 = 1;
    SPI1CONbits.CKE = 1;      // data changes when clock goes from hi to lo (since CKP is 0)
    SPI1CONbits.MSTEN = 1;    // master operation
    SPI1CONbits.ON = 1;       // turn on spi 4
}

void setVoltage(char channel, unsigned char voltage){
    CS = 0;
   
    CS = 1;
}