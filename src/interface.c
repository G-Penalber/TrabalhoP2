#include "../include/jogo.h"
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <stdlib.h>
#include <time.h>

void inicializar_sistema() {
    setlocale(LC_ALL, "");  
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
    srand(time(NULL));
}

int tamanho_utf8(const char *s) {
    int n = 0;
    for (; *s; s++) {
        if (((unsigned char)*s & 0xC0) != 0x80) n++;
    }
    return n;
}

int bytes_do_caractere(const char *s) {
    unsigned char c = (unsigned char)*s;
    if (c < 0x80) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    return 4;
}

void pedir_nome(personagem *p) {
    int tl, tc;
    getmaxyx(stdscr, tl, tc);
    const char *prompt = "Digite o nome do personagem (max 19 caracteres): ";

    erase();
    mvprintw(tl / 2 - 1, (tc - tamanho_utf8(prompt)) / 2, "%s", prompt);
    move(tl / 2 + 1, (tc) / 2 - 10);
    refresh();

    flushinp();
    echo();
    curs_set(1);
    getnstr(p->nome, sizeof(p->nome) - 1);
    noecho();
    curs_set(0);
    
    if (p->nome[0] == '\0') strncpy(p->nome, "Sobrevivente", sizeof(p->nome) - 1);
}

int mostrar_menu() {
    int tela_linhas, tela_colunas;
    getmaxyx(stdscr, tela_linhas, tela_colunas);

    int altura = 12;
    int largura = 40;
    int y = (tela_linhas - altura) / 2;
    int x = (tela_colunas - largura) / 2;

    clear();
    refresh();
    
    WINDOW *janela_menu = newwin(altura, largura, y, x);
    keypad(janela_menu, TRUE);

    char *opcoes[3] = {"1. Iniciar Jogo", "2. Controles", "3. Sair"};
    const char *titulo = "Prata, Sangue e Redencao.";
    int escolha_atual = 0;
    int tecla;

    while (1) {
        box(janela_menu, 0, 0);
        mvwprintw(janela_menu, 2, (largura - tamanho_utf8(titulo)) / 2, "%s", titulo);
        for (int i = 0; i < 3; i++) {
            if (i == escolha_atual) wattron(janela_menu, A_REVERSE);
            mvwprintw(janela_menu, 5 + (i * 2), (largura - tamanho_utf8(opcoes[i])) / 2, "%s", opcoes[i]);
            if (i == escolha_atual) wattroff(janela_menu, A_REVERSE);
        }
        wrefresh(janela_menu);
        
        tecla = wgetch(janela_menu);
        if (tecla == KEY_UP || tecla == 'w') {
            escolha_atual--;
            if (escolha_atual < 0) escolha_atual = 2;
        } else if (tecla == KEY_DOWN || tecla == 's') {
            escolha_atual++;
            if (escolha_atual > 2) escolha_atual = 0;
        } else if (tecla == 10 || tecla == '\n' || tecla == KEY_ENTER) {
            break;
        }
    }
    
    delwin(janela_menu);
    return escolha_atual;
}

void mostrar_controles() {
    clear();
    int tela_linhas, tela_colunas;
    getmaxyx(stdscr, tela_linhas, tela_colunas);

    char *controles[] = {
        "Controles do Jogo:",
        "W: Andar para frente",
        "A: Desviar para a esquerda",
        "S: Andar para tras",
        "D: Desviar para a direita",
        "Q: Sair do jogo",
        "",
        "Pressione qualquer tecla para voltar ao menu."
    };

    int num_controles = sizeof(controles) / sizeof(controles[0]);
    for (int i = 0; i < num_controles; i++) {
        mvprintw((tela_linhas - num_controles) / 2 + i,
                 (tela_colunas - tamanho_utf8(controles[i])) / 2,
                 "%s", controles[i]);
    }
    refresh();
    getch();
}

void mostrar_mensagem(const char *mensagem) {
    int tela_linhas, tela_colunas;
    getmaxyx(stdscr, tela_linhas, tela_colunas);
    clear();
    mvprintw(tela_linhas / 2, (tela_colunas - tamanho_utf8(mensagem)) / 2, "%s", mensagem);
    refresh();
    getch();
}

