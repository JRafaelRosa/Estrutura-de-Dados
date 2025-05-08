#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <windows.h>
#include <locale.h>
#include <sys/stat.h>
#include <stdbool.h>

#ifndef QUANT
#define QUANT 10000
#endif



typedef struct Produto Produto;
typedef struct Raiz Raiz;
typedef struct Lista Lista;

typedef struct Data
{
    int dia;
    int mes;
    int ano;
} Data;

struct Produto
{
    Data data;
    char* nome;
    int codigo;
    float preco;
    int vendas;
    int quant;
    struct Produto* prox;
};

struct Raiz
{
    Produto produto;
    int altura;
    struct Raiz* esquerda;
    struct Raiz* direita;
};

struct Lista
{
    struct Raiz* produto;
    struct Lista* proximo;
};

void verifica_data(int* dia, int* mes, int* ano);
void visualizar_produtos(char prod[][50], int total);
int altura(Raiz* node);
int altura_maxima(int a, int b);
Raiz* criar_raiz(Produto* prod);
int compara_data(int dia, int mes, int ano, int Rdia, int Rmes, int Rano);
int balanceamento(Raiz* node) ;
Raiz* rotacao_direita(Raiz* y);
Raiz* rotacao_esquerda(Raiz* x);
Raiz* inserir(Raiz* node, Produto* prod);
void visualizar_vendas(Raiz* node, int* ultimo_ano);
Lista* buscar_produto(Raiz* node, int dia, int mes, int ano, int* vendas);
void menu(char prod[][50], int iprod, FILE* compras, Raiz* produto);
void liberar(Raiz* node);

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "portuguese");

    Raiz* node = NULL;
    FILE *i, *compras;
    char prod[150][50];
    int cod = 1, iprod = 0, cont = 0, quant = QUANT;
    char linha[200];

    // Carrega os nomes dos produtos
    if ((i = fopen("nome_produtos.txt", "r")) == NULL) {
        perror("ERRO abertura entrada\n");
        return 1;
    }
    while (fgets(prod[iprod], sizeof(prod[iprod]), i)) {
        prod[iprod][strcspn(prod[iprod], "\n")] = 0;
        iprod++;
        if (iprod >= 150) break;
    }
    fclose(i);

    // Verifica se arquivo de vendas existe
    struct stat buffer;
    bool existe_arquivo = stat("venda_produtos.txt", &buffer) == 0;

    if (!existe_arquivo) {
        compras = fopen("venda_produtos.txt", "w");
        if (compras == NULL) {
            perror("ERRO ao criar venda_produtos.txt");
            return 1;
        }

        srand(time(NULL));
        if (argc > 1)
            quant = atoi(argv[1]);

        while (cont < quant) {
            int dia = 0, mes = 0, ano = 0;
            float preco = (rand() % 9999) / 100.0;
            double quantidade = (rand() % 999) / 100.0 + 1;

            verifica_data(&dia, &mes, &ano);
            cod = rand() % iprod;

            Produto produto;
            produto.data.dia = dia;
            produto.data.mes = mes;
            produto.data.ano = ano;
            produto.codigo = cod;
            produto.nome = (char *)malloc(strlen(prod[cod]) + 1);
            if (produto.nome == NULL) {
                printf("Erro ao alocar memória para o nome do produto!\n");
                exit(1);
            }
            strcpy(produto.nome, prod[cod]);
            produto.quant = quantidade;
            produto.preco = preco;

            node = inserir(node, &produto);

            sprintf(linha, "%02d/%02d/%04d;\t %3d;\t %s; %6.0f; %6.2f\n", dia, mes,
                    ano, cod, prod[cod], quantidade, preco);
            printf("%s\r", linha);
            fprintf(compras, "%s", linha);

            cont++;
        }

        fclose(compras);
    }

    system("cls");

    // Lê os dados já existentes e monta a árvore
    compras = fopen("venda_produtos.txt", "r");
    if (compras == NULL) {
        perror("ERRO ao abrir venda_produtos.txt");
        return 1;
    }

    // Recria a árvore a partir do arquivo
    while (fgets(linha, sizeof(linha), compras)) {
        int dia, mes, ano, codigo;
        float preco, quantidade;
        char nome[50];

        sscanf(linha, "%d/%d/%d; %d; %[^;]; %f; %f",
               &dia, &mes, &ano, &codigo, nome, &quantidade, &preco);

        Produto produto;
        produto.data.dia = dia;
        produto.data.mes = mes;
        produto.data.ano = ano;
        produto.codigo = codigo;
        produto.nome = (char *)malloc(strlen(nome) + 1);
        strcpy(produto.nome, nome);
        produto.quant = quantidade;
        produto.preco = preco;

        node = inserir(node, &produto);
    }

    rewind(compras); // Volta ao início para uso no menu
    menu(prod, iprod, compras, node);

    liberar(node);
    fclose(compras);
}

