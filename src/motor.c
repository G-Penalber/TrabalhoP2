#include "../include/jogo.h"
#include <time.h>

void executar_jogo(personagem *heroi) {
    timeout(50); 
    time_t tempo_inicio = time(NULL);
    
    while(true) {
        time_t tempo_atual = time(NULL);
        double segundos_passados = difftime(tempo_atual, tempo_inicio);
        
        if (segundos_passados >= TEMPO_LIMITE) {
            break;
        }
        
        int px = COLUNAS / 2;
        int py = LINHAS - 2;
        int jogando = 1;
        int tela_linhas, tela_colunas;

        inicializar_mapa();
        passos_dados = 0; 

        while (jogando) {
            clear();
            getmaxyx(stdscr, tela_linhas, tela_colunas);

            int offset_y = (tela_linhas - LINHAS) / 2;
            int offset_x = (tela_colunas - COLUNAS) / 2;

            for (int i = 0; i < LINHAS; i++) {
                int indice_real = (topo + i) % LINHAS;
                for (int j = 0; j < COLUNAS; j++) {
                    mvaddch(offset_y + i, offset_x + j, mapa[indice_real][j]);
                }
            }

            mvaddch(offset_y + py, offset_x + px, '@');
            mvprintw(offset_y + 0, offset_x + COLUNAS + 4, "Stamina: %d  ", heroi->stamina);
            refresh();

            // Verifica Morte por Cansaço
            if (heroi->stamina <= 0) {
                clear();
                const char *morte = "Sua visao escurece. Voce desabou de exaustao.";
                mvprintw(tela_linhas / 2, (tela_colunas - tamanho_utf8(morte)) / 2, "%s", morte);
                refresh();
                napms(3000);
                jogando = 0;
                break; // Sai do loop do jogo e volta para o menu inicial
            }

            int tecla = getch();
            int novo_x = px;

            if (tecla == 'a') novo_x--;
            if (tecla == 'd') novo_x++;
            if (tecla == 'q') jogando = 0;

            if (tecla == 'w') {
                topo = (topo - 1 + LINHAS) % LINHAS;
                gerar_linha(topo);
                passos_dados++; 
                
                // MÁGICA AQUI: Perde 1 de stamina a cada passo dado!
                heroi->stamina -= 1; 
            }

            if (mapa[(topo + py) % LINHAS][novo_x] != '|') {
                px = novo_x;
            }

            if (passos_dados == 20) {
                mostrar_casa(heroi);
                passos_dados = 21; // Impede a casa de repetir
                timeout(50); // Restaura a velocidade contínua do mapa
            }
        }
        
        // Se saiu do `while (jogando)`, quebra o while(true) externo também
        if (jogando == 0) {
            break; 
        }
    }
}