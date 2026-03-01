#include "../include/molCode.hpp"

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

    initscr();
    noecho();
    cbreak();
    keypad(stdscr, true);
    use_default_colors();
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

void molCode::linhaDeEstado()
{
    start_color();
    if(modo == 'n')
        init_pair(1, COLOR_CYAN, COLOR_BLACK);

    else
        init_pair(1, COLOR_RED, COLOR_BLACK);

    attron(A_REVERSE);
    attron(COLOR_PAIR(1));

    for(int i {}; i < COLS; ++i){
        mvprintw(LINES-1, i, " ");
    }

    mvprintw(LINES - 1, 0, estado.c_str());
    mvprintw(LINES - 1, COLS - sessao.length(), &sessao[0]);

    attroff(COLOR_PAIR(1));
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
        for(size_t i{}; i < linhas.size(); ++i){
            ofile << linhas[i] << '\n';
        }
        ofile.close();
    }
    else{
        refresh();
        endwin();
        throw std::runtime_error("Não é possível abrir o arquivo. Permissão negada");
    }
}
