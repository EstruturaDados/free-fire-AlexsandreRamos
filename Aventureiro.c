// TEMA 2 - AVENTUREIRO
// Nesse código explorei uma pouca mais do fget() que nos outros níveis não havia usado. 
// Exclusão dos itens agora por NOME não mais por id

#include <stdio.h>   // funções de entrada/saída: printf, scanf, fgets   // inclui biblioteca para IO padrão
#include <stdlib.h>  // funções de alocação de memória: malloc, free, strtol // inclui stdlib para malloc/free/strtol etc.
#include <string.h>  // manipulação de strings: strcpy, strlen, strcmp     // inclui funções de string

// constantes fixas
#define MAX_ITENS 10  // número máximo de itens na mochila                     
#define NOME_TAM 30   // tamanho máximo do nome de cada item                      
#define TIPO_TAM 20   // tamanho máximo do tipo de cada item                      
#define BUF_TAM 128   // buffer temporário para leitura de strings               

// definição da struct Item
struct Item {
    char nome[NOME_TAM];  // nome do item                                         // campo para armazenar nome (string)
    char tipo[TIPO_TAM];  // tipo do item (arma, cura, munição etc.)              // campo para armazenar tipo (string)
    int quantidade;       // quantidade disponível                                // campo para armazenar quantidade (inteiro)
};

// definição da struct No para lista encadeada
struct No {
    struct Item dados;   // dados do item                                           // nó guarda um Item (por cópia)
    struct No *proximo;  // ponteiro para o próximo nó                             // ponteiro para o próximo nó na lista
};

// variáveis globais
struct Item *vetor[MAX_ITENS];  // mochila em vetor (array de ponteiros)               // vetor de ponteiros a Item, posições podem ser NULL
struct No *cabeca = NULL;       // cabeça da lista encadeada                           // ponteiro inicial da lista encadeada
int vetorOrdenado = 0;          // flag indica se vetor está ordenado                   // 0 = não ordenado, 1 = ordenado

// funções utilitárias

// limparBuffer: descarta caracteres restantes no stdin até '\n'
void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}   // consome caracteres até o final da linha ou EOF
}

// enter: espera o usuário pressionar ENTER
void enter() {
    printf("\n||PRESSIONE 'ENTER' PARA CONTINUAR||\n"); // instrução ao usuário
    char tmp[BUF_TAM];
    if (fgets(tmp, sizeof(tmp), stdin) == NULL) clearerr(stdin); // lê a linha (aguarda ENTER) e evita erro se EOF
}

// lerString: lê string do usuário de forma segura (remove '\n' final)
void lerString(const char *prompt, char *saida, int tam) {
    printf("%s", prompt);                                  // exibe prompt passado
    if (fgets(saida, tam, stdin) == NULL) { clearerr(stdin); saida[0] = '\0'; return; } // trata EOF/erro
    size_t len = strlen(saida);                           // obtém comprimento lido
    if (len > 0 && saida[len-1] == '\n') saida[len-1] = '\0'; // remove newline final se presente
    else { int c; while ((c = getchar()) != '\n' && c != EOF) {} } // se não cabeu tudo no buffer, limpa resto da linha
}

// lerInteiro: lê inteiro do usuário, repetindo até entrada válida
int lerInteiro(const char *prompt) {
    char buf[BUF_TAM];
    long val;
    char *fim;
    while (1) {
        printf("%s", prompt);                              // mostra o prompt para número
        if (fgets(buf, sizeof(buf), stdin) == NULL) { clearerr(stdin); return 0; } // trata EOF/erro retornando 0
        val = strtol(buf, &fim, 10);                      // converte string para long, fim aponta para fim da conversão
        if (fim == buf) printf("Entrada inválida. Digite um número.\n"); // se nada foi convertido => inválido
        else return (int)val;                              // retorna valor convertido (casting para int)
    }
}

// comparação simples de strings (case-sensitive)
#define strcmp_case_insensitive strcmp   // alias atual aponta para strcmp (não altera comportamento) // placeholder para comparação

