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
#define BUTTON_MAIS  25          // PA07
#define BUTTON_MENOS  23         // PA10
#define BUTTON_ENTER  19         // PA20

#define qtdItensMenu01 33
#define qtdItensMenu02 8

#define qtdUnidades 1

int lcd;
int fd;


void criar_menu_01(char (*vetor_menu)[30]) {
    sprintf(vetor_menu[0], "Selecionar Todas as Unidades");
    for (int i = 1; i < qtdItensMenu01 - 1; i++) {
        sprintf(vetor_menu[i], "Selecionar Unidade %d", i);
    }
    sprintf(vetor_menu[qtdItensMenu01 - 1], "Sair");
}


void criar_menu_02(char (*vetor_menu)[30]) {
    sprintf(vetor_menu[0], "Acender Led");
    sprintf(vetor_menu[1], "Apagar Led");
    sprintf(vetor_menu[2], "Consultar Sensor Analogico A0");
    sprintf(vetor_menu[3], "Monitorar Sensor Analogico A0");
    sprintf(vetor_menu[4], "Consultar Sensor Digital D0");
    sprintf(vetor_menu[5], "Monitorar Sensor Digital D0");
    sprintf(vetor_menu[6], "Consultar Sensor Digital D1");
    sprintf(vetor_menu[7], "Monitorar Sensor Digital D1");
    sprintf(vetor_menu[8], "Voltar");
}


void imprimir_menu_lcd(char (*opcoes_menu)[30], int posicaoAtual) {
    lcdClear(lcd);     // Limpa o display
    lcdPrintf(lcd, ("%s", opcoes_menu[posicaoAtual])); //lcdPosition(lcd, 0, 0); se quiser mudar a posição
}


void mostrar_menu_01(char (*vetor_menu)[30], int posicaoAtual){
    imprimir_menu_lcd(vetor_menu, posicaoAtual);
}


void mostrar_menu_02(char (*vetor_menu)[30],int posicaoAtual){
    imprimir_menu_lcd(vetor_menu, posicaoAtual);
}


int configurar_uart_lcd_botoesInput(){
    if ((fd = serialOpen ("/dev/ttyS3", 9600)) < 0){
        printf("ERRO NA CONFIGURAÇÂO DA UART \n") ;
        return 1;
    }

    // Iniciar LCD
    lcd = lcdInit (2, 16, 4, LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7, 0, 0, 0, 0);
    lcdClear(lcd);
    lcdPuts(lcd, "BEM-VINDO");

    // Setar botões como entrada
    pinMode(BUTTON_MAIS, INPUT);
    pinMode(BUTTON_MENOS, INPUT);
    pinMode(BUTTON_ENTER, INPUT);

    return 0;
}


void enviar_dado_UART(int fd, char num) {
    serialPutchar(fd, num);
    delay(2);
}

int receber_dado_analogico_UART() {
    if (serialDataAvail(fd)){
      int inteiro = serialGetchar(fd); // Leitura do byte da porta UART
      delay(2);
      int resto = serialGetchar(fd); // Leitura do byte da porta UART
      int retorno = (inteiro * 10) + resto;
      return retorno;
    }else{
        return -1;
    }
}

int receber_dado_UART() {
    if (serialDataAvail(fd)){
      int dado = serialGetchar(fd); // Leitura do byte da porta UART
      return dado;
    }else{
        return -1;
    }
}


