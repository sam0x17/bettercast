#!/bin/bash
echo "downloading SDL2-2.0.5..."
wget https://www.libsdl.org/release/SDL2-2.0.5.tar.gz || exit 1
echo "extracting..."
tar -xzvf SDL2*.tar.gz || exit 1
rm SDL2*.tar.gz || exit 1
cd SDL* || exit 1
./configure || exit 1
make -j4 || exit 1
sudo make install || exit 1
cd .. || exit 1
rm SDL* -rf || exit 1
echo "done."
