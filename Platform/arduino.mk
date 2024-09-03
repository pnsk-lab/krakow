# $Id$

CC = avr-gcc
CFLAGS = -std=c99 -DPLATFORM_ARDUINO -Os -DF_CPU=16000000UL -mmcu=atmega328p
LDFLAGS = -mmcu=atmega328p
LIBS =
EXEC = .elf
TARGET = krakow.hex
SECTION = .eeprom
