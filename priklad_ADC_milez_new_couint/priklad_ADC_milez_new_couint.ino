#include <IRremote.h>
#include <IRremoteInt.h>
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>

#define shot_long 1000                          //время режима "выстрел"
#define muNumber 0xa9                    //номер комплекта
#define fail 160                                 //служебное значение - "Поражен"
#define rosed 255                                 //служебное значение - "Готов к бою"
#define ZUMM 4 
#define vibkik 7
uint16_t max_blink;
unsigned long timer_last;
uint64_t chanel_pr = 111111111 + muNumber;
//uint64_t chanel_pr_servise = 111222111+ muNumber;
bool trig = true;
bool trig1 = false;
bool trig2 = false;
uint16_t maxTrig = 0;
uint16_t minTrig = 0;
uint16_t temp;
IRsend irsend;
RF24 radio(9,10);

void setup() {
   uint16_t dat[1023];
   uint16_t i = 0;
   //radio.powerDown();
   //delay(10000);
   //radio.powerUp();
   pinMode(ZUMM ,OUTPUT); // зуммер на пине 2
   pinMode(vibkik ,INPUT);
   radio.begin();                             //включение радиомодуля
   radio.setPALevel(RF24_PA_LOW);
   radio.openReadingPipe(1,chanel_pr);     
   radio.startListening();
   Serial.begin(9600);
   Serial.println("hi");
   i= muNumber;
   Serial.print("priklad # ");
   Serial.println(int(i - 0xa0),DEC);
   
    delay(1000);
   minTrig = analogRead(5);
  // radio.stopListening();
  // radio.openWritePipe(chanel_pr_servise);
   while (1){          
         if (millis()-timer_last>999 && trig){
          //radio.write(&minTrig, sizeof(minTrig));
          timer_last= millis();
          trig=!trig;
          digitalWrite(ZUMM, HIGH);
          Serial.println(analogRead(5));
          }
        if (millis()-timer_last>50 && !trig){
          timer_last= millis();
          trig=!trig;       
          digitalWrite(ZUMM, LOW);          
          }
    maxTrig = analogRead(5);
    if(500 < (maxTrig )){
      digitalWrite(ZUMM,HIGH);
      Serial.println("dat[i]");
     // irsend.sendNEC(muNumber,32);
     // irsend.sendNEC(muNumber,32);
     // irsend.sendNEC(muNumber,32);
      irsend.sendNEC(0x2f,32);  
      delay(500);
      digitalWrite(ZUMM, LOW);
     // maxTrig = analogRead(5);
      Serial.println("!!!");
      for(i = 0; i< 10; i++){
        delay(100);
        digitalWrite(ZUMM, HIGH);
        delay(10);
        digitalWrite(ZUMM,LOW);
      }
      break;
    }
   }
   for(i = 0; i < 5; i++)
   {
          delay(100);
          digitalWrite(ZUMM, HIGH);  
          delay(10);
          digitalWrite(ZUMM,LOW);        
          
   }
   //minTrig = maxTrig+(minTrig-maxTrig)*0.75 ;
   trig = true;
   Serial.print("min_ADC  ");
   Serial.println(maxTrig);
   //radio.startListening();
}

void loop() {
  static uint8_t data = rosed;   
  temp = analogRead(5);               
    if((temp >  minTrig+ 75)&&(!trig)){//.. на активное
       trig=true;
       delay(50);
       irsend.sendNEC(0xe2,14);  
       Serial.println("ff");
      timer_last= millis();
    }
    if((trig)&&(millis()-timer_last>90)){
      trig = 0;      
    }
 
  if( radio.available()){
      radio.read( &data, sizeof(data) );             // Get the payload
      Serial.println("radio av"); // Variable for the received timestamp        
      Serial.println(data);
  }   
  if (data == 77)
  {

    if(trig1){
    trig1 = false;
    radio.stopListening();
    Serial.println("faer");
    radio.openWritingPipe(chanel_pr);
    for(int i = 0; i< 10; i++){
    radio.write(&data,sizeof(data));
    }
    radio.startListening();
    }
   }

     if(data == fail){
     Serial.println("death");
      while(1){  
        if( radio.available()){
        radio.read( &data, sizeof(data) );             // Get the payload
        }        
      if(data == rosed){       
        Serial.println("rosed");
        break;
      }                              
        // Serial.println(data);
    }
}
}

