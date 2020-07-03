
#include <ESP8266WiFi.h>
#include "SumdRx.h"

//remote control status
bool failSafe=true;

SumdRx *sumdDecoder;

void setup() {
  //Start communication to serial port
  Serial.begin(115200);
  delay(100);

  //Attach SUMD decoder to the serial port
  sumdDecoder= new SumdRx(); 
}

void loop()
{  
  uint8_t index;

  //process sumd input
  //due to lead time from previous decoding, buffer may overflow and being corrupted -> reset
  sumdDecoder->reset();
  while(Serial.available() > 0) sumdDecoder->add(Serial.read());

  //display failsafe status on console
  if (failSafe && !sumdDecoder->failSafe()) {
    failSafe=false;
    Serial.println("Reception OK");
  }
  else if (!failSafe && sumdDecoder->failSafe()) {
    failSafe=true;
    Serial.println("Reception Lost");
  }

  //display channel status
  if (!sumdDecoder->failSafe()) {
    for(index=0;index<sumdDecoder->channelRx;index++) {
      Serial.print(sumdDecoder->channel[index]);
      Serial.print(" ");
    }
    Serial.println();
  }
  delay(100);  
}
 
