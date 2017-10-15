#include "messgModel.h";

void MESSGModel::setData(Byte* _data) {
  data = _data;
}

void MESSGModel::setCheckSum(unsigned int sum) {
  checkSum = sum;
}

void MESSGModel::setSequenceNum(unsigned int num) {
  sequenceNum = num;
}

Byte MESSGModel::getSOH() {
  return soh;
}

Byte MESSGModel::getSTX() {
  return stx;
}

Byte* MESSGModel::getData() {
  return *data;
}

Byte MESSGModel::getETX() {
  return etx;
}

Byte MESSGModel::getCheckSum() {
  return checkSum;
}

unsigned int MESSGModel::getSequenceNum() {
  return sequenceNum;
}
