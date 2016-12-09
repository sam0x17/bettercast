#!/bin/bash
echo "downloading Mali OpenGL ES SDK for Linux..."
wget http://malideveloper.arm.com/downloads/SDK/LINUX/2.4.4/Mali_OpenGL_ES_SDK_v2.4.4.71fdbd_Linux_x64.tar.gz || exit 1
echo "extracting..."
tar -xzf Mali_OpenGL_ES_SDK_v2.4.4.71fdbd_Linux_x64.tar.gz || exit 1
rm Mali_OpenGL_ES_SDK_v2.4.4.71fdbd_Linux_x64.tar.gz || exit 1
echo "moving..."
mkdir -p ../srcbin || exit 1
rm -rf srcbin/* || exit 1
cp -r Mali_OpenGL_ES_SDK_v2.4.4/* ../srcbin/ || exit 1
rm -rf Mali_OpenGL_ES_SDK_v2.4.4 || exit 1
cd ../srcbin/ || exit 1
cmake . || exit 1
#-DTARGET=emulator || exit 1
cd samples || exit 1
cd opengles_20 || exit 1
cd triangle || exit 1
make -j4 || exit 1
./triangle
