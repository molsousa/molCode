#include "../include/molCode.hpp"

/*
*   Construtor para inicializar o editor de texto.
*   Caso tenha outro argumento inserido no terminal
    o arquivo é inicializado com título.
*/
molCode::molCode(const std::string& arquivo)
{
    setlocale(LC_ALL, STRING_VAZIA);

    x = y = 0;
    scroll_offset = 0;
    modo = 'n';
    estado = MODO_NORMAL;
    sessao = STRING_VAZIA;

    if(arquivo.empty())
        nome_arquivo = SEM_TITULO;

    else
        nome_arquivo = arquivo;

    abrir();
    initscr(); // inicializa ncurse
    raw();
    noecho(); // desativa a exibição automática dos caracteres digitados no terminal
    cbreak(); // os caracteres digitados pelo usuário são enviados pra tela
    keypad(stdscr, true);
    definir_constantes();
    use_default_colors(); // cores padrões
}

// Destrutor para finalizar o editor.
molCode::~molCode()
{
    refresh(); // mostrar atualizações na tela
    endwin(); // finaliza ncurses e volta pro terminal anterior
}

// Método para inicializar ncurses
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

// Método para atualizar estado.
void molCode::atualizar()
{
    switch(modo){
    case 'n':
        estado = MODO_NORMAL;
        break;

    case 'i':
        estado = MODO_INSERIR;
        break;

    case 'q':
        break;
    }

    sessao = " COLUNAS: " + std::to_string(x) + " | LINHAS: " + std::to_string(y) + " | ARQUIVO: " + nome_arquivo;
    estado.insert(0, ESPACO);
}

// Método para mostrar o estado.
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

    for(int i {}; i < COLS; ++i)
        mvprintw(LINES-1, i, ESPACO);

    // TODO mostrar linhas

    mvprintw(LINES - 1, 0, estado.c_str());
    mvprintw(LINES - 1, COLS - sessao.length(), &sessao[0]);

    attroff(COLOR_PAIR(1));
    attroff(A_REVERSE);
}

// Método para interpretar entrada do usuário.
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
    case ESC:
    case 'n':
        switch(c){
        // finaliza o editor
        case 'q':
            modo = 'q';
            break;

        // ativa o modo inserção
        case 'i':
            modo = 'i';
            break;

        // salva e finaliza o editor
        case 'w':
            modo = 'w';
            salvar_sair();
            refresh();
            endwin();
            std::printf("Salvo.");
            exit(0);
            break;

        // move o cursor para o final do arquivo
        case KEY_END:
            y = linhas.size()-1;
            x = linhas[y].length();

            imprimir();
            break;

        // move o cursor para a primeira linha da primeira coluna
        case KEY_HOME:
            x = 0;
            y = 0;

            imprimir();
            break;

        // move o cursor para a última coluna da linha atual
        case KEY_NPAGE:
            x = linhas[y].length();

            move(y, x);
            break;

        // move o cursor para a primeira coluna da linha atual
        case KEY_PPAGE:
            x = 0;

            move(y, x);
            break;
        }
        break;

    case 'i':
        switch(c){
        // retorna ao modo normal
        case ESC:
            modo = 'n';
            break;

        // configuração para backspace
        case KEY_BACKSPACE:
        case 127:
            if(x == 0 && y > 0){ // caso y > 0, então move o cursor para a linha anterior
                x = linhas[y-1].length();
                linhas[y-1] += linhas[y];
                ch_remover(y);
                cima();
            }
            else if(x > 0)
                linhas[y].erase(--x, 1);
            break;

        // configuração para delete
        case KEY_DC:
            if(x == linhas[y].length() && y != linhas.size() - 1)
                linhas[y] += linhas[y + 1]; // puxa a linha de baixo para cima

            else
                linhas[y].erase(x, 1);

            break;

        // configuração para enter
        case KEY_ENTER:
        case 10:
            if(x < linhas[y].length()){ // quebra linha e move os caracteres para a próxima linha
                ch_inserir(linhas[y].substr(x, linhas[y].length() - x), y+1);
                linhas[y].erase(x, linhas[y].length()-x);
            }
            else
                ch_inserir(STRING_VAZIA, y+1);

            x = 0;
            baixo();
            break;

        // configuração para tabulação
        case KEY_BTAB:
        case KEY_CTAB:
        case KEY_STAB:
        case KEY_CATAB:
        case 9:
            linhas[y].insert(x, 3, ' '); // insere 3 espaços
            x += 3;
            break;

        // move o cursor para o final do arquivo
        case KEY_END:
            y = linhas.size()-1;
            x = linhas[y].length();

            imprimir();
            break;

        // move o cursor para a primeira linha da primeira coluna
        case KEY_HOME:
            x = 0;
            y = 0;

            imprimir();
            break;

        // move o cursor para a última coluna da linha atual
        case KEY_NPAGE:
            x = linhas[y].length();

            move(y, x);
            break;

        // move o cursor para a primeira coluna da linha atual
        case KEY_PPAGE:
            x = 0;

            move(y, x);
            break;

        default:
            linhas[y].insert(x, 1, c);
            ++x;
            break;
        }
        break;
    }
}

