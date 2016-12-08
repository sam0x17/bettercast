#include <X11/Xlib.h>
#include <X11/X.h>
#include <iostream>
#include <functional>
#include <sstream>
#include "CImg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

using namespace cimg_library;

const int MAX_SCREEN_SIZE = 64000000; // larger than largest possible consumer screen resolution
const int PORT_NUM = 13314;
const int BACKLOG_SIZE = 10;
const int MSG_BUFFER_SIZE = 256;

class Size {
public:
  int width;
  int height;
};

long screen_buffer[MAX_SCREEN_SIZE];
Size screen_size;
char msg_buffer[MSG_BUFFER_SIZE];
bool closing = false;
int local_sock, remote_sock;
int screen_buffer_size;
Display *display = XOpenDisplay(NULL);
Window root = DefaultRootWindow(display);
XWindowAttributes gwa;

std::string prune_chars(std::string str, std::string blacklist) {
  std::string result = "";
  for(char& c : str) {
    bool found = false;
    for(char& b : blacklist) {
      if(b == c) {
        found = true;
        break;
      }
    }
    if(!found) result += c;
  }
  return result;
}

Size get_screen_size() {
  XGetWindowAttributes(display, root, &gwa);
  Size s;
  s.width = gwa.width;
  s.height = gwa.height;
  return s;
}

void init_screen_buffer() {
  std::cout << "initializing screen buffer..." << std::endl;
  screen_size = get_screen_size();
  screen_buffer_size = screen_size.width * screen_size.height;
  if(screen_buffer_size > MAX_SCREEN_SIZE) {
    std::cout << "fatal error: display resolution of " << screen_size.width << "x" << screen_size.height;
    std::cout << " is greater than the maximum supported display resolution. Terminating." << std::endl;
    exit(1);
  }
  for(int i = 0; i < MAX_SCREEN_SIZE; i++)
    screen_buffer[i] = 0;
  std::cout << "screen buffer initialized." << std::endl;
}

void foreach_screen_pixel(std::function<void (unsigned long&)> func) {
  XImage *image = XGetImage(display, root, 0, 0, screen_size.width, screen_size.height, AllPlanes, ZPixmap);
  //unsigned long red_mask = image->red_mask;
  //unsigned long green_mask = image->green_mask;
  //unsigned long blue_mask = image->blue_mask;
  for(int x = 0; x < screen_size.width; x++)
  for(int y = 0; y < screen_size.height ; y++) {
    unsigned long pixel = XGetPixel(image, x, y);
    /*unsigned char b = pixel & blue_mask;
    unsigned char g = (pixel & green_mask) >> 8;
    unsigned char r = (pixel & red_mask) >> 16;*/
    func(pixel);
  }
}

void error(std::string msg) {
  std::cout << msg << std::endl;
  exit(1);
}

std::string read_msg(int socket) {
  std::string result = "";
  bzero(msg_buffer, MSG_BUFFER_SIZE);
  int n = read(socket, msg_buffer, MSG_BUFFER_SIZE);
  result += std::string(msg_buffer);
  result = prune_chars(result, "\r\n");
  return result;
}

bool write_msg(int socket, std::string msg) {
  std::string final_msg = msg + "\n";
  int n = write(socket, final_msg.c_str(), final_msg.size());
  if(n < 0) return false;
  return true;
}

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
      std::cout << "received message: " << msg << std::endl;
      if(msg.size() < 2) msg = "close";
      if(!write_msg(remote_sock, "generic response")) msg = "close";
      if(msg == "diff") { // send a diff

      } else if(msg == "key") { // send a keyframe

      } else if(msg == "size") { // send the screen dimensions
        std::ostringstream os;
        os << screen_size.width << "x" << screen_size.height;
        write_msg(remote_sock, os.str());
      } else if(msg == "close") { // close the connection
        std::cout << "the connection was closed" << std::endl;
        communicating = false;
        shutdown(remote_sock, SHUT_RDWR);
        close(remote_sock);
      }
    }
  }
  close_sockets();
}

/*
int main() {
  int sockfd, newsockfd;
  socklen_t client_len;
  char buffer[256];
  struct sockaddr_in server_address, client_address;
  int n;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) error("ERROR opening socket");
  bzero((char *) &server_address, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.slocal_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(BROADCAST_PORT_NUM);
  if(bind(sockfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
    error("ERROR on binding");
  listen(sockfd,5);
  client_len = sizeof(client_address);
  newsockfd = accept(sockfd, (struct sockaddr *) &client_address, &client_len);
  if (newsockfd < 0) error("ERROR on accept");
  bzero(buffer, 256);
  n = read(newsockfd, buffer, 255);
  if(n < 0) error("ERROR reading from socket");
  printf("Here is the message: %s\n", buffer);
  n = write(newsockfd, "I got your message",18);
  if(n < 0) error("ERROR writing to socket");
  close(newsockfd);
  close(sockfd);
  return 0;
}
*/
