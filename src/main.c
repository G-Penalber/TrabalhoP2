#include "../include/jogo.h"

int main() {
    inicializar_sistema();

    int sair = 0;
    while (!sair) {
        int opcao = mostrar_menu();

        if (opcao == 0) {
        
            personagem heroi = { .stamina = 100, .missao = 0 };
            
        
            pedir_nome(&heroi);
            mostrar_introducao(heroi.nome);
            executar_jogo(&heroi);

        } else if (opcao == 1) {
            mostrar_controles();   
        } else {
            sair = 1;
        }
    }

    endwin();
    return 0;
}


