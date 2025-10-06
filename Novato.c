// TEMA 2 - NOVATO
#include <stdio.h>   // inclui as funções de entrada e saída (printf, scanf, getchar, etc.)
#include <string.h>  // inclui funções de manipulação de strings (strcmp, strcasecmp, etc.)
#include <stdlib.h>  // inclui malloc, free, atoi, strtol, etc.

struct Item {
    char nome[30];     // campo para guardar o nome do item 
    char tipo[20];     // campo para guardar o tipo do item 
    int quantidade;    // campo para guardar a quantidade do item
};

// variável global da mochila
struct Item *itens[10] = {NULL};  // array de 10 ponteiros para Item, inicializados com NULL (vazio)

// função limparBuffer criada para contornar erro do scanf que lê o \n deixado pelo Enter
void limparBuffer() {
    int c; // declara a variável que receberá os caracteres do buffer
    // while que percorre a variável c, que recebe o getchar() que puxa o que está no buffer,
    // e vai rodando o while descartando até encontrar o \n
    // o 'EOF' é para garantir que rode até o fim para não ocorrer erro
    while ((c = getchar()) != '\n' && c != EOF) {} // consome tudo até a próxima linha (ou EOF)
} 

// função criada especificamente para criar um sistema de pressionar a tecla 'ENTER' para continuar
void enter(){
   
    // mensagem de instrução
    printf("\n||PRESSIONE 'ENTER' PARA CONTINUAR||\n");
    // declaração de variável para capturar o Enter
    char enter; 
    // scanf da variável enter, onde ele lê tudo até ter uma quebra de linha (evitando problemas de ter que pressionar o ENTER duas vezes)
    scanf("%[^\n]", &enter); 
    // função de limpeza de buffer para não ocorrer problemas nos próximos scanf
    limparBuffer(); // consome o '\n' que ficou no buffer
}

// Função criada para vizualizar a mochila
void vizualizarMochila(){
    printf("\n|| MOCHILA ||\n"); // cabeçalho de visualização
    for (int i = 0; i < 10; i++) { // percorre todas as 10 posições da mochila
        if (itens[i] != NULL) { // se existir um item alocado naquela posição
           // imprime a posição (i+1) e os dados do item
           printf("%d - NOME DO ITEM: %s | TIPO DO ITEM: %s | QUANTIDAE DO ITEM: %d \n",
                  i + 1, itens[i]->nome, itens[i]->tipo, itens[i]->quantidade);
        }else{
             printf("%d - Vazio \n", i + 1); // se o ponteiro for NULL, mostra "Vazio"
        }
    }
}

// Função criada para criar itens
void criarItem(){
int espacoMochila = 0;
     for (int i = 0; i < 10; i++) { // procura a primeira posição livre (NULL)
        if (itens[i] == NULL) {
           itens[i] = (struct Item *)malloc(sizeof(struct Item)); 
           // aloca dinamicamente espaço para um Item e armazena o ponteiro em itens[i]
           // OBS: o cast (struct Item *) não é necessário em C, mas não é errado.

                    printf("DIGITE O NOME DO ITEM: ");
                    scanf(" %[^\n]", itens[i]->nome); // lê uma linha (até '\n') e guarda em nome
                    limparBuffer(); // limpa o buffer (consome '\n')

                    printf("DIGITE O TIPO DO ITEM: ");
                    scanf(" %[^\n]", itens[i]->tipo); // lê o tipo até '\n'
                    limparBuffer();

                    printf("DIGITE A QUANTIDADE: ");
                    scanf("%d", &itens[i]->quantidade); // lê um inteiro para quantidade
                    limparBuffer();
                    
                    printf("\n\nItem adicionado com sucesso!\n");
                    enter(); // pede para pressionar ENTER antes de voltar
                    break; // sai do for após adicionar o item
        }
        espacoMochila ++;
    }
    // OBS: se a mochila estiver cheia (nenhum itens[i] == NULL), a função sai sem aviso.
    if(espacoMochila == 10){
        printf("\n||MOCHILA CHEIA||\n");
    }
}


