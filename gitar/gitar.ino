
#include "pitches.h"
int cnyOku = 2;
int led = 4;
int potOku = A0;
int potDeger = 0;

void setup(){  
Serial.begin(9600);
pinMode(cnyOku, INPUT);
pinMode(led, OUTPUT);
digitalWrite(led, LOW);

}

void loop(){
int deger = digitalRead(cnyOku);
potDeger = analogRead(potOku);
potDeger = (potDeger*4.8)+30;
if(deger == HIGH){
  digitalWrite(led, HIGH);
  tone(8,potDeger);  
  //tone(pin, frequency) 
//tone(pin, frequency, duration)
}
else{
  digitalWrite(led, LOW);
  noTone(8);
}



Serial.println(potDeger);
delay(200);
}
