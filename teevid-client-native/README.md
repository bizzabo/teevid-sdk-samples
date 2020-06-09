# How to build and run demo UI application

#### 1. Install Qt framework and IDE (Qt Creator):	
```
sudo apt install qt5-default
sudo apt install qtcreator
```

#### 2. Install Gstreamer (otherwise the demo app will work but not dispay any streams)
```
sudo add-apt-repository universe
sudo add-apt-repository multiverse
sudo apt-get update
sudo apt-get install gstreamer1.0-tools gstreamer1.0-alsa \
  gstreamer1.0-plugins-base gstreamer1.0-plugins-good \
  gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly \
  gstreamer1.0-libav
sudo apt-get install libgstreamer1.0-dev \
  libgstreamer-plugins-base1.0-dev \
  libgstreamer-plugins-good1.0-dev \
  libgstreamer-plugins-bad1.0-dev
```

#### 3. Install dependencies (same reason as for previous item)
 ```
 sudo apt install \
gstreamer1.0-x \
libgstreamer1.0-dev \
libgstreamer-plugins-base1.0-dev \
gstreamer1.0-plugins-bad \
libgstreamer-plugins-bad1.0-dev \
libsoup2.4-dev \
libjson-glib-dev \
libnice-dev \
autoconf \
libtool \
gtk-doc-tools \
libglib2.0-dev \
gstreamer1.0-nice \
libssl-dev \
libreadline-dev \
gstreamer1.0-plugins-ugly \
libsrtp0-dev \
libsrtp2-dev
```

#### 4. In order to build demo UI app do the following:
	1.	Navigate to the directory where yoour sources are stored
		cd <your_source_dir>

	2. 	Download sources:
		git clone https://github.com/TeeVid/sdk_samples.git

	3.	Launch Qt creator
	4.	Choose "Projects" -> "Open project" -> <your_source_dir>/sdk_samples/desktop/teevid-client-native/teevid-client-native.pro
	5.	Select configuration (Debug or Release)
	6.	In menu bar select:
		*	"Project" -> "Clean all"
		*	"Project" -> "Run qmake"
		*	"Project" -> "Build all" or "Rebuild all"