// Função criada para buscarItens por nome
void buscarItems(char busca[]){
    int encontrado = 0; // flag para indicar se achou
    for(int i = 0; i < 10; i ++){ // percorre todas as 10 posições
        // compara ignorando maiúsculas/minúsculas (POSIX): strcasecmp retorna 0 se iguais
        if(itens[i] != NULL && strcasecmp(itens[i]->nome, busca) == 0){
           printf("\n\n||ITEM ENCONTRADO!||\n\n");
           printf("%d - NOME DO ITEM: %s | TIPO DO ITEM: %s | QUANTIDAE DO ITEM: %d \n",
                  i + 1, itens[i]->nome, itens[i]->tipo, itens[i]->quantidade);
            encontrado = 1; // marca como encontrado
            break; // sai do for após encontrar
        }
    }
    if(encontrado == 0){
     printf("\n||ITEM NÃO ENCONTRADO!||\n"); // se não encontrou nenhum item com esse nome
    }
}


// Função criada para excluir itens puxando pelo o Id
void excluirItem(char numId[]){ // recebe o id como string (para permitir '0' ou entrada inválida)
    char *fim;
    int id = (int)strtol(numId, &fim, 10); // converte string para int e coloca ponteiro de erro em fim

    // Verifica se string contém somente números
    if (*fim != '\0') { // se após a conversão ainda houver caracteres, a entrada não é um número puro
        printf("\nVALOR INVÁLIDO! Digite apenas números.\n");
        return; // sai da função sem excluir nada
    }

    // Se o usuário digitou 0 → voltar
    if (id == 0) {
        printf("\nVOLTANDO...\n");
        return; // comportamento solicitado: não exclui, volta ao menu
    }

    // Verifica se está dentro do intervalo válido
    if (id < 1 || id > 10) { // aceita somente 1..10 como IDs válidos
        printf("\nVALOR INVÁLIDO! Digite um número entre 1 e 10.\n");
        return;
    }

    // Ajusta índice (1–10 → 0–9)
    id -= 1;

    // Verifica se o item existe e exclui
    if (itens[id] != NULL) {
        free(itens[id]); // libera a memória alocada para o item
        itens[id] = NULL; // marca a posição como vazia
        printf("\nITEM EXCLUÍDO COM SUCESSO!\n");
    } else {
        printf("\nNÃO EXISTE ITEM NESSA POSIÇÃO!\n"); // se já estava vazio
    }
}


// Onde roda o programa
int main() {
    
printf("|| BEM-VINDO AO MENU PRINCIPAL! ||\n\n");
char opc[10]; // declara variavel opc
while(1){
    printf("SELECIONE A OPÇÃO QUE DESEJAR:\n(1) Abrir Mochila\n(2) Criar Item\n(3) Excluir item\n(4) Buscar item por nome\n(5) Sair\n\nDIGITE AQUI: ");
    scanf("%[^\n]", opc); // lê até o '\n' e salva em opc 
    limparBuffer(); // limpa o buffer após o scanf

    if(strcmp(opc, "1") == 0){ // se o usuário digitou "1"
        vizualizarMochila(); // mostra a mochila
        enter(); // espera ENTER
    }else if(strcmp(opc, "2") == 0){ // se digitou "2"
        criarItem(); // entra na função de criação de item
        enter();
    }else if(strcmp(opc, "3") == 0){ // se digitou "3"
        vizualizarMochila(); // mostra a mochila antes de perguntar o id
        printf("\nDIGITE O ID QUE DESEJA EXCLUIR OU '0' PARA VOLTAR: ");
        char id[20]; // buffer para ler o id como string
        scanf("%[^\n]", id); // lê a linha com o id (pode ser "0" ou um número)
        limparBuffer();
        excluirItem(id); // chama a função que trata validação e exclusão
        enter();
    }else if(strcmp(opc, "4") == 0){ // se digitou "4"
        printf("DIGITE O NOME DO ITEM QUE DESEJA BUSCAR: ");
        char busca [30]; // buffer para o nome da busca
        scanf("%[^\n]", busca); // lê o nome até '\n'
        limparBuffer();
        buscarItems(busca); // procura o item
        enter();
    }else if(strcmp(opc, "5") == 0){ // se digitou "5"
        break; // sai do loop principal e encerra o programa
    }else{
        printf("Valor Invalido!\n\n"); // qualquer outra entrada inválida
        enter();
    }
}

return 0;
}
