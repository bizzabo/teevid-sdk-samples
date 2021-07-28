#!/bin/sh

ROOT_DIR=$(pwd)
ARCHITECTURE=$(uname -m)
INSTALL_PREFIX=/usr
INSTALL_LIB_DIR=/usr/lib/$ARCHITECTURE-linux-gnu
if [ "$ARCHITECTURE" = "aarch64" ]; then
  OPENCV_LIB_DIR=./3rd_party_libs/embedded/opencv
else
  OPENCV_LIB_DIR=./3rd_party_libs/desktop/opencv
fi

LIB_BACKUP_DIR=$OPENCV_LIB_DIR/backup
LIB_SOURCE_DIR=$OPENCV_LIB_DIR/src

mkdir -p $LIB_BACKUP_DIR

sudo cp $INSTALL_LIB_DIR/libopencv_core.so.4.1.1 $LIB_BACKUP_DIR
sudo cp $INSTALL_LIB_DIR/libopencv_imgproc.so.4.1.1 $LIB_BACKUP_DIR
sudo cp $INSTALL_LIB_DIR/libopencv_imgcodecs.so.4.1.1 $LIB_BACKUP_DIR
sudo cp $INSTALL_LIB_DIR/libopencv_highgui.so.4.1.1 $LIB_BACKUP_DIR

sudo cp $LIB_SOURCE_DIR/libopencv_core.so.4.1.1 $INSTALL_LIB_DIR
sudo cp $LIB_SOURCE_DIR/libopencv_imgproc.so.4.1.1 $INSTALL_LIB_DIR
sudo cp $LIB_SOURCE_DIR/libopencv_imgcodecs.so.4.1.1 $INSTALL_LIB_DIR
sudo cp $LIB_SOURCE_DIR/libopencv_highgui.so.4.1.1 $INSTALL_LIB_DIR

