
/*
Editor: Cary Dobeck

TO-DO: clean up code, it's all long, ugly and hard to find things

Original version from Jacky at jacky@gmail.com, and Lauren from SainSmart
Original Date: 06.01.2012
*/

//BotBoarduino Manual (Based on Arduino Duemilanove)
//http://www.lynxmotion.com/images/html/build185.htm

//Currently used for test sensor printout
#define NUM_SENSORS 6

//Libraries for LCD screen
#include "LCD4884.h"
#include "DFrobot_bmp.h"
#include "DFrobot_chinese.h"

//SSC_32 integration files
#include "SSC_32_integration.h"

//defines for notes for speaker
#include "pitches.h"

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
byte noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4};

//Library for UltraSonic Sensor
#include <UltraSonic.h>

//Library for accelerometer in progress
//#include <AccelGyro.h>

//keypad debounce parameter
#define DEBOUNCE_MAX 15
#define DEBOUNCE_ON  10
#define DEBOUNCE_OFF  3 

#define NUM_KEYS 5

#define NUM_MENU_ITEM	7

//experimental for adding multiple menu screens
#define MAX_MENU_ITEMS 5

// joystick number
#define LEFT_KEY    0
#define CENTER_KEY  1
#define DOWN_KEY    2
#define RIGHT_KEY   3
#define UP_KEY      4

// menu starting points

#define MENU_X  10		// 0 - 83
#define MENU_Y   1		// 0 - 5

int adc_key_val[5] ={50, 200, 400, 600, 800};

// debounce counters
byte button_count[NUM_KEYS];
// button status - pressed/released
byte button_status[NUM_KEYS];
// button on flags for user program 
byte button_flag[NUM_KEYS];

// menu definition
char menu_items[NUM_MENU_ITEM][13] = {
  "Temperature",
  "Char Map",
  "UltraSonic",
  "About",
  "Test Sensors",
  "Test Code",
  "Init Robot"	
};

void (*menu_funcs[NUM_MENU_ITEM])(void) = {
  temperature,
  charmap,
  testUltraSonic,
  about,
  testSensors,
  testCode,
  initRobot
};

char current_menu_item;

void initTimer2() {
     // Setup timer2 -- Prescaler/256
  TCCR2A &= ~((1 << WGM21) | (1 << WGM20));
  TCCR2B &= ~(1 << WGM22);
  TCCR2B = (1 << CS22) | (1 << CS21);    //set prescaler to 1/256  

  ASSR |= (0 << AS2);    //sets timer to internal clock

  // Use normal mode  
  TCCR2A = 0;    
  //Timer2 Overflow Interrupt Enable  
  TIMSK2 |= (0 << OCIE2A);
  TCNT2 = 0x6;  // counting starts from 6;  
  
  //enable timer in overflow interrupt enable
  TIMSK2 = (1 << TOIE2);    

  SREG |= (1 << SREG_I);
}

//////////SETUP//////////SETUP//////////SETUP//////////SETUP//////////SETUP//////////SETUP//////////
void setup() {
  
    //startup for SSC_32
    Serial.begin(115200);

  // setup interrupt-driven keypad arrays  
  // reset button arrays
  for(byte i = 0; i < NUM_KEYS; i++) {
    button_count[i] = 0;
    button_status[i] = 0;
    button_flag[i] = 0;
  }

  initTimer2();

  lcd.LCD_init();
  lcd.LCD_clear();

  //menu initialization
  init_MENU(0, 0);
  current_menu_item = 0;	

  lcd.backlight(ON);    // Turn on the backlight
  //lcd.backlight(OFF); // Turn off the backlight  

  ultrasonic.initialize();
}

