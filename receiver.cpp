#include <stdio.h>        // Default for system call
#include <sys/socket.h>   // Socket creating and binding
#include <netinet/in.h>   // Needed to use struct sockaddr_in
#include <string.h>       // For string method
#include <stdlib.h>       // Standard lib
#include <time.h>         // For control timing

#include "Model/messgModel.h"
#include "Model/ackModel.h"

class Receiver {
  private:
    int udpSocket;
    struct sockaddr_in serverAddr;            // server address
    struct sockaddr_in clientAddr;            // client address
    struct sockaddr_storage serverStorage;
    socklen_t client_addr_size;               // client address size
    socklen_t addr_size;                      // address size
  
}