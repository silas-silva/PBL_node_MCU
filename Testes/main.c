//gcc -o main main.c
#include <stdio.h>


int qtdItensMenu01 = 34;
int qtdItensMenu02 = 8;

// void imprimir_menu_lcd(char opcoes_menu[][30], int tamanho, int posicaoAtual) {
//     // Limpa o display
//     lcdClear(lcd);    
//     // imprimindo as strings do vetor no LCD
//     lcdPosition(lcd, 0, 0);
//     lcdPuts(lcd, ("%s\n", opcoes_menu[posicaoAtual]));
// }


void imprimir(char vetor_menu[][30], int qtdItensMenu){
    // imprimindo as strings do vetor
    for (int i = 0; i < qtdItensMenu - 1; i++) {
        printf("%s\n", vetor_menu[i]);
    }
}

void criar_menu_01(char vetor_menu01[][30]){
    // atribuindo valores às strings do vetor
    sprintf(vetor_menu01[0], "Selecionar Todas as Unidades");
    for (int i = 1; i < qtdItensMenu01 - 2 ; i++) {
        sprintf(vetor_menu01[i], "Selecionar Unidade %d", i);
    }
    sprintf(vetor_menu01[qtdItensMenu01 - 1], "Sair");
}

void criar_menu_02(char vetor_menu02[][30]){
    // atribuindo valores às strings do vetor
    sprintf(vetor_menu02[0], "Acender Led");
    sprintf(vetor_menu02[1], "Sensor Analogico");
    sprintf(vetor_menu02[2], "Sensor Digital X1");
    sprintf(vetor_menu02[3], "Sensor Digital X2");
    sprintf(vetor_menu02[4], "Sensor Digital X3");
    sprintf(vetor_menu02[5], "Sensor Digital X4");
    sprintf(vetor_menu02[6], "Sensor Digital X5");
    sprintf(vetor_menu02[7], "Voltar");
}

int main() {
    char vetor_menu01[qtdItensMenu01][30];
    char vetor_menu02[qtdItensMenu02][30];

    criar_menu_01(vetor_menu01);
    criar_menu_02(vetor_menu02);

    //imprimir(vetor_menu01, qtdItensMenu01);
    //imprimir(vetor_menu02, qtdItensMenu02);
    printf("%s\n", vetor_menu01[qtdItensMenu01 - 1]);
    return 0;
}