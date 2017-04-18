#include<math.h>
#include"PIC32.h"

//Function prototypes
void i2c_master_setup(void);              // set up I2C 1 as a master, at 100 kHz
void i2c_master_start(void);              // send a START signal
void i2c_master_restart(void);            // send a RESTART signal
void i2c_master_send(unsigned char byte); // send a byte (either an address or data)
unsigned char i2c_master_recv(void);      // receive a byte of data
void i2c_master_ack(int val);             // send an ACK (0) or NACK (1)
void i2c_master_stop(void);               // send a stop
void initExpander();
void setExpander(char pin, char level);
char getExpander();

int writeaddr = 0b01000000;
int readaddr = 0b01000001;


void main(void){
     __builtin_disable_interrupts();
     
    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here

    __builtin_enable_interrupts();
    
    i2c_master_setup();
    initExpander();
    
    while(1){

        if ((getExpander() & 0b10000000) == 0b10000000){
            setExpander(0b00000001,1);
        }
        else {
            setExpander(0b00000001,0);
        }
    }
    
}




//I2C Functions
void i2c_master_setup(void) {
  ANSELBbits.ANSB2 = 0;
  ANSELBbits.ANSB3 = 0;
  I2C2BRG = 233;            // I2CBRG = [1/(2*Fsck) - PGD]*Pblck - 2 
                                    // look up PGD for your PIC32
  I2C2CONbits.ON = 1;               // turn on the I2C1 module
  i2c_master_start();
  i2c_master_send(writeaddr);
  i2c_master_send(0x00);
  i2c_master_send(0b11110000);
  i2c_master_stop();
}

// Start a transmission on the I2C bus
void i2c_master_start(void) {
    I2C2CONbits.SEN = 1;            // send the start bit
    while(I2C2CONbits.SEN) { ; }    // wait for the start bit to be sent
}

void i2c_master_restart(void) {     
    I2C2CONbits.RSEN = 1;           // send a restart 
    while(I2C2CONbits.RSEN) { ; }   // wait for the restart to clear
}

void i2c_master_send(unsigned char byte) { // send a byte to slave
  I2C2TRN = byte;                   // if an address, bit 0 = 0 for write, 1 for read
  while(I2C2STATbits.TRSTAT) { ; }  // wait for the transmission to finish
  if(I2C2STATbits.ACKSTAT) {        // if this is high, slave has not acknowledged
    // ("I2C2 Master: failed to receive ACK\r\n");
  }
}

unsigned char i2c_master_recv(void) { // receive a byte from the slave
    I2C2CONbits.RCEN = 1;             // start receiving data
    while(!I2C2STATbits.RBF) { ; }    // wait to receive the data
    return I2C2RCV;                   // read and return the data
}

void i2c_master_ack(int val) {        // sends ACK = 0 (slave should send another byte)
                                      // or NACK = 1 (no more bytes requested from slave)
    I2C2CONbits.ACKDT = val;          // store ACK/NACK in ACKDT
    I2C2CONbits.ACKEN = 1;            // send ACKDT
    while(I2C2CONbits.ACKEN) { ; }    // wait for ACK/NACK to be sent
}

void i2c_master_stop(void) {          // send a STOP:
  I2C2CONbits.PEN = 1;                // comm is complete and master relinquishes bus
  while(I2C2CONbits.PEN) { ; }        // wait for STOP to complete
}

void initExpander(){
    i2c_master_start();
    i2c_master_send(writeaddr);
    i2c_master_send(0x0A);
    i2c_master_send(0x0000);
    i2c_master_stop();
}

void setExpander(char pin, char level){
    char level_send = 0xFFFF;
    if (level == 0){
        level_send = 0x0000;
    }
    i2c_master_start();
    i2c_master_send(writeaddr);
    i2c_master_send(0x0A);
    i2c_master_send(level_send & pin);
    i2c_master_stop();
}

char getExpander(){
    i2c_master_start();
    i2c_master_send(writeaddr);
    i2c_master_send(0x09);
    i2c_master_restart();
    i2c_master_send(readaddr);
    char read = i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
    return read;
}