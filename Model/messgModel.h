#ifndef MESSGMODEL_H
#define MESSGMODEL_H

#include "format.h";

class MESSGModel {
  private:
    Byte soh = SOH;
    Byte stx = STX;
    Byte *data;
    Byte etx = ETX;
    Byte checkSum;
    unsigned int sequenceNum;

  public:
    void setData(Byte*);
    void setCheckSum(Byte);
    void setSequenceNum(unsigned int);
    Byte getSOH();
    Byte getSTX();
    Byte* getData();
    Byte getETX();
    Byte getCheckSum();
    unsigned int getSeqNum();
};

#endif
