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


//int qtdItensMenu01 = 35;
//int qtdItensMenu02 = 10;


int unidadeSelecionada = 0;
int escolhaFazerNaNode = 0;


/* 
void imprimir_menu_lcd(char opcoes_menu[][30], int posicaoAtual) {
    // Limpa o display
    lcdClear(lcd);    
    // imprimindo as strings do vetor no LCD
    //lcdPosition(lcd, 0, 0);
    lcdPrintf(lcd, "%s\n", opcoes_menu[posicaoAtual]);
}



void mostrar_menu_01(int vetor_menu01[][30], int posicaoAtual){
    // atribuindo valores às strings do vetor
    imprimir_menu_lcd(vetor_menu01, posicaoAtual);
}



void mostrar_menu_02(int vetor_menu02[][30],int posicaoAtual){
    // atribuindo valores às strings do vetor
    imprimir_menu_lcd(vetor_menu02, posicaoAtual);
} */

// Envia a informacao com base no vetor e posicao passadas
void sendData(int fd, unsigned char* array, unsigned char pos) {
    serialPutchar(fd, array[pos]);
    //printf(" -> %3d", array[pos]);
    //fflush(stdout);
    //delay(2);
}

// Implementa o timeout
int recvDigitalData(int fd) {
    if (serialDataAvail(fd) > 0) return serialGetchar(fd);
    delay(8);
    if (serialDataAvail(fd) > 0) return serialGetchar(fd);
    return INT_MAX; // Nao existe nenhum dado a ser lido no buffer
}


unsigned short int bytes2short(unsigned char* analogbytes) {
    unsigned short int analogdata = 0;
    analogdata = (analogdata | analogbytes[1]) << 8;
    analogdata = (analogdata | analogbytes[0]);
    return analogdata;
}

// Move os dados do buffer para o vetor de
// acordo com as regras definidas no protocolo
unsigned char getAnalogData(int fd, unsigned char* analogBytes) {
    if (serialDataAvail(fd) > 0) {
        analogBytes[0] = serialGetchar(fd);
        delay(3);
        analogBytes[1] = serialGetchar(fd);
        return 1;
    }
    return 0;
}

int recvAnalogData(int fd, unsigned char* analogBytes) {
    // Tenta ler o buffer
    if(getAnalogData(fd, analogBytes)) return bytes2short(analogBytes);
    // Tempo de timeout
    delay(8);
    // Tenta ler o buffer
    if (getAnalogData(fd, analogBytes)) return bytes2short(analogBytes);
    return INT_MAX; // Nao existe nenhum dado a ser lido no buffer
}


int recvdData(int fd, unsigned char* analogBytes, int pos) {
    // Sensor Analogico
    if (pos == 1) return recvAnalogData(fd, analogBytes);
    // Sensor digital
    return recvDigitalData(fd);
}

void refreshCounter(unsigned short int* pos, unsigned short int size) {
    if (*pos == -1) *pos = size - 1;
    else if (*pos > size) *pos = 0;
}

