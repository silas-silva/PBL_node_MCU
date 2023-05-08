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
#define BUTTON_MAIS  25           // PA07
#define BUTTON_MENOS  23        // PA10
#define BUTTON_ENTER  19         // PA20



int lcd;
int fd;


int qtdItensMenu01 = 34;
int qtdItensMenu02 = 8;


int unidadeSelecionada = 0;
int escolhaFazerNaNode = 0;



void imprimir_menu_lcd(char opcoes_menu[][30], int posicaoAtual) {
    // Limpa o display
    lcdClear(lcd);    
    // imprimindo as strings do vetor no LCD
    //lcdPosition(lcd, 0, 0);
    lcdPrintf(lcd, ("%s\n", opcoes_menu[posicaoAtual]));
}



void mostrar_menu_01(int vetor_menu01[][30], int posicaoAtual){
    // atribuindo valores às strings do vetor
    imprimir_menu_lcd(vetor_menu01, posicaoAtual);
}



void mostrar_menu_02(int vetor_menu02[][30],int posicaoAtual){
    // atribuindo valores às strings do vetor
    imprimir_menu_lcd(vetor_menu02, posicaoAtual);
}



int main() {
    printf("Passou 01");
    // Menus
    char vetor_menu01[qtdItensMenu01][30];
    char vetor_menu02[qtdItensMenu02][30];
    
    // Criar dados do menu 01
    // atribuindo valores às strings do vetor
    sprintf(vetor_menu01[0], "Selecionar Todas as Unidades");
    for (int i = 1; i < qtdItensMenu01 - 2 ; i++) {
        sprintf(vetor_menu01[i], "Selecionar Unidade %d", i);
    }
    sprintf(vetor_menu01[qtdItensMenu01 - 2], "Sair");


    // Criar dados do menu 02
    // atribuindo valores às strings do vetor
    sprintf(vetor_menu02[0], "Acender Led");
    sprintf(vetor_menu02[1], "Sensor Analogico");
    sprintf(vetor_menu02[2], "Sensor Digital X1");
    sprintf(vetor_menu02[3], "Sensor Digital X2");
    sprintf(vetor_menu02[4], "Sensor Digital X3");
    sprintf(vetor_menu02[5], "Sensor Digital X4");
    sprintf(vetor_menu02[6], "Sensor Digital X5");
    sprintf(vetor_menu02[6], "Voltar");



    printf("Passou 01");
    
    // Inicializar biblioteca
    wiringPiSetup();

    // Setar botões como entrada
    pinMode(BUTTON_MAIS, INPUT);
    pinMode(BUTTON_MENOS, INPUT);
    pinMode(BUTTON_ENTER, INPUT);
    
    printf("Passou 02");

    int buttonMaisState;
    int buttonMenosState;
    int buttonEnterState;

    printf("Passou 03");

    // Configurar UART
    if ((fd = serialOpen ("/dev/ttyS3", 115200)) < 0){
        printf ("Erro abrir UART") ;
        return 1 ;
    }

    printf("Passou 04");

    // Iniciar LCD
    lcd = lcdInit (2, 16, 4, LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7, 0, 0, 0, 0);
    lcdClear(lcd);
    lcdPuts(lcd, "BEM-VINDO");
    
    delay(3000);
    // Menu ativo no momento
    int menu01 = 1;
    int menu02 = 0;

    //
    int escolhaMenu01;
    int escolhaMenu02;

    // Variavel para controlar o que vai ser mostrado no Menu
    int posicao = 0;
    
    // Mostrar a primeira opção do primeiro Menu
    mostrar_menu_01(vetor_menu01, posicao);
    
    while(1){
        // Verificar se algum botão foi pressionado
        buttonMaisState = digitalRead(BUTTON_MAIS);
        buttonMenosState = digitalRead(BUTTON_MENOS);
        buttonEnterState = digitalRead(BUTTON_ENTER);
        
        // Entender esse Dalay, se precisa ou não
        delay(100);
        
        if (!buttonMaisState) {
            // O botão foi pressionado
            posicao ++;
            
            // Verificar se o que vai ser mostrado é o menu 1 ou 2
            if (menu01 == 1){
                // Se posição passar da quantidade de itens do menu, levar para a posição zero do menu
                if (posicao == qtdItensMenu01){
                    posicao = 1;
                }
                mostrar_menu_01(vetor_menu01, posicao);
            
            }else if (menu02 == 1){
                // Se posição passar da quantidade de itens do menu, levar para a posição zero do menu
                if (posicao == qtdItensMenu02){
                    posicao = 1;
                }
                mostrar_menu_02(vetor_menu02, posicao);
            }
        
        }
        else if (!buttonMenosState){
            // O botão foi pressionado
            posicao --;

            // Verificar se o que vai ser mostrado é o menu 1 ou 2
            if (menu01 == 1){
                // Se posição for menor que a quantidade de itens do menu, levar para a posição 32 do menu
                if (posicao == -1){
                    posicao = qtdItensMenu01 - 2;
                }
                mostrar_menu_01(vetor_menu01, posicao);
            
            }else if (menu02 == 1){
                // Se posição for menor que a quantidade de itens do menu, levar para a posição 6 do menu
                if (posicao == -1){
                    posicao = qtdItensMenu02 - 2;
                }
                mostrar_menu_02(vetor_menu02, posicao);
            }
        
        }
        else if (!buttonEnterState){
            // O botão foi pressionado
            if (menu01 == 1){
                // Verificar se apertou enter na posição Sair
                escolhaMenu01 = posicao;
                if (escolhaMenu01 == qtdItensMenu02 - 1){
                    lcdPuts(lcd, "TCHAU...");
                    break;
                }else{
                    menu01 = 0;
                    menu02 = 1;
                    posicao = 0;
                    mostrar_menu_02(vetor_menu02, posicao);
                }

            }else if (menu02 == 1){
                int escolhaMenu02 = posicao;
                // Verificar se apertou enter na posição Voltar
                if (escolhaMenu02 == qtdItensMenu02 - 1){
                    menu01 = 1;
                    menu02 = 0;
                    mostrar_menu_01(vetor_menu01, escolhaMenu01);
                }else{
                    // Mandar mensagem para a node e pegar o dado para exibir no LCD
                    lcdPuts(lcd, "ENVIANDO COMANDO...");
                    serialPutchar(fd, 0x1);
                    delay(500);
                    lcdClear(lcd);
                    lcdPuts(lcd, "COMANDO ENVIADO!");
                    delay(500);
                    // Mostrar dado que foi recebido
                    // Mostrar Menu 02
                    mostrar_menu_02(vetor_menu02, posicao);
                }   
            }
        }
    }
    return 0;
}
