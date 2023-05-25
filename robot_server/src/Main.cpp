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
#include "Classes.h"


#define PORT 8888  // arbitrary


/* Nah new classes here fr */
class RobotState {
  // will hold the following info...
  // 1. state name
  // 2. state verb
  // 3. next state
  //

  std::string state_name;
  std::string verb_name;

  std::shared_ptr<RobotState> next_state;

  public:
    // setters
    void set_state_name(const std::string sn) {
      state_name = sn;
    }
    void set_verb_name(const std::string vn) {
      verb_name = vn;
    }
    void set_next_state(std::shared_ptr<RobotState> ns) {
      next_state = ns;
    }

    // getters
    std::string get_state_name() {
      return state_name;
    }
    std::string get_verb_name() {
      return verb_name;
    }
    std::shared_ptr<RobotState> get_next_state() {
      return next_state;
    }
};



class StateMachine {
  uint64_t init_time = 0;
  uint64_t curr_time = 0;
  uint64_t elapsed_time = 0;
  uint64_t time_to_wait = 2000;

  std::shared_ptr<RobotState> curr_state;

  public:
    void set_curr_state(std::shared_ptr<RobotState> state) {
      curr_state = state;
    }

    void tick(const small_world::SM_Event & event) {
      // set time
      set_init_time(event);
      set_curr_time(event);
      set_elapsed_time();

      // print message based on current state
      if (elapsed_time < time_to_wait) {
        print_doing_message(curr_state);

      } else {
        // print messages for grading
        print_finished_message(curr_state);

        // curr_state = (*curr_state.get())->next_state;  // this will throw an error
        RobotState *temp = curr_state.get();
        curr_state = temp->get_next_state();

        print_began_message(curr_state);

        // change init time
        match_init_curr_time();
      }
    }
  
  private:
    // this will print the messages that will be graded
    void print_doing_message(std::shared_ptr<RobotState> state) {
      
    }
    void print_began_message(std::shared_ptr<RobotState> state) {

    }
    void print_finished_message(std::shared_ptr<RobotState> state) {

    }

    // private setters
    void set_init_time(const small_world::SM_Event & event) {
      char *ptr;
      init_time = strtoull(event.event_time().c_str(), &ptr, 10);
    }
    void set_curr_time(const small_world::SM_Event & event) {
      char *ptr;
      curr_time = strtoull(event.event_time().c_str(), &ptr, 10);
    }
    void set_elapsed_time() {
      elapsed_time = curr_time - init_time;
    }
    void match_init_curr_time() {
      init_time = curr_time;
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


int listen_routine(int &sckt, struct sockaddr_in &address, std::shared_ptr<StateMachine> sm) {
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


  // make states
  std::shared_ptr<RobotState> s0(new RobotState());
  s0->set_state_name("move state");
  s0->set_verb_name("moving");

  std::shared_ptr<RobotState> s1(new RobotState());
  s1->set_state_name("wait state");
  s1->set_verb_name("waiting");

  // make states point to each other
  s0->set_next_state(s1);
  s1->set_next_state(s0);

  // make state machine. Start on wait state
  std::shared_ptr<StateMachine> sm(new StateMachine());
  sm->set_curr_state(s1);

  std::cout << "finished state machine setup" << std::endl;


  // the connection routine below
  int sckt;
  struct sockaddr_in address;

  socket_setup(sckt, address);
  std::cout << "finished socket setup" << std::endl;

  std::cout << "listening on port " << PORT << std::endl;
  listen_routine(sckt, address, sm);

	return EXIT_SUCCESS;
}
