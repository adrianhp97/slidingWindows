#include <stdio.h>        // Default for system call
#include <sys/socket.h>   // Socket creating and binding
#include <netinet/in.h>   // Needed to use struct sockaddr_in
#include <string.h>       // For string method
#include <stdlib.h>       // Standard lib
#include <time.h>         // For control timing
#include <iostream>
#include <fstream>
#include <cerrno>
#include <vector>
#include <unistd.h>
#include <cerrno>

#include "Model/messgModel.h"
#include "Model/ackModel.h"

using namespace std;

int udpSocket;
struct sockaddr_in myaddr;            // this address
struct sockaddr_in remaddr;            // sender address
socklen_t addrlen = sizeof(remaddr);  /* length of addresses */
int SERVICE_PORT;
char* filename;
unsigned int WINDOW_SIZE;
unsigned int BUFFER_SIZE;
vector<unsigned char> buffer;
unsigned int NFE = 0; //next frame expected
unsigned int LFA = WINDOW_SIZE - 1; //largest frame acceptable

void writeToFile(char* filename) {
	ofstream fout;
	fout.open(filename, fstream::app);
	
	while (!buffer.empty()) {
		fout << buffer.front();
		//cout << "Writing : " << buffer.front() << endl;
		buffer.erase(buffer.begin());
	}
}

void storeInBuffer(unsigned char msg) {
	buffer.push_back(msg);
	//cout << "store in buffer : " << msg << endl;
	if (buffer.size() == BUFFER_SIZE) {
		cout << "buffer is full, empty it by writing to file" << endl;
		writeToFile(filename);
	}
	else
	if (msg == '.') {
		cout << "eof detected, empty the rest of data in buffer" << endl;
		writeToFile(filename);
	}
}

void recvMsg(int udpSocket) {
	unsigned char msg[9];
	//bool approved[WINDOW_SIZE+1];
	//setAllFalse(approved, WINDOW_SIZE);
	printf("Waiting on port %d\n", SERVICE_PORT);
	while (true) {
		//std::this_thread::sleep_for(std::chrono::milliseconds(500));
		int test = recvfrom(udpSocket, msg, 9, 0, (struct sockaddr *)&remaddr, &addrlen);

		messgModel temp(msg);
		if (temp.isError())
			cout << "Error detected -> message rejected" << endl;
		else {
			storeInBuffer(temp.getData());
			
			if (temp.getSeqNum() == NFE) { //as expected then accepted
				NFE++;
				LFA++;
			}
			//else, send ACK of expected frame 

			ACKModel tempAck(NFE, BUFFER_SIZE - buffer.size());
			unsigned char* msg = tempAck.setFrameFormat();

			//send ACK
			if (sendto(udpSocket, msg, 7, 0, (struct sockaddr *)&remaddr, sizeof(remaddr)) < 0)
				cout << errno << endl;
			else
				cout << "sending ACK : " << tempAck.getNextSeqNum() << endl;	
		}
	}
} 


int main(int argc, char** argv) {
	if (argc < 5) {
		perror("<filename> <windowsize> <buffersize> <port>");
		exit(1);
	}

	filename = argv[1];
	WINDOW_SIZE = atoi(argv[2])	;
	BUFFER_SIZE = atoi(argv[3]);
	SERVICE_PORT = atoi(argv[4]);

	int fd;
	/* create a UDP socket */
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("cannot create socket\n");
		return 0;
	}

	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(SERVICE_PORT);

	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) { 
		cout << "bind failed : " << errno << endl;
		return 0; 
	}

	recvMsg(fd);
}	