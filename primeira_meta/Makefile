ifneq ($V,1)
Q ?= @
endif

#DEBUG	= -g -O0
DEBUG	= -O3
CC	= gcc
INCLUDE	= -I/usr/local/include
CFLAGS	= $(DEBUG) -Wall $(INCLUDE) -Winline -pipe

LDFLAGS	= -L/usr/local/lib
LDLIBS    = -lwiringPi -lwiringPiDev -lpthread -lm -lcrypt -lrt

SRC	=	led.c init_lcd.c

OBJ	=	$(SRC:.c=.o)

BINS	=	$(SRC:.c=)

rm:
	rm led init_lcd

led:	led.o
		$Q $(CC) -o $@ led.o $(LDFLAGS) $(LDLIBS)
		sudo ./led
	
init_lcd:	init_lcd.o
		$Q $(CC) -o $@ init_lcd.o $(LDFLAGS) $(LDLIBS)
		sudo ./init_lcd