#include <stdio.h>        // Default for system call
#include <sys/socket.h>   // Socket creating and binding
#include <netinet/in.h>   // Needed to use struct sockaddr_in
#include <string.h>       // For string method
#include <stdlib.h>       // Standard lib
#include <time.h>         // For control timing

#include "Model/messgModel.h"
#include "Model/ackModel.h"

class Sender {
  private:
    int clientSocket;
    int portNum;                    // port number
    struct sockaddr_in serverAddr;  // server address
    socklen_t addr_size;            // address size
}