
#include <iostream>

#include <arpa/inet.h>     
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 

#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>

#define PORT 8888

// global state variable here
int state = 0;


int socket_setup(int &sckt, struct sockaddr_in &address) {
  // make useable socket
  sckt = socket(AF_INET , SOCK_STREAM , 0);
  if (sckt == 0) {
    std::cerr << "setsockopt" << std::endl;  
    exit(EXIT_FAILURE);  
  }

  // let the socket be re-used
  if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )  
  {  
    std::cerr << "setsockopt" << std::endl;  
    exit(EXIT_FAILURE);  
  }

  // define socket address and type
  address.sin_family = AF_INET;  
  address.sin_addr.s_addr = INADDR_ANY;  
  address.sin_port = htons(PORT);  

  //bind the socket
  if (bind(sckt, (struct sockaddr *)&address, sizeof(address))<0)  
  {  
    std::cerr << "bind failed" << std::endl;  
    exit(EXIT_FAILURE);  
  }

  return EXIT_SUCCESS;
}


int listen_routine(int &sckt) {

  // listen to, at max, 3 connections
  if (listen(sckt, 3) < 0)  
  {  
    std::cerr << "error listening" << std::endl;  
    exit(EXIT_FAILURE);  
  }

  addrlen = sizeof(address);
  while (true) {
    // listen infinitely for a new message to accept
    int accepted_sckt = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)
    if (accepted_sckt < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // read message from accepted 
    // socket then clean up
    handle_message(accepted_sckt);
    close(accepted_sckt);
  }

  return EXIT_SUCCESS;
}


int handle_message(int &sckt) {
  // read messages here
  return EXIT_SUCCESS;
}


int main(int argc, char * argv[]) {

  // use these strings to indicate the state transitions
  // the robot progresses through.  Do not modify these strings

  std::string robot_waiting = "The robot is waiting";
  std::string robot_moving = "The robot is moving";

  std::string robot_finished_waiting = "The robot finished waiting";
  std::string robot_finished_moving = "The robot finished moving";

  std::string robot_began_waiting = "The robot began waiting";
  std::string robot_begin_moving = "The robot begain moving";


  int sckt;
  struct sockaddr_in address;
  socket_setup(sckt, address);

  listen_routine(sckt);

	return EXIT_SUCCESS;
}

