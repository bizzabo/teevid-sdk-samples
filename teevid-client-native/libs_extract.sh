#!/bin/sh

userid=$(id -u)

if [ $userid -ne 0 ]; then
	rm -rf $(pwd)/libs
    unzip $(pwd)/libs.zip
else
	echo "ERROR: You need to run this script without sudo permissions!"
fi

