#include "../include/jogo.h"

int main() {
    inicializar_sistema();

    int sair = 0;
    while (!sair) {
        int opcao = mostrar_menu();

        if (opcao == 0) {
            // Inicializa herói padrão
            personagem heroi = { .stamina = 100, .missao = 0 };
            
            // Inicia o fluxo de gameplay
            pedir_nome(&heroi);
            mostrar_introducao(heroi.nome);
            executar_jogo(&heroi);

        } else if (opcao == 1) {
            mostrar_controles();   
        } else {
            sair = 1;
        }
    }

    // Finaliza o ambiente Ncurses antes de fechar o programa
    endwin();
    return 0;
}