/*
*   Método para desenhar o conteúdo do buffer na tela
*   Caso tenha linhas não existentes no buffer, limpa
*   Cursor fica na posição original
*/
void molCode::imprimir()
{
    for(size_t i {}; i < (size_t) LINES-1; ++i){
        move(i, 0);
        clrtoeol();
    }

    int linhas_visiveis = LINES-1;

    for(size_t i {}; i < (size_t) linhas_visiveis; ++i){
        size_t linha = scroll_offset+i;

        if(linha < linhas.size())
            mvprintw(i, 0, linhas[linha].c_str());
    }

    int cursor_y = y - scroll_offset;

    if(cursor_y >= 0 && cursor_y < linhas_visiveis)
        move(cursor_y, x);

    else{
        cursor_y = (cursor_y < 0) ? 0 : linhas_visiveis-1;
        move(cursor_y, x);
    }

    refresh();
}

// Método para remover um caractere
void molCode::ch_remover(int numero)
{
    linhas.erase(linhas.begin() + numero);
}

// Método para manipular tab
std::string molCode::ch_tabs(std::string& linha)
{
    std::size_t tab = linha.find('\t');
    return (tab == linha.npos) ? linha : ch_tabs(linha.replace(tab, 1, "   "));
}

// Método para inserir caractere no meio da linha
void molCode::ch_inserir(std::string linha, int numero)
{
    linha = ch_tabs(linha);
    linhas.insert(linhas.begin() + numero, linha);
}

// Método para inserir ao final da linha
void molCode::ch_anexo(std::string& linha)
{
    linha = ch_tabs(linha);
    linhas.push_back(linha);
}

// Método para mover cursor pra cima.
void molCode::cima()
{
    if(y > 0){
        --y;

        if(y < scroll_offset)
            scroll_offset = y;
    }

    if(x >= linhas[y].length())
        x = linhas[y].length();

    imprimir();
}

/*
*   Método para mover cursor para a esquerda.
*   Caso o cursor chegue no início da coluna e tenha texto
    acima, o cursor é movido pra cima na última linha.
*/
void molCode::esquerda()
{
    if(x > 0){
        --x;
        move(y, x);
    }
    else{
        if(y > 0){ // caso tenha linhas acima
            --y;
            x = linhas[y].length();
            move(y, x);
        }
    }
}

/*
*   Método para mover cursor para a direita.
*   Caso o cursor chegue ao final da coluna e tenha texto
    abaixo, o cursor é movido pro início da próxima linha.
*/
void molCode::direita()
{
    if(x <= ((size_t)COLS) && x <= linhas[y].length()-1){
        ++x;
        move(y, x);
    }
    else{
        if(y < linhas.size()-1){ // caso tenha linhas anbaixo
            y++;
            x = 0;

            move(y, x);
        }
    }
}

// Método para mover cursor pra baixo.
void molCode::baixo()
{
    if(y < linhas.size() - 1) {
        ++y;

        int linhas_visiveis = LINES - 1;
        if(y >= scroll_offset + linhas_visiveis) {
            scroll_offset = y - linhas_visiveis + 1;
        }
    }

    if(x >= linhas[y].length())
        x = linhas[y].length();

    imprimir();
}

// Método para abrir arquivo.
void molCode::abrir()
{
    if(std::filesystem::exists(nome_arquivo)){
        std::ifstream ifile(nome_arquivo);

        if(ifile.is_open()){
            while(!ifile.eof()){
                std::string buffer;
                std::getline(ifile, buffer); // caso tenha texto reinsere para não perder dados

                ch_anexo(buffer);
            }
        }
        else // joga exceção caso o arquivo tenha permissão especial para não modificar
            throw std::runtime_error("Não é possível abrir o arquivo. Permissão negada");
    }
    else{
        std::string str {};
        ch_anexo(str); // não existindo o arquivo, cria o arquivo vazio
    }
}

// Método para salvar arquivo e sair em seguida.
void molCode::salvar_sair()
{
    std::ofstream ofile(nome_arquivo);

    if(ofile.is_open()){
        for(size_t i{}; i < linhas.size(); ++i){
            ofile << linhas[i]; // salva no arquivo de saída

            // Não insere enter ao final do arquivo
            if(i < linhas.size()-1)
                ofile << '\n';
        }

        ofile.close();
    }
    else{ // joga exceção caso o arquivo tenha permissão especial para não modificar
        refresh();
        endwin();
        throw std::runtime_error("Não é possível abrir o arquivo. Permissão negada");
    }
}

// Método para definição de constantes
void molCode::definir_constantes()
{
    define_key("\033[1;5A", CTRL_CIMA);
    define_key("\033[1;5D", CTRL_ESQUERDA);
    define_key("\033[1;5C", CTRL_DIREITA);
    define_key("\033[1;5B", CTRL_BAIXO);
}
