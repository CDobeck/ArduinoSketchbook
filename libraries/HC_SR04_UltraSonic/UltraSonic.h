
#ifndef UltraSonic_h
#define UltraSonic_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

//Used for ultrasonic sensor
#define CM 1      //Centimeter
#define INC 0     //Inch
#define TP 12      //Trig_pin
#define EP 13      //Echo_pin


class UltraSonic
{
public:
UltraSonic(void);
void initialize(void);
void initialize(int, int);
long readRawDistance();
long distance(long time, int flag);
long readDistanceCM(void);
long readDistanceInch(void);
//void LCD_write_byte(unsigned char dat, unsigned char dat_type);
//void LCD_draw_bmp_pixel(unsigned char X,unsigned char Y,unsigned char *map,unsigned char Pix_x,unsigned char Pix_y);
};
extern UltraSonic ultrasonic;   
              
#endif   // 
