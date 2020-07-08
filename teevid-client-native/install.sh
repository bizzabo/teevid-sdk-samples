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
sudo apt-get install --yes build-essential dpkg-dev flex bison autotools-dev automake liborc-dev autopoint libtool gtk-doc-tools libgstreamer1.0-dev
sudo apt install --yes libnice-dev gstreamer1.0-nice libssl-dev

#gstreamer plugins-base
sudo apt-get install --yes libxv-dev libasound2-dev libtheora-dev libogg-dev libvorbis-dev

#gstreamer plugins-good
sudo apt-get install --yes libbz2-dev libv4l-dev libvpx-dev libjack-jackd2-dev libsoup2.4-dev libpulse-dev

#gstreamer plugins-bad
sudo apt-get install --yes faad libfaad-dev libfaac-dev

#gstreamer plugins-ugly
sudo apt-get install --yes libx264-dev libmad0-dev

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

if [  "$ARCHITECTURE" = "aarch64" ]; then
  echo "Install GStreamer with 'gst-install'"
  sudo gst-install --prefix=$INSTALL_PREFIX --version=1.16.2
# temporary fix for missed vp8 keyframe
  sudo cp ./3rd_party_libs/embedded/libgstrtp.so $INSTALL_LIB_DIR/gstreamer-1.0
else
  echo "Install GStreamer manually"
  ROOT_GST_DIR=$ROOT_DIR/gst-1.16
  mkdir $ROOT_GST_DIR
  cd $ROOT_GST_DIR

  git clone https://github.com/GStreamer/gstreamer.git
  cd gstreamer
  git checkout 1.16
  git submodule init
  git submodule update
  libtoolize
  ./autogen.sh --prefix=$INSTALL_PREFIX --libdir=$INSTALL_LIB_DIR
  ./autogen.sh --prefix=$INSTALL_PREFIX --libdir=$INSTALL_LIB_DIR
  make
  sudo make install
  export LD_LIBRARY_PATH=$INSTALL_LIB_DIR
  export PKG_CONFIG_PATH=$INSTALL_LIB_DIR/pkgconfig:$PKG_CONFIG_PATH
  cd $ROOT_GST_DIR

  git clone https://github.com/GStreamer/gst-plugins-base.git
  cd gst-plugins-base
  git checkout 1.16
  git submodule init
  git submodule update
  libtoolize
  ./autogen.sh --prefix=$INSTALL_PREFIX --libdir=$INSTALL_LIB_DIR
  ./autogen.sh --prefix=$INSTALL_PREFIX --libdir=$INSTALL_LIB_DIR
  make
  sudo make install
  cd $ROOT_GST_DIR

  git clone https://github.com/GStreamer/gst-plugins-good.git
  cd gst-plugins-good
  git checkout 1.16
  git submodule init
  git submodule update
  libtoolize
  ./autogen.sh --prefix=$INSTALL_PREFIX --libdir=$INSTALL_LIB_DIR
  ./autogen.sh --prefix=$INSTALL_PREFIX --libdir=$INSTALL_LIB_DIR
  make
  sudo make install
  cd ..

  git clone https://github.com/GStreamer/gst-plugins-bad.git
  cd gst-plugins-bad
  git checkout 1.16
  git submodule init
  git submodule update
  libtoolize
  ./autogen.sh --prefix=$INSTALL_PREFIX --libdir=$INSTALL_LIB_DIR
  ./autogen.sh --prefix=$INSTALL_PREFIX --libdir=$INSTALL_LIB_DIR
  make
  sudo make install
  cd ..

  git clone https://github.com/GStreamer/gst-plugins-ugly.git
  cd gst-plugins-ugly
  git checkout 1.16
  git submodule init
  git submodule update
  libtoolize
  ./autogen.sh --prefix=$INSTALL_PREFIX --libdir=$INSTALL_LIB_DIR
  ./autogen.sh --prefix=$INSTALL_PREFIX --libdir=$INSTALL_LIB_DIR
  make
  sudo make install
  cd ..

  cd $ROOT_DIR
fi

#install libnice one more time to be sure we use exactly this version
cd libnice
sudo ninja -C build install
cd ..