int main() {
    int resposta;
    
    // vetores de comandos
    unsigned char selecionar_node[] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                                    0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x10,
                                    0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                                    0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F };
    
    unsigned char comandos_node[] = { 0xC0, 0xCA, 0xC1, 0xF1, 0xC2, 0xF2, 0xC3, 
                                    0xF3, 0xC4, 0xF4, 0xC5, 0xF5, 0xC6, 0xF6 };




    // Menus
    char vetor_menu01[qtdItensMenu01][30];
    char vetor_menu02[qtdItensMenu02][30];
    criar_menu_01(vetor_menu01);
    criar_menu_02(vetor_menu02);
    // Menu ativo no momento
    int menu01 = 1;
    int menu02 = 0;
    // Opção selecionada no Menu
    int selecaoMenu01 = 0;
    int selecaoMenu02 = 0;
    // Variavel para controlar o que vai ser mostrado no Menu
    int posicao = 0;
    
    //
    int nodeSelecionada = 0;
    // Inicializar biblioteca
    wiringPiSetup();
    
    // Configurar LCD, UART e Configurar Botões como entrada
    int configuracoesCorretas = configurar_uart_lcd_botoesInput();
    if (configuracoesCorretas == 1){
        return 1;
    }

    lcdClear(lcd);
    lcdPuts(lcd, "BEM-VINDO");
    
    // Estados dos botões, 0 ou 1
    int estadoBotaoMais;
    int estadoBotaoMenos;
    int estadoBotaoEnter;

    // Mostrar a primeira opção do primeiro Menu
    delay(2000);
    mostrar_menu_01(vetor_menu01, posicao);

    while(1){
        // Verificar se algum botão foi pressionado
        estadoBotaoMais = digitalRead(BUTTON_MAIS);
        estadoBotaoMenos = digitalRead(BUTTON_MENOS);
        estadoBotaoEnter = digitalRead(BUTTON_ENTER);
        delay(200);
        
        if (!estadoBotaoMais) { // O botão mais foi pressionado
            posicao ++;
            if (menu01 == 1){
                if (posicao >= qtdItensMenu01){
                    posicao = 0;
                }
                mostrar_menu_01(vetor_menu01, posicao);
            }else if (menu02 == 1){
                if (posicao >= qtdItensMenu02){
                    posicao = 0;
                }
                mostrar_menu_02(vetor_menu02, posicao);
            }
        }
        else if (!estadoBotaoMenos) { // O botão menos foi pressionado
            posicao --;
            if (menu01 == 1){
                if (posicao < 0){
                    posicao = qtdItensMenu01 - 1;
                }
                mostrar_menu_01(vetor_menu01, posicao);
            }else if (menu02 == 0){
                if (posicao < 0){
                    posicao = qtdItensMenu02 - 1;
                }
                mostrar_menu_02(vetor_menu02, posicao);
            }
        }
        else if (!estadoBotaoEnter) { // O botão enter foi pressionado
            if (menu01 == 1){
                selecaoMenu01 = posicao;
                if (selecaoMenu01 == qtdItensMenu01 - 1){   // Verificar se apertou enter na posição Sair
					lcdClear(lcd);
                    lcdPuts(lcd, "TCHAU...");
                    break;
                }else{
                    if(posicao == 0){
                        // Fazer logica para monitorar os pinos de todas as nodes
                        while(1){
                            for(int i=2; i <= qtdUnidades; i++){
                                enviar_dado_UART(fd, selecionar_node[i]);
                                // dado analogico
                                enviar_dado_UART(fd, comandos_node[3]);
                                resposta = receber_dado_analogico_UART();
                                lcdClear(fd);
                                lcdPuts(lcd, resposta);
                                delay(2000);
                                // dado digital d0
                                enviar_dado_UART(fd, comandos_node[4]);
                                resposta = receber_dado_UART();
                                lcdClear(fd);
                                lcdPuts(lcd, resposta);
                                delay(2000);
                                // dado digital d1
                                enviar_dado_UART(fd, comandos_node[6]);
                                resposta = receber_dado_UART();
                                lcdClear(fd);
                                lcdPuts(lcd, resposta);
                                delay(2000);
                            }
                            if (!estadoBotaoEnter){
                                mostrar_menu_01(vetor_menu01, posicao);
                                break;
                            }
                        }
                    }else{
                        // Seleciona a Node com o ID escolhido
                        enviar_dado_UART(fd, selecionar_node[posicao]);
                        menu01 = 0;      // Desabilita menu 1
                        menu02 = 1;      // Habilita menu 2
                        posicao = 0;     // Reseta o contador de posicoes
                        mostrar_menu_02(vetor_menu02, posicao);
                    }                    
                }
            }else if (menu02 == 1){
                selecaoMenu02 = posicao;
                if (selecaoMenu02 == qtdItensMenu02 - 1){   // Verificar se apertou enter na posição Voltar
                    menu01 = 1;
                    menu02 = 0;
                    posicao = selecaoMenu01;
                    mostrar_menu_01(vetor_menu01, posicao);
                }else{
                    // Mandar mensagem para a node e pegar o dado para exibir no LCD
                    lcdPuts(lcd, "ENVIANDO COMANDO...");
                    delay(50);
                    lcdClear(lcd);
                    delay(50);
                    lcdPuts(lcd, "COMANDO ENVIADO!");
                    delay(50);
                    lcdClear(lcd);
                    
                    if(posicao == 2 || posicao == 3){
                        if (posicao == 2){  //Consultar
                            enviar_dado_UART(fd, selecionar_node[posicao]);
                            resposta = receber_dado_analogico_UART();
                            lcdPuts(lcd, resposta);
                        }
                        else{ //Monitorar
                            while (1){
                                enviar_dado_UART(fd, selecionar_node[posicao - 1]);
                                resposta = receber_dado_analogico_UART();
                                lcdPuts(lcd, resposta);
                                if(!estadoBotaoEnter){
                                    break;
                                }else{
                                    delay(1000);
                                }
                            }                     
                        }                        
                    }
                    else{
                        enviar_dado_UART(fd, selecionar_node[posicao]);
                        resposta = receber_dado_UART();
                        lcdPuts(lcd, resposta);
                    }
                    delay(1000);
                    // Mostrar Menu 02
                    mostrar_menu_02(vetor_menu02, posicao);
                }   
            }
        }
    }



    return 0;
}