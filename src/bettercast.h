#include <iostream>
#include <functional>
#include <sstream>
#include <unordered_set>

#include "CImg.h"
#include <X11/Xlib.h>
#include <X11/X.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <math.h>

using namespace cimg_library;

const int MAX_SCREEN_SIZE = 1920 * 1080;
const int PORT_NUM = 13314;
const int BACKLOG_SIZE = 10;
const int MSG_BUFFER_SIZE = 256;
const int PATCH_SIZE = 96; // 96 x 96
const int NUM_PATCHES = MAX_SCREEN_SIZE / (PATCH_SIZE * PATCH_SIZE);

class Size {
public:
  int width;
  int height;
};

typedef struct {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} RGB888;

std::unordered_set<int> active_patches;

RGB888 screen_buffer[MAX_SCREEN_SIZE];
Size screen_size;
char msg_buffer[MSG_BUFFER_SIZE];
bool closing = false;
int local_sock, remote_sock;
int screen_buffer_size;
Display *display = XOpenDisplay(NULL);
Window root = DefaultRootWindow(display);
XWindowAttributes gwa;
unsigned long red_mask;
unsigned long green_mask;
unsigned long blue_mask;

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
  RGB888 empty;
  empty.r = 0;
  empty.g = 0;
  empty.b = 0;
  for(int i = 0; i < MAX_SCREEN_SIZE; i++)
    screen_buffer[i] = empty;
  std::cout << "screen buffer initialized." << std::endl;
}

void foreach_screen_pixel(std::function<void (unsigned char&, unsigned char&, unsigned char&, int&)> func) {
  XImage *image = XGetImage(display, root, 0, 0, screen_size.width, screen_size.height, AllPlanes, ZPixmap);
  red_mask = image->red_mask;
  green_mask = image->green_mask;
  blue_mask = image->blue_mask;
  int i = 0;
  for(int y = 0; y < screen_size.height; y++)
  for(int x = 0; x < screen_size.width; x++) {
    unsigned long pixel = XGetPixel(image, x, y);
    unsigned char b = pixel & blue_mask;
    unsigned char g = (pixel & green_mask) >> 8;
    unsigned char r = (pixel & red_mask) >> 16;
    if(b == 0) b = 1;
    if(g == 0) g = 1;
    if(r == 0) r = 1;
    if(b == 255) b = 254;
    if(g == 255) g = 254;
    if(r == 255) r = 254;
    func(r, g, b, i);
    i++;
  }
  XDestroyImage(image);
}

bool screen_diffpatch() {
  active_patches.clear();
  int patch_x = 0;
  int patch_y = 0;
  int patch_xx = 0;
  int patch_yy = 0;
  foreach_screen_pixel([&](unsigned char r, unsigned char g, unsigned char b, int i) {
    //TODO: simpler formula
    //int y = i / screen_size.width;
    //int x = i - (y * screen_size.width);
    if(patch_xx >= PATCH_SIZE) {
      patch_xx = 0;
      patch_x++;
      if(patch_x >= screen_size.width / PATCH_SIZE)
        patch_x = 0;
    }
    if(patch_yy >= PATCH_SIZE) {
      patch_yy = 0;
      patch_y++;
    }
    int patch_num = patch_y * (screen_size.width / PATCH_SIZE) + patch_x;
    RGB888 *old_pixel = &screen_buffer[i];
    if(old_pixel->r != r || old_pixel->g != g || old_pixel->b != b)
      active_patches.insert(patch_num);
    screen_buffer[i].r = r;
    screen_buffer[i].g = g;
    screen_buffer[i].b = b;
  });
  return active_patches.size() < NUM_PATCHES * 0.80;
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
