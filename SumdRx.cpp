
//Library for Sumd Decoding written with the help of:
//https://github.com/RC-Navy/DigisparkArduinoIntegration/tree/master/libraries/RcBusRx
//which unfortunately cause exception on esp8266

#include <ESP8266WiFi.h>
#include "SumdRx.h"

SumdRx::SumdRx() {
  int index;
  
  //init buffer pointer
  reset();
  failsafe=true;
  channelRx=0;
  channel[0]=HOTT_MIN;
  for (index=1;index < MAX_CHANNEL_NUMBER;index ++) channel[index]=HOTT_CENTER;
  lastFrameTime=millis();
}

//reseet decoding
void SumdRx::reset() {
  //init buffer
  bufferStart=0;
  bufferStop=0;
  headerFound=false;
}

//refresh status attributes of the GoPro
void SumdRx::add(uint8_t rxByte) {

  uint8_t SUMD_header[3];
  uint16_t crc;
  int index;

  //add byte to the ring buffer
  ringBuffer[bufferStop]=rxByte;
  bufferStop=(bufferStop+1) % RING_BUFFER_SIZE; 
  
  //in case of buffer overflow, forgot oldest byte received
  if (bufferStart==bufferStop) bufferStart=(bufferStart+1) % RING_BUFFER_SIZE;
  
  //as long as header not found and nb of byte avail > header size

  while (!headerFound && bufferLength()>=HEADER_SIZE) {
    
    //check the header is validity
     SUMD_header[0]=ringBuffer[bufferStart];
     SUMD_header[1]=ringBuffer[(bufferStart+1) % RING_BUFFER_SIZE];
     SUMD_header[2]=ringBuffer[(bufferStart+2) % RING_BUFFER_SIZE];

     headerFound=(SUMD_header[0]==0xA8 && (SUMD_header[1]==0x01 ||  SUMD_header[1]==0x81) && (0x02<=  SUMD_header[2])  && (SUMD_header[2]<=MAX_CHANNEL_NUMBER));
    
    //if the header is valid
    if (headerFound) {
      //get failsafe flag and channel number
      failsafe=(SUMD_header[1]==0x81);
      channelNb=SUMD_header[2];
      //Serial.print("Header Found ");
    }
    //else forgot oldest byte of the buffer
    else {
      bufferStart=(bufferStart+1) % RING_BUFFER_SIZE;
      //Serial.print("Syn Err ");
    }
  }
  
  //if header is valid buffer content length >= frame length
  if (headerFound && (bufferLength() >=(HEADER_SIZE+ 2*channelNb+CRC_SIZE))) {
    //compute CRC with header and data
    crc=0;
    for (index=0;index < (HEADER_SIZE+2*channelNb+CRC_SIZE);index++) crc=crc16_CCITT(crc,ringBuffer[(bufferStart+index) % RING_BUFFER_SIZE]);
    //if frame is valid
    if (crc==0) {
      //update channel output values
      for (index=0;index < channelNb;index ++) channel[index]=(ringBuffer[(bufferStart+HEADER_SIZE+2*index) % RING_BUFFER_SIZE] << 5)+(ringBuffer[(bufferStart+HEADER_SIZE+2*index+1) % RING_BUFFER_SIZE] >>3);
      channelRx=channelNb;
      lastFrameTime=millis();
      
      //forgot decoded bytes from the ring buffer
       bufferStart=(bufferStart+HEADER_SIZE+ 2*channelNb+CRC_SIZE) % RING_BUFFER_SIZE;
       //Serial.println("CRC OK");
      //reinit header search
      headerFound=false;
    }
    //else forgot first byte of the frame, reset header found information 
    else {
      bufferStart=(bufferStart+1) % RING_BUFFER_SIZE;
      headerFound=false;
      //Serial.println("CRC Error");
    }
  }

}

//function used to check the failsafe status
bool SumdRx::failSafe() {
  bool timeout;
  timeout=((millis()-lastFrameTime) > SUMD_MAX_FRAME_PERIOD);
  if (timeout) {
    //Serial.println("Timeout Error");
  }
  return (failsafe || timeout);
}

//function used to process frame crc
uint16_t SumdRx::crc16_CCITT(uint16_t crc, uint8_t value)
{
  uint8_t i;

  crc = crc ^ (int16_t)value << 8;

  for (i = 0; i < 8; i++)
  {
    if (crc & 0x8000)
    {
      crc = crc << 1 ^ 0x1021;
    } else
    {
      crc = crc << 1;
    }
  }
  return crc;
}

//function used to calculate length of data available in the buffer
int SumdRx::bufferLength() {
  int temp;
  temp=(bufferStop-bufferStart) % RING_BUFFER_SIZE;
  if (temp < 0) temp+=RING_BUFFER_SIZE;
  return(temp);
}
