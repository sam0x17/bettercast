// BetterCast Server
// Copyright 2016 Sam Kelly
// run on BetterCast device / ODROID XU4

#include <SDL2/SDL.h>
#include "bettercast.h"

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Surface *surface;

void close() {
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void init_sdl() {
  std::cout << "initializing SDL2..." << std::endl;
  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cout << "could not initialize SDL: " << SDL_GetError() << std::endl;
    exit(1);
  }
  SDL_CreateWindowAndRenderer(screen_size.width, screen_size.height,
                              SDL_WINDOW_FULLSCREEN_DESKTOP, &window, &renderer);
  if(window == NULL) {
    std::cout << "could not create window: " << SDL_GetError() << std::endl;
    exit(1);
  }
  SDL_SetWindowTitle(window, "BetterCast");
  surface = SDL_GetWindowSurface(window);
  std::cout << "SDL2 successfully initialized." << std::endl;
}

int main() {
  screen_size = get_screen_size();
  init_sdl();
  SDL_UpdateWindowSurface(window);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
  SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
  SDL_RenderClear(renderer);
  bool col = true;
  for(int i = 0; i < 100; i++) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    if(col) {
      SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
      col = false;
    } else {
      SDL_SetRenderDrawColor(renderer, 230, 0, 0, 255);
      col = true;
    }
    for(int y = 0; y < screen_size.height; y++) {
      for(int x = 0; x < screen_size.height; x++) {
        SDL_RenderDrawPoint(renderer, x, y);
      }
    };
    SDL_RenderPresent(renderer);
  }
  SDL_Delay(1000);
  close();
}

/*int main(void) {
   Display *d;
   Window w;
   XEvent e;
   char *msg = "Hello, World!";
   int s;

   d = XOpenDisplay(NULL);
   if (d == NULL) {
      fprintf(stderr, "Cannot open display\n");
      exit(1);
   }

   s = DefaultScreen(d);
   w = XCreateSimpleWindow(d, RootWindow(d, s), 10, 10, 100, 100, 1,
                           BlackPixel(d, s), WhitePixel(d, s));
   XSelectInput(d, w, ExposureMask | KeyPressMask);
   XMapWindow(d, w);

   while (1) {
      XNextEvent(d, &e);
      if (e.type == Expose) {
         XFillRectangle(d, w, DefaultGC(d, s), 20, 20, 10, 10);
         XDrawString(d, w, DefaultGC(d, s), 10, 50, msg, strlen(msg));
      }
      if (e.type == KeyPress)
         break;
   }

   XCloseDisplay(d);
   return 0;
}*/

/*
int main () {
  Display* dis = XOpenDisplay(NULL);
  int screen = DefaultScreen(dis);
  Window win = XCreateSimpleWindow(dis, RootWindow(dis, 0), 0, 0, 10, 10, 0, BlackPixel (dis, 0), BlackPixel(dis, 0));

  Atom wm_state = XInternAtom(dis, "_NET_WM_STATE", False);
  Atom fullscreen = XInternAtom(dis, "_NET_WM_STATE_FULLSCREEN", False);

  XEvent xev;
  memset(&xev, 0, sizeof(xev));
  xev.type = ClientMessage;
  xev.xclient.window = win;
  xev.xclient.message_type = wm_state;
  xev.xclient.format = 32;
  xev.xclient.data.l[0] = 1;
  xev.xclient.data.l[1] = fullscreen;
  xev.xclient.data.l[2] = 0;

  XMapWindow(dis, win);

  XSendEvent (dis, DefaultRootWindow(dis), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);

  XFlush(dis);

  //unsigned int white = WhitePixel(dis, screen);
  //unsigned int black = BlackPixel(dis, screen);
  //GC gc;
  ///gc = XCreateGC(dis, win, 0, NULL);
  //XSync(dis, False);
  //XClearWindow(dis, win);
  //XSetBackground(dis, gc, white);
  //XSetForeground(dis, gc, white);
  char msg[] = "Hello, World!";
  XFillRectangle(dis, win, DefaultGC(dis, screen), 20, 20, 10, 10);
  XDrawString(dis, win, DefaultGC(dis, screen), 10, 50, msg, strlen(msg));
  XFlush(dis);


  sleep(5);
  return(0);
}*/
