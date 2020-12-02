#!/bin/sh

ROOT_DIR=$(pwd)
ARCHITECTURE=$(uname -m)
INSTALL_PREFIX=/usr
INSTALL_LIB_DIR=/usr/lib/$ARCHITECTURE-linux-gnu
if [ "$ARCHITECTURE" = "aarch64" ]; then
  #run the fan
  sudo jetson_clocks
fi

#gstreamer
sudo add-apt-repository universe
sudo add-apt-repository multiverse
sudo apt-get update
sudo apt-get install --yes gstreamer1.0-tools gstreamer1.0-alsa \
 gstreamer1.0-plugins-base gstreamer1.0-plugins-good \
 gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly \
 gstreamer1.0-libav
sudo apt-get install --yes libgstreamer1.0-dev \
 libgstreamer-plugins-base1.0-dev \
 libgstreamer-plugins-good1.0-dev \
 libgstreamer-plugins-bad1.0-dev
sudo apt install --yes libnice-dev gstreamer1.0-nice libssl-dev

install_libnice() {
  echo "Install libnice"
  #libnice
  sudo apt install --yes python3-pip
  pip3 install meson
  PATH=~/.local/bin:$PATH
  sudo apt-get install libgnutls28-dev --yes
  sudo apt-get install ninja-build --yes
  git clone https://github.com/TeeVid/libnice.git
  cd libnice
  meson --prefix=$INSTALL_PREFIX build && ninja -C build && sudo ninja -C build install
  cd $ROOT_DIR
  echo "libnice installed"
}

install_libnice

echo "Install custom GStreamer libraries"
if [ "$ARCHITECTURE" = "aarch64" ]; then
  CUSTOM_LIB_DIR=./3rd_party_libs/embedded
else
  CUSTOM_LIB_DIR=./3rd_party_libs/desktop
fi

echo "Install $CUSTOM_LIB_DIR/libgstrtp-1.0.so.0.1405.0"
# temporary fix for missed vp8/vp9 keyframe
sudo cp $CUSTOM_LIB_DIR/libgstrtp-1.0.so.0.1405.0 $INSTALL_LIB_DIR

echo "Install $CUSTOM_LIB_DIR/libgstwebrtc-1.0.so.0.1405.0"
sudo cp $CUSTOM_LIB_DIR/libgstwebrtc-1.0.so.0.1405.0 $INSTALL_LIB_DIR

echo "Install $CUSTOM_LIB_DIR/gstreamer-1.0/libgstrtpmanager.la"
sudo cp $CUSTOM_LIB_DIR/gstreamer-1.0/libgstrtpmanager.la $INSTALL_LIB_DIR/gstreamer-1.0/

echo "Install $CUSTOM_LIB_DIR/gstreamer-1.0/libgstrtpmanager.so"
sudo cp $CUSTOM_LIB_DIR/gstreamer-1.0/libgstrtpmanager.so $INSTALL_LIB_DIR/gstreamer-1.0/

echo "Install $CUSTOM_LIB_DIR/gstreamer-1.0/libgstwebrtc.la"
sudo cp $CUSTOM_LIB_DIR/gstreamer-1.0/libgstwebrtc.la $INSTALL_LIB_DIR/gstreamer-1.0/

echo "Install $CUSTOM_LIB_DIR/gstreamer-1.0/libgstwebrtc.so"
sudo cp $CUSTOM_LIB_DIR/gstreamer-1.0/libgstwebrtc.so $INSTALL_LIB_DIR/gstreamer-1.0/

#if [ "$ARCHITECTURE" = "aarch64" ]; then
#  echo "Install $CUSTOM_LIB_DIR/gstreamer-1.0/libgstnvjpeg.so"
#  sudo cp $CUSTOM_LIB_DIR/gstreamer-1.0/libgstjpeg.so $INSTALL_LIB_DIR/gstreamer-1.0/
#  sudo ln -sf $INSTALL_LIB_DIR/tegra/libnvjpeg.so $INSTALL_LIB_DIR/libjpeg.so
#  sudo ln -sf $INSTALL_LIB_DIR/tegra/libnvjpeg.so $INSTALL_LIB_DIR/libjpeg.so.8
#fi
