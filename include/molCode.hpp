#pragma once
#pragma clang diagnostic ignored "-Wformat-security"

#include <ncurses.h>
#include <memory>
#include <string>
#include <vector>

class molCode{
    private:
            size_t x, y;
            char modo;
            std::string nome_arquivo, estado;
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
};