// ---------- MOCHILA VETOR ----------

// inicializa vetor: todas posições NULL
void inicializarVetor() {
    for (int i = 0; i < MAX_ITENS; i++) vetor[i] = NULL; // define cada posição do vetor como NULL
    vetorOrdenado = 0;                                  // marca vetor como não ordenado
}

// listarVetor: percorre vetor, imprime itens ou "Vazio"
void listarVetor() {
    printf("\n|| MOCHILA (VETOR) ||\n");                  // cabeçalho de listagem
    int total = 0;
    for (int i = 0; i < MAX_ITENS; i++) {
        if (vetor[i] != NULL) {                         // se há um Item alocado naquela posição
            printf("%d - NOME: %s | TIPO: %s | QUANTIDADE: %d\n",
                   i + 1, vetor[i]->nome, vetor[i]->tipo, vetor[i]->quantidade); // imprime detalhes do item
            total++;                                    // incrementa contador de itens existentes
        } else printf("%d - Vazio\n", i + 1);           // se NULL, imprime "Vazio" para essa posição
    }
    printf("Total de itens: %d\n", total);              // imprime total de itens encontrados
    printf("Vetor ordenado: %s\n", vetorOrdenado ? "SIM" : "NAO"); // mostra se vetor está marcado como ordenado
}

// inserirVetor: encontra primeiro NULL, aloca Item, lê dados
void inserirVetor() {
    int pos = -1;
    for (int i = 0; i < MAX_ITENS; i++) if (vetor[i] == NULL) { pos = i; break; } // procura primeira posição livre
    if (pos == -1) { printf("\n||MOCHILA CHEIA||\n"); return; } // se não encontrou espaço, avisa e retorna

    struct Item *novo = malloc(sizeof(struct Item)); // aloca memória para um novo Item
    if (!novo) { printf("Erro: falha na alocação.\n"); return; } // trata falha de malloc

    lerString("DIGITE O NOME DO ITEM: ", novo->nome, NOME_TAM);   // lê nome do item com fgets via função segura
    lerString("DIGITE O TIPO DO ITEM: ", novo->tipo, TIPO_TAM);   // lê tipo do item
    novo->quantidade = lerInteiro("DIGITE A QUANTIDADE: ");       // lê quantidade via lerInteiro

    vetor[pos] = novo;             // guarda o ponteiro do novo item na posição encontrada
    vetorOrdenado = 0;             // ao inserir, vetor pode deixar de estar ordenado
    printf("Item adicionado na posição %d.\n", pos + 1); // confirma posição adicionada
}

// buscarSequencialVetor: busca nome no vetor, retorna índice ou -1
int buscarSequencialVetor(const char *nome, int *comparacoes) {
    *comparacoes = 0;               // inicializa contador de comparações
    for (int i = 0; i < MAX_ITENS; i++) {
        if (vetor[i] != NULL) {     // só compara nas posições preenchidas
            (*comparacoes)++;       // incrementa comparações realizadas
            if (strcmp(vetor[i]->nome, nome) == 0) return i; // compara nomes (case-sensitive) e retorna índice se igual
        }
    }
    return -1;                      // retorna -1 se não encontrou
}

// removerVetor: pede nome, busca sequencial, libera memória se encontrado
void removerVetor() {
    char busca[NOME_TAM];
    lerString("DIGITE O NOME DO ITEM QUE DESEJA EXCLUIR (ou vazio p/ cancelar): ", busca, NOME_TAM); // lê nome a remover
    if (strlen(busca) == 0) { printf("Remoção cancelada.\n"); return; } // se vazio, cancela remoção

    int comps = 0;
    int idx = buscarSequencialVetor(busca, &comps); // realiza busca sequencial e obtém número de comparações
    printf("Comparações na busca sequencial: %d\n", comps); // mostra quantas comparações foram feitas
    if (idx >= 0) { free(vetor[idx]); vetor[idx] = NULL; vetorOrdenado = 0; printf("Item removido da posição %d.\n", idx + 1); } // se encontrado, libera memória e marca NULL
    else printf("Item NÃO encontrado.\n"); // se não encontrado, informa
}

