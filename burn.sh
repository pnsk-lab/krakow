#!/bin/sh
make PLATFORM=arduino || exit 1
if [ "x$DEVICE" = "x" ]; then
        echo -n "Device? "
        read DEVICE
fi
avrdude -F -V -c arduino -p ATMEGA328P -P $DEVICE -b 115200 -U flash:w:BASIC/krakow.hex
