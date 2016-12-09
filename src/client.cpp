// BetterCast Client
// Copyright 2016 Sam Kelly
// run on the device you want to cast / PC

#include "bettercast.h"

void close_sockets() {
  if(local_sock >= 0) {
    shutdown(local_sock, SHUT_RDWR);
    close(local_sock);
  }
  if(remote_sock >= 0) {
    shutdown(remote_sock, SHUT_RDWR);
    close(remote_sock);
  }
}

void sigint_handler(int sig) {
  std::cout << "\nCTRL+C detected, closing sockets" << std::endl;
  closing = true;
  close_sockets();
}

int main() {
  signal(SIGINT, sigint_handler);
  init_screen_buffer();
  struct sockaddr_in local_addr, remote_addr;
  socklen_t addr_len;
  int n;
  local_sock = socket(AF_INET, SOCK_STREAM, 0);
  int yes = 1;
  setsockopt(local_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  if(local_sock < 0)
    error("error opening socket!");
  bzero((char *) &local_addr, sizeof(local_addr));
  local_addr.sin_family = AF_INET;
  local_addr.sin_addr.s_addr = INADDR_ANY;
  local_addr.sin_port = htons(PORT_NUM);
  n = bind(local_sock, (struct sockaddr *) &local_addr, sizeof(local_addr));
  if(n < 0) error("error binding to socket!");
  addr_len = sizeof(remote_addr);
  bool communicating = false;
  while(!closing) {
    listen(local_sock, BACKLOG_SIZE);
    std::cout << "waiting for incoming connections on port " << PORT_NUM << std::endl;
    remote_sock = accept(local_sock, (struct sockaddr *) &remote_addr, &addr_len);
    setsockopt(remote_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    if(closing) break;
    if(remote_sock < 0) error("error accepting incoming connection!");
    communicating = true;
    while(communicating) {
      std::string msg = read_msg(remote_sock);
      std::cout << " << " << msg << std::endl;
      if(msg.size() < 2) msg = "close";
      if(msg == "diff") { // send a diff
        //TODO: implement
      } else if(msg == "key") { // send a keyframe
        std::cout << "sending keyframe" << std::endl;
        foreach_screen_pixel([&](unsigned char r, unsigned char g, unsigned char b, int i) {
          screen_buffer[i].r = r;
          screen_buffer[i].g = g;
          screen_buffer[i].b = b;
        });
        if(!write(remote_sock, (void *)screen_buffer, screen_buffer_size * 3)) communicating = false;
        if(!write_msg(remote_sock, "ok")) communicating = false;
      } else if(msg == "size") { // send the screen dimensions
        std::ostringstream os;
        os << screen_size.width << "x" << screen_size.height;
        if(!write_msg(remote_sock, os.str())) communicating = false;
      } else if(msg == "bettercast:probe") {
        if(!write_msg(remote_sock, "bettercast:ack")) communicating = false;
      } else if(msg == "close") { // close the connection
        communicating = false;
      }
    }
    std::cout << "the connection was closed" << std::endl;
    shutdown(remote_sock, SHUT_RDWR);
    close(remote_sock);
  }
  close_sockets();
}