// bubbleSortVetor: ordena vetor preenchido, compacta buracos
void bubbleSortVetor() {
    int write = 0;
    for (int read = 0; read < MAX_ITENS; read++) if (vetor[read] != NULL) {
        if (read != write) { vetor[write] = vetor[read]; vetor[read] = NULL; } // move itens para frente (compacta)
        write++;
    }
    int n = write;                   // n = quantidade de itens reais após compactação
    if (n <= 1) { vetorOrdenado = 1; printf("Ordenação concluída (itens < 2 ou vazio).\n"); return; } // nada a ordenar

    for (int i = 0; i < n-1; i++) {  // loop externo do bubble sort
        int trocou = 0;
        for (int j = 0; j < n-1-i; j++) { // loop interno
            if (strcmp(vetor[j]->nome, vetor[j+1]->nome) > 0) { // compara nomes e troca se fora de ordem
                struct Item *tmp = vetor[j]; vetor[j] = vetor[j+1]; vetor[j+1] = tmp; // efetua troca de ponteiros
                trocou = 1;
            }
        }
        if (!trocou) break;           // se não houve trocas, já está ordenado - sai cedo
    }
    vetorOrdenado = 1;               // marca vetor como ordenado
    printf("Vetor ordenado por nome (Bubble Sort).\n"); // mensagem indicando sucesso
}

// buscaBinariaVetor: somente se vetor ordenado, retorna índice ou -1
int buscaBinariaVetor(const char *nome, int *comparacoes) {
    *comparacoes = 0;
    if (!vetorOrdenado) return -1;   // se vetor não estiver ordenado, não realiza busca binária
    int n = 0; for (int i = 0; i < MAX_ITENS; i++) if (vetor[i] != NULL) n++; else break; // conta itens consecutivos do início
    int low = 0, high = n-1;
    while (low <= high) {
        int mid = low + (high - low)/2;
        (*comparacoes)++;            // conta comparação feita na etapa
        int cmp = strcmp(vetor[mid]->nome, nome); // compara valor do meio
        if (cmp == 0) return mid;    // encontrado
        else if (cmp < 0) low = mid+1; // procura na metade superior
        else high = mid-1;           // procura na metade inferior
    }
    return -1;                       // não encontrado
}

// liberarVetor: libera memória de todos itens
void liberarVetor() {
    for (int i = 0; i < MAX_ITENS; i++) if (vetor[i] != NULL) { free(vetor[i]); vetor[i] = NULL; } // libera cada item não-nulo
    vetorOrdenado = 0;               // reseta flag de ordenação
}

// ---------- MOCHILA LISTA ENCADEADA ----------

// listarLista: percorre lista e imprime todos nós
void listarLista() {
    printf("\n|| MOCHILA (LISTA ENCADEADA) ||\n"); // cabeçalho
    if (!cabeca) { printf("Lista vazia.\n"); return; } // se cabeça é NULL, lista vazia
    struct No *p = cabeca; int pos = 1;
    while (p) { printf("%d - NOME: %s | TIPO: %s | QUANTIDADE: %d\n", pos, p->dados.nome, p->dados.tipo, p->dados.quantidade); p = p->proximo; pos++; } // percorre e imprime cada nó
    printf("Total de itens na lista: %d\n", pos-1); // imprime total (pos-1 porque pos foi incrementado após último)
}

// inserirLista: cria nó, lê dados, anexa ao final
void inserirLista() {
    struct No *novo = malloc(sizeof(struct No)); // aloca novo nó
    if (!novo) { printf("Erro: falha na alocação.\n"); return; } // trata falha de malloc
    lerString("DIGITE O NOME DO ITEM: ", novo->dados.nome, NOME_TAM); // lê nome para o campo dados.nome do nó
    lerString("DIGITE O TIPO DO ITEM: ", novo->dados.tipo, TIPO_TAM); // lê tipo
    novo->dados.quantidade = lerInteiro("DIGITE A QUANTIDADE: "); // lê quantidade
    novo->proximo = NULL; // nó novo aponta para NULL (final da lista)
    if (!cabeca) cabeca = novo; // se lista vazia, novo nó vira cabeça
    else { struct No *p = cabeca; while (p->proximo) p = p->proximo; p->proximo = novo; } // caso contrário, anexa ao fim
    printf("Item adicionado na lista encadeada.\n"); // confirma inserção
}

