#include <X11/Xlib.h>
#include <X11/X.h>
#include <iostream>
#include <functional>
#include "CImg.h"

using namespace cimg_library;

const int MAX_SCREEN_SIZE = 64000000; // larger than largest possible consumer screen resolution

class Size {
public:
  int width;
  int height;
};

long screen_buffer[MAX_SCREEN_SIZE];
Size screen_size;
int screen_buffer_size;
Display *display = XOpenDisplay(NULL);
Window root = DefaultRootWindow(display);
XWindowAttributes gwa;

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

int main() {
  init_screen_buffer();
  for(int i = 0; i < 30; i++)
  foreach_screen_pixel([&](unsigned long raw) {

  });
}
