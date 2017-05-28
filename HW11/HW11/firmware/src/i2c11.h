#ifndef IC2
#define IC2

#include<sys/attribs.h> 
#include<xc.h>    

void i2c_master_setup(void);              // set up I2C 1 as a master, at 100 kHz
void i2c_master_start(void);              // send a START signal
void i2c_master_restart(void);            // send a RESTART signal
void i2c_master_send(unsigned char byte); // send a byte (either an address or data)
unsigned char i2c_master_recv(void);      // receive a byte of data
void i2c_master_ack(int val);             // send an ACK (0) or NACK (1)
void i2c_master_stop(void);               // send a stop
void I2C_send_register(unsigned char address, unsigned char * data, unsigned char length);
void I2C_read_multiple(unsigned char address, unsigned char register_s, unsigned char * data, unsigned char length);

#endif