#pragma once
#pragma clang diagnostic ignored "-Wformat-security"

#include <ncurses.h>
#include <memory>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <locale.h>

#define SEM_TITULO "sem_titulo"
#define MODO_NORMAL "NORMAL"
#define MODO_INSERIR "INSERIR"
#define STRING_VAZIA ""
#define ESPACO " "

#define ESC 27

class molCode
{
private:
    size_t x, y;
    char modo;
    std::string nome_arquivo, estado, sessao;
    std::vector<std::string> linhas;

    // Método para remover um caractere
    void ch_remover(int);
    // Método para manipular tab
    std::string ch_tabs(std::string&);
    // Método para inserir caractere no meio da linha
    void ch_inserir(std::string, int);
    // Método para inserir ao final da linha
    void ch_anexo(std::string&);

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
    // Método para salvar arquivo e sair em seguida.
    void salvar_sair();
};
