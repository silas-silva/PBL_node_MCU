#include <wiringSerial.h>
#include <stdio.h>

int main()
{
  int serial_port;
  
  if ((serial_port = serialOpen("/dev/ttyS0", 9600)) < 0)
  {
    fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));
    return 1;
  }

  serialFlush(serial_port);

  char* myString = "Hello, world!"; // string a ser enviada
  serialPuts(serial_port, myString); // envia a string pela porta serial

  serialClose(serial_port);
  return 0;
}