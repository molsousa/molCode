#pragma once
#pragma clang diagnostic ignored "-Wformat-security"

#include <ncurses.h>
#include <memory>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

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

    void ch_remover(int);
    std::string ch_tabs(std::string&);
    void ch_inserir(std::string, int);
    void ch_anexo(std::string&);

public:
    molCode(const std::string&);
    ~molCode();
    void inicializar();

protected:
    void atualizar();
    void linhaDeEstado();
    void entrada(int);
    void imprimir();

    void cima();
    void esquerda();
    void direita();
    void baixo();

    void abrir();
    void salvar();
};
