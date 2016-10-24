#include <X11/X.h>
#include <X11/Xlib.h>
#include <strings.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main ()
{
  Display* dis = XOpenDisplay(NULL);
  Window win = XCreateSimpleWindow(dis, RootWindow(dis, 0), 0, 0, 10, 10,
                                   0, BlackPixel (dis, 0), BlackPixel(dis, 0));

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

  XSendEvent (dis, DefaultRootWindow(dis), False,
                  SubstructureRedirectMask | SubstructureNotifyMask, &xev);

  XFlush(dis);
  /*Sleep 5 seconds before closing.*/
  sleep(5);
  return(0);
}
