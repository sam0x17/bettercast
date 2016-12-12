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

const int SCREEN_W = 1920;
const int SCREEN_H = 1080;
const int NUM_PIXELS = SCREEN_W * SCREEN_H;
const int PORT_NUM = 13314;
const int BACKLOG_SIZE = 10;
const int MSG_BUFFER_SIZE = 256;
const int PATCH_SIZE = 60;
const int NUM_PATCHES = NUM_PIXELS / (PATCH_SIZE * PATCH_SIZE);
const float PATCH_FACTOR = 0.60; // percent of patches that need to change for us
                                 // to not perform a diffpatch

typedef struct {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} RGB888;

class Size {
public:
  int width;
  int height;
};

class Position {
public:
  int x;
  int y;
};

bool debug_mode = false;
int num_active_patches = 0;
int num_active_pixels = 0;
bool active_patches[SCREEN_W / PATCH_SIZE][SCREEN_H / PATCH_SIZE];
Position active_patches_list[NUM_PATCHES];
Size screen_size;
RGB888 screen_buffer[NUM_PIXELS];
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
  if(screen_buffer_size > NUM_PIXELS) {
    std::cout << "error: this version of better cast was not compiled to support " << screen_size.width << "x" << screen_size.height;
    exit(1);
  }
  RGB888 empty;
  empty.r = 0;
  empty.g = 0;
  empty.b = 0;
  for(int i = 0; i < NUM_PIXELS; i++)
    screen_buffer[i] = empty;
  std::cout << "screen buffer initialized." << std::endl;
}

void foreach_screen_pixel(std::function<void (unsigned char&, unsigned char&, unsigned char&, int&, int&, int&)> func) {
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
    func(r, g, b, i, x, y);
    i++;
  }
  XDestroyImage(image);
}

RGB888 current_patch[PATCH_SIZE * PATCH_SIZE];

void load_patch(int x, int y) {
  int start_x = x * PATCH_SIZE;
  int start_y = y * PATCH_SIZE;
  int i = 0;
  for(int y = 0; y < PATCH_SIZE; y++) {
    for(int x = 0; x < PATCH_SIZE; x++) {
      current_patch[i] = screen_buffer[(start_y + y) * screen_size.width + start_x + x];
      i++;
    }
  }
}

// "diffpatch" algorithm
bool screen_diffpatch() {
  for(int y = 0; y < screen_size.height / PATCH_SIZE; y++)
    for(int x = 0; x < screen_size.width / PATCH_SIZE; x++)
      active_patches[x][y] = false;
  bool full_refresh = false;
  num_active_patches = 0;
  num_active_pixels = 0;
  foreach_screen_pixel([&](unsigned char r, unsigned char g, unsigned char b, int i, int x, int y) {
    if(full_refresh) {
      screen_buffer[i].r = r;
      screen_buffer[i].g = g;
      screen_buffer[i].b = b;
      return;
    }
    RGB888 *old_pixel = &screen_buffer[i];
    if(old_pixel->r != r || old_pixel->g != g || old_pixel->b != b) {
      screen_buffer[i].r = r;
      screen_buffer[i].g = g;
      screen_buffer[i].b = b;
      num_active_pixels++;
      Position patch;
      patch.x = x / PATCH_SIZE;
      patch.y = y / PATCH_SIZE;
      if(!active_patches[patch.x][patch.y]) {
        active_patches[patch.x][patch.y] = true;
        active_patches_list[num_active_patches] = patch;
        num_active_patches++;
      }
      if(num_active_pixels > NUM_PIXELS * PATCH_FACTOR ||
         num_active_patches > NUM_PATCHES * PATCH_FACTOR)
        full_refresh = true;
    }
  });
  return !full_refresh;
}

void error(std::string msg) {
  std::cout << msg << std::endl;
  exit(1);
}

std::string read_msg(int socket) {
  std::string result = "";
  bzero(msg_buffer, MSG_BUFFER_SIZE);
  int n = read(socket, msg_buffer, MSG_BUFFER_SIZE);
  n++;
  result += std::string(msg_buffer);
  result = prune_chars(result, "\r\n");
  return result;
}

bool write_msg(int socket, std::string msg) {
  if(debug_mode) std::cout << " >> " << msg << std::endl;
  std::string final_msg = msg + "\n";
  int n = write(socket, final_msg.c_str(), final_msg.size());
  if(n < 0) return false;
  return true;
}