//////////LOOP//////////LOOP//////////LOOP//////////LOOP//////////LOOP//////////LOOP//////////LOOP//////////LOOP//////////LOOP//////////
void loop() {

  byte i;
  char page;
  for(i = 0; i < NUM_KEYS; i++){
    if(button_flag[i] != 0){
      button_flag[i] = 0;  // reset button flag
      
      switch(i) {
        case UP_KEY:
          // unhighlight the previous selection
          lcd.LCD_write_string(MENU_X, MENU_Y + current_menu_item % MAX_MENU_ITEMS, menu_items[current_menu_item], MENU_NORMAL);
          --current_menu_item;
          if(current_menu_item < 0)
            current_menu_item = NUM_MENU_ITEM - 1;
          if(current_menu_item == NUM_MENU_ITEM - 1 || current_menu_item % MAX_MENU_ITEMS == MAX_MENU_ITEMS - 1) {
            init_MENU(current_menu_item / MAX_MENU_ITEMS, current_menu_item);
          } else {
            // highlight the new selection
            lcd.LCD_write_string(MENU_X, MENU_Y + current_menu_item % MAX_MENU_ITEMS, menu_items[current_menu_item], MENU_HIGHLIGHT);
          }
          break;
        
        case DOWN_KEY:
          // unhighlight the previous selection
          lcd.LCD_write_string(MENU_X, MENU_Y + current_menu_item % MAX_MENU_ITEMS, menu_items[current_menu_item], MENU_NORMAL);
          ++current_menu_item;
          if(current_menu_item >(NUM_MENU_ITEM - 1))  current_menu_item = 0;
          if(current_menu_item == 0 || current_menu_item % MAX_MENU_ITEMS == 0) {
            init_MENU(current_menu_item / MAX_MENU_ITEMS, current_menu_item);
          }
          else {
            // highlight the new selection
            lcd.LCD_write_string(MENU_X, MENU_Y + current_menu_item % MAX_MENU_ITEMS, menu_items[current_menu_item], MENU_HIGHLIGHT);
          }
          break;
          
        case LEFT_KEY:
          page = current_menu_item / MAX_MENU_ITEMS - 1;
          if (page == -1) {
            page = NUM_MENU_ITEM / MAX_MENU_ITEMS;
          }
          current_menu_item = page * MAX_MENU_ITEMS;
          init_MENU(page, current_menu_item);
          break;   
          
        case RIGHT_KEY:
          page = current_menu_item / MAX_MENU_ITEMS + 1;
          if (page > NUM_MENU_ITEM / MAX_MENU_ITEMS) {
             page = 0; 
          }
          current_menu_item = page * MAX_MENU_ITEMS;
          init_MENU(page, current_menu_item);
          break;
          
        case CENTER_KEY:
          lcd.LCD_clear();
          (*menu_funcs[current_menu_item])();
          lcd.LCD_clear();
          init_MENU(0, 0);
          current_menu_item = 0;           
          break;	
      }
    }
  }
}

//////////Menu Functions//////////Menu Functions//////////Menu Functions//////////Menu Functions//////////

void init_MENU(byte page, char current) {
  byte start = page * MAX_MENU_ITEMS;
  byte i = start;
  String titlePage = "Page: ";
  titlePage += page + 1;
  titlePage += " of ";
  titlePage += NUM_MENU_ITEM / MAX_MENU_ITEMS + 1;
  char titleArray[titlePage.length() + 1];
  titlePage.toCharArray(titleArray, titlePage.length() + 1);
  
  lcd.LCD_clear();

  lcd.LCD_write_string(2, 0, titleArray, MENU_NORMAL);
  for (; i < min(NUM_MENU_ITEM, ((page + 1) * MAX_MENU_ITEMS)); i++){
    lcd.LCD_write_string(MENU_X, MENU_Y + i - start, menu_items[i], MENU_NORMAL);
  }

  lcd.LCD_write_string(MENU_X, MENU_Y + current % MAX_MENU_ITEMS, menu_items[current], MENU_HIGHLIGHT);
}

// waiting for center key press
void waitfor_OKkey() {
  byte i;
  byte key = 0xFF;
  while (key != CENTER_KEY) {
    for(i = 0; i < NUM_KEYS; i++) {
      if(button_flag[i] != 0) {
        button_flag[i] = 0;  // reset button flag
        if(i == CENTER_KEY) key = CENTER_KEY;
      }
    }
  }
}

int isEnterKeyPressed() {
  byte i;
  byte key = 0xFF;
    for(i = 0; i < NUM_KEYS; i++) {
      if(button_flag[i] != 0) {
        button_flag[i] = 0;  // reset button flag
        if(i == CENTER_KEY) return 1;
      }
    }
    return 0;
}

void temperature() {

  //accelerometer testing code inside here
//  initializeAccel();
  lcd.LCD_write_string_big(10, 1, "+12.30", MENU_NORMAL);
  lcd.LCD_write_string(78, 2, "C", MENU_NORMAL);
  lcd.LCD_write_string(38, 5, "OK", MENU_HIGHLIGHT );
//  accelDataCollect();
  waitfor_OKkey();
}

void charmap() {
  char i, j;
  int numTone = 200;
  for(i = 0; i < 5; i++){
    for(j = 0; j < 14; j++){
      delay(25);
      lcd.LCD_set_XY(j * 6, i);
      lcd.LCD_write_char(i * 14 + j + 32, MENU_NORMAL);
      tone(11, numTone);
      numTone += 5;
    }
  }
  lcd.LCD_write_string(38, 5, "OK", MENU_HIGHLIGHT );
  noTone(11);
  waitfor_OKkey();   
}

//void bitmap(){
//  lcd.LCD_draw_bmp_pixel(0,0, DFrobot_bmp, 84,24);
//  lcd.LCD_write_chinese(6,3, DFrobot_chinese,12,6,0,0);
//  lcd.LCD_write_string(38, 5, "OK", MENU_HIGHLIGHT );
//  waitfor_OKkey();
//}


