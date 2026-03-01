#include "../include/molCode.hpp"

/*
*   Construtor para inicializar o editor de texto.
*   Caso tenha outro argumento inserido no terminal
    o arquivo é inicializado com título.
*/
molCode::molCode(const std::string& arquivo)
{
    x = y = 0;
    modo = 'n';
    estado = "NORMAL";
    sessao = "";

    if(arquivo.empty())
        nome_arquivo = "sem_titulo";

    else
        nome_arquivo = arquivo;

    abrir();
    initscr(); // inicializa ncurses
    noecho(); // desativa a exibição automática dos caracteres digitados no terminal
    cbreak(); // os caracteres digitados pelo usuário são enviados pra tela
    keypad(stdscr, true);
    use_default_colors(); // cores padrões
}

/*
*   Destrutor para finalizar o editor.
*/
molCode::~molCode()
{
    refresh(); // mostrar atualizações na tela
    endwin(); // finaliza ncurses e volta pro terminal anterior
}

void molCode::inicializar()
{
    while(modo != 'q'){
        atualizar();
        linhaDeEstado();
        imprimir();
        int c = getch();
        entrada(c);
    }
}

// Método para atualizar estado
void molCode::atualizar()
{
    switch(modo){
    case 'n':
        estado = "NORMAL";
        break;

    case 'i':
        estado = "INSERIR";
        break;

    case 'q':
        break;
    }

    sessao = " COLUNAS: " + std::to_string(x) + " | LINHAS: " + std::to_string(y) + " | ARQUIVO: " + nome_arquivo;
    estado.insert(0, " ");
}

// Método para mostrar o estado
void molCode::linhaDeEstado()
{
    start_color();
    if(modo == 'n')
        init_pair(1, COLOR_CYAN, COLOR_BLACK); // cor ciano caso esteja em modo normal

    else
        init_pair(1, COLOR_RED, COLOR_BLACK); // vermelho caso esteja em modo de inserção

    attron(A_REVERSE);
    attron(COLOR_PAIR(1));
    // TODO lista de comandos

    for(int i {}; i < COLS; ++i){
        mvprintw(LINES-1, i, " ");
    }

    // TODO mostrar linhas

    mvprintw(LINES - 1, 0, estado.c_str());
    mvprintw(LINES - 1, COLS - sessao.length(), &sessao[0]);

    attroff(COLOR_PAIR(1));
    attroff(A_REVERSE);
}

// Método para interpretar entrada do usuário
void molCode::entrada(int c)
{
    // switch para mover cursor, tanto em tela normal quanto em inserção
    switch(c){
    // mover cursor pra cima
    case KEY_UP:
        cima();
        return;

    // mover cursor pra esquerda
    case KEY_LEFT:
        esquerda();
        return;

    // mover cursor pra direita
    case KEY_RIGHT:
        direita();
        return;

    // mover cursor pra baixo
    case KEY_DOWN:
        baixo();
        return;
    }

    switch(modo){
    case 27:
    case 'n':
        switch(c){
        case 'q':
            modo = 'q';
            break;

        case 'i':
            modo = 'i';
            break;

        case 'w':
            modo = 'w';
            salvar();
            refresh();
            endwin();
            std::printf("Salvo");
            exit(0);
            // TODO salvar sem sair
            break;
        }
        break;

    case 'i':
        switch(c){
        // TODO ctrl+bs
        case 27:
            modo = 'n';
            break;

        case KEY_BACKSPACE:
        case 127:
            if(x == 0 && y > 0){
                x = linhas[y-1].length();
                linhas[y-1] += linhas[y];
                ch_remover(y);
                cima();
            }
            else if(x > 0)
                linhas[y].erase(--x, 1);
            break;

        case KEY_DC:
            if(x == linhas[y].length() && y != linhas.size() - 1)
                linhas[y] += linhas[y + 1];

            else
                linhas[y].erase(x, 1);

            break;
        case KEY_ENTER:
        case 10:
            if(x < linhas[y].length()){
                ch_inserir(linhas[y].substr(x, linhas[y].length() - x), y+1);
                linhas[y].erase(x, linhas[y].length()-x);
            }
            else
                ch_inserir("", y+1);

            x = 0;
            baixo();
            break;

        case KEY_BTAB:
        case KEY_CTAB:
        case KEY_STAB:
        case KEY_CATAB:
        case 9:
            linhas[y].insert(x, 2, ' ');
            x += 2;
            break;

        default:
            linhas[y].insert(x, 1, c);
            ++x;
            break;
        }
        break;
    }
}

void molCode::imprimir()
{
    for(size_t i {}; i < (size_t) LINES-1; ++i){
        if(i >= linhas.size()){
            move(i, 0);
            clrtoeol();
        }
        else
            mvprintw(i, 0, linhas[i].c_str());

        clrtoeol();
    }
    move(y, x);
}

void molCode::ch_remover(int numero)
{
    linhas.erase(linhas.begin() + numero);
}

std::string molCode::ch_tabs(std::string& linha)
{
    std::size_t tab = linha.find('\t');
    return (tab == linha.npos) ? linha : ch_tabs(linha.replace(tab, 1, "  "));
}

void molCode::ch_inserir(std::string linha, int numero)
{
    linha = ch_tabs(linha);
    linhas.insert(linhas.begin() + numero, linha);
}

void molCode::ch_anexo(std::string& linha)
{
    linha = ch_tabs(linha);
    linhas.push_back(linha);
}

void molCode::cima()
{
    if(y > 0)
        --y;

    if(x >= linhas[y].length())
        x = linhas[y].length();

    move(y, x);
}

void molCode::esquerda()
{
    if(x > 0){
        --x;
        move(y, x);
    }
}

void molCode::direita()
{
    if(x <= ((size_t)COLS) && x <= linhas[y].length()-1){
        ++x;
        move(y, x);
    }
}

void molCode::baixo()
{
    if(y < ((size_t)LINES) && y < linhas.size() -1)
        ++y;

    if(x >= linhas[y].length())
        x = linhas[y].length();

    move(y, x);
}

void molCode::abrir()
{
    if(std::filesystem::exists(nome_arquivo)){
        std::ifstream ifile(nome_arquivo);

        if(ifile.is_open()){
            while(!ifile.eof()){
                std::string buffer;
                std::getline(ifile, buffer);

                ch_anexo(buffer);
            }
        }
        else
            throw std::runtime_error("Não é possível abrir o arquivo. Permissão negada");
    }
    else{
        std::string str {};
        ch_anexo(str);
    }
}

void molCode::salvar()
{
    std::ofstream ofile(nome_arquivo);

    if(ofile.is_open()){
        for(size_t i{}; i < linhas.size(); ++i)
            ofile << linhas[i] << '\n';

        ofile.close();
    }
    else{
        refresh();
        endwin();
        throw std::runtime_error("Não é possível abrir o arquivo. Permissão negada");
    }
}
