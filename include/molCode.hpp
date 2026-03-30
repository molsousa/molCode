#pragma once
#pragma clang diagnostic ignored "-Wformat-security"

#include <ncurses.h>
#include <memory>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <locale>

// Macro para condicional de caracteres válidos
#define CARACTERES_VALIDOS ((ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122) || (ch >= '0' && ch <= '9') || (ch == '.') || (ch == '_'))

// Definição de constantes para valores numéricos
const int ESC {27};

// Definição de constantes com ctrl
const int CTRL_ESQUERDA {0x270};
const int CTRL_DIREITA {0x271};
const int CTRL_X {0x272};
const int CTRL_V {0x273};
const int CTRL_A {0x275};
const int CTRL_S {0x276};
const int CTRL_Z {0x277};

// Definição de constantes com ctrl+shift
const int CTRL_SH_CIMA {0x278};
const int CTRL_SH_ESQUERDA {0x279};
const int CTRL_SH_DIREITA {0x27A};
const int CTRL_SH_BAIXO {0x27B};

// Definição de constantes para string literal
const std::string STRING_VAZIA {""};
const std::string SEM_TITULO {"SEM_TITULO"};
const std::string MODO_NORMAL {"NORMAL"};
const std::string MODO_INSERIR {"INSERIR"};
const std::string MODO_SALVAR {"SALVO"};
const std::string MODO_EDICAO {"EDITAR"};
const std::string ESPACO {" "};

/*
 *  Classe para criar e gerenciar o editor de texto.
 *  Responsável por criar o editor, e gerenciar comandos.
 */
class molCode
{
private:
    size_t x, y; // movimentar cursor
    char modo; // modo de operação
    std::string nome_arquivo;
    std::string estado, sessao; // estado atual e linha de estado
    std::vector<std::string> linhas; // manipular linhas do arquivo
    std::vector<std::string> copia_linhas; // vetor de string auxiliar para copiar linhas
    std::string copia; // string auxiliar para copiar linha
    size_t total_linhas, scroll_offset; // movimentar tela

    // Função membro para remover um caractere.
    void ch_remover(const int);
    // Função membro para manipular tab.
    std::string ch_tabs(std::string&) const;
    // Função membro para inserir caractere no meio da linha.
    void ch_inserir(std::string, int);
    // Função membro para inserir ao final da linha.
    void ch_anexo(std::string&);
    // Função membro para selecionar uma única linha.
    void selecionar_linha(const char);
    // Função membro para selecionar uma todas as linhas.
    void selecionar_todas_linhas(const char);
    // Função membro para troca de strings.
    void trocar_posicao_vertical(std::string&, std::string&);
    // Função membro para definição de constantes.
    void definir_constantes();

public:
    /*
     *   Construtor para inicializar o editor de texto.
     *   Caso tenha outro argumento inserido no terminal
        o arquivo é inicializado com título.
     */
    molCode(const std::string&);
    //Destrutor para finalizar o editor.
    virtual ~molCode();
    // Função membro para inicializar ncurses.
    void inicializar();

protected:
    /*
     *  Manipulação dos atributos tipo size_t.
     */
    // Função membro que retorna o valor de x.
    size_t get_x() const;
    // Função membro que atualiza a posição x.
    void set_x(size_t);
    // Função membro que retorna o valor de y.
    size_t get_y() const;
    // Função membro que atualiza a posição y.
    void set_y(size_t);

    /*
     *  Manipulação dos atributos tipo vetor de strings.
     */
    // Função membro que retorna o vetor de linhas.
    std::vector<std::string> get_linhas() const;
    // Função membro que atualiza o vetor de linhas.
    void set_linhas(std::vector< std::string > linhas);

    /*
     *  Manipulação da tela.
     */
    // Função membro para atualizar estado.
    void atualizar();
    // Função membro para mostrar o estado.
    void linha_de_estado();
    // Função membro para interpretar entrada do usuário.
    void entrada(const int);
    // Função membro para processar entrada no modo normal.
    void entrada_modo_normal(const int);
    // Função membro para processar entrada no modo inserir.
    void entrada_modo_inserir(const int);
    /*
     *   Função membro para desenhar o conteúdo do buffer na
        tela.
     *   Caso tenha linhas não existentes no buffer, limpa.
     *   Cursor fica na posição original.
     */
    void imprimir();

    /*
     *  Movimentação de cursor.
     */
    // Função membro para mover cursor pra cima.
    void cima();
    /*
     *   Função membro para mover cursor para a esquerda.
     *   Caso o cursor chegue no início da coluna e tenha texto
        acima, o cursor é movido pra cima na última linha.
     */
    void esquerda();
    /*
     *   Função membro para mover cursor para a direita.
     *   Caso o cursor chegue ao final da coluna e tenha texto
        abaixo, o cursor é movido pro início da próxima linha.
     */
    void direita();
    // Função membro para mover cursor pra baixo.
    void baixo();

    /*
     *  Manipulação do arquivo.
     */
    // Função membro para abrir arquivo.
    void abrir();
    // Função membro para editar nome do arquivo.
    void editar_nome(const std::string&);
    // Função membro para salvar arquivo.
    void salvar();
    // Função membro para buscar caminho absoluto.
    std::string caminho() const;

    /*
     *  Funções membros para implementar em personalizações de diferentes extensões de arquivo.
     */
    // Função membro para implementar personalização em palavras-chave.
    virtual void palavras_chave();
    // Função membro para implementar identação automática personalizada.
    virtual void identacao();
    // Função membro para implementar reconhecimento de pilha de procedimentos.
    virtual void pilha_procedimentos();
    // Função membro para implementar criação de arquivo com texto inicializado.
    virtual void template_personalizado();
};
