#ifndef ACKMODEL_H
#define ACKMODEL_H

#include "format.h"

class ACKModel {
  private:
    unsigned char advWindowSize;
    char checkSum;
    unsigned int nextSeqNum;
    bool error;

  public:
    ACKModel(unsigned int nextSeqNum, unsigned char winSize) {
        this->nextSeqNum = nextSeqNum;
        advWindowSize = winSize;
        error = false;
    }

    ACKModel(char* frame) {
        nextSeqNum = (frame[1] << 24) + (frame[2] << 16) + (frame[3] << 8) + (frame[4]);
        advWindowSize = frame[5];
        checkSum = frame[6];
        //error checking
        char temp = frame[0];
        for (int i = 1; i < 6; i++)
            temp += frame[i];
        error = (checkSum != temp);
    }

    char* setFrameFormat() {
        char* frame = new char[1 + 4 + 1 + 1];
        frame[0] = SOH;
        frame[1] = (nextSeqNum >> 24) & 0xFF;
        frame[2] = (nextSeqNum >> 16) & 0xFF;
        frame[3] = (nextSeqNum >> 8) & 0xFF;
        frame[4] = nextSeqNum & 0xFF;
        frame[5] = advWindowSize;
        frame[6] = checkSum;
        
        return frame;
    }

    void setAdvWindowSize(unsigned char winSize) {
        advWindowSize = winSize;
    }
    void setCheckSum(char sum) {
      checkSum = sum;
    }
    void setNextSeqNum(unsigned int nextNum) {
      nextSeqNum = nextNum;
    }
    unsigned char getAdvWindowSize() {
      return advWindowSize;
    }
    char getCheckSum() {
      return checkSum;
    }
    unsigned int getNextSeqNum() {
      return nextSeqNum;
    }
};

#endif