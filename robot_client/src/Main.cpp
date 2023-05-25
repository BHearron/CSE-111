/* Socket setup help: https://www.geeksforgeeks.org/socket-programming-cc/# */


#include <iostream>

#include <arpa/inet.h>     
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 

#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>

#include <string.h>  // for testing purposes

#include <chrono>
#include <thread>

#define PORT 8888


/* Credit to Week7 LectureB slides */
uint64_t timeSinceEpochMillisec() {
  using namespace std::chrono;
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}


int send_message(int &sckt_fd) {
  while(1) {
    const char* msg = "tick";
    int rtn = write(sckt_fd, msg, strlen(msg));
    std::cerr << "rtn: " << rtn << " |  sent tick. Sleeping for 500ms" << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    // sleep(1);
  }
  std::cerr << "how did i get here?" << std::endl;
  return EXIT_SUCCESS;
}


int setup_socket(int &sckt_fd, struct sockaddr_in &serv_addr) {
  // create socket
  sckt_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sckt_fd < 0) {
    std::cerr << "\n Socket creation error \n" << std::endl;
    exit(EXIT_FAILURE); 
  }

  // setup address info
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  // Convert ip address
  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    std::cerr << "\nInvalid address/ Address not supported \n" << std::endl;
    exit(EXIT_FAILURE); 
  }

  // connect to server
  if ((connect(sckt_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
    std::cerr << "\nConnection Failed \n" << std::endl;
    exit(EXIT_FAILURE); 
  }

  return EXIT_SUCCESS;
}


int main(int argc, char * argv[]) {
  int sckt_fd;
  struct sockaddr_in serv_addr;

  setup_socket(sckt_fd, serv_addr);
  send_message(sckt_fd);

	return EXIT_SUCCESS;
}

