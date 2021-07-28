#!/bin/sh

ROOT_DIR=$(pwd)
ARCHITECTURE=$(uname -m)
INSTALL_PREFIX=/usr
INSTALL_LIB_DIR=/usr/lib/$ARCHITECTURE-linux-gnu
if [ "$ARCHITECTURE" = "aarch64" ]; then
  LIB_BACKUP_DIR=./3rd_party_libs/embedded/opencv/backup
else
  LIB_BACKUP_DIR=./3rd_party_libs/desktop/opencv/backup
fi

sudo cp $LIB_BACKUP_DIR/libopencv_core.so.4.1.1 $INSTALL_LIB_DIR
sudo cp $LIB_BACKUP_DIR/libopencv_imgproc.so.4.1.1 $INSTALL_LIB_DIR
sudo cp $LIB_BACKUP_DIR/libopencv_imgcodecs.so.4.1.1 $INSTALL_LIB_DIR
sudo cp $LIB_BACKUP_DIR/libopencv_highgui.so.4.1.1 $INSTALL_LIB_DIR

