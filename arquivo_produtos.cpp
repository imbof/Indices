#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINHA 256
#define MAX_PRODUTOS 5000

typedef struct {
    int indice;
    char product_id[50];
    char category_id[50];
    char category_code[50];
    char brand[50];
    float price;
} Produto;

// Função para carregar os produtos do arquivo
int carregar_produtos(const char *arquivo, Produto produtos[], int *quantidade) {
    FILE *file = fopen(arquivo, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo %s.\n", arquivo);
        return 0;
    }

    char linha[MAX_LINHA];
    *quantidade = 0;

    while (fgets(linha, MAX_LINHA, file) != NULL && *quantidade < MAX_PRODUTOS) {
        Produto p;
        char *token = strtok(linha, "|");
        if (token) p.indice = atoi(token);

        token = strtok(NULL, ",");
        if (token) strcpy(p.product_id, token);

        token = strtok(NULL, ",");
        if (token) strcpy(p.category_id, token);

        token = strtok(NULL, ",");
        if (token) strcpy(p.category_code, token);

        token = strtok(NULL, ",");
        if (token) strcpy(p.brand, token);

        token = strtok(NULL, ",");
        if (token) p.price = atof(token);

        produtos[(*quantidade)++] = p;
    }

    fclose(file);
    return 1;
}

// Função para buscar um produto pelo product_id
void buscar_produto(const Produto produtos[], int quantidade, const char *product_id) {
    for (int i = 0; i < quantidade; i++) {
        if (strcmp(produtos[i].product_id, product_id) == 0) {
            printf("Produto encontrado:\n");
            printf("Indice: %d\n", produtos[i].indice);
            printf("Product ID: %s\n", produtos[i].product_id);
            printf("Category ID: %s\n", produtos[i].category_id);
            printf("Category Code: %s\n", produtos[i].category_code);
            printf("Brand: %s\n", produtos[i].brand);
            printf("Price: %.2f\n", produtos[i].price);
            return;
        }
    }
    printf("Produto com o ID %s não encontrado.\n", product_id);
}

int main() {
    Produto produtos[MAX_PRODUTOS];
    int quantidade;

    // Nome do arquivo
    const char *arquivo = "produtos_indexados_5mil.txt";

    // Carrega os produtos do arquivo
    if (!carregar_produtos(arquivo, produtos, &quantidade)) {
        return 1; // Erro ao carregar o arquivo
    }

    printf("Produtos carregados com sucesso (%d produtos).\n", quantidade);

    // Loop de consulta
    char product_id[50];
    while (1) {
        printf("\nDigite o product_id para consulta (ou 'sair' para encerrar): ");
        scanf("%s", product_id);

        if (strcmp(product_id, "sair") == 0) {
            break;
        }

        buscar_produto(produtos, quantidade, product_id);
    }

    printf("Programa encerrado.\n");
    return 0;
}

