#include "ackModel.h";

void ACKModel::setAdvWindowSize(Byte winSize) {
  advWindowSize = winSize;
}

void ACKModel::setCheckSum(Byte sum) {
  checkSum = sum;
}

void ACKModel::setNextSeqNum(unsigned int nextNum) {
  nextSeqNum = nextNum;
}

Byte ACKModel::getAck() {
  return ack;
}

Byte ACKModel::getAdvWindowSize() {
  return advWindowSize;
}

Byte ACKModel::getCheckSum() {
  return checkSum;
}

unsigned int ACKModel::getNextSeqNum() {
  return nextSeqNum;
}
