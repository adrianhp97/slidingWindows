#include <stdio.h>        // Default for system call
#include <sys/socket.h>
#include <time.h>         // For control timing
#include <iostream>
#include <fstream>
#include <netinet/in.h>
#include <string.h>
#include <cerrno>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include "Model/messgModel.h"
#include "Model/ackModel.h"
#include <mutex>
#include <thread>
#include <chrono>
#include <vector>

using namespace std;

int fd; //socket UDP
int SERVICE_PORT = 21234;   //port receiver
char* server = "127.0.0.1";   //receiver's address
char* filename = "test.txt";
struct sockaddr_in myaddr, remaddr;
const int WINDOW_SIZE = 4;
int seqNum = 0;
vector<char> buffer;

void sendSingleFrame(messgModel frame) {
	char* msg = frame.setFrameFormat();
	// for (int i = 0; i < strlen(msg); i++)
	// 	cout << msg[i];
	// cout << endl;
	// timeFrame temp;
	// temp.waktu = time(0);
	// temp.frameNum = frame.getFrameNumber();
	// timeBuffer.push_back(temp);
	cout << "Sending : " << frame.getSeqNum() << endl;
	int temp = sendto(fd, msg, 9, 0, (struct sockaddr *)&remaddr, sizeof(remaddr));
	// if (temp < 0)
	// 	cout << errno << endl;
	// else
	// 	cout << "sending succeded" << endl;	
}

void sendBuffer() {
    // mtx.lock();
    while (!buffer.empty()) {
    	int size = (buffer.size() >= 4) ? WINDOW_SIZE : buffer.size();
    	for (int i = 0; i < size; i++) {
    		sleep(1);
	    	messgModel sendThis(seqNum++);
	    	sendThis.setData(buffer.front()); 
			sendSingleFrame(sendThis);
			buffer.erase(buffer.begin());
		}
    } 
	// mtx.unlock();
}

void readToBuffer(char* filename) {
	ifstream fin(filename);	
    char temp;	

    cout << "Filling buffer with data ..." << endl;
	fin >> temp;
	while (!fin.eof()) {
		sleep(1);
		if (buffer.size() <= BUFFER_SIZE) {
			printf("read : %c\n", temp);
			buffer.push_back(temp);
			fin >> temp;
		}
		else {
			cout << "Buffer full, emptying buffer by transmitting the data" << endl;
			sendBuffer();
		}
	}
	if (!buffer.empty()) {
		cout << "Transmitting the rest of buffer data";
		sendBuffer();
	}

	fin.close();
}

void recvSign() {
  while(1) {
  	sleep(1);
  	char* sign = new char [7];
    recvfrom(fd, sign, 7, 0, NULL, NULL);
    ACKModel frame(sign);
    // frame.printContent();
 	// timeBuffer.erase(timeBuffer.begin());
 	printf("ACK %d\n", frame.getNextSeqNum());	
  }
}

int main() {
	if ((fd = socket(PF_INET, SOCK_DGRAM, 0))==-1)
		printf("socket created\n");

	// sender address
	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(0);

	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		cout << "bind failed";
		return 0;
	}  

	//receiver address
	memset((char *) &remaddr, 0, sizeof(remaddr));
	remaddr.sin_family = AF_INET;
	remaddr.sin_port = htons(SERVICE_PORT);
	if (inet_aton(server, &remaddr.sin_addr)==0) {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

	thread th1(recvSign);
	thread th2(readToBuffer, filename);

	th1.join();
	th2.join();
}