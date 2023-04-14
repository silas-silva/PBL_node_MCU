CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -O2
LIBS = -lwiringPi -lwiringSerial

uart_orange: uart_orange.c
	$(CC) $(CFLAGS) -o uart_orange uart_orange.c $(LIBS)

clean:
	rm -f uart_orange


uart_orange_2: uart_orange_2.c
	$(CC) $(CFLAGS) -o uart_orange_2 uart_orange_2.c $(LIBS)
