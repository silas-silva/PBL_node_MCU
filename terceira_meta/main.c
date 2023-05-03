#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>



int open_serial_port(char *port_name) {
    int fd;
    struct termios config;

    // Abrir o dispositivo de porta serial
    fd = open(port_name, O_RDWR | O_NOCTTY | O_NDELAY);

    if (fd < 0) {
        perror("Erro ao abrir a porta serial");
        return -1;
    }

    // Configurar a porta serial
    tcgetattr(fd, &config);
    config.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    config.c_iflag = IGNPAR;
    config.c_oflag = 0;
    config.c_lflag = 0;
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &config);

    return fd;
}



void send_hex_data(int fd, unsigned char data) {
    unsigned char hex_data[] = {data};
    write(fd, hex_data, sizeof(hex_data));
}



unsigned char read_serial_data(int fd) {
    unsigned char response;
    int num_bytes = read(fd, &response, 1);

    if (num_bytes < 0) {
        perror("Erro ao ler a porta serial");
        return 0;
    }

    return response;
}



int main() {
    int fd;
    unsigned char hex_data = 0x0;
    unsigned char response;

    fd = open_serial_port("/dev/ttyS3");

    if (fd < 0) {
        printf("Erro no acesso à UART\n");
        return -1;
    }

    send_hex_data(fd, hex_data);
    
    // response = read_serial_data(fd);

    // printf("Resposta recebida: 0x%X\n", response);

    close(fd);

    return 0;
}
