#!/bin/bash
if [ ! -f src/CImg.h ]; then
    ./scripts/get_cimg.sh
fi
cd src || exit 1
g++ client.cpp -L/usr/X11R6/lib -lX11 -std=c++11 -O2 -Wall -g -o ../client || exit 1
