#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

int main() {
  int fd;
  struct termios config;

  // Abrir o dispositivo de porta serial
  fd = open("/dev/ttyS1", O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd < 0) {
    perror("Erro ao abrir a porta serial");
    return -1;
  }

  // Configurar a porta serial
  memset(&config, 0, sizeof(config));
  config.c_iflag = IGNPAR;
  config.c_oflag = 0;
  config.c_cflag = CS8 | CLOCAL | CREAD;
  config.c_lflag = 0;
  config.c_cc[VTIME] = 0;
  config.c_cc[VMIN] = 1;
  cfsetispeed(&config, B9600);
  cfsetospeed(&config, B9600);
  tcsetattr(fd, TCSANOW, &config);

  // Enviar dados pela porta serial
  char msg[] = "Hello, world!";
  write(fd, msg, sizeof(msg));

  // Fechar a porta serial
  close(fd);

  return 0;
}