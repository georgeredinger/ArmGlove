#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include "CircularBuffer.h"
const int LED = 13;
long count=0;

  struct {
  unsigned index;
  unsigned long mills;
  sensors_event_t event;
} 
cap;
#define BUFFERSIZE 250
CircularBuffer<cap,BUFFERSIZE> capture;

// This example code is in the public domain.


Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

boolean fast=false;


void setup()
{
  Serial1.begin(9600);
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  Serial1.println("LED OFF. Press 1 to LED ON!");  // print message
  Serial1.println("LED ON. Press 0 to LED OFF!");

  char s[20];
  Serial.println("before sprintf");
  sprintf(s,"%f\n","123.456");
  Serial.print(s);
  Serial.println("after sprintf");
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while(1);
  }

  /* Set the range to whatever is appropriate for your project */
  accel.setRange(ADXL345_RANGE_16_G);
  // displaySetRange(ADXL345_RANGE_8_G);
  // displaySetRange(ADXL345_RANGE_4_G);
  // displaySetRange(ADXL345_RANGE_2_G);

}


void loop() {
  char incomingByte;
  if (Serial1.available() > 0) {  // if the data came
    incomingByte = Serial1.read(); // read byte
    if(incomingByte == '0') {
      digitalWrite(LED, LOW);  // if 1, switch LED Off
      Serial1.println("LED OFF. Press 1 to LED ON!");  // print message
      Serial.println("LED OFF. Press 1 to LED ON!");  // print message
      fast=false;

    }
    if(incomingByte == '1') {
      digitalWrite(LED, HIGH); // if 0, switch LED on
      Serial1.println("LED ON. Press 0 to LED OFF!");
      Serial.println("LED ON. Press 0 to LED OFF!");
      fast=true;
    }
  }

  if(fast){
    fast=false;
    unsigned long trigger_time;
    unsigned long start=millis();
    unsigned index=0;
    capture.clear();
    sensors_event_t event; 
    cap capt;
    bool trigger=false;
    int sample=0;
    while((millis()-start)<10000){
      accel.getEvent(&event);
      capt.index=index++;
      capt.mills=millis();
      capt.event = event;
      capture.push(capt);
       if(trigger){
         if(sample++ > BUFFERSIZE/4)
          break;
      } else  if((abs(event.acceleration.x) + abs(event.acceleration.y) + abs(event.acceleration.z)) > 40.0) {
        trigger=true;
        trigger_time=millis();
      }
     
      delay(3);
    }

    digitalWrite(LED, LOW);  // if 1, switch LED Off
    if(trigger){
       unsigned long end = capture.pop().mills;

      Serial1.println("Start");
      while(capture.remain()> 0){
        capt=capture.pop();
        char s[30];
        sprintf(s,"%d,%2.1f,%2.1f,%2.1f\n",(capt.index),capt.event.acceleration.x,capt.event.acceleration.y,capt.event.acceleration.z);
        Serial.print(s);
        Serial1.print(s);
      }
      Serial1.println("End");
    }
    else{
      Serial1.println("no trigger");
    }
  }
}





