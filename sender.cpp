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
#include <thread>
#include <chrono>
#include <vector>
#include <mutex>

using namespace std;

typedef struct {
	int waktu;
	unsigned int frameId;
	messgModel frame;
} timeoutframe;

int fd; //socket UDP
int SERVICE_PORT = 21234;   //port receiver
char* server = "127.0.0.1";   //receiver's address
char* filename = "test.txt";
struct sockaddr_in myaddr, remaddr;
unsigned int WINDOW_SIZE = 4;
unsigned int seqNum = 0;
vector<unsigned char> buffer;
vector<timeoutframe> timeout;
unsigned int LAR = 0; //last acknowledgement received
unsigned int LFS = 0; //last frame sent
bool bufferReceiverFull = false;
int ACK_TIMEOUT = 3;  // timeout 2 detik
mutex key;

void sendSingleFrame(messgModel frame) {
	timeoutframe temp;
	temp.waktu = time(0);  //get current time
	temp.frameId = frame.getSeqNum();
	temp.frame = frame;
	timeout.push_back(temp);

	unsigned char* msg = frame.setFrameFormat();
	cout << "SYNC " << frame.getSeqNum() << endl;
	sendto(fd, msg, 9, 0, (struct sockaddr *)&remaddr, sizeof(remaddr));
}

void sendBuffer() {
	//wait until it's empty
	while (bufferReceiverFull) {
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		cout << "Waiting for receiver buffer... " << endl;
	}

    while (!buffer.empty()) {
    	std::this_thread::sleep_for(std::chrono::milliseconds(500));
    	//cout << LFS << " " << LAR << " " << LFS - LAR << endl;
    	while ((LFS < LAR) || (LFS - LAR) < (WINDOW_SIZE - 1)) {
    		if (buffer.empty())
    			break;
    		else {
    			LFS = seqNum; //update last frame sent
		    	messgModel sendThis(seqNum++);
		    	sendThis.setData(buffer.front()); 
				sendSingleFrame(sendThis);
				buffer.erase(buffer.begin());
    		}
    	} 
    }
}

void readToBuffer(char* filename) {
	ifstream fin(filename);	
    unsigned char temp;	

    cout << "Filling buffer with data ..." << endl;
	fin >> temp;
	while (!fin.eof()) {
		if (buffer.size() <= BUFFER_SIZE) {
			buffer.push_back(temp);
			fin >> temp;
		}
		else {
			cout << "Buffer full, emptying buffer by transmitting the data" << endl;
			sendBuffer();
		}
	}
	if (!buffer.empty()) {
		cout << "Transmitting the rest of buffer data" << endl;
		sendBuffer();
	}
	fin.close();
}

// Find timeoutframe with frameid equals to num
vector<timeoutframe>::iterator findtimeoutframe(unsigned int num) {
	for (vector<timeoutframe>::iterator it=timeout.begin(); it < timeout.end(); it++) {
		if (it->frameId == num) 
			return it;
	}
	return timeout.begin();
}

void recvSign() {
  while(1) {
  	unsigned char* sign = new unsigned char [7];
    recvfrom(fd, sign, 7, 0, NULL, NULL);
    ACKModel frame(sign);
    // frame.printContent();
 	// timeBuffer.erase(timeBuffer.begin());
 	// if receiver's buffer can't contain window
 	if (frame.getAdvWindowSize() < WINDOW_SIZE) {
 		bufferReceiverFull = true;
 		cout << "buffer's receiver is full" << endl;
 	}
 	else {
 		timeout.erase(findtimeoutframe(frame.getNextSeqNum() - 1));

 		bufferReceiverFull = false;
 		LAR = frame.getNextSeqNum();
 		cout << "ACK " << frame.getNextSeqNum() << endl;
 	}
  }
}

// Resend data if timeout
void checkTimeout() {
	sleep(2);
	while (true) {
		if (!timeout.empty()) {
			for (int i = 0; i < timeout.size(); i++) {
				int now = time(0);
				if ((now - timeout[i].waktu) > ACK_TIMEOUT) {
					sendSingleFrame(timeout[i].frame);
					timeout.erase(timeout.begin() + i);
				}
			}
		}
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
	thread th3(checkTimeout);

	th1.join();
	th2.join();
	th3.join();
}