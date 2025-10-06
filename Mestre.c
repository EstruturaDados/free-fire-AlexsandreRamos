// TEMA 2 - MESTRE

#include <stdio.h>   // funções de entrada/saída: printf, fgets, getchar
#include <stdlib.h>  // alocação dinâmica: malloc, free, realloc; conversão de string: strtol
#include <string.h>  // manipulação de strings: strncpy, strcmp, strlen
#include <time.h>    // medir tempo de execução: clock(), CLOCKS_PER_SEC
#include <ctype.h>   // manipulação de caracteres: tolower

#define MAX_COMPONENTES 20   // número máximo de componentes permitidos
#define NOME_TAM 30          // tamanho máximo do campo 'nome' de um componente
#define TIPO_TAM 20          // tamanho máximo do campo 'tipo' de um componente
#define BUF_TAM 128          // tamanho de buffer temporário para leitura de strings

// Struct que representa um componente necessário para montar a torre
typedef struct {
    char nome[NOME_TAM];    // nome do componente (ex: "chip central")
    char tipo[TIPO_TAM];    // tipo do componente (ex: "controle")
    int prioridade;         // prioridade do componente: 1 a 10 (10 = mais crítico)
} Componente;

// Nó da lista encadeada que guarda um Componente
typedef struct No {
    Componente dados;       // cópia do componente armazenado no nó
    struct No *proximo;     // ponteiro para o próximo nó da lista
} No;

// Estrutura do vetor dinâmico
typedef struct {
    Componente **arr;       // array de ponteiros para Componente
    int size;               // número atual de elementos no vetor
    int capacity;           // capacidade atual do vetor (aumenta via realloc)
    int ordenado_por_nome;  // flag: 1 se o vetor estiver ordenado por nome
} VetorDin;

// ---------- FUNÇÕES DE UTILITÁRIO DE I/O ----------

// Limpa o buffer do teclado (descarta todos os caracteres até '\n')
void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {} // lê e descarta
}

// Lê uma string segura do usuário, removendo '\n' ao final
void lerString(const char *prompt, char *saida, int tam) {
    printf("%s", prompt);                               // mostra prompt
    if (fgets(saida, tam, stdin) == NULL) {             // lê linha
        clearerr(stdin);                                // reseta flags de erro
        saida[0] = '\0';                                // string vazia se erro
        return;
    }
    size_t len = strlen(saida);
    if (len > 0 && saida[len-1] == '\n') saida[len-1] = '\0'; // remove '\n'
    else {                                              // se string excedeu buffer
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {} // limpa restante
    }
}

// Lê um inteiro com validação de intervalo [min, max]
int lerInteiro(const char *prompt, int min, int max) {
    char buf[BUF_TAM]; 
    long val; 
    char *fim;
    while (1) {
        printf("%s", prompt);
        if (fgets(buf, sizeof(buf), stdin) == NULL) { clearerr(stdin); return min; } // trata EOF
        val = strtol(buf, &fim, 10);                    // converte string para long
        if (fim == buf) {                               // nenhum dígito válido
            printf("Entrada inválida. Digite um número.\n");
            continue;
        }
        if (val < min || val > max) {                   // fora do intervalo permitido
            printf("Valor fora do intervalo [%d - %d]. Tente novamente.\n", min, max);
            continue;
        }
        return (int)val;                                // valor válido
    }
}

// Imprime cabeçalho de tabela para exibição de componentes
void imprimirCabecalho() {
    printf("ID | %-28s | %-15s | PR\n", "NOME", "TIPO");
    printf("-------------------------------------------------------------\n");
}

// Imprime um componente no formato tabular
void imprimirComponente(const Componente *c, int id) {
    printf("%2d | %-28s | %-15s | %2d\n", id, c->nome, c->tipo, c->prioridade);
}

// Comparação de strings case-insensitive que também conta comparações (para estatística)
int strcmp_ci_count(const char *a, const char *b, long *comparacoes) {
    if (comparacoes) (*comparacoes)++;                  // conta 1 comparação conceitual
    while (*a && *b) {
        char ca = tolower((unsigned char)*a);          // normaliza caractere a
        char cb = tolower((unsigned char)*b);          // normaliza caractere b
        if (ca != cb) return (int)(unsigned char)ca - (int)(unsigned char)cb; // retorna diferença
        a++; b++;                                     // avança para próximo caractere
    }
    return (int)(unsigned char)tolower((unsigned char)*a) - (int)(unsigned char)tolower((unsigned char)*b);
}

