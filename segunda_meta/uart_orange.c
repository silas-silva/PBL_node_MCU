#include <wiringSerial.h>

int main ()
{
  int serial_port;

  if ((serial_port = serialOpen ("/dev/ttyS0", 9600)) < 0)
  {
    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno));
    return 1;
  }

  // Configuração da porta UART
  serialFlush(serial_port);
  serialPutchar(serial_port, 'H');
  serialPutchar(serial_port, 'e');
  serialPutchar(serial_port, 'l');
  serialPutchar(serial_port, 'l');
  serialPutchar(serial_port, 'o');

  serialClose(serial_port);
  return 0;
}