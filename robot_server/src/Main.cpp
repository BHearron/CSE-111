
#include <iostream>

#include <arpa/inet.h>     
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 

#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>

#include <string.h>  // used for testing

#include <chrono>
#include <thread>

#include "Message.h"

#define PORT 8888


/* Credit to Week7 LectureB slides */
class Tickable {

  uint64_t last_tick_time = 0;
  std::string last_tick_time_string = "";

  public:

    virtual void tick(const small_world::SM_Event & event) {
      // convert string to uint64_t
      char* end;
      last_tick_time = strtoull(event.event_time().c_str(), &end, 10);
      std::cout << last_tick_time << std::endl;
    }
};

class StateMachine {

};

class RobotState {
  uint64_t init_time;
  uint64_t curr_time;

  std::map<std::string, std::shared_ptr<RobotState>> next_states;
  std::shared_ptr<StateMachine> owner;

  uint64_t get_elapsed() {
    return curr_time - init_time;
  }

  void set_next_state(const std::string & state_name, std::shared_ptr<RobotState> state) {
    next_states[state_name] = state;
  }
};




/* Credit to Week7 LectureB slides */
uint64_t timeSinceEpochMillisec() {
  using namespace std::chrono;
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}


int handle_message(int &sckt) {
  int client_msg_len = 0;
  do {
    // get and echo request from client
    char buf[1025] = {0};
    client_msg_len = read(sckt, buf, 1024);
    std::cerr << buf << std::endl;

    // carry out client request

    // send client success
    const char* resp = "Success";
    write(sckt, resp, strlen(resp));
  }
  while (client_msg_len > 0);

  return EXIT_SUCCESS;
}


int socket_setup(int &sckt, struct sockaddr_in &address) {
  // make useable socket
  sckt = socket(AF_INET , SOCK_STREAM , 0);
  if (sckt == 0) {
    std::cerr << "setsockopt" << std::endl;  
    exit(EXIT_FAILURE);  
  }

  // let the socket be re-used
  int opt = true;
  if( setsockopt(sckt, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )  
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


int listen_routine(int &sckt, struct sockaddr_in &address) {
  // listen to, at max, 3 connections
  if (listen(sckt, 3) < 0)  
  {  
    std::cerr << "error listening" << std::endl;  
    exit(EXIT_FAILURE);  
  }

  // listen for new requests to connect
  int addrlen = sizeof(address);
  while (true) {
    // accept incoming connection
    int accepted_sckt = accept(sckt, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if (accepted_sckt < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // read and handle message from client
    handle_message(accepted_sckt);

    // clean up
    close(accepted_sckt);
  }

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

  
  // the connection routine below
  int sckt;
  struct sockaddr_in address;

  socket_setup(sckt, address);
  listen_routine(sckt, address);

	return EXIT_SUCCESS;
}

