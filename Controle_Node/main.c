#include <stdio.h>
#include <wiringPi.h>
#include <lcd.h>



//USE WIRINGPI PIN NUMBERS
#define LCD_RS  13               //Register select pin
#define LCD_E   18               //Enable Pin
#define LCD_D4  21               //Data pin 4
#define LCD_D5  24               //Data pin 5
#define LCD_D6  26               //Data pin 6
#define LCD_D7  27               //Data pin 7
#define BUTTON_MAIS  23          // PA10
#define BUTTON_MENOS  25         // PA20
#define BUTTON_ENTER  19         // PA07



int lcd = 0;

int qtdItensMenu01 = 33;
char vetor_menu01[qtdItensMenu01][30];

int qtdItensMenu02 = 32;
char vetor_menu02[qtdItensMenu02][30];

int unidadeSelecionada = 0;
int escolhaFazerNaNode = 0;



void imprimir_menu_lcd(char opcoes_menu[][30], int tamanho, int posicaoAtual) {
    // Limpa o display
    lcdClear(lcd);    
    // imprimindo as strings do vetor no LCD
    lcdPosition(lcd, 0, 0);
    lcdPuts(lcd, ("%s\n", opcoes_menu[posicaoAtual]));
}



void criar_menu_01(){
    // atribuindo valores às strings do vetor
    for (int i = 0; i < 32; i++) {
        sprintf(vetor_menu01[i], "Selecionar Unidade %d", i + 1);
    }
}



void criar_menu_02(){
    // atribuindo valores às strings do vetor
    for (int i = 0; i < 32; i++) {
        sprintf(vetor_menu01[i], "Acender Led %d", i + 1);
    }
}



void mostrar_menu_01(int posicaoAtual){
    // atribuindo valores às strings do vetor
    imprimir_menu_lcd(vetor_menu01, qtdItensMenu01, posicaoAtual);
}



void mostrar_menu_02(int posicaoAtual){
    // atribuindo valores às strings do vetor
    imprimir_menu_lcd(vetor_menu02, qtdItensMenu02, posicaoAtual);
}



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
    // Inicializar biblioteca
    wiringPiSetup();

    // Setar botões como entrada
    pinMode(BUTTON_MAIS, INPUT);
    pinMode(BUTTON_MENOS, INPUT);
    pinMode(BUTTON_ENTER, INPUT);
    
    int buttonMaisState;
    int buttonMenosState;
    int buttonEnterState;

    // Dados de controle da porta UART
    int fd;
    unsigned char hex_data = 0x0;
    unsigned char response;
    
    fd = open_serial_port("/dev/ttyS3");

    if (fd < 0) {
        printf("Erro no acesso à UART\n");
        return -1;
    }

    // Iniciar LCD
    lcd = lcdInit (2, 16, 4, LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7, 0, 0, 0, 0);

    // Menu ativo no momento
    int menu01 = 1;
    int menu02 = 0;

    // Variavel para controlar o que vai ser mostrado no Menu
    int posicao = 0;
    
    // Mostrar a primeira opção do primeiro Menu
    mostrar_menu_01(posicao)
    
    while(1){
        // Verificar se algum botão foi pressionado
        buttonMaisState = digitalRead(BUTTON_MAIS);
        buttonMenosState = digitalRead(BUTTON_MENOS);
        buttonEnterState = digitalRead(BUTTON_ENTER);

        if (buttonMaisState == LOW) {
            // O botão foi pressionado
            posicao ++;
            
            // Verificar se o que vai ser mostrado é o menu 1 ou 2
            if (menu01 == 1){
                // Se posição passar da quantidade de itens do menu, levar para a posição zero do menu
                if (posicao == qtdItensMenu01){
                    posicao = 0;
                }
                mostrar_menu_01(posicao);
            
            }else if (menu02 == 1){
                // Se posição passar da quantidade de itens do menu, levar para a posição zero do menu
                if (posicao == qtdItensMenu02){
                    posicao = 0;
                }
                mostrar_menu_02(posicao);
            }
        
        }else if (buttonMenosState == LOW){
            // O botão foi pressionado
            posicao --;

            // Verificar se o que vai ser mostrado é o menu 1 ou 2
            if (menu01 == 1){
                // Se posição for menor que a quantidade de itens do menu, levar para a posição 31 do menu
                if (posicao == -1){
                    posicao = qtdItensMenu01 - 1;
                }
                mostrar_menu_01(posicao);
            
            }else if (menu02 == 1){
                // Se posição for menor que a quantidade de itens do menu, levar para a posição 31 do menu
                if (posicao == -1){
                    posicao = qtdItensMenu02 - 1;
                }
                mostrar_menu_02(posicao);
            }
        
        }else if (buttonEnterState == LOW){
            // O botão foi pressionado
            if (menu01 == 1){
                menu01 = 0;
                menu02 = 1;
                unidadeSelecionada = posicao;
                posicao = 0;
                mostrar_menu_02(posicao);
            }else if (menu02 == 1){
                escolhaFazerNaNode = posicao;
                // Mandar mensagem para a node e pegar o dado para exibir no LCD
                hex_data = 0x01;
                send_hex_data(fd, hex_data);
                //mostrar_menu_02(posicao);
            }
        }
        
        break;
    }
    
    
    return 0;
}
