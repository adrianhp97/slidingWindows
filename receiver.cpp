#include <stdio.h>        // Default for system call
#include <sys/socket.h>   // Socket creating and binding
#include <netinet/in.h>   // Needed to use struct sockaddr_in
#include <string.h>       // For string method
#include <stdlib.h>       // Standard lib
#include <time.h>         // For control timing
#include <iostream>
#include <fstream>

using namespace std;

#include "Model/messgModel.h"
#include "Model/ackModel.h"

int udpSocket;
struct sockaddr_in myaddr;            // this address
struct sockaddr_in remaddr;            // sender address
socklen_t addrlen = sizeof(remaddr);  /* length of addresses */
int SERVICE_PORT = 21234;
char filename[100] = "receive.txt";

void writeFileAndStore(char* filename, char msg) {
	ofstream fout;
	fout.open(filename, fstream::app);
    char temp;

    cout << "Writing to file" << endl;

	fout << msg;
	fout << endl;
}

void recvMsg(int udpSocket) {
	char msg[9];
	//bool approved[WINDOW_SIZE+1];
	//setAllFalse(approved, WINDOW_SIZE);
	while (true) {
		printf("Waiting on port %d\n", SERVICE_PORT);
		
		int test = recvfrom(udpSocket, msg, 9, 0, (struct sockaddr *)&remaddr, &addrlen);

		if (test > 0)
			cout << "Received : ";
		for (int i = 0; i < 9; i++)
			printf("%02hhX ", msg[i]);
		cout << endl;

		messgModel temp(msg);
		//temp.printContent();
		if (temp.isError())
			cout << "Error detected -> message rejected" << endl;
		else {
			writeFileAndStore(filename, temp.getData());
			//send ACK
			ACKModel tempAck(temp.getSeqNum()+1, 4);
			char* msg = tempAck.setFrameFormat();
			for (int i = 0; i < 7; i++)
				printf("%02hhX ", msg[i]);
			cout << endl;
		}
			
		// TransmitterFrame frame(msg);
		// //printf("Frame : "); frame.printBytes();
		// if(!frame.isError()) {
		// 	sendACK(frame.getFrameNumber(), udpSocket);
		// 	//cout << frame.getData() <<endl;
		// 	if(!approved[frame.getFrameNumber()]) {
		// 		//cout << frame.getData() <<endl;
		// 		buffer.push_back(frame);
		// 		//printf("Frame Number : ");
		// 		//printf("%d\n", buffer.top().getFrameNumber()); 
		// 		//cout << buffer.size() << endl;
		// 		approved[frame.getFrameNumber()] = true;
		// 		if(isAllTrue(approved, WINDOW_SIZE+1)) {
		// 			setAllFalse(approved, WINDOW_SIZE+1);
		// 		}
		// 	}
		// } else {
		// 	sendNAK(frame.getFrameNumber(), udpSocket);
		// }
	}
} 


int main() {
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
		cout << "bind failed : " << endl;
		return 0; 
	}

	recvMsg(fd);
}	