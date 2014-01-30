/*
  &object:Ultrasonic Ranging and display the distance in LCD4884
  &LCD4884 connections Specification:
    Power Supply: 5V
    SPI Interface (using Arduino Digital Pin 2,3,4,5,6)
    One Reset button
    Backlit control (Arduino DIO Pin 7) 
	for more detial see the picture  
	
 &Ultrasonic connection Specification:
 *Vcc pin to  +5
 *Trig pin to digital pin 12
 *Echo pin to digital pin 13
 fuction of this exampla :get the distance  data from HCS-RO4 and display in LCD4884
							Any suggestions are welcome.upload the code to Arduino and you will see 	"Dis_CM:0XX"
			notice :if you before you try to call lcd.LCD_write_string to display a value ,you should read  "LCD4884.ccp first"
		E-mail:sky@thesunrain.com
 */
#include <LiquidCrystal.h>
#define CM 1      //Centimeter
#define INC 0     //Inch
#define TP 12      //Trig_pin
#define EP 13      //Echo_pin
#include "LCD4884.h"
#include "SainSmart.h"
#include "SainSmart_chinese.h"

// these constants describe the pins. They won't change:
const int groundpin = 18;             // analog input pin 4 -- ground
const int powerpin = 19;              // analog input pin 5 -- voltage

const char nomber_[]="0123456789";
//char nomber_s[8]="0000";
char distance_text[]="000";

void setup()
{

  

  // Provide ground and power by using the analog inputs as normal
  // digital pins.  This makes it possible to directly connect the
  // breakout board to the Arduino.  If you use the normal 5V and
  // GND pins on the Arduino, you can remove these lines.
  pinMode(groundpin, OUTPUT);
  pinMode(powerpin, OUTPUT);
  digitalWrite(groundpin, LOW); 
  digitalWrite(powerpin, HIGH);
  
  // Setup timer2 -- Prescaler/256
  TCCR2A &= ~((1<<WGM21) | (1<<WGM20));
  TCCR2B &= ~(1<<WGM22);
  TCCR2B = (1<<CS22)|(1<<CS21);      

  ASSR |=(0<<AS2);

  // Use normal mode  
  TCCR2A =0;    
  //Timer2 Overflow Interrupt Enable  
  TIMSK2 |= (0<<OCIE2A);
  TCNT2=0x6;  // counting starts from 6;  
  TIMSK2 = (1<<TOIE2);    



  SREG|=1<<SREG_I;
  
  //confign 4884
  lcd.LCD_init();
  lcd.LCD_clear();
  lcd.backlight(ON);//Turn on the backlight
  //config  HCS-RO4  control pins
  pinMode(TP,OUTPUT);       // set TP output for trigger  
  pinMode(EP,INPUT);   
  //lcd.backlight(OFF); // Turn off the backlight  
}


/* loop */

void loop()
{
  SainSmart();
}

/* menu functions */
// waiting for center key press



void SainSmart(){
  long microseconds = TP_init();
  long distacne_cm = Distance(microseconds, CM);
  
  LCD_cover_data(distacne_cm,distance_text);
  
 // nomber_s[4]=char(123);
  lcd.LCD_write_string( 3, 2, "Dis_CM:", MENU_NORMAL);
  lcd.LCD_write_string( 45, 2, distance_text, MENU_NORMAL);
  lcd.LCD_write_string(10, 5, "SainSmart", MENU_HIGHLIGHT );
  //waitfor_OKkey();


}

//conculate the distance 
long Distance(long time, int flag)
{
  /*
  
  */
  long distacne;
  if(flag)
    distacne = time /29 / 2  ;     // Distance_CM  = ((Duration of high level)*(Sonic :340m/s))/2
                                   //              = ((Duration of high level)*(Sonic :0.034 cm/us))/2
                                   //              = ((Duration of high level)/(Sonic :29.4 cm/us))/2
  else
    distacne = time / 74 / 2;      // INC
  return distacne;
}

//get the distance date from HCS-RO4  and cover to an value 
long TP_init()
{                     
  digitalWrite(TP, LOW);                    
  delayMicroseconds(2);
  digitalWrite(TP, HIGH);                 // pull the Trig pin to high level for more than 10us impulse 
  delayMicroseconds(10);
  digitalWrite(TP, LOW);
  long microseconds = pulseIn(EP,HIGH);   // waits for the pin to go HIGH, and returns the length of the pulse in microseconds
  return microseconds;                    // return microseconds
}


ISR(TIMER2_OVF_vect) {  
  TCNT2  = 6;
 // update_adc_key();
}

//cover number to string 
void LCD_cover_data( long data_tem,char *nomber_s){
  char k=0;
  if(data_tem!=0){
     for(int i=2;i!=0&&data_tem!=0;i--){
         k=data_tem%10;
         nomber_s[i]=nomber_[k];
         data_tem/=10;
     
	}
	}
}