// ---------- FUNÇÕES DO VETOR DINÂMICO ----------

// Cria e inicializa um vetor dinâmico
VetorDin* criarVetor() {
    VetorDin *v = malloc(sizeof(VetorDin));           // aloca struct
    if (!v) return NULL;                               // falha na alocação
    v->capacity = 4;                                   // capacidade inicial pequena
    v->arr = malloc(sizeof(Componente*) * v->capacity); // aloca array de ponteiros
    v->size = 0;                                       // nenhum elemento ainda
    v->ordenado_por_nome = 0;                          // não ordenado inicialmente
    return v;
}

// Libera todos os componentes alocados e o próprio vetor
void liberarVetorDin(VetorDin *v) {
    if (!v) return;
    for (int i = 0; i < v->size; i++) if (v->arr[i]) free(v->arr[i]); // libera cada Componente
    free(v->arr);                                       // libera array de ponteiros
    free(v);                                            // libera struct do vetor
}

// Garante capacidade mínima do vetor (realloc se necessário)
int vetor_reserve(VetorDin *v, int newcap) {
    if (newcap <= v->capacity) return 1;               // já tem capacidade suficiente
    if (newcap > MAX_COMPONENTES) return 0;           // não ultrapassa limite máximo
    Componente **tmp = realloc(v->arr, sizeof(Componente*) * newcap); // realoca array
    if (!tmp) return 0;                                // falha realloc
    v->arr = tmp; v->capacity = newcap;
    return 1;
}

// Adiciona um componente ao vetor
int vetor_push(VetorDin *v, Componente *c) {
    if (v->size >= MAX_COMPONENTES) return 0;         // limite atingido
    if (v->size >= v->capacity) {                     // precisa crescer
        int newcap = v->capacity * 2;
        if (newcap > MAX_COMPONENTES) newcap = MAX_COMPONENTES;
        if (!vetor_reserve(v, newcap)) return 0;
    }
    v->arr[v->size++] = c;                            // adiciona ponteiro e incrementa size
    v->ordenado_por_nome = 0;                         // vetor não mais garantidamente ordenado
    return 1;
}

// Remove componente por nome (case-sensitive)
int vetor_remove_por_nome(VetorDin *v, const char *nome) {
    for (int i = 0; i < v->size; i++) {
        if (strcmp(v->arr[i]->nome, nome) == 0) {     // achou componente
            free(v->arr[i]);                          // libera Componente
            for (int j = i; j < v->size-1; j++) v->arr[j] = v->arr[j+1]; // compacta array
            v->size--;
            v->ordenado_por_nome = 0;                 // ordenação inválida
            return 1;
        }
    }
    return 0;                                         // não encontrado
}

// Lista todos os componentes do vetor
void vetor_listar(VetorDin *v) {
    printf("\n|| MOCHILA (VETOR DINAMICO) ||\n");
    if (v->size == 0) { printf("Vetor vazio.\n"); return; }
    imprimirCabecalho();
    for (int i = 0; i < v->size; i++) imprimirComponente(v->arr[i], i+1);
    printf("Total: %d | Ordenado por nome: %s\n", v->size, v->ordenado_por_nome?"SIM":"NAO");
}
// ---------- FUNÇÕES DA LISTA ENCADEADA ----------

// Insere um novo nó no final da lista, copiando os dados do componente
No* inserir_lista_no_final(No *cabeca, const Componente *c) {
    No *novo = malloc(sizeof(No));           // aloca nó
    if (!novo) return cabeca;                // se falha alocação, retorna lista original
    novo->dados = *c;                        // copia os dados do componente
    novo->proximo = NULL;                    // marca como último nó
    if (!cabeca) return novo;                // se lista vazia, novo nó vira cabeça
    No *p = cabeca;                          // percorre lista
    while (p->proximo) p = p->proximo;      // vai até último nó
    p->proximo = novo;                       // anexa novo nó ao final
    return cabeca;                           // retorna cabeça (inalterada)
}

// Remove primeiro nó que coincida com o nome (case-sensitive)
No* remover_lista_por_nome(No *cabeca, const char *nome, int *removido) {
    No *p = cabeca, *ant = NULL;
    while (p) {                               // percorre lista
        if (strcmp(p->dados.nome, nome) == 0) { // achou componente
            if (!ant) { cabeca = p->proximo; free(p); } // remove cabeça
            else { ant->proximo = p->proximo; free(p); } // remove nó do meio/fim
            if (removido) *removido = 1;      // sinaliza remoção
            return cabeca;
        }
        ant = p; p = p->proximo;              // avança na lista
    }
    if (removido) *removido = 0;             // não encontrado
    return cabeca;
}

