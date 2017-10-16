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

using namespace std;

int clientSocket;
int portNum;                    // port number
struct sockaddr_in myaddr, remaddr;
char* dataFromFile;
const int WINDOW_SIZE = 4;
int seqNum = 0;
char buffer[256];

void readFileAndStore(char* filename) {
	ifstream fin(filename);	
    char temp;
    int nMsg = 0;	

	fin >> temp;
	while (!fin.eof()) {
		dataFromFile[nMsg++] = temp;
		fin >> temp;
	}

	fin.close();
}

void readBuffer(int fd, char* filename) {
	ifstream fin(filename);	
    char temp;
    int nMsg = 0;	

	fin >> temp;
	while (!fin.eof()) {
		if (nMsg < 256) {
			buffer[nMsg++] = temp;
			fin >> temp;
		}
		else {
			sendBuffer(buffer);
			nMsg = 0;
		}
	}

	fin.close();
}

void sendBuffer(int fd) {
	int nMsg = 0;
	while (nMsg < 256) {
		messgModel frame(nMsg);
		frame.setData(buffer[nMsg]); 
		sendSingleFrame(fd, frame);
	}
}


void sendSingleFrame(int fd, messgModel frame) {
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


void recvSign(int clientSocket) {
  while(1) {
  	char* sign = new char [7];
    recvfrom(clientSocket, sign, 7, 0, NULL, NULL);
    ACKModel frame(sign);
    // frame.printContent();
 	// timeBuffer.erase(timeBuffer.begin());
 	printf("ACK %d\n", frame.getNextSeqNum());	
  }
}

void sendtoclient(int udpSocket) {
    // fillBuffer();
    
    // mtx.lock();
    for (int i = 0; i < WINDOW_SIZE; i++) {
    	messgModel sendThis(seqNum++);
    	sendThis.setData(dataFromFile[i]); 
		sendSingleFrame(udpSocket, sendThis);
		// buffer.erase(buffer.begin());
	}
	// mtx.unlock();
}


int main() {
	int fd;
	dataFromFile = new char [1000];
	int SERVICE_PORT = 21234;
	char *server = "127.0.0.1";

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

	readFileAndStore("test.txt");
	thread th1(recvSign, fd);
	thread th2(sendtoclient, fd);

	th1.join();
	th2.join();
}