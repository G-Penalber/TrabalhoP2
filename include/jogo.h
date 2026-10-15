#ifndef JOGO_H
#define JOGO_H

#include <ncurses.h>
#include <stdbool.h>

// --- Configurações do Jogo ---
#define LINHAS 18
#define COLUNAS 18
#define TEMPO_EVENTO 20
#define TEMPO_LIMITE 120

// --- Estruturas ---
typedef struct {
    int stamina;
    int missao;
    char nome[20];
} personagem;

// --- Variáveis Globais (Declaração) ---
extern char mapa[LINHAS][COLUNAS];
extern int topo;
extern int passos_dados;

// --- Protótipos das Funções ---

// Utilitários e Sistema (interface.c)
void inicializar_sistema();
int tamanho_utf8(const char *s);
int bytes_do_caractere(const char *s);

// Mapa (mapa.c)
void gerar_linha(int indice_linha);
void inicializar_mapa();

// Telas e Menus (interface.c)
int mostrar_menu();
void mostrar_controles();
void mostrar_mensagem(const char *mensagem);
void pedir_nome(personagem *p);
void mostrar_introducao(const char *nome);
void mostrar_casa(personagem *p);
void explorar_casa(personagem *p);

// Motor do Jogo (motor.c)
void executar_jogo(personagem *heroi);

#endif // JOGO_H