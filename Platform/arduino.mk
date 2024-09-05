# $Id$

CC = avr-gcc
LD = avr-gcc
CFLAGS = -std=c99 -DPLATFORM_ARDUINO -Os -DF_CPU=16000000UL -mmcu=atmega328p -c
LDFLAGS = -mmcu=atmega328p -o
LIBS =
EXEC = .elf
OBJ = .o
TARGET = krakow.hex
SECTION = .eeprom
