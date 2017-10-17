#include <stdio.h>
#include <iostream>
#include <ctime>
using namespace std;

int main() {
	unsigned int nextSeqNum = 258;
	unsigned char* frame = new unsigned char[4];
        frame[0] = (nextSeqNum >> 24) & 0xFF;
        frame[1] = (nextSeqNum >> 16) & 0xFF;
        frame[2] = (nextSeqNum >> 8) & 0xFF;
        frame[3] = nextSeqNum & 0xFF;

    // for (int i = 0; i < 4; i++)
    // 	 printf("%02hhX ", frame[i]);
    unsigned char temp = frame[0];
   	for (int i = 1; i < 4; i++)
   		temp += frame[i];
   	printf("%02hhX %02hhX", temp, nextSeqNum);
}