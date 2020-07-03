#ifndef SUMDRX_H
#define SUMDRX_H

//SUMD protocol constant
#define MAX_CHANNEL_NUMBER 16 //shall be equal or smaller than 32
#define HEADER_SIZE 3
#define CRC_SIZE 2
#define SUMD_MAX_FRAME_PERIOD 750  //above this delay, switch to failsafe

//SUMD decoding buffer declaration
#define RING_BUFFER_SIZE (HEADER_SIZE+MAX_CHANNEL_NUMBER*2+CRC_SIZE+1) //buffer size : 1 extra byte for easy empty/full detection using start and stop


class SumdRx {
  private:

    //ring buffer
    uint8_t ringBuffer[RING_BUFFER_SIZE]; //buffer size : 2 Bytes per channel + synchro, failsafe, channel nb, chksum(2) and 1 extra byte for empty/full detection 
    int bufferStart;
    int bufferStop;
    int bufferLength(); //function used to calculate length of data available in the buffer
    
    //decoding status
    bool headerFound;
    int channelNb; //number of channel available in the frame
    bool failsafe;
    unsigned long lastFrameTime;


    uint16_t crc16_CCITT(uint16_t crc, uint8_t value); //calculate frame crc

  public:
    //HOTT Ctrl typical value
    static const int HOTT_MIN=1100;
    static const int HOTT_M50=1300;
    static const int HOTT_CENTER=1500;
    static const int HOTT_P50=1700;
    static const int HOTT_MAX=1900;
    static const int HOTT_DEAD_ZONE=20;
    
    int channelRx;
    int channel[MAX_CHANNEL_NUMBER];
    SumdRx();
    void reset();
    void add(byte);  //add one by at the end of the buffer
    bool failSafe(); //check the fail safe status
};

#endif
