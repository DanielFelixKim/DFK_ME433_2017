#include <xc.h>
#include <stdio.h>
#include "IMU9.h"
#include "LCD9.h"

void SPI1_init() {
	SDI1Rbits.SDI1R = 0b0100; // B8 is SDI1
    RPA1Rbits.RPA1R = 0b0011; // A1 is SDO1
    TRISBbits.TRISB7 = 0; // SS is B7
    LATBbits.LATB7 = 1; // SS starts high

    // A0 / DAT pin
    ANSELBbits.ANSB15 = 0;
    TRISBbits.TRISB15 = 0;
    LATBbits.LATB15 = 0;
	
	SPI1CON = 0; // turn off the spi module and reset it
    SPI1BUF; // clear the rx buffer by reading from it
    SPI1BRG = 1; // baud rate to 12 MHz [SPI1BRG = (48000000/(2*desired))-1]
    SPI1STATbits.SPIROV = 0; // clear the overflow bit
    SPI1CONbits.CKE = 1; // data changes when clock goes from hi to lo (since CKP is 0)
    SPI1CONbits.MSTEN = 1; // master operation
    SPI1CONbits.ON = 1; // turn on spi1
}

unsigned char spi_io(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}

void LCD_command(unsigned char com) {
    LATBbits.LATB15 = 0; // DAT
    LATBbits.LATB7 = 0; // CS
    spi_io(com);
    LATBbits.LATB7 = 1; // CS
}

void LCD_data(unsigned char dat) {
    LATBbits.LATB15 = 1; // DAT
    LATBbits.LATB7 = 0; // CS
    spi_io(dat);
    LATBbits.LATB7 = 1; // CS
}

void LCD_data16(unsigned short dat) {
    LATBbits.LATB15 = 1; // DAT
    LATBbits.LATB7 = 0; // CS
    spi_io(dat>>8);
    spi_io(dat);
    LATBbits.LATB7 = 1; // CS
}

void LCD_init() {
    int time = 0;
    LCD_command(CMD_SWRESET);//software reset
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 48000000/2/2) {} //delay(500);

	LCD_command(CMD_SLPOUT);//exit sleep
    time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/200) {} //delay(5);

	LCD_command(CMD_PIXFMT);//Set Color Format 16bit
	LCD_data(0x05);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/200) {} //delay(5);

	LCD_command(CMD_GAMMASET);//default gamma curve 3
	LCD_data(0x04);//0x04
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {} //delay(1);

	LCD_command(CMD_GAMRSEL);//Enable Gamma adj
	LCD_data(0x01);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {} //delay(1);

	LCD_command(CMD_NORML);

	LCD_command(CMD_DFUNCTR);
	LCD_data(0b11111111);
	LCD_data(0b00000110);

    int i = 0;
	LCD_command(CMD_PGAMMAC);//Positive Gamma Correction Setting
	for (i=0;i<15;i++){
		LCD_data(pGammaSet[i]);
	}

	LCD_command(CMD_NGAMMAC);//Negative Gamma Correction Setting
	for (i=0;i<15;i++){
		LCD_data(nGammaSet[i]);
	}

	LCD_command(CMD_FRMCTR1);//Frame Rate Control (In normal mode/Full colors)
	LCD_data(0x08);//0x0C//0x08
	LCD_data(0x02);//0x14//0x08
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {} //delay(1);

	LCD_command(CMD_DINVCTR);//display inversion
	LCD_data(0x07);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {} //delay(1);

	LCD_command(CMD_PWCTR1);//Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
	LCD_data(0x0A);//4.30 - 0x0A
	LCD_data(0x02);//0x05
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {} //delay(1);

	LCD_command(CMD_PWCTR2);//Set BT[2:0] for AVDD & VCL & VGH & VGL
	LCD_data(0x02);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {} //delay(1);

	LCD_command(CMD_VCOMCTR1);//Set VMH[6:0] & VML[6:0] for VOMH & VCOML
	LCD_data(0x50);//0x50
	LCD_data(99);//0x5b
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {} //delay(1);

	LCD_command(CMD_VCOMOFFS);
	LCD_data(0);//0x40
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {} //delay(1);

	LCD_command(CMD_CLMADRS);//Set Column Address
	LCD_data16(0x00);
    LCD_data16(_GRAMWIDTH);

	LCD_command(CMD_PGEADRS);//Set Page Address
	LCD_data16(0x00);
    LCD_data16(_GRAMHEIGH);

	LCD_command(CMD_VSCLLDEF);
	LCD_data16(0); // __OFFSET
	LCD_data16(_GRAMHEIGH); // _GRAMHEIGH - __OFFSET
	LCD_data16(0);

	LCD_command(CMD_MADCTL); // rotation
    LCD_data(0b00001000); // bit 3 0 for RGB, 1 for GBR, rotation: 0b00001000, 0b01101000, 0b11001000, 0b10101000

	LCD_command(CMD_DISPON);//display ON
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {} //delay(1);

	LCD_command(CMD_RAMWR);//Memory Write
}


