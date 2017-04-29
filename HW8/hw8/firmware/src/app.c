/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "LCD.h"
#include "i2c.h"
#include "IMU.h"
#include <stdio.h>
// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;
   

    
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;
            
       
        
            if (appInitialized)
            {
            
                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
            SPI1_init();
    i2c_master_setup();
    
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
    while(1) {
	    // use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to test the PIC timing
		  // remember the core timer runs at half the sysclk
        _CP0_SET_COUNT(0);
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
        
        
        
        
        
    }
            
            
            
            
            
            
            break;
        }

        /* TODO: implement your application state machine.*/
        

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

 

/*******************************************************************************
 End of File
 */