// buscarSequencialLista: percorre lista, retorna ponteiro ou NULL
struct No* buscarSequencialLista(const char *nome, int *comparacoes) {
    *comparacoes = 0;               // zera contador de comparações
    struct No *p = cabeca;
    while (p) { (*comparacoes)++; if (strcmp(p->dados.nome, nome) == 0) return p; p = p->proximo; } // percorre e compara nome por nó
    return NULL;                    // retorna NULL se não encontrar
}

// removerLista: remove nó com determinado nome, ajusta ponteiros
void removerLista() {
    char busca[NOME_TAM];
    lerString("DIGITE O NOME DO ITEM QUE DESEJA EXCLUIR (ou vazio p/ cancelar): ", busca, NOME_TAM); // lê nome a remover
    if (strlen(busca) == 0) { printf("Remoção cancelada.\n"); return; } // cancela se entrada vazia

    struct No *p = cabeca, *ant = NULL;
    int comps = 0;
    while (p) {
        comps++;
        if (strcmp(p->dados.nome, busca) == 0) { // achou o nó que deve ser removido
            if (!ant) cabeca = p->proximo;      // se é o primeiro nó, atualiza cabeça
            else ant->proximo = p->proximo;    // senão, ajusta ponteiro do anterior
            free(p);                            // libera memória do nó removido
            printf("Item removido. Comparações até encontrar: %d\n", comps); // informa quantas comparações foram feitas
            return;
        }
        ant = p; p = p->proximo;                // avança na lista, mantendo ponteiro anterior
    }
    printf("Item NÃO encontrado. Comparações: %d\n", comps); // se saiu do loop sem remover, item não existe
}

// liberarLista: libera todos nós da lista
void liberarLista() {
    struct No *p = cabeca;
    while (p) { struct No *tmp = p; p = p->proximo; free(tmp); } // percorre e libera cada nó
    cabeca = NULL; // deixa cabeça como NULL após liberar
}

