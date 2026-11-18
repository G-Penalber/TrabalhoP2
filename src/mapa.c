#include "../include/jogo.h"
#include <stdlib.h>

char mapa[LINHAS][COLUNAS];
int topo = 0;
int passos_dados = 0;

void gerar_linha(int indice_linha) {
    for (int j = 0; j < COLUNAS; j++) {
    
        if (j == 0 || j == COLUNAS - 1) {
            mapa[indice_linha][j] = '|'; 
        } else {
        
            mapa[indice_linha][j] = (rand() % 100 < 10) ? '.' : ' ';
        }
    }
}

void inicializar_mapa() {
    topo = 0;
    for (int i = 0; i < LINHAS; i++) {
        gerar_linha(i);
    }
}