// Lista todos os nós da lista encadeada
void listar_lista(No *cabeca) {
    printf("\n|| MOCHILA (LISTA ENCADEADA) ||\n");
    if (!cabeca) { printf("Lista vazia.\n"); return; }
    imprimirCabecalho();
    int id = 1;
    No *p = cabeca;
    while (p) {                               // percorre lista
        imprimirComponente(&p->dados, id++);  // imprime dados
        p = p->proximo;                       // avança
    }
    printf("Total: %d\n", id-1);
}

// Libera todos os nós da lista
void liberar_lista(No *cabeca) {
    No *p = cabeca;
    while (p) {
        No *tmp = p;                          // guarda nó atual
        p = p->proximo;                       // avança
        free(tmp);                             // libera nó
    }
}

// ---------- ORDENAÇÕES NO VETOR ----------

// Bubble Sort por nome (case-insensitive) para vetor
double bubbleSortVetor_porNome(VetorDin *v, long *comparacoes) {
    *comparacoes = 0; 
    clock_t inicio = clock();                  // marca início tempo
    int n = v->size;
    for (int i = 0; i < n-1; i++) {
        int trocou = 0;
        for (int j = 0; j < n-1-i; j++) {
            int cmp = strcmp_ci_count(v->arr[j]->nome, v->arr[j+1]->nome, comparacoes); // compara nomes
            if (cmp > 0) {                      // se fora de ordem
                Componente *tmp = v->arr[j]; v->arr[j] = v->arr[j+1]; v->arr[j+1] = tmp; // troca ponteiros
                trocou = 1;                     // marcou troca
            }
        }
        if (!trocou) break;                     // otimização: se não houve troca, já ordenado
    }
    v->ordenado_por_nome = 1;                   // marca vetor como ordenado por nome
    clock_t fim = clock(); 
    return (double)(fim - inicio)/CLOCKS_PER_SEC; // retorna tempo gasto
}

// Insertion Sort por tipo (case-insensitive) para vetor
double insertionSortVetor_porTipo(VetorDin *v, long *comparacoes) {
    *comparacoes = 0; 
    clock_t inicio = clock();
    int n = v->size;
    for (int i = 1; i < n; i++) {
        Componente *key = v->arr[i]; 
        int j = i-1;
        while (j >= 0) {
            int cmp = strcmp_ci_count(v->arr[j]->tipo, key->tipo, comparacoes); // compara tipo
            if (cmp <= 0) break;                 // posição correta encontrada
            v->arr[j+1] = v->arr[j];             // desloca elemento
            j--;
        }
        v->arr[j+1] = key;                       // insere elemento
    }
    v->ordenado_por_nome = 0;                   // vetor não está mais ordenado por nome
    clock_t fim = clock();
    return (double)(fim - inicio)/CLOCKS_PER_SEC;
}

// Selection Sort por prioridade (maior prioridade primeiro) para vetor
double selectionSortVetor_porPrioridade(VetorDin *v, long *comparacoes) {
    *comparacoes = 0; 
    clock_t inicio = clock();
    int n = v->size;
    for (int i = 0; i < n-1; i++) {
        int idxMax = i;
        for (int j = i+1; j < n; j++) {
            (*comparacoes)++;                   // conta comparação
            if (v->arr[j]->prioridade > v->arr[idxMax]->prioridade) idxMax = j; // busca maior
        }
        if (idxMax != i) {                      // troca se necessário
            Componente *tmp = v->arr[i]; v->arr[i] = v->arr[idxMax]; v->arr[idxMax] = tmp;
        }
    }
    v->ordenado_por_nome = 0;
    clock_t fim = clock();
    return (double)(fim - inicio)/CLOCKS_PER_SEC;
}

// ---------- BUSCA BINÁRIA NO VETOR ----------

// Busca binária por nome (somente se vetor estiver ordenado por nome)
int buscaBinariaVetor_porNome(VetorDin *v, const char *chave, long *comparacoes) {
    *comparacoes = 0;
    if (!v->ordenado_por_nome) return -2;       // erro se não ordenado
    int low = 0, high = v->size - 1;
    while (low <= high) {
        int mid = low + (high - low)/2;
        int cmp = strcmp_ci_count(v->arr[mid]->nome, chave, comparacoes); // compara nome
        if (cmp == 0) return mid;              // encontrado
        else if (cmp < 0) low = mid + 1;       // busca à direita
        else high = mid - 1;                   // busca à esquerda
    }
    return -1;                                 // não encontrado
}
// ---------- ORDENAÇÕES NA LISTA ----------

