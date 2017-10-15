#ifndef ACKMODEL_H
#define ACKMODEL_H

#include "format.h";

class ACKModel {
  private:
    Byte ack = ACK;
    Byte advWindowSize;
    Byte checkSum;
    unsigned int nextSeqNum;

  public:
    void setAdvWindowSize(Byte);
    void setCheckSum(Byte);
    void setNextSeqNum(unsigned int);
    Byte getAck();
    Byte getAdvWindowSize();
    Byte getCheckSum();
    unsigned int getNextSeqNum();

};

#endif