void testUltraSonic() {
    
  while(!isEnterKeyPressed()) {
    //Gets raw value
    long microseconds = ultrasonic.readRawDistance();
  
    //converts to CM and Inches
    long distance_cm = ultrasonic.distance(microseconds, CM);
    long distance_inch = ultrasonic.distance(microseconds, INC);
  
    //Converts value to something that can get printed on screen
    char array[8];
    String str = String(distance_cm);
    str.toCharArray(array, 8);
  
    lcd.LCD_write_string( 3, 2, "Dis_CM:", MENU_NORMAL);
    lcd.LCD_write_string(32, 5, " OK ", MENU_HIGHLIGHT);
    lcd.LCD_write_string( 45, 2, "    ", MENU_NORMAL);
    lcd.LCD_write_string( 45, 2, array, MENU_NORMAL);
  
    char arrayInch[8];
    String strInch = String(distance_inch);
    strInch.toCharArray(arrayInch, 8);
  
    lcd.LCD_write_string( 3, 3, "Dis_IN:", MENU_NORMAL);
    lcd.LCD_write_string( 45, 3, "    ", MENU_NORMAL);
    lcd.LCD_write_string( 45, 3, arrayInch, MENU_NORMAL);
  }
}

void testMusic() {
  int noteDuration;
    // iterate over the notes of the melody:
    // disable timer2 for full speed playback
    TIMSK2 &= ~(1 << TOIE2);    

  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second 
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    noteDuration = 1000 / noteDurations[thisNote];
    tone(11, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
  }
  noTone(11);
    // re-enable timer2 for buttons
    TIMSK2 |= (1 << TOIE2);    
}

void about() {
  lcd.LCD_write_string( 0, 1, "LCD4884 Shield", MENU_NORMAL);
  lcd.LCD_write_string( 0, 3, "www.sainsmart.com", MENU_NORMAL);
  lcd.LCD_write_string(38, 5, "OK", MENU_HIGHLIGHT);
  testMusic();
  waitfor_OKkey();
}

void testSensors() {  
  for(byte i = 0; i < NUM_SENSORS; i++) {
    char name[8];
    String str = "A" + String(i) + ":";
    str.toCharArray(name, 8);
    lcd.LCD_write_string(0, i, name, MENU_NORMAL);
  }
  lcd.LCD_write_string(54, 5, " OK ", MENU_HIGHLIGHT );
  
  while(!isEnterKeyPressed()) { 
    for(int i = 0; i < NUM_SENSORS; i++) {  
      lcd.LCD_write_string(20, i, "    ", MENU_NORMAL);
      int sensor = analogRead(i);
      char array[8];
      String str = String(sensor);
      str.toCharArray(array, 8);
      lcd.LCD_write_string(20, i, array, MENU_NORMAL);
    }
    delay(50);
  }
}

void testCode() {
  while(!isEnterKeyPressed()) {
    long microseconds = ultrasonic.readRawDistance();
    tone(11, microseconds, 100);
  }
  noTone(11);
}

void initRobot() {
  for(byte i = 0; i < NUM_SERVOS; i++) {
    moveServo(i, 1500, 500);
  }
}

/*
//may not need anymore
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
*/



// The followinging are interrupt-driven keypad reading functions
// which includes DEBOUNCE ON/OFF mechanism, and continuous pressing detection

// Convert ADC value to key number
char get_key(unsigned int input) {
  char k;

  for (k = 0; k < NUM_KEYS; k++) {
    if (input < adc_key_val[k]) {
      return k;
    }
  }

  if (k >= NUM_KEYS)
    k = -1;     // No valid key pressed

  return k;
}

void update_adc_key() {
  int adc_key_in;
  char key_in;
  byte i;

  adc_key_in = analogRead(0);
  key_in = get_key(adc_key_in);
  for(i = 0; i < NUM_KEYS; i++) {
    if(key_in == i) {  //one key is pressed 
      if(button_count[i] < DEBOUNCE_MAX) {
        button_count[i]++;
        if(button_count[i] > DEBOUNCE_ON) {
          if(button_status[i] == 0) {
            button_flag[i] = 1;
            button_status[i] = 1; //button debounced to 'pressed' status
          }
        }
      }
    }
    else { // no button pressed
      if (button_count[i] > 0) {  
        button_flag[i] = 0;	
        button_count[i]--;
        if(button_count[i] < DEBOUNCE_OFF) {
          button_status[i] = 0;   //button debounced to 'released' status
        }
      }
    }
  }
}

// Timer2 interrupt routine -
// 1/(160000000/256/(256-6)) = 4ms interval

ISR(TIMER2_OVF_vect) {  
  TCNT2 = 0x6;
  update_adc_key();
}

