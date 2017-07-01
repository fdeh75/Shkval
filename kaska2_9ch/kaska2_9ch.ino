#include <SPI.h>
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>
#include <nRF24L01.h>
//#include "ir.h"
#include <IRremote.h> 
#include <IRremoteInt.h>                    
#define fail 160
#define rosed 255
#define muNumber 0xa5

uint16_t max_blink = 4500;
void send_death(int16_t dat);
void send_rose(int16_t dat);
uint64_t chanel_ar = 222222222 + muNumber;
uint64_t chanel_pr = 111111111 + muNumber;      
unsigned long timer_last;
uint8_t massiv[3];
int RECV_PIN = 3;
int RELAY_PIN = 5;



//IRsend irsend;
RF24 radio(9,10);
IRrecv irrecv(RECV_PIN);
decode_results results;


void setup() {
  pinMode(2, INPUT);
  pinMode(14, INPUT);
  pinMode(7, INPUT);
  pinMode(6, INPUT);
  pinMode(8, INPUT);
  pinMode(3, INPUT);
  pinMode(15, INPUT);
  pinMode(16, INPUT);
  pinMode(17, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  pinMode(4, OUTPUT);

  Serial.begin(9600);
  //printf_begin();
  radio.begin();
  radio.openWritingPipe(chanel_pr);
  radio.openReadingPipe(1,chanel_ar);
  radio.startListening();
  //radio.printDetails();
  irrecv.enableIRIn(); // Start the receiver
  Serial.print("hi");
}

void loop() {
  
  //Serial.println("loop");
  uint16_t data = rosed;
  uint16_t data1 = 0;
  uint8_t mN = muNumber;
  bool trig = true;
  radio.startListening();
    if( radio.available()){
      Serial.println("radio av");      
      //while (radio.available()) {                                   // While there is data ready
        radio.read( &data, sizeof(data) );             // Get the payload
        if( radio.available()){
        radio.read(&data1,sizeof(data1));     
        }

       }
       
             if ((data == 85)&&(data1>1000))
       {
        max_blink = data1;


        for(int i = 0; i< 20; i++){        
        pinMode(RELAY_PIN, INPUT);
        digitalWrite(RELAY_PIN, LOW);
        delay(20);
        pinMode(RELAY_PIN, OUTPUT);
        digitalWrite(RELAY_PIN, LOW);
        delay(50);        
      }
      radio.stopListening();
      
      for(int n = 0; n< 10; n++){ 
      radio.write( &data, sizeof(data) );              // Send the final one back.      
      delay(20);
      }
      radio.startListening();                        
      Serial.println(data);
    }    
      if(irrecv.decode(&results)) {
      Serial.println("irresiv");
      if (results.decode_type == NEC) {
      if(!(results.value == muNumber)){
      data = fail;                                     // First, stop listening so we can talk   
                        
      Serial.println(data);
      }
      }
       
        irrecv.resume();
        }
      
    
    if (data == fail){
      radio.stopListening();
      timer_last = millis();
      for(int i = 0; i< 20; i++){        
        pinMode(RELAY_PIN, OUTPUT);
        digitalWrite(RELAY_PIN, LOW);
        delay(100);        
        pinMode(RELAY_PIN, INPUT);
        digitalWrite(RELAY_PIN, LOW);
        delay(100);
        radio.write( &data, sizeof(data) );
      }
      radio.startListening();
     while(1){
    
      if (millis()-timer_last>999 && trig){
        
          timer_last= millis();
          trig=!trig;
          pinMode(RELAY_PIN, INPUT);
          digitalWrite(RELAY_PIN, LOW);
          digitalWrite(4, HIGH);
          }
        if (millis()-timer_last>5 && !trig){
            radio.stopListening();
           
          timer_last= millis();
          trig=!trig;       
          pinMode(RELAY_PIN, OUTPUT);
          
          digitalWrite(RELAY_PIN, LOW);
          digitalWrite(4, LOW);
           radio.write( &data, sizeof(data) );
            radio.startListening();
          }
      if( radio.available()){
                                                                    // Variable for the received timestamp
      while (radio.available()) {                                   // While there is data ready
        radio.read( &data, sizeof(data) );             // Get the payload

      }
      if(data== rosed){
        
       radio.stopListening();                                        // First, stop listening so we can talk   
      //for(int i; i< 10; i++){
      //radio.write(&mN, sizeof(mN));
      radio.write( &data, sizeof(data) );              // Send the final one back.      
      delay(20);
      //}              // Send the final one back.      
       radio.openReadingPipe(1,chanel_ar);
       radio.startListening();                        
      Serial.println(data);
      break;
      }
      }
      if( irrecv.decode(&results)){
      if (results.value==0xE0E040BF) {
      radio.stopListening();
      data = rosed;                                     // First, stop listening so we can talk   
      //for(int i; i< 10; i++){
          radio.write( &data, sizeof(data) );              // Send the final one back.      
          delay(20);
      //}              // Send the final one back.      
     radio.openReadingPipe(1,chanel_ar);
      radio.startListening();                        
      Serial.println(data);
      irrecv.resume();
      break;
      
     }
      irrecv.resume();
        }

      
    
       }
      }
    }
