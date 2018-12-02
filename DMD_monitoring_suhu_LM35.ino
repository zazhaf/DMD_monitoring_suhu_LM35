// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#include <SPI.h>        //SPI.h must be included as DMD is written by SPI (the IDE complains otherwise)
#include <DMD.h>        //
#include <TimerOne.h>   //
#include "SystemFont5x7.h"
#include "Arial_black_16.h"
#include <EEPROM.h>
//Fire up the DMD library as dmd
#define DISPLAYS_ACROSS 1
#define DISPLAYS_DOWN 1

DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

uint16_t setpoint;
uint8_t modesetup = 0;
uint8_t i = 0;
String DataIn1;
char DataInArray[255];

/*--------------------------------------------------------------------------------------
  Interrupt handler for Timer1 (TimerOne) driven DMD refresh scanning, this gets
  called at the period set in Timer1.initialize();
--------------------------------------------------------------------------------------*/
void tombol(){
  if(digitalRead(A0) == 0){
    digitalWrite(2,HIGH);
    delay(20);
    digitalWrite(2,LOW);
    if(modesetup==0){
      while(digitalRead(A0) == 0){
        modesetup = 1;
      }
    }
    else{
      while(digitalRead(A0) == 0){
        modesetup = 0;
      }
    }
    i = 200;
  }
  
  if (modesetup == 1){
    if(digitalRead(A1) == 0){
      digitalWrite(2,HIGH);
      delay(20);
      digitalWrite(2,LOW);
      setpoint++;
      if(setpoint>50){setpoint = 10;}
      EEPROM.write(0,setpoint);
      while(digitalRead(A1) == 0){}
    }
  }
  
}

void ScanDMD()
{ 
  dmd.scanDisplayBySPI();
  //tombol();
}

void setup() {
  //EEPROM.write(0,30);
  analogReference(INTERNAL);
  setpoint = EEPROM.read(0);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT); 
  
  pinMode(2, OUTPUT);
  digitalWrite(2,HIGH);
  delay(100);
  digitalWrite(2,LOW);
  delay(100);
  digitalWrite(2,HIGH);
  delay(100);
  digitalWrite(2,LOW);
  delay(100);
  Serial.begin(9600);

     //initialize TimerOne's interrupt/CPU usage used to scan and refresh the display
   Timer1.initialize( 2500 );           //period in microseconds to call ScanDMD. Anything longer than 5000 (5ms) and you can see flicker.
   Timer1.attachInterrupt( ScanDMD );   //attach the Timer1 interrupt to ScanDMD which goes to dmd.scanDisplayBySPI()

   //clear/init the DMD pixels held in RAM
   dmd.clearScreen( true );   //true is normal (all pixels off), false is negative (all pixels on)
}

void loop() {
  if (modesetup == 0){
    tombol();

    int dataAnalog = analogRead(A7);
    double t = dataAnalog / 9.31;
    Serial.println(dataAnalog);
     
    // 10 x 14 font clock, including demo of OR and NOR modes for pixels so that the flashing colon can be overlayed
    dmd.clearScreen( true );
    dmd.selectFont(Arial_Black_16); 
    DataIn1 = (int) t;
    DataIn1 += "C";
    memset(DataInArray, 0, sizeof(DataInArray));
    DataIn1.toCharArray(DataInArray,255);
    dmd.drawString(  3,  1, DataInArray, DataIn1.length(), GRAPHICS_NORMAL );
    if((int)t > setpoint){
      digitalWrite(2,HIGH);
      dmd.clearScreen( true );
      dmd.drawMarquee("Warning",7,(32*DISPLAYS_ACROSS)-1,0);
         long start=millis();
         long timer=start;
         boolean ret=false;
         while(!ret){
           if(digitalRead(A0)==0){i=200;}
           if(digitalRead(A1)==0){i=200;}
           tombol();
           if ((timer+20) < millis()) {
             ret=dmd.stepMarquee(-1,0);
             timer=millis();
           }
         }
    }
    //digitalWrite(2,HIGH);
    //delay(20);
    //digitalWrite(2,LOW);
      dmd.drawString(  3,  1, DataInArray, DataIn1.length(), GRAPHICS_NORMAL );
      digitalWrite(2,LOW);      
    for(i=0;i<100;i++){
      if(digitalRead(A0)==0){i=200;}
      if(digitalRead(A1)==0){i=200;}
      tombol();
      delay(10);
    }

  }
  else{
    dmd.clearScreen( true );
    dmd.selectFont(SystemFont5x7); 
    DataIn1 = "< ";
    DataIn1 += setpoint;
    memset(DataInArray, 0, sizeof(DataInArray));
    DataIn1.toCharArray(DataInArray,255);
    dmd.drawString(  3,  5, DataInArray, DataIn1.length(), GRAPHICS_NORMAL );
    for(i=0;i<200;i++){
      if(digitalRead(A0)==0){i=200;}
      if(digitalRead(A1)==0){i=200;}
      tombol();
      delay(10);
    }  
  }
}
