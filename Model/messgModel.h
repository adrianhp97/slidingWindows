#ifndef MESSGMODEL_H
#define MESSGMODEL_H

#include "format.h"
#include <iostream>
#include <stdio.h>

using namespace std;

class messgModel {
  private:
    unsigned char data;
    unsigned char checkSum;
    unsigned int sequenceNum;
    bool error;

  public:
    messgModel() {
    }

    messgModel(unsigned int seqNum) {
        sequenceNum = seqNum;
        error = false;
    };

    messgModel(unsigned char* frame) {
        sequenceNum = (frame[1] << 24) + (frame[2] << 16) + (frame[3] << 8) + (frame[4]);
        data = frame[6];
        checkSum = frame[8];
        //error checking
        unsigned char temp = frame[0] + sequenceNum + frame[5] + frame[6] + frame[7];
        error = (checkSum != temp);
    }

    unsigned char* setFrameFormat() {
        unsigned char* frame = new unsigned char[1 + 4 + 1 + 1 + 1 + 1];
        frame[0] = SOH;
        frame[1] = (sequenceNum >> 24) & 0xFF;
        frame[2] = (sequenceNum >> 16) & 0xFF;
        frame[3] = (sequenceNum >> 8) & 0xFF;
        frame[4] = sequenceNum & 0xFF;
        frame[5] = STX;
        frame[6] = data;
        frame[7] = ETX;
        frame[8] = checkSum;
        
        return frame;
    }

    void setData(unsigned char data) {
        this->data = data;
        //now able to set checksum
        this->checkSum = SOH + sequenceNum + STX + data + ETX;
    };

    void setSequenceNum(unsigned int num){
        sequenceNum = num;
    };

    unsigned char getData() {
        return data;
    };
    unsigned char getCheckSum() {
        return checkSum;
    }
    unsigned int getSeqNum() {
        return sequenceNum;
    }
    bool isError(){
        return error;
    }

    void printContent() {
        cout << sequenceNum << " " << data << " ";
        printf("%02hhX ", checkSum);
        cout << endl;
    }
};

#endif
