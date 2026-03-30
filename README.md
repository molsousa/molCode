# Criação de um editor de texto em C++

Esse repositório contém a implementação de um editor de texto em C++ utilizando a biblioteca `ncurses`.

## Funcionalidades

É possível utilizar o editor em dois modos de operação:

### Modo normal:

Nesse modo é possível:

- Selecionar linha atual.

- Selecionar todas as linhas.

- Exibir caminho absoluto.

- Editar nome do arquivo presente.

- Salvar.

- Salvar e Sair.

- Entrar no modo de inserir.

- Sair do editor.

---

### Modo inserir:

Nesse modo é possível:

- Selecionar linha atual.

- Selecionar todas as linhas.

- Movimentar string verticalmente (seja pra baixo, seja pra cima).

- Percorrer entre substrings separadas por espaço.

- Inserir identação movimentando a string a direita, ou remover identação a esquerda.

- Movimentar facilmente entre o início e fim da página.

- Movimentar facilmente entre o início e fim da string atual.

- Copiar e colar uma linha inteira.

- Remover substring separada por espaço.

## Interface

A interface é simples, as cores das linhas de comando são alteradas com base na ação aplicada no momento, por exemplo:

- Verde: para modo normal.

- Vermelho: para modo inserir.

- Branco: por 1500ms ao salvar sem sair e também para caminho absoluto.

## Expansão

Fica em aberto a possibilidade de expandir o editor, adicionando personalizações como identação personalizada, cores em palavras-chave, reconhecimento de pilha de procedimento para criar '*brackets*' personalizados e template de inicialização personalizado.


## Como utilizar

Para utilizar o editor, é preciso compilar utilizando o seguinte comando na pasta raiz:

```bash
make
```

Para executar a aplicação é necessário somente executar a seguinte linha na pasta em que o executável se encontra, a linha abaixo é pra criar um arquivo sem título e sem extensão:

```bash
./molCode.out
```

Para criar um arquivo com título e com a extensão desejada, deve-se executar a seguinte linha:

```bash
./molCode.out <arquivo>.<extensao>
```

---
