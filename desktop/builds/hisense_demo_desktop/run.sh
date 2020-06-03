#!/bin/sh
CURRENT_DIR=$(pwd)
case ":$LD_LIBRARY_PATH:" in
  *":$CURRENT_DIR:"*) :;;
  *) LD_LIBRARY_PATH="$CURRENT_DIR:$LD_LIBRARY_PATH";;
esac
export LD_LIBRARY_PATH
./hisense_demo
