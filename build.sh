#!/bin/bash
if [ ! -f src/CImg.h ]; then
    ./scripts/get_cimg.sh
fi
cd src || exit 1
#g++ client.cpp -L/usr/X11R6/lib -lm -lpthread -lX11 -std=c++11 -O3 -o ../client || exit 1
g++ server.cpp -lm -w -I$HOME/SDL/include -L$HOME/SDL/lib -lSDL2 -O3 -std=c++11 -o ../server || exit 1