// Bubble Sort por nome (case-insensitive) para lista encadeada
// Troca os dados entre nós em vez de rearranjar nós
double bubbleSortLista_porNome(No *cabeca, long *comparacoes) {
    *comparacoes = 0; 
    if (!cabeca) return 0.0;                  // lista vazia
    clock_t inicio = clock();
    int trocou;
    do {
        trocou = 0;
        No *p = cabeca;
        while (p->proximo) {
            int cmp = strcmp_ci_count(p->dados.nome, p->proximo->dados.nome, comparacoes);
            if (cmp > 0) {                     // fora de ordem
                Componente tmp = p->dados;     // troca dados entre nós
                p->dados = p->proximo->dados;
                p->proximo->dados = tmp;
                trocou = 1;                    // houve troca
            }
            p = p->proximo;                    // avança
        }
    } while (trocou);                           // repete enquanto houver troca
    clock_t fim = clock();
    return (double)(fim - inicio)/CLOCKS_PER_SEC;
}

// Insertion Sort por tipo para lista encadeada
// Cria uma lista auxiliar ordenada e depois copia os dados de volta
double insertionSortLista_porTipo(No *cabeca, long *comparacoes) {
    *comparacoes = 0; 
    if (!cabeca) return 0.0;
    clock_t inicio = clock();
    No *sorted = NULL;                          // lista auxiliar
    No *current = cabeca;
    while (current) {
        No *next = current->proximo;           // salva próximo
        // inserir current->dados em sorted na posição correta (tipo)
        if (!sorted || strcmp_ci_count(sorted->dados.tipo, current->dados.tipo, comparacoes) > 0) {
            No *nnew = malloc(sizeof(No));
            nnew->dados = current->dados;
            nnew->proximo = sorted;
            sorted = nnew;                      // insere no início
        } else {
            No *s = sorted;
            while (s->proximo && strcmp_ci_count(s->proximo->dados.tipo, current->dados.tipo, comparacoes) <= 0) {
                s = s->proximo;                // percorre até achar posição
            }
            No *nnew = malloc(sizeof(No));
            nnew->dados = current->dados;
            nnew->proximo = s->proximo;
            s->proximo = nnew;                 // insere no meio/fim
        }
        current = next;                         // avança no input original
    }
    // copia dados da lista auxiliar para a lista original (mantendo nós originais)
    No *p = cabeca; No *s = sorted;
    while (p && s) { p->dados = s->dados; p = p->proximo; s = s->proximo; }
    // libera lista auxiliar
    s = sorted; while (s) { No *t = s; s = s->proximo; free(t); }
    clock_t fim = clock();
    return (double)(fim - inicio)/CLOCKS_PER_SEC;
}

// Selection Sort por prioridade (maior primeiro) para lista encadeada
double selectionSortLista_porPrioridade(No *cabeca, long *comparacoes) {
    *comparacoes = 0;
    if (!cabeca) return 0.0;
    clock_t inicio = clock();
    for (No *i = cabeca; i && i->proximo; i = i->proximo) {   // percorre cada nó
        No *max = i;
        for (No *j = i->proximo; j; j = j->proximo) {
            (*comparacoes)++;                                  // conta comparação
            if (j->dados.prioridade > max->dados.prioridade) max = j; // encontra maior prioridade
        }
        if (max != i) {                                       // troca dados se necessário
            Componente tmp = i->dados;
            i->dados = max->dados;
            max->dados = tmp;
        }
    }
    clock_t fim = clock();
    return (double)(fim - inicio)/CLOCKS_PER_SEC;
}

// ---------- FUNÇÃO DE CRIAÇÃO DE COMPONENTE ----------

// Cria um Componente alocado no heap, copiando nome, tipo e prioridade
Componente* criar_componente(const char *nome, const char *tipo, int prioridade) {
    Componente *c = malloc(sizeof(Componente));
    if (!c) return NULL;                                      // erro de alocação
    strncpy(c->nome, nome, NOME_TAM-1); c->nome[NOME_TAM-1] = '\0'; // copia nome com truncamento
    strncpy(c->tipo, tipo, TIPO_TAM-1); c->tipo[TIPO_TAM-1] = '\0'; // copia tipo com truncamento
    c->prioridade = prioridade;                                // seta prioridade
    return c;                                                  // retorna ponteiro
}