void mostrar_introducao(const char *nome) {
    char linha_nome[100];
    snprintf(linha_nome, sizeof(linha_nome), "%s esta so nessa, agora, sobreviver depende de voce.", nome);
    const char *texto[] = {
        "Voce sente suas pernas pesadas, sua boca seca, a barriga vazia e a mente fraca pelo sono.",
        "",
        "O vento carregado de areia passa pelo seu rosto, aos poucos o sol cai e a estrada escurece.",
        "",
        linha_nome,
        "",
    };
    int num_linhas = sizeof(texto) / sizeof(texto[0]);

    int tela_linhas, tela_colunas;
    getmaxyx(stdscr, tela_linhas, tela_colunas);

    clear();
    nodelay(stdscr, TRUE);  
    int atraso = 45;         

    for (int i = 0; i < num_linhas; i++) {
        int y = (tela_linhas - num_linhas) / 2 + i;
        int x = (tela_colunas - tamanho_utf8(texto[i])) / 2;
        move(y, x);

        for (const char *p = texto[i]; *p; p += bytes_do_caractere(p)) {
            addnstr(p, bytes_do_caractere(p));  
            refresh();

            if (getch() != ERR) atraso = 0;     
            napms(atraso);
        }
        napms(atraso ? 400 : 0);                
    }

    nodelay(stdscr, FALSE);   
    flushinp();               

    const char *aviso = "[ Pressione qualquer tecla para comecar ]";
    mvprintw(tela_linhas - 3, (tela_colunas - tamanho_utf8(aviso)) / 2, "%s", aviso);
    refresh();
    getch();
}

