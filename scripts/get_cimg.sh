#!/bin/bash
echo "downloading latest version of the CImg Library..."
wget http://cimg.eu/files/CImg_latest.zip || exit 1
echo "extracting..."
unzip CImg_latest.zip || exit 1
cp CImg*/CImg.h . || exit 1
rm CImg_latest.zip || exit 1
rm -rf CImg-* || exit 1
echo "done"
