#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <limits.h>

#include <lcd.h>
#include <wiringPi.h>
#include <wiringSerial.h>

//USE WIRINGPI PIN NUMBERS
#define LCD_RS  13               //Register select pin
#define LCD_E   18               //Enable Pin
#define LCD_D4  21               //Data pin 4
#define LCD_D5  24               //Data pin 5
#define LCD_D6  26               //Data pin 6
#define LCD_D7  27               //Data pin 7
#define BUTTON_MAIS  25          // PA07
#define BUTTON_MENOS  23         // PA10
#define BUTTON_ENTER  19         // PA20

#define UART_3 "/dev/ttyS3"
#define BAUD_RATE 115200


int lcd;
int fd;

int qtdItensMenu01 = 35;
int qtdItensMenu02 = 10;


int unidadeSelecionada = 0;
int escolhaFazerNaNode = 0;

// Configurar UART
if ((fd = serialOpen (UART_3, BAUD_RATE)) < 0){
    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
    return 1 ;
}


void enviarDadoUART(int fd, int num) {
    serialPutchar(fd, num); //fd e dado
}


int receberDadoUART(int fd) {
    if (serialDataAvail(fd)){
      int data = serialGetchar(fd); // Leitura do byte da porta UART
      return data;
      //printf("Byte recebido: %02X\n", data);
    }else{
        return -1;
    }
}


int main() {
    while(1){
        enviarDadoUART(fd, 0xF1);
        receberDadoUART(fd);
    }
    return 0;
}


