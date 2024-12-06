#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ORDEM 4 // Ordem da árvore B+

typedef struct Registro {
    int chave;       // Chave de ordenação (índice da linha)
    long endereco;   // Endereço do registro no arquivo
} Registro;

typedef struct No {
    int num_chaves;                 // Número de chaves no nó
    int folhas;                     // 1 se for folha, 0 caso contrário
    int chaves[ORDEM - 1];          // Chaves armazenadas no nó
    struct No *filhos[ORDEM];       // Ponteiros para os filhos
    Registro registros[ORDEM - 1]; // Registros (apenas para folhas)
    struct No *prox_folha;          // Ponteiro para a próxima folha (somente em nós folha)
} No;

typedef struct ArvoreBMais {
    No *raiz; // Ponteiro para a raiz da árvore
} ArvoreBMais;

// Cria um novo nó
No *criar_no(int folhas) {
    No *no = (No *)malloc(sizeof(No));
    no->num_chaves = 0;
    no->folhas = folhas;
    memset(no->filhos, 0, sizeof(no->filhos));
    memset(no->registros, 0, sizeof(no->registros));
    no->prox_folha = NULL;
    return no;
}

// Cria uma nova árvore B+
ArvoreBMais *criar_arvore() {
    ArvoreBMais *arvore = (ArvoreBMais *)malloc(sizeof(ArvoreBMais));
    arvore->raiz = criar_no(1); // Raiz inicial é uma folha
    return arvore;
}

// Insere uma chave e endereço em uma folha
void inserir_na_folha(No *folha, int chave, long endereco) {
    int i;
    for (i = folha->num_chaves - 1; i >= 0 && folha->chaves[i] > chave; i--) {
        folha->chaves[i + 1] = folha->chaves[i];
        folha->registros[i + 1] = folha->registros[i];
    }
    folha->chaves[i + 1] = chave;
    folha->registros[i + 1].chave = chave;
    folha->registros[i + 1].endereco = endereco;
    folha->num_chaves++;
}

// Divide um nó e retorna o novo nó criado
No *dividir_no(No *no, int *chave_promovida) {
    int meio = no->num_chaves / 2;
    No *novo_no = criar_no(no->folhas);
    novo_no->num_chaves = no->num_chaves - meio - 1;

    // Transfere chaves e filhos para o novo nó
    for (int i = 0; i < novo_no->num_chaves; i++) {
        novo_no->chaves[i] = no->chaves[meio + 1 + i];
        if (!no->folhas) {
            novo_no->filhos[i] = no->filhos[meio + 1 + i];
        } else {
            novo_no->registros[i] = no->registros[meio + 1 + i];
        }
    }
    if (!no->folhas) {
        novo_no->filhos[novo_no->num_chaves] = no->filhos[no->num_chaves];
    }
    *chave_promovida = no->chaves[meio];
    no->num_chaves = meio;

    // Atualiza ponteiros de folha
    if (no->folhas) {
        novo_no->prox_folha = no->prox_folha;
        no->prox_folha = novo_no;
    }
    return novo_no;
}

// Insere uma chave na árvore recursivamente
No *inserir_recursivo(No *no, int chave, long endereco, int *chave_promovida) {
    if (no->folhas) {
        inserir_na_folha(no, chave, endereco);
        if (no->num_chaves == ORDEM - 1) {
            return dividir_no(no, chave_promovida);
        }
        return NULL;
    }

    int i;
    for (i = no->num_chaves - 1; i >= 0 && no->chaves[i] > chave; i--)
        ;
    i++;
    int chave_promovida_filho;
    No *novo_no = inserir_recursivo(no->filhos[i], chave, endereco, &chave_promovida_filho);
    if (novo_no) {
        for (int j = no->num_chaves - 1; j >= i; j--) {
            no->chaves[j + 1] = no->chaves[j];
            no->filhos[j + 2] = no->filhos[j + 1];
        }
        no->chaves[i] = chave_promovida_filho;
        no->filhos[i + 1] = novo_no;
        no->num_chaves++;
        if (no->num_chaves == ORDEM - 1) {
            return dividir_no(no, chave_promovida);
        }
    }
    return NULL;
}

// Insere uma chave na árvore B+
void inserir(ArvoreBMais *arvore, int chave, long endereco) {
    int chave_promovida;
    No *novo_no = inserir_recursivo(arvore->raiz, chave, endereco, &chave_promovida);
    if (novo_no) {
        No *nova_raiz = criar_no(0);
        nova_raiz->chaves[0] = chave_promovida;
        nova_raiz->filhos[0] = arvore->raiz;
        nova_raiz->filhos[1] = novo_no;
        nova_raiz->num_chaves = 1;
        arvore->raiz = nova_raiz;
    }
}

// Imprime a árvore (apenas para depuração)
void imprimir_arvore(No *no, int nivel) {
    printf("Nivel %d: ", nivel);
    for (int i = 0; i < no->num_chaves; i++) {
        printf("%d ", no->chaves[i]);
    }
    printf("\n");
    if (!no->folhas) {
        for (int i = 0; i <= no->num_chaves; i++) {
            imprimir_arvore(no->filhos[i], nivel + 1);
        }
    }
}


// Função para buscar uma chave na árvore B+
Registro *buscar_na_arvore(No *no, int chave) {
    if (no->folhas) {
        for (int i = 0; i < no->num_chaves; i++) {
            if (no->chaves[i] == chave) {
                return &no->registros[i];
            }
        }
        return NULL; // Chave não encontrada
    }
    for (int i = 0; i < no->num_chaves; i++) {
        if (chave < no->chaves[i]) {
            return buscar_na_arvore(no->filhos[i], chave);
        }
    }
    return buscar_na_arvore(no->filhos[no->num_chaves], chave);
}

int main() {
    // Inicializa a árvore e lê o arquivo como antes
    ArvoreBMais *arvore = criar_arvore();
    FILE *arquivo = fopen("produtos_indexados_5mil.txt", "r");
    if (!arquivo) {
        perror("Erro ao abrir produtos.txt");
        return EXIT_FAILURE;
    }

    char linha[1024];
    long posicao = ftell(arquivo);
    while (fgets(linha, sizeof(linha), arquivo)) {
        int indice;
        sscanf(linha, "%d |", &indice);
        inserir(arvore, indice, posicao);
        posicao = ftell(arquivo);
    }
    fclose(arquivo);

    // Consulta na árvore B+
    int consulta_indice = 51; // Substitua pelo índice desejado
    Registro *registro = buscar_na_arvore(arvore->raiz, consulta_indice);
    if (registro) {
        arquivo = fopen("produtos_indexados_5mil.txt", "r");
        fseek(arquivo, registro->endereco, SEEK_SET);
        fgets(linha, sizeof(linha), arquivo);
        fclose(arquivo);
        printf("Registro encontrado: %s", linha);
    } else {
        printf("Registro com indice %d não encontrado.\n", consulta_indice);
    }

    return 0;
}