// ---------- MENU PRINCIPAL ----------
int main() {
    inicializarVetor();  // inicializa vetor com NULL                       // prepara estrutura do vetor
    while (1) {
        printf("\n|| MENU PRINCIPAL - ESCOLHA A ESTRUTURA ||\n"); // menu principal
        printf("(1) Mochila com VETOR (lista sequencial)\n");
        printf("(2) Mochila com LISTA ENCADEADA\n");
        printf("(3) Sair\n");
        int escolha = lerInteiro("DIGITE AQUI: "); // lê escolha do usuário

        if (escolha == 1) {
            while (1) {
                // menu da mochila vetor
                printf("\n--- MOCHILA (VETOR) ---\n");
                printf("(1) Abrir/Mostrar Mochila\n");
                printf("(2) Inserir Item\n");
                printf("(3) Remover Item por NOME\n");
                printf("(4) Buscar (Sequencial)\n");
                printf("(5) Ordenar (Bubble Sort)\n");
                printf("(6) Buscar (Binária) [apenas se ordenado]\n");
                printf("(7) Voltar\n");
                int op = lerInteiro("Escolha: "); // lê opção de operação no submenu vetor

                if (op == 1) { listarVetor(); enter(); } // mostra mochila e espera ENTER
                else if (op == 2) { inserirVetor(); enter(); } // insere novo item e espera ENTER
                else if (op == 3) { listarVetor(); removerVetor(); enter(); } // lista, remove por nome e espera ENTER
                else if (op == 4) {
                    char nome[NOME_TAM]; lerString("DIGITE O NOME PARA BUSCA (sequencial): ", nome, NOME_TAM); // lê nome a buscar
                    if (strlen(nome) == 0) printf("Busca cancelada.\n"); // cancela se vazio
                    else {
                        int comps = 0;
                        int idx = buscarSequencialVetor(nome, &comps); // realiza busca sequencial e conta comparações
                        printf("Comparações na busca sequencial: %d\n", comps); // imprime comparações
                        if (idx >= 0) printf("Item encontrado na posição %d: %s | %s | %d\n", idx + 1, vetor[idx]->nome, vetor[idx]->tipo, vetor[idx]->quantidade); // mostra item
                        else printf("Item NÃO encontrado.\n"); // avisa não encontrado
                    }
                    enter(); // espera ENTER
                }
                else if (op == 5) { bubbleSortVetor(); enter(); } // ordena vetor (e compacta) e espera ENTER
                else if (op == 6) {
                    if (!vetorOrdenado) printf("Vetor NÃO está ordenado. Ordene antes de usar busca binária.\n"); // exige ordenação
                    else {
                        char nome[NOME_TAM]; lerString("DIGITE O NOME PARA BUSCA (binária): ", nome, NOME_TAM); // lê nome para busca binária
                        if (strlen(nome) == 0) printf("Busca cancelada.\n"); // cancela se vazio
                        else {
                            int comps = 0;
                            int idx = buscaBinariaVetor(nome, &comps); // executa busca binária e retorna índice
                            printf("Comparações na busca binária: %d\n", comps); // imprime comparações realizadas
                            if (idx >= 0) printf("Item encontrado na posição %d: %s | %s | %d\n", idx + 1, vetor[idx]->nome, vetor[idx]->tipo, vetor[idx]->quantidade); // mostra item
                            else printf("Item NÃO encontrado.\n");
                        }
                    }
                    enter(); // espera ENTER
                }
                else if (op == 7) break; // volta ao menu principal
                else printf("Opção inválida.\n"); // entrada inválida
            }

        } else if (escolha == 2) {
            while (1) {
                // menu da mochila lista encadeada
                printf("\n--- MOCHILA (LISTA ENCADEADA) ---\n");
                printf("(1) Abrir/Mostrar Mochila\n");
                printf("(2) Inserir Item\n");
                printf("(3) Remover Item por NOME\n");
                printf("(4) Buscar (Sequencial)\n");
                printf("(5) Voltar\n");
                int op = lerInteiro("Escolha: "); // lê opção do submenu lista

                if (op == 1) { listarLista(); enter(); } // mostra lista e espera ENTER
                else if (op == 2) { inserirLista(); enter(); } // insere nó ao final e espera ENTER
                else if (op == 3) { listarLista(); removerLista(); enter(); } // lista, remove por nome e espera ENTER
                else if (op == 4) {
                    char nome[NOME_TAM]; lerString("DIGITE O NOME PARA BUSCA (sequencial): ", nome, NOME_TAM); // lê nome para busca na lista
                    if (strlen(nome) == 0) printf("Busca cancelada.\n"); // cancela se vazio
                    else {
                        int comps = 0;
                        struct No *res = buscarSequencialLista(nome, &comps); // busca sequencial na lista e conta comparações
                        printf("Comparações na busca sequencial (lista): %d\n", comps); // imprime comparações
                        if (res != NULL) printf("Item encontrado: %s | %s | %d\n", res->dados.nome, res->dados.tipo, res->dados.quantidade); // mostra item encontrado
                        else printf("Item NÃO encontrado.\n"); // não encontrado
                    }
                    enter(); // espera ENTER
                }
                else if (op == 5) break; // volta ao menu principal
                else printf("Opção inválida.\n"); // opção inválida
            }

        } else if (escolha == 3) {
            liberarVetor(); // libera memória dos itens do vetor
            liberarLista(); // libera memória dos nós da lista
            printf("Saindo... memória liberada.\n"); // mensagem final
            break; // encerra loop principal e termina o programa
        } else {
            printf("Opção inválida no menu principal.\n"); // entrada inválida no menu principal
        }
    }

    return 0; // finaliza main com código 0 (sucesso)
}