void LCD_drawPixel(unsigned short x, unsigned short y, unsigned short color) {
    // check boundary
    LCD_setAddr(x,y,x+1,y+1);
    LCD_data16(color);
}

void LCD_setAddr(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1) {
    LCD_command(CMD_CLMADRS); // Column
    LCD_data16(x0);
	LCD_data16(x1);

	LCD_command(CMD_PGEADRS); // Page
	LCD_data16(y0);
	LCD_data16(y1);

	LCD_command(CMD_RAMWR); //Into RAM
}

void LCD_clearScreen(unsigned short color) {
    int i;
    LCD_setAddr(0,0,_GRAMWIDTH,_GRAMHEIGH);
		for (i = 0;i < _GRAMSIZE; i++){
			LCD_data16(color);
		}
}

void LCD_Draw_Char(unsigned short x, unsigned short y, char c){
    int i;
    int j;
    char row = c - 0x20;
 
    if ((x<124) & (y<121)){
        for(i=0; i<5; i++){
            char byte = ASCII[row][i];
            for (j=7; j>=0; j--){
                if (((byte>>j)&1) == 1){
                    LCD_drawPixel(x+i, y+j, 0x0000);
                }
                if (((byte>>j)&1) == 0){
                    LCD_drawPixel(x+i, y+j, 0xFFFF);
                }
            }
        }
    }
    else{
        LCD_clearScreen(BLACK);
    }
}

void LCD_Draw_Line_X(unsigned short x, unsigned short y, int line_len_x){
    
    if(line_len_x > 0){
        int i = 0;
        if(line_len_x >50){
            line_len_x = 50;
        }
        for (i = 0; i<line_len_x;i++){
             LCD_drawPixel(x-i, y, 0x0000);
        }
        if(line_len_x < 50){
            for (i = line_len_x; i<50;i++){
                    LCD_drawPixel(x-i, y, 0xFFFF);
            }
        }
    }
        
    if(line_len_x < 0){
        int i = 0;
        if(line_len_x < -50){
            line_len_x = -50;
        }
        for (i = 0; i>line_len_x;i--){
             LCD_drawPixel(x-i, y, 0x0000);
        }
        if(line_len_x > -50){
            for (i = line_len_x; i > -50;i--){
                    LCD_drawPixel(x-i, y, 0xFFFF);
            }
        }
    }
}
    


void LCD_Draw_Line_Y(unsigned short x, unsigned short y, short line_len_y){
    if(line_len_y > 0){
        int i = 0;
        if(line_len_y >50){
            line_len_y = 50;
        }
        for (i = 0; i<line_len_y;i++){
             LCD_drawPixel(x, y-i, 0x0000);
        }
        if(line_len_y < 50){
            for (i = line_len_y; i<50;i++){
                    LCD_drawPixel(x, y-i, 0xFFFF);
            }
        }
    }
        
    if(line_len_y < 0){
        int i = 0;
        if(line_len_y < -50){
            line_len_y = -50;
        }
        for (i = 0; i>line_len_y;i--){
             LCD_drawPixel(x, y-i, 0x0000);
        }
        if(line_len_y > -50){
            for (i = line_len_y; i > -50;i--){
                    LCD_drawPixel(x, y-i, 0xFFFF);
            }
        }
    }
}

void LCD_Draw_String(unsigned short x, unsigned short y, char *string){
    int i = 0;
    while(string[i]){
         LCD_Draw_Char(x, y, string[i]);
         x=x+7;
         if (x>123){
             x = 0;
             y = y + 10;
         }
         if (y>120){
             y = 0;
         }
         i++;
    }
}

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
    

    LCD_init();
    LCD_clearScreen(WHITE);

// **********
    
   
    unsigned char check_who_am_i[1];
    I2C_read_multiple(IMU_REG, WHO_AM_I, check_who_am_i, 1);
}



