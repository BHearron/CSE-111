
#include <iostream>

#include <arpa/inet.h>     
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 

#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>


int socket_routine(int & sckt, struct sockaddr_in & address) {
  // make useable socket
  sckt = socket(AF_INET , SOCK_STREAM , 0);
  if (sckt == 0) {
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
  socket_routine(&sckt, &address);
  
	return EXIT_SUCCESS;
}

