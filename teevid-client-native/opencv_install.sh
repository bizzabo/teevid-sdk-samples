#!/bin/sh

ROOT_DIR=$(pwd)
ARCHITECTURE=$(uname -m)
INSTALL_PREFIX=/usr
INSTALL_LIB_DIR=/usr/lib/$ARCHITECTURE-linux-gnu
if [ "$ARCHITECTURE" = "aarch64" ]; then
  #run the fan
  sudo jetson_clocks
fi

echo "*** Preparing directory for OpenCV..."
mkdir -p ~/opencv_build && cd ~/opencv_build
sudo rm -rf opencv
sudo rm -rf opencv_contrib

echo "*** Cloning OpenCV from repo..."
git clone https://github.com/opencv/opencv.git
git clone https://github.com/opencv/opencv_contrib.git

cd ~/opencv_build/opencv_contrib
git checkout 4.1.1

cd ~/opencv_build/opencv
git checkout 4.1.1
mkdir build && cd build

echo "*** Building OpenCV..."
cmake -D CMAKE_BUILD_TYPE=RELEASE \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    -D INSTALL_C_EXAMPLES=ON \
    -D INSTALL_PYTHON_EXAMPLES=ON \
	-D ENABLE_PRECOMPILED_HEADERS=OFF \
    -D OPENCV_GENERATE_PKGCONFIG=ON \
    -D OPENCV_EXTRA_MODULES_PATH=~/opencv_build/opencv_contrib/modules \
    -D WITH_CUDA=ON \
    -D CUDA_ARCH_BIN=${ARCH_BIN} \
    -D CUDA_ARCH_PTX="" \
    -D ENABLE_FAST_MATH=ON \
    -D CUDA_FAST_MATH=ON \
    -D WITH_CUBLAS=ON \
    -D WITH_LIBV4L=ON \
    -D WITH_V4L=ON \
    -D WITH_GSTREAMER=ON \
    -D WITH_GSTREAMER_0_10=OFF \
    -D OPENCV_DNN_CUDA=ON \
    -D WITH_QT=ON \
    -D WITH_TBB=ON \
    -D BUILD_opencv_python2=ON \
    -D BUILD_opencv_python3=ON \
    -D BUILD_TESTS=OFF \
    -D BUILD_PERF_TESTS=OFF ..

make -j4

echo "*** Installing OpenCV..."
sudo make install

sudo cp /usr/local/lib/libopencv_core.so.4.1 $INSTALL_LIB_DIR
sudo cp /usr/local/lib/libopencv_core.so.4.1.1 $INSTALL_LIB_DIR
sudo cp /usr/local/lib/libopencv_imgproc.so.4.1 $INSTALL_LIB_DIR
sudo cp /usr/local/lib/libopencv_imgproc.so.4.1.1 $INSTALL_LIB_DIR
sudo cp /usr/local/lib/libopencv_imgcodecs.so.4.1 $INSTALL_LIB_DIR
sudo cp /usr/local/lib/libopencv_imgcodecs.so.4.1.1 $INSTALL_LIB_DIR
sudo cp /usr/local/lib/libopencv_highgui.so.4.1 $INSTALL_LIB_DIR
sudo cp /usr/local/lib/libopencv_highgui.so.4.1.1 $INSTALL_LIB_DIR

