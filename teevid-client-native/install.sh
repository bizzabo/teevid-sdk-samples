#!/bin/sh

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

sudo apt install --yes python3-pip
pip3 install meson
PATH=~/.local/bin:$PATH
sudo apt-get install libgnutls28-dev --yes
sudo apt-get install ninja-build --yes
git clone https://github.com/TeeVid/libnice.git
cd libnice
meson --prefix="/usr" build && ninja -C build && sudo ninja -C build install
cd ..

sudo chmod +x build-debug.sh
sudo chmod +x build-release.sh
