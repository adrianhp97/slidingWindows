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

using namespace std;

int clientSocket;
int portNum;                    // port number
struct sockaddr_in myaddr, remaddr;
char* dataFromFile;

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


void sendSingleFrame(int fd, messgModel frame) {
	char* msg = frame.setFrameFormat();
	// for (int i = 0; i < strlen(msg); i++)
	// 	cout << msg[i];
	// cout << endl;
	// timeFrame temp;
	// temp.waktu = time(0);
	// temp.frameNum = frame.getFrameNumber();
	// timeBuffer.push_back(temp);
	cout << "Sending message ke-" << frame.getSeqNum() << " : " << frame.getData() << endl;
	int temp = sendto(fd, msg, 9, 0, (struct sockaddr *)&remaddr, sizeof(remaddr));
	if (temp < 0)
		cout << errno << endl;
	else
		cout << "sending succeded" << endl;	
}


int main() {
	int fd;
	dataFromFile = new char [1000];
	int windowsize = 4;
	int buffersize = 8;
	int SERVICE_PORT = 21234;
	char *server = "127.0.0.1";

	if ((fd = socket(PF_INET, SOCK_DGRAM, 0))==-1)
		printf("socket created\n");

	/* bind it to all local addresses and pick any port number */
	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(0);

	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		cout << "bind failed";
		return 0;
	}  

	/* now define remaddr, the address to whom we want to send messages */
	/* For convenience, the host address is expressed as a numeric IP address */
	/* that we will convert to a binary format via inet_aton */
	memset((char *) &remaddr, 0, sizeof(remaddr));
	remaddr.sin_family = AF_INET;
	remaddr.sin_port = htons(SERVICE_PORT);
	if (inet_aton(server, &remaddr.sin_addr)==0) {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

	readFileAndStore("test.txt");
	messgModel sendThis(2);
	sendThis.setData(dataFromFile[0]); 
	sendSingleFrame(fd, sendThis);
	messgModel sendThis2(3);
	sendThis2.setData(dataFromFile[1]); 
	sendSingleFrame(fd, sendThis2);
}