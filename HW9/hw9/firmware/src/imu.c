#include <xc.h>
#include "IMU.h"
#include "LCD.h"

void imu_init()
{
    char send_I2C_vals[2];
    
    send_I2C_vals[0] = CTRL1_XL; // Accel reg
    send_I2C_vals[1] = 0x80; // High performance 1.66kHz, 2g, 100hz
    I2C_send_register(IMU_REG, send_I2C_vals, 2);
    
    send_I2C_vals[0] = CTRL2_G; // Gyroscope reg
    send_I2C_vals[1] = 0x80; // High performance
    I2C_send_register(IMU_REG, send_I2C_vals, 2);

    send_I2C_vals[0] = CTRL3_C; // Increment
    send_I2C_vals[1] = 0x04; // Continue incrementing registers 
    I2C_send_register(IMU_REG, send_I2C_vals, 2);
    
           unsigned char data_vals[14];
    short temperature;
    short accel_X;
    short accel_X_temp = 0;
    short accel_X_delta = 0;
    short accel_Y;
    short accel_Y_temp = 0;
    short accel_Y_delta = 0;
    short accel_Z;
    short gyro_X;
    short gyro_Y;
    short gyro_Z;
    LCD_init();
    LCD_clearScreen(WHITE);
    
   
    unsigned char check_who_am_i[1];
    I2C_read_multiple(IMU_REG, WHO_AM_I, check_who_am_i, 1);
 
    int ii=0;
    int jj=0;
    int start = 0;
}

void imu_read(){
     char send_msg[100];
        sprintf(send_msg, "IMU Test");
        LCD_Draw_String(1,2, send_msg);
        
        I2C_read_multiple(IMU_REG, OUT_TEMP_L, data_vals, 14);
        temperature = data_vals[1] << 8 | data_vals[0];
        gyro_X = data_vals[3] << 8 | data_vals[2];
        gyro_Y = data_vals[5] << 8 | data_vals[4];
        gyro_Z = data_vals[7] << 8 | data_vals[6];
        accel_X = data_vals[9] << 8 | data_vals[8];
        accel_Y = data_vals[11] << 8 | data_vals[10];
        accel_Z = data_vals[13] << 8 | data_vals[12];
}

    
    while(1) {
	    // use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to test the PIC timing
		  // remember the core timer runs at half the sysclk
        _CP0_SET_COUNT(0);
       
        
        
        while(_CP0_GET_COUNT()< 4800000){
            ;
        }
        if (start == 0){
            accel_X = accel_X - accel_X; 
            accel_Y = accel_Y - accel_Y; 
        }
        start = 1;
        if(accel_X + 500 > accel_X_temp){
            ii= ii + 5;
        }
        if(accel_X - 500 < accel_X_temp){
            ii= ii - 5;
        }
        if(accel_Y + 500 > accel_Y_temp){
            jj = jj + 5;
        }
        if(accel_Y - 500 < accel_Y_temp){
            jj = jj - 5;
        }
        
//        LCD_Draw_Line_X(64,64,ii);
//       
//        
//        LCD_Draw_Line_Y(64,64,jj);
        
        
        
        accel_X_temp = accel_X;
        accel_Y_temp = accel_Y;
        
        sprintf(send_msg, "%d", check_who_am_i[0]);
        LCD_Draw_String(1, 16, send_msg);
        
//        sprintf(send_msg, "%d   ", ii);
//        LCD_Draw_String(64, 1, send_msg);
//        
//        sprintf(send_msg, "%d   ", jj);
//        LCD_Draw_String(90, 16, send_msg);
        
        sprintf(send_msg, "X:%d   ", accel_X);
        LCD_Draw_String(84, 80, send_msg);

 
        sprintf(send_msg, "Y%d   ", accel_Y);
        LCD_Draw_String(84, 100, send_msg);
        
//        sprintf(send_msg, "%d   ", accel_X_delta);
//        LCD_Draw_String(64, 22, send_msg);
//        
//        sprintf(send_msg, "%d   ", accel_Y_delta);
//        LCD_Draw_String(64, 32, send_msg);
//        
        LCD_Draw_Line_X(64,64,ii);
        LCD_Draw_Line_Y(64,64,jj);