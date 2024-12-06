#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TAMANHO_TABELA 100 // Tamanho da tabela hash

typedef struct NoHash {
    int product_id;         // Chave: ID do produto
    long endereco;          // Valor: Endereço no arquivo
    struct NoHash *proximo; // Ponteiro para resolver colisões
} NoHash;

typedef struct TabelaHash {
    NoHash *buckets[TAMANHO_TABELA]; // Array de ponteiros para listas
} TabelaHash;

// Função hash simples
int hash(int chave) {
    return chave % TAMANHO_TABELA;
}

// Inicializa a tabela hash
TabelaHash *inicializar_tabela() {
    TabelaHash *tabela = (TabelaHash *)malloc(sizeof(TabelaHash));
    for (int i = 0; i < TAMANHO_TABELA; i++) {
        tabela->buckets[i] = NULL;
    }
    return tabela;
}

// Insere um par (product_id, endereço) na tabela hash
void inserir_na_tabela(TabelaHash *tabela, int product_id, long endereco) {
    int indice = hash(product_id);
    NoHash *novo_no = (NoHash *)malloc(sizeof(NoHash));
    novo_no->product_id = product_id;
    novo_no->endereco = endereco;
    novo_no->proximo = tabela->buckets[indice];
    tabela->buckets[indice] = novo_no;
}

// Busca um product_id na tabela hash
long buscar_na_tabela(TabelaHash *tabela, int product_id) {
    int indice = hash(product_id);
    NoHash *no = tabela->buckets[indice];
    while (no) {
        if (no->product_id == product_id) {
            return no->endereco;
        }
        no = no->proximo;
    }
    return -1; // Não encontrado
}

// Libera a memória da tabela hash
void liberar_tabela(TabelaHash *tabela) {
    for (int i = 0; i < TAMANHO_TABELA; i++) {
        NoHash *no = tabela->buckets[i];
        while (no) {
            NoHash *temp = no;
            no = no->proximo;
            free(temp);
        }
    }
    free(tabela);
}

long buscar_na_tabela(TabelaHash *tabela) {
	int product_id;
    int indice = hash(product_id);
    NoHash *no = tabela->buckets[indice];
    while (no) {
        if (no->product_id == product_id) {
            return no->endereco;
        }
        no = no->proximo;
    }
    return -1; // Não encontrado
}

int main() {
    TabelaHash *tabela = inicializar_tabela();
    FILE *arquivo = fopen("produtos_indexados_5mil.txt", "r");
    if (!arquivo) {
        perror("Erro ao abrir produtos.txt");
        return EXIT_FAILURE;
    }

    char linha[1024];
    long posicao = ftell(arquivo);
    while (fgets(linha, sizeof(linha), arquivo)) {
        int product_id;
        sscanf(linha, "%*d | %d |", &product_id);
        inserir_na_tabela(tabela, product_id, posicao);
        posicao = ftell(arquivo);
    }
    fclose(arquivo);

    // Consulta na tabela hash
    int consulta_id = 3901101; // Substitua pelo ID desejado
    long endereco = buscar_na_tabela(tabela, consulta_id);
    if (endereco != -1) {
        arquivo = fopen("produtos_indexados_5mil.txt", "r");
        fseek(arquivo, endereco, SEEK_SET);
        fgets(linha, sizeof(linha), arquivo);
        fclose(arquivo);
        printf("Registro encontrado: %s", linha);
    } else {
        printf("Product ID %d nao encontrado.\n", consulta_id);
    }

    liberar_tabela(tabela);
    return 0;
}

