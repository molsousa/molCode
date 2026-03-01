#include "../include/molCode.hpp"

molCode::molCode(const std::string& arquivo)
{
    linhas.push_back("");
    x = y = 0;
    modo = 'n';
    estado = "NORMAL";

    if(arquivo.empty())
        nome_arquivo = "sem_titulo";

    else
        nome_arquivo = arquivo;

    initscr();
    noecho();
    cbreak();
    keypad(stdscr, true);
}

molCode::~molCode()
{
    refresh();
    endwin();
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

void molCode::atualizar()
{
    switch(modo){
    case 27:
    case 'n':
        estado = "NORMAL";
        break;

    case 'i':
        estado = "INSERT";
        break;

    case 'q':
        break;
    }
}

void molCode::linhaDeEstado()
{
    attron(A_REVERSE);
    mvprintw(LINES - 1, 0, estado.c_str());
    attroff(A_REVERSE);
}

void molCode::entrada(int c)
{
    switch(c){
    case KEY_UP:
        cima();
        return;

    case KEY_LEFT:
        esquerda();
        return;

    case KEY_RIGHT:
        direita();
        return;

    case KEY_DOWN:
        baixo();
        return;
    }

    switch(modo){
    case 27:
    case 'n':
        switch(c){
        case 27:
        case 'q':
            modo = 'q';
            break;
        case 'i':
            modo = 'i';
            break;
        case 'w':
            modo = 'w';
            break;
        }
        break;

    case 'i':
        switch(c){
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
            else if(x > 0){
                linhas[y].erase(--x, 1);
            }
            break;
        case KEY_DC:
            if(x == linhas[y].length() && y != linhas.size() - 1){
                linhas[y] += linhas[y + 1];
            }
            else{
                linhas[y].erase(x, 1);
            }
            break;
        case KEY_ENTER:
        case 10:
            if(x < linhas[y].length()){
                ch_inserir(linhas[y].substr(x, linhas[y].length() - x), y+1);
                linhas[y].erase(x, linhas[y].length()-x);
            }
            else{
                ch_inserir("", y+1);
            }
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
        {
            linhas[y].insert(x, 1, c);
            ++x;
        }
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
        else{
            mvprintw(i, 0, linhas[i].c_str());
        }
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
    if(y > 0){
        --y;
    }
    if(x >= linhas[y].length()){
        x = linhas[y].length();
    }

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
    if(y < ((size_t)LINES) && y < linhas.size() -1){
        ++y;
    }

    if(x >= linhas[y].length()){
        x = linhas[y].length();
    }

    move(y, x);
}
