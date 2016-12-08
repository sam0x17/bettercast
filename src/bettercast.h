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

void foreach_screen_pixel(std::function<void (unsigned long&, int&)> func) {
  XImage *image = XGetImage(display, root, 0, 0, screen_size.width, screen_size.height, AllPlanes, ZPixmap);
  //unsigned long red_mask = image->red_mask;
  //unsigned long green_mask = image->green_mask;
  //unsigned long blue_mask = image->blue_mask;
  int i = 0;
  for(int x = 0; x < screen_size.width; x++)
  for(int y = 0; y < screen_size.height ; y++) {
    unsigned long pixel = XGetPixel(image, x, y);
    /*unsigned char b = pixel & blue_mask;
    unsigned char g = (pixel & green_mask) >> 8;
    unsigned char r = (pixel & red_mask) >> 16;*/
    func(pixel, i);
    i++;
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
  std::cout << " >> " << msg << std::endl;
  std::string final_msg = msg + "\n";
  int n = write(socket, final_msg.c_str(), final_msg.size());
  if(n < 0) return false;
  return true;
}
