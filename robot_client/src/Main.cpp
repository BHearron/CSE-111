/*
 * Socket setup help: https://www.geeksforgeeks.org/socket-programming-cc/#
 */


#include <iostream>

#include <arpa/inet.h>     
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 

#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>

#define PORT 8888

int setup_socket(int &sckt_fd, struct sockaddr_in &serv_addr) {
  // create socket
  sckt_fd = socket(AF_INET, SOCK_STREAM, 0)
  if (sckt_fd < 0) {
    cout:cerr << "\n Socket creation error \n" << std::endl;
    exit(EXIT_FAILURE); 
  }

  // setup address info
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    std:cerr << "\nInvalid address/ Address not supported \n" << std::endl;
    exit(EXIT_FAILURE); 
  }

  // connect to server
  if ((connect(sckt_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
    std:cerr << "\nConnection Failed \n" << std::endl;
    exit(EXIT_FAILURE); 
  }

  handle_messages(sckt_fd);

  return EXIT_SUCCESS;
}

int handle_messages(int &sckt_fd) {
  return EXIT_SUCCESS;
}

int main(int argc, char * argv[]) {
  int sckt_fd;
  struct sockaddr_in serv_addr;
  setup_socket(sckt_fd, serv_addr);

	return EXIT_SUCCESS;
}

