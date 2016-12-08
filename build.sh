#!/bin/bash
if [ ! -f CImg.h ]; then
    ./scripts/get_cimg.sh
fi
g++ bettercast_client.cpp -L/usr/X11R6/lib -lm -lpthread -lX11 -std=c++11 -O3 -o client || exit 1