int main() {
    //int qtdItensMenu01 = 35;
    //int qtdItensMenu02 = 10;
    // Guarda o indice da node selecionada
    unsigned char selectedNode;
    // Guarda os bytes provenientes do sensor analogico da Node
    unsigned char analogBytes[2];
    // Comandos de consulta
    unsigned char followCommands[] = { 0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6 };
    // Comandos de monitoramento
    unsigned char ConsultCommands[] = { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6 };
    // Codigo de cada node possivel de conectar
    unsigned char select_node[] = {
                                        0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
                                        0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0x10
                                 };
    unsigned char deselect_node[] = {   
                                        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                                        0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F
                                    };
    // Menus
    char vetor_menu01[35][30] = {
                                                {"SELECIONAR NODE 0"},
                                                {"SELECIONAR NODE 1"},
                                                {"SELECIONAR NODE 2"},
                                                {"SELECIONAR NODE 3"},
                                                {"SELECIONAR NODE 4"},
                                                {"SELECIONAR NODE 5"},
                                                {"SELECIONAR NODE 6"},
                                                {"SELECIONAR NODE 7"},
                                                {"SELECIONAR NODE 8"},
                                                {"SELECIONAR NODE 9"},
                                                {"SELECIONAR NODE 10"},
                                                {"SELECIONAR NODE 11"},
                                                {"SELECIONAR NODE 12"},
                                                {"SELECIONAR NODE 13"},
                                                {"SELECIONAR NODE 14"},
                                                {"SELECIONAR NODE 15"},
                                                {"SELECIONAR NODE 16"},
                                                {"SELECIONAR NODE 17"},
                                                {"SELECIONAR NODE 18"},
                                                {"SELECIONAR NODE 19"},
                                                {"SELECIONAR NODE 20"},
                                                {"SELECIONAR NODE 21"},
                                                {"SELECIONAR NODE 22"},
                                                {"SELECIONAR NODE 23"},
                                                {"SELECIONAR NODE 24"},
                                                {"SELECIONAR NODE 25"},
                                                {"SELECIONAR NODE 26"},
                                                {"SELECIONAR NODE 27"},
                                                {"SELECIONAR NODE 28"},
                                                {"SELECIONAR NODE 29"},
                                                {"SELECIONAR NODE 30"},
                                                {"SELECIONAR NODE 31"},
                                                {"SAIR"}
                                            };
    char vetor_menu02[9][30] = {
                                                {"ACENDER LED"},
                                                {"SENSOR ANALOGICO A0"},
                                                {"MONITORAR SENSOR A0"},
                                                {"SENSOR DIGITAL D0"},
                                                {"MONITORAR DIGITAL D0"},
                                                {"SENSOR DIGITAL D1"},
                                                {"MONITORAR DIGITAL D1"},
                                                {"VOLTAR"}
                                            };
                                            
    // Inicializar biblioteca
    if (wiringPiSetup () == -1)
    {
        fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
        return 1;
    }

    // Setar botões como entrada
    pinMode(BUTTON_MAIS, INPUT);
    pinMode(BUTTON_MENOS, INPUT);
    pinMode(BUTTON_ENTER, INPUT);
    
    int buttonMaisState;
    int buttonMenosState;
    int buttonEnterState;

    // Configurar UART
    if ((fd = serialOpen (UART_3, BAUD_RATE)) < 0){
        fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
        return 1 ;
    }

    // Iniciar LCD
    if ((lcd = lcdInit (2, 16, 4, LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7, 0, 0, 0, 0) > 0))
    {
        printf("Erro ao inicializar o LCD");
        return 1;
    }

    lcdClear(lcd);
    //lcdPuts(lcd, "BEM-VINDO");
    
    
    // Menu ativo no momento
    int menu01 = 1;
    int menu02 = 0;

    unsigned char msg;

    //
    int escolhaMenu01;
    int escolhaMenu02;

    // Guarda os dados provenientes da Node
    int recvData = 0;

    // Variavel para controlar o que vai ser mostrado no Menu
    unsigned short int posicao = 0;
    
    // Mostrar a primeira opção do primeiro Menu
    //delay(2000);
    //mostrar_menu_01(vetor_menu01, posicao);
    lcdPrintf(lcd, "%s", vetor_menu01[posicao]);

    while(1){
        // Verificar se algum botão foi pressionado
        buttonMaisState = digitalRead(BUTTON_MAIS);
        buttonMenosState = digitalRead(BUTTON_MENOS);
        buttonEnterState = digitalRead(BUTTON_ENTER);
        
        // Entender esse Dalay, se precisa ou não
        delay(200);
        
        if (!buttonMaisState) {
            // O botão foi pressionado
            posicao++;
            
            // Verificar se o que vai ser mostrado é o menu 1 ou 2
            if (menu01 == 1){
                // Se posição passar da quantidade de itens do menu, levar para a posição zero do menu
/*                 if (posicao == qtdItensMenu01){
                    posicao = 0;
                } */
                refreshCounter(&posicao, 33);
                //mostrar_menu_01(vetor_menu01, posicao);
                lcdClear(lcd);
                lcdPrintf(lcd, "%s", vetor_menu01[posicao]);
            
            }else if (menu02 == 1){
                // Se posição passar da quantidade de itens do menu, levar para a posição zero do menu
/*                 if (posicao == qtdItensMenu02){
                    posicao = 0;
                } */
                refreshCounter(&posicao, 8);
                //mostrar_menu_02(vetor_menu02, posicao);
                lcdClear(lcd);
                lcdPrintf(lcd, "%s", vetor_menu02[posicao]);
            }
        
        }
        else if (!buttonMenosState){
            // O botão foi pressionado
            posicao--;

            // Verificar se o que vai ser mostrado é o menu 1 ou 2
            if (menu01 == 1){
                // Se posição for menor que a quantidade de itens do menu, levar para a posição 32 do menu
/*                 if (posicao == -1){
                    posicao = qtdItensMenu01 - 1;
                } */
                refreshCounter(&posicao, 33);
                //mostrar_menu_01(vetor_menu01, posicao);
                lcdClear(lcd);
                lcdPrintf(lcd, "%s", vetor_menu01[posicao]);
            
            }else if (menu02 == 1){
                // Se posição for menor que a quantidade de itens do menu, levar para a posição 6 do menu
/*                 if (posicao == -1){
                    posicao = qtdItensMenu02 - 1;
                } */
                refreshCounter(&posicao, 8);
                //mostrar_menu_02(vetor_menu02, posicao);
                lcdClear(lcd);
                lcdPrintf(lcd, "%s", vetor_menu02[posicao]);
            }
        
        }
        else if (!buttonEnterState){
            // O botão foi pressionado
            if (menu01 == 1){
                // Verificar se apertou enter na posição Sair
                escolhaMenu01 = posicao;
                if (escolhaMenu01 == 32){
                    lcdClear(lcd);
                    lcdPuts(lcd, "TCHAU...");
                    delay(3000);
                    break;
                }else{
                    // Seleciona a Node com o ID escolhido
                    sendData(fd, select_node, posicao);
                    recvData = recvDigitalData(fd);
                    lcdClear(lcd);
                    if(recvData == select_node[posicao]) {
                        lcdPuts(lcd, "NODE SELECIONADA COM SUCESSO!");
                        delay(3000);
                    }
                    // Carro ocorra a node nao responda
                    else {
                        lcdPuts(lcd, "ERRO - NODE INALCANCAVEL");
                        delay(3000);
                        continue;
                    }
                    // Salva a posicao da node selecionada no vetor de ID das Nodes
                    selectedNode = posicao;
                    //serialPutchar(fd, select_node[posicao - 1]);
                    // Desabilita menu 1
                    menu01 = 0;
                    // Habilita menu 2
                    menu02 = 1;
                    // Reseta o contador de posicoes
                    posicao = 0;
                    //mostrar_menu_02(vetor_menu02, posicao);
                    lcdClear(lcd);
                    lcdPrintf(lcd, "%s", vetor_menu02[posicao]);
                }

            }else if (menu02 == 1){
                escolhaMenu02 = posicao;
                // Verificar se apertou enter na posição Voltar
                if (escolhaMenu02 == 7){
                    // Desseleciona a Node previamente selecionada
                    sendData(fd, deselect_node, selectedNode);
                    //serialPutchar(fd, deselect_node[posicao - 1]);
                    lcdClear(lcd);
                    lcdPuts(lcd, "NODE DESSELECIONADA COM SUCESSO!");
                    delay(3000);
                    menu01 = 1;
                    menu02 = 0;
                    posicao = escolhaMenu01;
                    //mostrar_menu_01(vetor_menu01, posicao);
                    lcdClear(lcd);
                    lcdPrintf(lcd, "%s", vetor_menu01[posicao]);
                }else{
                    // Mandar mensagem para a node e pegar o dado para exibir no LCD
                    lcdPuts(lcd, "ENVIANDO COMANDO...");
                    // Enviando comando a Node selecionada
                    // Logica para implementar a consulta ou monitoramento
                    
                    // Acender o LED
                    if(posicao == 0) {
                        msg = ConsultCommands[posicao];
                        serialPutchar(fd, 0xC0);
                        continue;
                    }
                    else {
                        // Se posicao impar (consultar)
                        if(posicao%2 == 1) {
                            msg = ConsultCommands[posicao];
                            serialPutchar(fd, msg);
                        }
                        else {
                            msg = followCommands[posicao];
                            serialPutchar(fd, msg);
                        }
                    }
                    delay(1000);
                    lcdClear(lcd);
                    lcdPuts(lcd, "COMANDO ENVIADO!");
                    delay(2000);
                    recvData = recvdData(fd, analogBytes, posicao);
                    printf("RECVDATA -> %d\n", recvData);
                    lcdClear(lcd);
                    if (recvData < INT_MAX) {
                        lcdPrintf(lcd, "DADO DO SENSOR -> %d", recvData);
                    }
                    else {
                        lcdPuts(lcd, "NODE PAROU DE RESPONDER");
                    }
                    delay(3000);   
                    //serialPutchar(fd, array_command[menu_counter]);
                    // Mostrar dado que foi recebido
                    // Mostrar Menu 02
                    //mostrar_menu_02(vetor_menu02, posicao);
                    lcdClear(lcd);
                    lcdPrintf(lcd, "%s", vetor_menu02[posicao]);
                }
            }
        }
    }
    return EXIT_SUCCESS;
}
