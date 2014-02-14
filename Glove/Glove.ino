#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include "CircularBuffer.h"
const int LED = 13;
long count=0;
CircularBuffer cb;

struct ElemType cap;
#define BUFFER_SIZE 500
//CircularBuffer<cap,BUFFERSIZE> capture;

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
  int testBufferSize = 10; /* arbitrary size */
  cbInit(&cb, BUFFER_SIZE);

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
  accel.setDataRate(ADXL345_DATARATE_800_HZ);
  //  ADXL345_DATARATE_3200_HZ    = 0b1111, // 1600Hz Bandwidth   140µA IDD
  //  ADXL345_DATARATE_1600_HZ    = 0b1110, //  800Hz Bandwidth    90µA IDD
  //  ADXL345_DATARATE_800_HZ     = 0b1101, //  400Hz Bandwidth   140µA IDD
  //  ADXL345_DATARATE_400_HZ     = 0b1100, //  200Hz Bandwidth   140µA IDD
  //  ADXL345_DATARATE_200_HZ     = 0b1011, //  100Hz Bandwidth   140µA IDD
  //  ADXL345_DATARATE_100_HZ     = 0b1010, //   50Hz Bandwidth   140µA IDD
  //  ADXL345_DATARATE_50_HZ      = 0b1001, //   25Hz Bandwidth    90µA IDD
  //  ADXL345_DATARATE_25_HZ      = 0b1000, // 12.5Hz Bandwidth    60µA IDD
  //  ADXL345_DATARATE_12_5_HZ    = 0b0111, // 6.25Hz Bandwidth    50µA IDD
  //  ADXL345_DATARATE_6_25HZ     = 0b0110, // 3.13Hz Bandwidth    45µA IDD
  //  ADXL345_DATARATE_3_13_HZ    = 0b0101, // 1.56Hz Bandwidth    40µA IDD
  //  ADXL345_DATARATE_1_56_HZ    = 0b0100, // 0.78Hz Bandwidth    34µA IDD
  //  ADXL345_DATARATE_0_78_HZ    = 0b0011, // 0.39Hz Bandwidth    23µA IDD
  //  ADXL345_DATARATE_0_39_HZ    = 0b0010, // 0.20Hz Bandwidth    23µA IDD
  //  ADXL345_DATARATE_0_20_HZ    = 0b0001, // 0.10Hz Bandwidth    23µA IDD  accel.setDataRate(ADXL345_DATARATE_100_HZ);

  //  ADXL345_DATARATE_0_10_HZ    = 0b0000  // 0.05Hz Bandwidth    23µA IDD (default value)
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
    sensors_event_t event; 
    struct ElemType capt;
    bool trigger=false;
    int sample=0;
    while((millis()-start)<10000){
      accel.getEvent(&event);
      capt.index=index++;
      capt.mills=millis();
      capt.event = event;
      cbWrite(&cb,&capt);
      if(trigger){
        if(sample++ > BUFFER_SIZE/4)
          break;
      } 
      else  if((abs(event.acceleration.x) + abs(event.acceleration.y) + abs(event.acceleration.z)) > 40.0) {
        trigger=true;
        trigger_time=millis();
      }

    }

    digitalWrite(LED, LOW);  // if 1, switch LED Off
    struct ElemType cb_start;
    if(trigger){
      cbRead(&cb,&cb_start);
      unsigned long end = cb_start.mills;

      Serial1.println("Start");
      while (!cbIsEmpty(&cb)) {
        cbRead(&cb, &capt);    
        char s[30];
        sprintf(s,"%d,%2.1f,%2.1f,%2.1f\n",(capt.mills-trigger_time),capt.event.acceleration.x,capt.event.acceleration.y,capt.event.acceleration.z);
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