void explorar_casa(personagem *p) {
    int tela_linhas, tela_colunas;
    getmaxyx(stdscr, tela_linhas, tela_colunas);
    clear();

    // 1. Narrativa de entrada
    const char *texto[] = {
        "A pesada porta de madeira se abre.",
        "O interior e silencioso. O ar cheira a poeira mas parece minimamente arrumado.",
    };
    int num_linhas = sizeof(texto) / sizeof(texto[0]);
    int texto_y_base = (tela_linhas / 4); 

    nodelay(stdscr, TRUE);  
    int atraso = 45;         

    for (int i = 0; i < num_linhas; i++) {
        int y = texto_y_base + i;
        int x = (tela_colunas - tamanho_utf8(texto[i])) / 2;
        move(y, x);
        for (const char *ptr = texto[i]; *ptr; ptr += bytes_do_caractere(ptr)) {
            addnstr(ptr, bytes_do_caractere(ptr));  
            refresh();
            if (getch() != ERR) atraso = 0;     
            napms(atraso);
        }
        napms(atraso ? 300 : 0);                
    }
    
    napms(800);
    nodelay(stdscr, FALSE);
    flushinp();

    // 2. Construção dos Mapas (Andar 1 e Andar 2)
    const char *mapa_andar1[] = {
        "==================       ====================",
        "||           ||  //      \\  ||            ||",
        "||  _____    ||.//        \\.||         ___||",
        "|| |     |   ||              ||        |   ||",
        "|| |     |   ||              ||        |   ||",
        "|| |     |                             |   ||",
        "|| |_____|                             |___||",
        "||                                         ||",
        "||            ||=============||            ||",
        "||            ||=============||            ||",
        "||            ||=============||            ||",
        "==========================================="
    };

    const char *mapa_andar2[] = {
        "===========================================",
        "||           ||             ||           ||",
        "||           ||             ||        ___||",
        "||           ||             ||       |   ||",
        "||           ||             ||       |   ||",
        "||           |.                      |   ||",
        "||           |                       |___||",
        "||           |                           ||",
        "||           ||=============||           ||",
        "||           ||=============||           ||",
        "||           ||=============||           ||",
        "==========================================="
    };
    
    int linhas_casa = 12;
    int colunas_casa = 43;

    // Estado do Jogador
    int andar_atual = 1;
    int px = 21; // Começa no meio do Andar 1
    int py = 7;
    int jogando_casa = 1;
    
    int msg_timer = 0;
    char mensagem_interacao[100] = "";

    timeout(50); 

    // 3. Loop de Gameplay Interno (Fase da Casa)
    while (jogando_casa) {
        clear();
        getmaxyx(stdscr, tela_linhas, tela_colunas);

        int offset_y = (tela_linhas - linhas_casa) / 2;
        int offset_x = (tela_colunas - colunas_casa) / 2;

        
        for (int i = 0; i < linhas_casa; i++) {
            if (andar_atual == 1) {
                mvprintw(offset_y + i, offset_x, "%s", mapa_andar1[i]);
            } else {
                mvprintw(offset_y + i, offset_x, "%s", mapa_andar2[i]);
            }
        }

  
        mvaddch(offset_y + py, offset_x + px, '@');


        mvprintw(offset_y - 3, offset_x, "Stamina: %d ", p->stamina);
        mvprintw(offset_y - 2, offset_x, "[W,A,S,D] Andar | [E] Interagir | [Q] Sair da casa");
        mvprintw(offset_y - 1, offset_x, "Andar Atual: %d", andar_atual);

        
        if (msg_timer > 0) {
            attron(A_REVERSE);
            mvprintw(offset_y + linhas_casa + 2, (tela_colunas - tamanho_utf8(mensagem_interacao)) / 2, "%s", mensagem_interacao);
            attroff(A_REVERSE);
            msg_timer--; 
        }

        refresh();

        
        int tecla = getch();
        int novo_x = px;
        int novo_y = py;

        if (tecla == 'a') novo_x--;
        if (tecla == 'd') novo_x++;
        if (tecla == 'w') novo_y--;
        if (tecla == 's') novo_y++;
        if (tecla == 'q') jogando_casa = 0; 

       
        if (tecla == 'e') {
            if (andar_atual == 1) {
                if (py >= 8) {
                    
                    andar_atual = 2;
                    py = 7; 
                    snprintf(mensagem_interacao, sizeof(mensagem_interacao), "Voce subiu para o segundo andar.");
                    msg_timer = 60;
                } 
                else if (px < 15 && py < 7) {
                    snprintf(mensagem_interacao, sizeof(mensagem_interacao), "Voce revira a mesa e encontra comida enlatada!");
                    p->stamina += 15;
                    if (p->stamina > 100) p->stamina = 100;
                    msg_timer = 60; 
                } 
                else if (px > 28 && py < 7) {
                    snprintf(mensagem_interacao, sizeof(mensagem_interacao), "O armario esta trancado. Com um cadeado pesado.");
                    msg_timer = 60;
                } 
                else {
                    snprintf(mensagem_interacao, sizeof(mensagem_interacao), "Nao ha nada de util aqui.");
                    msg_timer = 40;
                }
            } 
            else if (andar_atual == 2) {
                if (py >= 8) {
                   
                    andar_atual = 1;
                    py = 7; 
                    snprintf(mensagem_interacao, sizeof(mensagem_interacao), "Voce desceu para o primeiro andar.");
                    msg_timer = 60;
                } 
                else if (px > 28 && py < 7) {
                    snprintf(mensagem_interacao, sizeof(mensagem_interacao), "Um armario velho. Voce encontra remedios organicos!");
                    p->stamina += 20;
                    if (p->stamina > 100) p->stamina = 100;
                    msg_timer = 60;
                } 
                else if (px < 15 && py < 7) {
                    snprintf(mensagem_interacao, sizeof(mensagem_interacao), "Um quarto vazio com uma janela quebrada. O vento uiva.");
                    msg_timer = 60;
                } 
                else {
                    snprintf(mensagem_interacao, sizeof(mensagem_interacao), "Apenas chao de madeira rangendo sob seus pes.");
                    msg_timer = 40;
                }
            }
        }

       
        if (novo_y >= 0 && novo_y < linhas_casa && novo_x >= 0 && novo_x < colunas_casa) {
            char espaco_destino = (andar_atual == 1) ? mapa_andar1[novo_y][novo_x] : mapa_andar2[novo_y][novo_x];
            
            if (espaco_destino == ' ') {
                px = novo_x;
                py = novo_y;
            }
        }
    }
    
    clear();
    refresh();
}