void verifica_data(int* dia, int* mes, int* ano)
{

    int dias_mes[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

    *ano = rand()%5+2020;
    *mes = rand()%12 + 1;
    *dia = rand()%dias_mes[*mes -1] + 1;

    if(*mes == 2 && (((*ano % 4 == 0) && (*ano %100 != 0)) || (*ano %400 == 0)))
    {
        *dia = rand()%29 +1;
    }
}

void visualizar_produtos(char prod[][50], int total)
{

    system("cls");
    printf("\t\tLISTAGEM DE PRODUTOS\n");
    printf("________________________________________________\n");
    printf("\n");
    for(int i = 0; i < total; i++)
    {
        printf("%s\n", prod[i]);
    }
}

int altura(Raiz* node)
{
    if(node == NULL)
    {
        return 0;
    }
    return node->altura;
}

int altura_maxima(int altura_e, int altura_d)
{

    if(altura_e > altura_d)
    {
        return altura_e;
    }
    else
    {
        return altura_d;
    }
}

Raiz* criar_raiz(Produto* prod)
{

    Raiz* node = (Raiz*)malloc(sizeof(Raiz));

    if(node == NULL)
    {
        printf("Erro ao alocar");
        return NULL;
    }
    node->produto = *prod;
    node->esquerda = (Raiz*)NULL;
    node->direita = (Raiz*)NULL;
    node->altura = 1;

    return node;
}

int compara_data(int dia, int mes, int ano, int Rdia, int Rmes, int Rano)
{
    if(ano < Rano) return -1;
    if(ano > Rano) return  1;
    if(mes < Rmes) return -1;
    if(mes > Rmes) return  1;
    if(dia < Rdia) return -1;
    if(dia > Rdia) return  1;
    return 0; // data igual
}

int balanceamento(Raiz* node)
{
    if(node == NULL)
    {
        return 0;
    }

    return altura(node->esquerda) - altura(node->direita);
}

Raiz* rotacao_direita(Raiz* y)
{
    if (y == NULL || y->esquerda == NULL)
        return y; // nada a fazer

    Raiz* x = y->esquerda;
    Raiz* T2 = x->direita;

    x->direita = y;
    y->esquerda = T2;

    y->altura = altura_maxima(altura(y->esquerda), altura(y->direita)) + 1;
    x->altura = altura_maxima(altura(x->esquerda), altura(x->direita)) + 1;

    return x;
}


Raiz* rotacao_esquerda(Raiz* x)
{
    if (x == NULL || x->direita == NULL)
        return x;

    Raiz* y = x->direita;
    Raiz* T2 = y->esquerda;

    y->esquerda = x;
    x->direita = T2;

    x->altura = altura_maxima(altura(x->esquerda), altura(x->direita)) + 1;
    y->altura = altura_maxima(altura(y->esquerda), altura(y->direita)) + 1;

    return y;
}


Raiz* inserir(Raiz* node, Produto* prod)
{

    if (node == NULL)
    {
        return criar_raiz(prod);
    }

    // Comparar as datas usando a função auxiliar
    int data_comp = compara_data(prod->data.dia, prod->data.mes, prod->data.ano, node->produto.data.dia, node->produto.data.mes, node->produto.data.ano);

    if (data_comp < 0)
    {
        node->esquerda = inserir(node->esquerda, prod);
    }
    else if (data_comp > 0)
    {
        node->direita = inserir(node->direita, prod);
    }
    else
    {
        // Se data é igual, comparar pelos códigos
        if (prod->codigo < node->produto.codigo)
        {
            node->esquerda = inserir(node->esquerda, prod);
        }
        else if (prod->codigo > node->produto.codigo)
        {
            node->direita = inserir(node->direita, prod);
        }
        else
        {
            // Se data e código são iguais, somar a quantidade
            node->produto.quant += prod->quant;
            return node;
        }
    }

    // Atualiza a altura do nó após a inserção
    node->altura = 1 + altura_maxima(altura(node->esquerda), altura(node->direita));

    // Verifica o balanceamento da árvore
    int balance = balanceamento(node);

    // Rotações para balanceamento
    if (balance > 1 && node->esquerda != NULL &&
            compara_data(prod->data.dia, prod->data.mes, prod->data.ano,
                         node->esquerda->produto.data.dia,
                         node->esquerda->produto.data.mes,
                         node->esquerda->produto.data.ano) < 0)
    {
        return rotacao_direita(node);
    }

    if (balance < -1 && node->direita != NULL &&
            compara_data(prod->data.dia, prod->data.mes, prod->data.ano,
                         node->direita->produto.data.dia,
                         node->direita->produto.data.mes,
                         node->direita->produto.data.ano) > 0)
    {
        return rotacao_esquerda(node);
    }

    if (balance > 1 && node->esquerda != NULL &&
            compara_data(prod->data.dia, prod->data.mes, prod->data.ano,
                         node->esquerda->produto.data.dia,
                         node->esquerda->produto.data.mes,
                         node->esquerda->produto.data.ano) > 0)
    {
        node->esquerda = rotacao_esquerda(node->esquerda);
        return rotacao_direita(node);
    }

    if (balance < -1 && node->direita != NULL &&
            compara_data(prod->data.dia, prod->data.mes, prod->data.ano,
                         node->direita->produto.data.dia,
                         node->direita->produto.data.mes,
                         node->direita->produto.data.ano) < 0)
    {
        node->direita = rotacao_direita(node->direita);
        return rotacao_esquerda(node);
    }

    return node;
}

void visualizar_vendas(Raiz* node, int* ultimo_ano)
{
    if(node != NULL)
    {
        visualizar_vendas(node->esquerda, ultimo_ano);
        if(node->produto.data.ano != *ultimo_ano)
        {
            *ultimo_ano = node->produto.data.ano;
            printf("\n");
            printf("________________________________________________\n\n");
            printf("------------------ ANO %d --------------------\n", *ultimo_ano);
        }
        printf("________________________________________________\n");
        printf("\n");
        printf("\tDATA: %d/%d/%d\n", node->produto.data.dia, node->produto.data.mes, node->produto.data.ano);
        printf("\tNOME: %s\n", node->produto.nome);
        printf("\tCODIGO: %d\n", node->produto.codigo);
        printf("\tPREÇO: %.2f\n", node->produto.preco);
        printf("\tQUANTIDADE: %d\n", node->produto.quant);

        visualizar_vendas(node->direita, ultimo_ano);
    }
}

Lista* buscar_produto(Raiz* node, int dia, int mes, int ano, int* vendas)
{

    if (node == NULL)
    {
        return NULL;
    }

    int comparacao = compara_data(dia, mes, ano, node->produto.data.dia, node->produto.data.mes, node->produto.data.ano);
    Lista* lista_resultados = NULL;

    // verifica se a data é igual para adicinar a lista
    if (comparacao == 0)
    {
        Lista* novo_produto = (Lista*) malloc(sizeof(Lista));
        novo_produto->produto = node;
        novo_produto->proximo = lista_resultados;
        lista_resultados = novo_produto;

        *vendas += node->produto.quant;
    }

    // Procura os filhos
    if (comparacao <= 0)
    {
        Lista* esquerda_resultados = buscar_produto(node->esquerda, dia, mes, ano, vendas);
        if (esquerda_resultados)
        {

            Lista* temp = esquerda_resultados;
            while (temp->proximo)
            {
                temp = temp->proximo;
            }
            temp->proximo = lista_resultados;
            lista_resultados = esquerda_resultados;
        }
    }

    if (comparacao >= 0)
    {
        Lista* direita_resultados = buscar_produto(node->direita, dia, mes, ano, vendas);
        if (direita_resultados)
        {
            Lista* temp = direita_resultados;
            while (temp->proximo)
            {
                temp = temp->proximo;
            }
            temp->proximo = lista_resultados;
            lista_resultados = direita_resultados;
        }
    }

    return lista_resultados;
}

void menu(char prod[][50], int iprod, FILE* compras, Raiz* produto)
{
    int resp;
    while (1)
    {
        printf("\n\n");
        printf("\t1-Listagem de produtos\n");
        printf("\t2-Listagem de vendas\n");
        printf("\t3-Pesquisar produtos\n");
        printf("\t4-Sair\n");
        printf("\tR: ");
        scanf("%d", &resp);
        getchar();

        switch (resp)
        {
        case 1:
            system("cls");
            visualizar_produtos(prod, iprod);
            printf("________________________________________________\n");
            system("pause");
            system("cls");
            break;

        case 2:
        {
            system("cls");
            int ultimo_ano = -1;
            visualizar_vendas(produto, &ultimo_ano);
            printf("________________________________________________\n");
            system("pause");
            system("cls");
            break;
        }

        case 3:
        {
            int dia, mes, ano;
            char data[11];
            int vendas = 0;

            system("cls");
            printf("Pesquisar por data (dd dd dddd)\n");
            printf("\n");
            printf("Data: ");
            fgets(data, sizeof(data), stdin);
            data[strcspn(data, "\n")] = 0;
            sscanf(data, "%d %d %d", &dia, &mes, &ano);
            Lista* encontrados = buscar_produto(produto, dia, mes, ano, &vendas);
            printf("________________________________________________\n");
            if (encontrados != NULL)
            {
                Lista* temp = encontrados;
                while (temp != NULL)
                {
                    printf("\n");
                    Raiz* node = temp->produto;
                    printf("\tNome: %s\n", node->produto.nome);
                    printf("\tCódigo: %d\n", node->produto.codigo);
                    printf("\tData: %d/%d/%d\n", node->produto.data.dia, node->produto.data.mes, node->produto.data.ano);
                    printf("\tPreço: %.2f\n", node->produto.preco);
                    printf("\tQuantidade: %d\n", node->produto.quant);
                    printf("________________________________________________\n");
                    temp = temp->proximo;
                }
                printf("\n");
                printf("\tVendas do dia: %d\n", vendas);
            }
            else
            {
                printf("\tProduto não encontrado\n");
            }
            printf("________________________________________________\n");
            system("pause");
            system("cls");
            break;
        }

        case 4:
            printf("SAINDO...");
            exit(0);

        default:
            printf("Opção inválida");
            system("pause");
            system("cls");
            break;
        }
    }
}

void liberar(Raiz* node)
{
    if (node != NULL)
    {
        liberar(node->esquerda);
        liberar(node->direita);
        free(node->produto.nome);
        free(node);
    }
}
