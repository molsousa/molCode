#include "../include/molCode.hpp"

/*
*   Construtor para inicializar o editor de texto.
*   Caso tenha outro argumento inserido no terminal
    o arquivo é inicializado com título.
*/
molCode::molCode(const std::string& arquivo)
{
    setlocale(LC_ALL, STRING_VAZIA.c_str());

    x = y = 0;
    scroll_offset = 0;
    modo = 'n';
    estado = MODO_NORMAL;
    copia = sessao = STRING_VAZIA;

    if(arquivo.empty()){
        nome_arquivo = SEM_TITULO;
    }

    else{
        nome_arquivo = arquivo;
    }

    abrir();
    initscr(); // inicializa ncurse
    raw(); // ignorar comandos do terminal externo
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

// Função membro para inicializar ncurses
void molCode::inicializar()
{
    while(modo != 'q'){
        atualizar();
        linhaDeEstado();
        imprimir();
        int c {getch()};
        entrada(c);
    }
}

// Função membro para atualizar estado.
void molCode::atualizar()
{
    switch(modo){
    case 'n':
        estado = MODO_NORMAL;
        break;

    case 'i':
        estado = MODO_INSERIR;
        break;

    case 's':
        estado = MODO_SALVAR;
        break;

    case 'q':
        break;
    }

    sessao = " COLUNAS: " + std::to_string(x) + " | LINHAS: " + std::to_string(y) + " | ARQUIVO: " + nome_arquivo;
    estado.insert(0, ESPACO);
}

// Função membro para mostrar o estado.
void molCode::linhaDeEstado()
{
    start_color();
    if(modo == 'n'){
        init_pair(1, COLOR_CYAN, COLOR_BLACK); // cor ciano caso esteja em modo normal
    }

    else if(modo == 'i'){
        init_pair(1, COLOR_RED, COLOR_BLACK); // vermelho caso esteja em modo de inserção
    }

    else if(modo == 's'){
        init_pair(1, COLOR_WHITE, COLOR_BLACK); // cor branca pra salvar
    }

    attron(A_REVERSE);
    attron(COLOR_PAIR(1));

    for(int i {}; i < COLS; ++i){
        mvprintw(LINES-1, i, ESPACO.c_str());
    }

    if(modo == 'n'){
        mvprintw(LINES - 1, static_cast<int>(estado.length()), " | W=Salvar e Sair | P=Caminho | S=Salvar | I=Inserir | Q=Sair");
    }

    else if(modo == 'i'){
        mvprintw(LINES - 1, static_cast<int>(estado.length()), " | ESC=Normal | CTRL+X=Copiar | CTRL+V=Colar");
    }

    mvprintw(LINES - 1, 0, "%s", estado.c_str());
    mvprintw(LINES - 1, COLS - sessao.length(), "%s", sessao.c_str());

    attroff(COLOR_PAIR(1));
    attroff(A_REVERSE);
}

// Função membro para interpretar entrada do usuário.
void molCode::entrada(const int c)
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
                if(linhas[y].length() > 0){
                    direita();
                }
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
                salvar();
                refresh();
                endwin();
                std::printf("Salvo.");
                exit(0);
                break;

            // salva sem sair do editor
            case 's':
                modo = 's';
                salvar();
                atualizar();
                linhaDeEstado();
                imprimir();
                napms(1500);
                modo = 'n';
                break;

            // editar nome do arquivo atual
            case 'r':
                {
                    std::string novo_nome {STRING_VAZIA};
                    int ch;
                    size_t i {};
                    move(LINES-1, 0);
                    clrtoeol();

                    mvprintw(LINES - 1, 0, ":");

                    do{
                        ch = getch();

                        switch(ch){
                            case KEY_BACKSPACE:
                            case 127:
                            case '\b':
                                if(i > 0){
                                    novo_nome.erase(--i, 1);
                                    move(LINES, i);
                                    mvprintw(LINES - 1, 0, ":%s", novo_nome.c_str());
                                    refresh();
                                }
                                break;
                            case ESC:
                                ch = '\n';
                                novo_nome = nome_arquivo;
                                break;
                            default:
                                if(CARACTERES_VALIDOS){ // só é permitido letras, números, underline e ponto
                                    novo_nome.insert(i++, 1, ch);
                                }
                                break;
                        }
                        move(LINES, i);
                        clrtoeol();

                        mvprintw(LINES - 1, 0, ":%s", novo_nome.c_str());
                        move(LINES, i+1);

                        refresh();

                    }while(ch != '\n');

                    editar_nome(novo_nome);

                    break;
                }

        // mostrar caminho absoluto do arquivo atual
        case 'p':
            attron(A_REVERSE);
            move(LINES-1, 0);
            clrtoeol();

            mvprintw(LINES-1, 0, ":%s", caminho().c_str());
            refresh();

            napms(2500);
            attroff(A_REVERSE);

            break;

        // move o cursor para o final do arquivo
        case KEY_END:
            y = linhas.size()-1;
            x = linhas[y].length();
            scroll_offset = linhas.size() - (LINES-1);

            imprimir();
            break;

        // move o cursor para a primeira linha da primeira coluna
        case KEY_HOME:
            x = 0;
            y = 0;
            scroll_offset = 0;

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

        // copia uma linha inteira
        case CTRL_X:
            copia = linhas[y];
            break;


        // mover cursor a cada substring separada por espaço
        case CTRL_ESQUERDA:

            break;

        // mover cursor a cada substring separada por espaço
        case CTRL_DIREITA:
            x = linhas[y].find(ESPACO, x+1);

            if(x < linhas[y].length())
                move(y, x);

            else
                x = linhas[y].length();

            break;

        // selecionar linha única
        case CTRL_B:
            selecionar_linha('n');

            break;

        // selecionar todas as linhas do arquivo
        case CTRL_A:
            selecionar_todas_linhas('n');

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
                else if(x > 0){
                    linhas[y].erase(--x, 1);
                }
                break;

            // configuração para delete
            case KEY_DC:
                if(x == linhas[y].length() && y != linhas.size() - 1){
                    linhas[y] += linhas[y + 1]; // puxa a linha de baixo para cima

                    // todo corrigir delete
                    // linhas[y+1].erase(0, linhas[y+1].length());
                }

                else{
                    linhas[y].erase(x, 1);
                }

                break;

            // configuração para enter
            case KEY_ENTER:
            case 10:
                if(x < linhas[y].length()){ // quebra linha e move os caracteres para a próxima linha
                    ch_inserir(linhas[y].substr(x, linhas[y].length() - x), y+1);
                    linhas[y].erase(x, linhas[y].length()-x);
                }
                else{
                    ch_inserir(STRING_VAZIA, y+1);
                }

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
                scroll_offset = linhas.size() - (LINES-1);

                imprimir();
                break;

            // move o cursor para a primeira linha da primeira coluna
            case KEY_HOME:
                x = 0;
                y = 0;
                scroll_offset = 0;

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

            // copia uma linha inteira
            case CTRL_X:
                copia = linhas[y];
                break;

            // cola a string copiada
            case CTRL_V:
                linhas[y].insert(x, copia);
                break;

            // remove tabulação no início da string
            case CTRL_SH_ESQUERDA:
                {
                    bool flag = true;
                    for(size_t i{}; i < 3; i++){
                        if(linhas[y][i] != ' '){
                            flag = false;
                        }
                    }
                    if(flag){
                        linhas[y].erase(0, 3);
                    }
                }
                break;

            // insere tabulação no início da string
            case CTRL_SH_DIREITA:
                linhas[y].insert(0, "   ");
                break;

            // mover cursor a cada substring separada por espaço
            case CTRL_DIREITA:
                x = linhas[y].find(ESPACO, x+1);

                if(x < linhas[y].length())
                    move(y, x);

                else
                    x = linhas[y].length();

                break;

            // selecionar linha única
            case CTRL_B:
                selecionar_linha('i');

                break;

            // selecionar todas as linhas do arquivo
            case CTRL_A:
                selecionar_todas_linhas('i');

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
*   Função membro para desenhar o conteúdo do buffer na
    tela
*   Caso tenha linhas não existentes no buffer, limpa
*   Cursor fica na posição original
*/
void molCode::imprimir()
{
    for(size_t i {}; i < static_cast<size_t>(LINES-1); ++i){
        move(i, 0);
        clrtoeol();
    }

    int linhas_visiveis {LINES-1};

    for(size_t i {}; i < static_cast<size_t>(linhas_visiveis); ++i){
        size_t linha {scroll_offset+i};

        if(linha < linhas.size()){
            mvprintw(i, 0, "%s", linhas[linha].c_str());
        }
    }

    int cursor_y = y - scroll_offset;

    if(cursor_y >= 0 && cursor_y < linhas_visiveis){
        move(cursor_y, x);
    }

    else{
        cursor_y = (cursor_y < 0) ? 0 : linhas_visiveis-1;
        move(cursor_y, x);
    }

    refresh();
}

// Função membro para mover cursor pra cima.
void molCode::cima()
{
    if(y > 0){
        --y;

        if(y < scroll_offset){
            scroll_offset = y;
        }
    }

    if(x >= linhas[y].length())
        x = linhas[y].length();

    imprimir();
}

/*
*   Função membro para mover cursor para a esquerda.
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
*   Função membro para mover cursor para a direita.
*   Caso o cursor chegue ao final da coluna e tenha texto
    abaixo, o cursor é movido pro início da próxima linha.
*/
void molCode::direita()
{
    if(x <= static_cast<size_t>(COLS) && x <= linhas[y].length()-1){
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

// Função membro para mover cursor pra baixo.
void molCode::baixo()
{
    if(y < linhas.size() - 1){
        ++y;

        int linhas_visiveis {LINES - 1};
        if(y >= scroll_offset + linhas_visiveis){
            scroll_offset = y - linhas_visiveis + 1;
        }
    }

    if(x >= linhas[y].length()){
        x = linhas[y].length();
    }

    imprimir();
}

// Função membro para abrir arquivo.
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
        else{ // joga exceção caso o arquivo tenha permissão especial para não modificar
            throw std::runtime_error("Não é possível abrir o arquivo. Permissão negada");
        }
    }
    else{
        std::string str {};
        ch_anexo(str); // não existindo o arquivo, cria o arquivo vazio
    }
}

// Função membro para editar nome do arquivo.
void molCode::editar_nome(const std::string& novo_nome)
{
    rename(&nome_arquivo[0], &novo_nome[0]);
    nome_arquivo = novo_nome;
}

// Função membro para salvar arquivo.
void molCode::salvar()
{
    std::ofstream ofile(nome_arquivo);

    if(ofile.is_open()){
        for(size_t i{}; i < linhas.size(); ++i){
            ofile << linhas[i]; // salva no arquivo de saída

            // Não insere enter ao final do arquivo
            if(i < linhas.size()-1){
                ofile << '\n';
            }
        }

        ofile.close();
    }
    else{ // joga exceção caso o arquivo tenha permissão especial para não modificar
        refresh();
        endwin();
        throw std::runtime_error("Não é possível abrir o arquivo. Permissão negada");
    }
}

// Função membro para buscar caminho absoluto
std::string molCode::caminho() const
{
    return std::filesystem::absolute(nome_arquivo);
}

// Função membro para remover um caractere
void molCode::ch_remover(const int numero)
{
    linhas.erase(linhas.begin() + numero);
}

// Função membro para manipular tab
std::string molCode::ch_tabs(std::string& linha) const
{
    std::size_t tab {linha.find('\t')};
    return (tab == linha.npos) ? linha : ch_tabs(linha.replace(tab, 1, "   "));
}

// Função membro para inserir caractere no meio da linha
void molCode::ch_inserir(std::string linha, int numero)
{
    linha = ch_tabs(linha);
    linhas.insert(linhas.begin() + numero, linha);
}

// Função membro para inserir ao final da linha
void molCode::ch_anexo(std::string& linha)
{
    linha = ch_tabs(linha);
    linhas.push_back(linha);
}

// Função membro para selecionar uma única linha
void molCode::selecionar_linha(const char modo_atual)
{
    init_pair(1, COLOR_WHITE, COLOR_BLACK);

    attron(A_REVERSE);
    attron(COLOR_PAIR(1));

    clrtoeol();

    mvprintw(y-scroll_offset, 0, "%s", linhas[y].c_str());

    refresh();

    int ch {getch()};

    switch(ch){
        case CTRL_X:
            copia = linhas[y];

            break;

        case KEY_BACKSPACE:
            if(linhas[y].length() > 0 && modo_atual == 'i'){
                x = linhas[y].length();

                if(y > 0 && y+1 < linhas.size()){
                    linhas[y] += linhas[y+1];
                }
                else{
                    linhas[y].erase(0, x);
                }

                ch_remover(y);
                cima();
            }
            break;

        default:
            break;
    }

    attroff(COLOR_PAIR(1));
    attroff(A_REVERSE);
}

// Função membro para selecionar uma todas as linhas
void molCode::selecionar_todas_linhas(const char modo_atual)
{
    init_pair(1, COLOR_WHITE, COLOR_BLACK);

    attron(A_REVERSE);
    attron(COLOR_PAIR(1));

    clrtoeol();

    for(size_t i{}; i < linhas.size(); i++){
        mvprintw(i - scroll_offset, 0, "%s", linhas[i].c_str());
    }

    int ch {getch()};

    switch(ch){
        case KEY_BACKSPACE:
        case 127:
        case '\b':
            if(modo_atual == 'i'){
                size_t i {linhas.size()-1};
                move(y, linhas[0].length());

                while(i > 0){
                    linhas[i].erase(0, linhas[i].length());
                    --i;

                    if(i > 0){
                        x = linhas[i-1].length();
                        linhas[i-1] += linhas[i];
                        ch_remover(i);
                        cima();
                    }
                }
                linhas.erase(linhas.begin());

                y = x = scroll_offset = 0;
                move(y, x);
            }
            break;
    }

    refresh();

    attroff(COLOR_PAIR(1));
    attroff(A_REVERSE);
}

// Função membro para definição de constantes
void molCode::definir_constantes()
{
    define_key("\033[1;5A", CTRL_CIMA);
    define_key("\033[1;5D", CTRL_ESQUERDA); // percorrer entre espaços [ ]
    define_key("\033[1;5C", CTRL_DIREITA); // percorrer entre espaços [X]
    define_key("\033[1;5B", CTRL_BAIXO);

    define_key("\033[1;6D", CTRL_SH_ESQUERDA);
    define_key("\033[1;6C", CTRL_SH_DIREITA); // inserir identação à direita [X]

    define_key("\x18", CTRL_X); // copiar linha inteira [X]
    define_key("\x16", CTRL_V); // colar linha copiada [X]
    define_key("\x02", CTRL_B); // selecionar toda a linha [X]
    define_key("\x01", CTRL_A); // selecionar todo o arquivo [X]
    define_key("\x0E", CTRL_N); // novo arquivo
}