void mostrar_casa(personagem *p) {
    int tela_linhas, tela_colunas;
    getmaxyx(stdscr, tela_linhas, tela_colunas);
    clear();

    const char *cena_casa[] = {
        "                                     __                             ",
        "                      ______________=!!=______________              ",
        "                  __====================================__          ",
        "                  |  ________        ==        ________  |          ",
        "                  | | *      |       ==       | *      | |          ",
        "          ___     | |        |       ==       |     *  | |          ",
        "        __| |__   | |_____*__|       ==       |___*____| |      _   ",
        "        |     |   |                  ==                  |    _ ||  ",
        "        |_____|   |======================================|    ||||  ",
        "   .      | |     |  ________     ________     ________  |    \\||  .",
        " .. .     | | ..  | |       *|    |==||==|    | *      | | . ..| |..",
        ". .. .    |_| . ..|_|__*_____|____|==||==|____|_____*__| |. .. |_| .",
        "_ _ . . _ _._ .. .|_|________|____|_.||._|____|________|_| .. _._ ._",
        "===================================================================="
    };
    
    int linhas_arte = 14;
    int largura_arte = 68;
    
    int arte_y = (tela_linhas / 2) - 10;
    int arte_x = (tela_colunas - largura_arte) / 2;
    
    for (int i = linhas_arte - 1; i >= 0; i--) {
        mvprintw(arte_y + i, arte_x, "%s", cena_casa[i]);
        refresh();
        napms(150); 
    }
    napms(600); 

    char linha_nome[100];
    snprintf(linha_nome, sizeof(linha_nome), "%s! Apos andar por horas, voce encontra o que parece ser uma casa.", p->nome);
    const char *texto[] = {
        linha_nome,
        "",
        "Voce precisa escolher: entrar nela e descansar, ou continuar a andar?",
    };
    int num_linhas = sizeof(texto) / sizeof(texto[0]);

    nodelay(stdscr, TRUE);  
    int atraso = 45;         
    
    int texto_y_base = arte_y + linhas_arte + 2; 

    for (int i = 0; i < num_linhas; i++) {
        int y = texto_y_base + i;
        int x = (tela_colunas - tamanho_utf8(texto[i])) / 2;
        move(y, x);

        for (const char *ptr = texto[i]; *ptr; ptr += bytes_do_caractere(ptr)) {
            addnstr(ptr, bytes_do_caractere(ptr));  
            refresh();
            if (getch() != ERR) atraso = 0;     
            napms(atraso);
        }
        napms(atraso ? 400 : 0);                
    }
    nodelay(stdscr, FALSE);
    flushinp();

    int altura_menu = 7;
    int largura_menu = 40;
    int menu_y = texto_y_base + num_linhas + 2; 
    int menu_x = (tela_colunas - largura_menu) / 2;

    WINDOW *janela_menu = newwin(altura_menu, largura_menu, menu_y, menu_x);
    keypad(janela_menu, TRUE);

    char *opcoes[2] = {"1. Entrar na casa", "2. Continuar caminhando"};
    const char *titulo = "Qual o seu destino?";
    int escolha_atual = 0;
    int tecla;

    while (1) {
        box(janela_menu, 0, 0);
        mvwprintw(janela_menu, 1, (largura_menu - tamanho_utf8(titulo)) / 2, "%s", titulo);
        for (int i = 0; i < 2; i++) {
            if (i == escolha_atual) wattron(janela_menu, A_REVERSE);
            mvwprintw(janela_menu, 3 + i, (largura_menu - tamanho_utf8(opcoes[i])) / 2, "%s", opcoes[i]);
            if (i == escolha_atual) wattroff(janela_menu, A_REVERSE);
        }
        wrefresh(janela_menu);
        
        tecla = wgetch(janela_menu);
        if (tecla == KEY_UP || tecla == 'w') {
            escolha_atual--;
            if (escolha_atual < 0) escolha_atual = 1;
        } else if (tecla == KEY_DOWN || tecla == 's') {
            escolha_atual++;
            if (escolha_atual > 1) escolha_atual = 0;
        } else if (tecla == 10 || tecla == '\n' || tecla == KEY_ENTER) {
            break;
        }
    }

    delwin(janela_menu); 

    // Roteamento
    if (escolha_atual == 0) {
        explorar_casa(p); 
    } else {
        clear();
        const char *msg_fuga = "Voce ignora a casa.";
        mvprintw(tela_linhas / 2, (tela_colunas - tamanho_utf8(msg_fuga)) / 2, "%s", msg_fuga);
        refresh();
        napms(2500); 
    }
}