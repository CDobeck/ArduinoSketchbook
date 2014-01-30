
#include "UltraSonic.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#include "WConstants.h"
#endif


extern "C" 
{
#include <avr/pgmspace.h>
#include <avr/io.h>
}



UltraSonic::UltraSonic()
{};

UltraSonic ultrasonic = UltraSonic();

//Setup for the ultrasonic sensor
void UltraSonic::initialize(void)
{
  pinMode(TP,OUTPUT);       // set TP output for trigger  
  pinMode(EP,INPUT); 
}  

void UltraSonic::initialize(int triggerPin, int echoPin)
{
  pinMode(triggerPin, OUTPUT);       // set TP output for trigger  
  pinMode(echoPin, INPUT); 
}  

//get the distance date from HC-SRO4 and cover to a value 
long UltraSonic::readRawDistance(void)
{                     
  digitalWrite(TP, LOW);                    
  delayMicroseconds(2);
  digitalWrite(TP, HIGH);                 // pull the Trig pin to high level for more than 10us impulse 
  delayMicroseconds(10);
  digitalWrite(TP, LOW);
  long microseconds = pulseIn(EP,HIGH);   // waits for the pin to go HIGH, and returns the length of the pulse in microseconds
  return microseconds;                    // return microseconds
}

//calculate the distance 
long UltraSonic::distance(long time, int flag)
{
  long distance;
  if(flag)
    distance = time /29 / 2  ;     // Distance_CM  = ((Duration of high level)*(Sonic :340m/s))/2
                                   //              = ((Duration of high level)*(Sonic :0.034 cm/us))/2
                                   //              = ((Duration of high level)/(Sonic :29.4 cm/us))/2
  else
    distance = time / 74 / 2;      // INC
  return distance;
}


long UltraSonic::readDistanceCM(void)
{                     
  digitalWrite(TP, LOW);                    
  delayMicroseconds(2);
  digitalWrite(TP, HIGH);                 // pull the Trig pin to high level for more than 10us impulse 
  delayMicroseconds(10);
  digitalWrite(TP, LOW);
  long microseconds = pulseIn(EP,HIGH);   // waits for the pin to go HIGH, and returns the length of the pulse in microseconds
  return (microseconds /29 / 2);                    // return microseconds
}

long UltraSonic::readDistanceInch(void)
{                     
  digitalWrite(TP, LOW);                    
  delayMicroseconds(2);
  digitalWrite(TP, HIGH);                 // pull the Trig pin to high level for more than 10us impulse 
  delayMicroseconds(10);
  digitalWrite(TP, LOW);
  long microseconds = pulseIn(EP,HIGH);   // waits for the pin to go HIGH, and returns the length of the pulse in microseconds
  return (microseconds / 74 / 2);                    // return microseconds
}