// ---------- FUNÇÃO MAIN: FLUXO PRINCIPAL ----------

int main() {
    printf("=== TORRE DE RESGATE: MODO HIBRIDO (VETOR DINAMICO + LISTA) ===\n");

    VetorDin *vet = criarVetor();        // cria vetor dinâmico
    No *cabeca = NULL;                   // lista encadeada vazia
    if (!vet) { printf("Erro: falha ao criar vetor.\n"); return 1; }

    // cadastro inicial
    printf("Cadastre até %d componentes. Deixe o NOME vazio para encerrar o cadastro.\n", MAX_COMPONENTES);
    while (vet->size < MAX_COMPONENTES) {
        char nome[BUF_TAM]; lerString("Nome do componente (ou vazio para terminar): ", nome, BUF_TAM);
        if (strlen(nome) == 0) break;      // se vazio, encerra cadastro
        char tipo[BUF_TAM]; lerString("Tipo (ex: controle, suporte, propulsao): ", tipo, BUF_TAM);
        int pr = lerInteiro("Prioridade (1..10): ", 1, 10); // lê prioridade
        Componente *c = criar_componente(nome, tipo, pr);
        if (!c) { printf("Erro de alocação.\n"); break; }
        if (!vetor_push(vet, c)) { printf("Vetor cheio! Não é possível adicionar mais.\n"); free(c); break; }
        cabeca = inserir_lista_no_final(cabeca, c); // adiciona também à lista
        printf("Componente cadastrado. Total agora: %d\n", vet->size);
    }

    if (vet->size == 0) { printf("Nenhum componente cadastrado. Encerrando.\n"); liberarVetorDin(vet); liberar_lista(cabeca); return 0; }

    // fluxo do menu principal
    int estrutura = 0; // 1 = vetor, 2 = lista
    int sair = 0;
    while (!sair) {
        printf("\n--- MENU PRINCIPAL ---\n");
        printf("Escolha a estrutura para operar:\n");
        printf("1) Vetor dinâmico (rápido para buscas e ordenações)\n");
        printf("2) Lista encadeada (flexível em inserções/remocoes)\n");
        printf("3) Exibir ambas as estruturas\n");
        printf("4) Sair\n");
        int op = lerInteiro("Escolha: ", 1, 4); // lê opção do menu principal

        // manipulação de submenus para vetor ou lista
        if (op == 1 || op == 2) {
            estrutura = op;                   // define estrutura escolhida
            int voltar = 0;
            while (!voltar) {                 // submenu
                printf("\n--- MENU %s ---\n", (estrutura==1)?"VETOR":"LISTA");
                printf("(1) Listar componentes\n");
                printf("(2) Inserir componente\n");
                printf("(3) Remover componente por NOME\n");
                printf("(4) Ordenar e medir (Bubble/In/Selection)\n");
                printf("(5) Buscar componente-chave (apenas vetor ordenado por NOME)\n");
                printf("(6) Voltar\n");
                int escolha = lerInteiro("Opcao: ", 1, 6);

                // cada opção do submenu será detalhada na próxima etapa
                // --- SUBMENU OPÇÕES ---

                if (escolha == 1) {  
                    // Listar componentes
                    if (estrutura == 1) 
                        vetor_listar(vet);     // chama função que percorre vetor e imprime cada componente
                    else 
                        listar_lista(cabeca);  // percorre lista encadeada e imprime cada nó
                } 
                else if (escolha == 2) { 
                    // Inserir novo componente
                    if (vet->size >= MAX_COMPONENTES) { 
                        printf("Capacidade maxima atingida (%d).\n", MAX_COMPONENTES); 
                        continue;              // não permite inserir além do limite
                    }
                    char nome[BUF_TAM]; 
                    lerString("Nome: ", nome, BUF_TAM);  // lê nome
                    if (strlen(nome) == 0) { 
                        printf("Insercao cancelada.\n"); 
                        continue;              // aborta se nome vazio
                    }
                    char tipo[BUF_TAM]; 
                    lerString("Tipo: ", tipo, BUF_TAM);  // lê tipo
                    int pr = lerInteiro("Prioridade (1..10): ", 1, 10); // lê prioridade
                    Componente *c = criar_componente(nome, tipo, pr);   // cria componente no heap
                    if (!c) { printf("Erro de alocacao.\n"); continue; } // falha na alocação
                    if (!vetor_push(vet, c)) { 
                        printf("Falha ao adicionar no vetor.\n"); 
                        free(c); 
                        continue; 
                    }
                    cabeca = inserir_lista_no_final(cabeca, c); // adiciona também na lista (cópia)
                    printf("Componente inserido com sucesso. Total: %d\n", vet->size);
                } 
                else if (escolha == 3) { 
                    // Remover componente pelo nome
                    char busca[BUF_TAM]; 
                    lerString("Nome do componente a remover: ", busca, BUF_TAM);
                    if (strlen(busca) == 0) { 
                        printf("Remocao cancelada.\n"); 
                        continue; 
                    }
                    int rem = 0; 
                    rem = vetor_remove_por_nome(vet, busca);             // remove do vetor (libera memória)
                    cabeca = remover_lista_por_nome(cabeca, busca, &rem); // remove da lista também
                    if (rem) printf("Componente removido.\n"); 
                    else printf("Componente nao encontrado.\n");
                } 
                else if (escolha == 4) { 
                    // Ordenar componentes e medir tempo + comparações
                    printf("Escolha o algoritmo de ordenacao e criterio:\n");
                    printf("1) Bubble Sort por NOME (strings)\n");
                    printf("2) Insertion Sort por TIPO (strings)\n");
                    printf("3) Selection Sort por PRIORIDADE (inteiro, desc)\n");
                    int alg = lerInteiro("Algoritmo: ", 1, 3);
                    long comps = 0; double tempo = 0.0;
                    if (estrutura == 1) { 
                        // Vetor
                        if (alg == 1) tempo = bubbleSortVetor_porNome(vet, &comps);
                        else if (alg == 2) tempo = insertionSortVetor_porTipo(vet, &comps);
                        else tempo = selectionSortVetor_porPrioridade(vet, &comps);
                        printf("\nResultado da ordenacao (VETOR):\n"); 
                        vetor_listar(vet);  // lista após ordenação
                        printf("Comparacoes: %ld | Tempo: %.6f s\n", comps, tempo);
                    } 
                    else { 
                        // Lista
                        if (alg == 1) tempo = bubbleSortLista_porNome(cabeca, &comps);
                        else if (alg == 2) tempo = insertionSortLista_porTipo(cabeca, &comps);
                        else tempo = selectionSortLista_porPrioridade(cabeca, &comps);
                        printf("\nResultado da ordenacao (LISTA):\n"); 
                        listar_lista(cabeca);
                        printf("Comparacoes: %ld | Tempo: %.6f s\n", comps, tempo);
                    }
                } 
                else if (escolha == 5) { 
                    // Busca de componente-chave (somente vetor ordenado por nome)
                    if (estrutura == 2) { 
                        printf("Busca binaria nao suportada para lista. Use vetor ordenado por NOME.\n"); 
                        continue; 
                    }
                    if (!vet->ordenado_por_nome) { 
                        printf("Vetor nao esta ordenado por NOME. Ordene com Bubble Sort por NOME antes de buscar.\n"); 
                        continue; 
                    }
                    char chave[BUF_TAM]; 
                    lerString("Nome do componente-chave a buscar: ", chave, BUF_TAM);
                    if (strlen(chave) == 0) { 
                        printf("Busca cancelada.\n"); 
                        continue; 
                    }
                    long compsBusca = 0; 
                    int idx = buscaBinariaVetor_porNome(vet, chave, &compsBusca); // busca binária
                    if (idx == -2) { 
                        printf("Vetor nao ordenado por nome.\n"); 
                    }
                    else if (idx >= 0) { 
                        printf("Componente encontrado na posicao %d:\n", idx+1); 
                        imprimirCabecalho(); 
                        imprimirComponente(vet->arr[idx], idx+1); 
                        printf("Comparacoes na busca: %ld\n", compsBusca); 
                    }
                    else { 
                        printf("Componente NAO encontrado. Comparacoes na busca: %ld\n", compsBusca); 
                    }
                } 
                else if (escolha == 6) { 
                    voltar = 1;   // volta ao menu principal
                }
            }
        } 
        else if (op == 3) { 
            // Exibir estado atual das estruturas
            printf("\n--- ESTADO ATUAL DAS ESTRUTURAS ---\n");
            vetor_listar(vet); 
            listar_lista(cabeca);
        } 
        else if (op == 4) { 
            // Sair do programa
            sair = 1; 
            printf("Saindo... liberando memoria. Boa sorte na fuga!\n");
        }
    }

    // Libera memória antes de encerrar
    liberarVetorDin(vet); 
    liberar_lista(cabeca);
    return 0;   // fim do programa
}
