#pragma once
#pragma clang diagnostic ignored "-Wformat-security"

#include <ncurses.h>
#include <memory>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <locale.h>

// Definição de macros para string literal
#define SEM_TITULO "sem_titulo"
#define MODO_NORMAL "NORMAL"
#define MODO_INSERIR "INSERIR"
#define MODO_SALVAR "SALVO"
#define MODO_EDICAO "EDITAR"
#define STRING_VAZIA ""
#define ESPACO " "

// Definição de macros para valores numéricos
#define CTRL_CIMA 0x270
#define CTRL_ESQUERDA 0x271
#define CTRL_DIREITA 0x272
#define CTRL_BAIXO 0x273
#define CTRL_COLCHETE_A 0x274
#define CTRL_COLCHETE_F 0x275
#define CTRL_BACKSPACE 0x276
#define CTRL_ENTER 0x277
#define CTRL_X 0x278
#define CTRL_V 0x279
#define CTRL_C 0x27A
#define CTRL_A 0x27B
#define CTRL_B 0x27C
#define CTRL_N 0x27D
#define ESC 27

#define CARACTERES_VALIDOS ((ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122) || (ch >= '0' && ch <= '9') || (ch == '.') || (ch == '_'))

class molCode
{
private:
    size_t x, y;
    char modo;
    std::string nome_arquivo, estado, sessao;
    std::vector<std::string> linhas;
    std::string copia;
    size_t total_linhas, scroll_offset;

    // Método para remover um caractere
    void ch_remover(int);
    // Método para manipular tab
    std::string ch_tabs(std::string&);
    // Método para inserir caractere no meio da linha
    void ch_inserir(std::string, int);
    // Método para inserir ao final da linha
    void ch_anexo(std::string&);
    // Método para definição de constantes
    void definir_constantes();

public:
    /*
    *   Construtor para inicializar o editor de texto.
    *   Caso tenha outro argumento inserido no terminal
        o arquivo é inicializado com título.
    */
    molCode(const std::string&);
    //Destrutor para finalizar o editor.
    ~molCode();
    // Método para inicializar ncurses
    void inicializar();

protected:
    // Método para atualizar estado.
    void atualizar();
    // Método para mostrar o estado.
    void linhaDeEstado();
    // Método para interpretar entrada do usuário.
    void entrada(int);
    /*
    *   Método para desenhar o conteúdo do buffer na tela
    *   Caso tenha linhas não existentes no buffer, limpa
    *   Cursor fica na posição original
    */
    void imprimir();

    // Método para mover cursor pra cima.
    void cima();
    /*
    *   Método para mover cursor para a esquerda.
    *   Caso o cursor chegue no início da coluna e tenha texto
        acima, o cursor é movido pra cima na última linha.
    */
    void esquerda();
    /*
    *   Método para mover cursor para a direita.
    *   Caso o cursor chegue ao final da coluna e tenha texto
        abaixo, o cursor é movido pro início da próxima linha.
    */
    void direita();
    // Método para mover cursor pra baixo.
    void baixo();

    // Método para abrir arquivo.
    void abrir();
    // Método para editar nome do arquivo.
    void editar_nome(std::string&);
    // Método para salvar arquivo.
    void salvar();
    // Método para buscar caminho absoluto
    std::string caminho();
};
