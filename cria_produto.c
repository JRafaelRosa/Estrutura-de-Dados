#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <windows.h>
#include <locale.h>

#ifndef QUANT
#define QUANT 100
#endif

void verifica_data(int* dia, int* mes, int* ano) {

    int dias_mes[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

    *ano = rand()%5+2020;
    *mes = rand()%12 + 1;
    *dia = rand()%dias_mes[*mes -1] + 1;

    if(*mes == 2 && ((*ano % 4 == 0 && *ano %100 != 0) || (*ano %400 == 0))) {
        *dia = rand()%29 +1;
    }
}

void visualizar_produtos(char prod[][50], int total) {

    system("cls");
    printf("\t\tLISTAGEM DE PRODUTOS\n");
    printf("________________________________________________\n");
    for(int i = 0; i < total; i++) {
        printf("%s\n", prod[i]);
    }
}

void visualizar_vendas(FILE* compras) {
    char linha[256];

    printf("\n\n\t\tLISTA DE COMPRAS\n");
    printf("_______________________________________________________________________\n");

    if(compras != NULL) {
        rewind(compras);
        while(fgets(linha, sizeof(linha), compras)) {
            printf("%s", linha);
        }
    }
}

typedef struct Data {
    int dia;
    int mes;
    int ano;
} Data;

typedef struct Produto {
    Data data;
    char* nome;
    int codigo;
    float preco;
    int vendas;
    int quant;
    Produto* prox;
} Produto;

typedef struct Raiz {
    Produto produto;
    int altura;
    Raiz* esquerda;
    Raiz* direita;
} Raiz;

int altura(Raiz* node) {
    if(node == NULL) {
        return 0;
    }
    return node->altura;
}

int altura_maxima(int a, int b) {

    if(a > b) {
        return a;
    } else {
        return b;
    }
}


Raiz* criar_raiz(char* nome,int dia, int mes, int ano, int cod,int quant,float preco ) {

    Raiz* node = (Raiz*)malloc(sizeof(Raiz));

    if(node == NULL) {
        printf("Erro ao alocar No!!");
        return NULL;
    }

    node->produto.nome = strdup(nome); //copia o nome
    node->produto.data.dia = dia;
    node->produto.data.mes = mes;
    node->produto.data.ano = ano;
    node->produto.codigo = cod;
    node->produto.quant = quant;
    node->produto.preco = preco;

    node->esquerda = NULL;
    node->direita = NULL;
    node->altura = 1;

    return node;
}

int compara_data(int dia, int mes, int ano, int Rdia, int Rmes, int Rano) {
    if(ano < Rano) return -1;
    if(ano > Rano) return  1;
    if(mes < Rmes) return -1;
    if(mes > Rmes) return  1;
    if(dia < Rdia) return -1;
    if(dia > Rdia) return  1;
    return 0; // data igual
}

int balanceamento(Raiz* node) {
    if(node == NULL) {
        return 0;
    }

    return altura(node->esquerda) - altura(node->direita);
}

Raiz* rotacao_direita(Raiz* y) {
    Raiz* x = y->esquerda;
    Raiz* T2 = x->direita;

    //rotaciona
    x->direita = y;
    y->esquerda = T2;

    y->altura = altura_maxima(altura(y->esquerda),altura(y->direita))+1;
    x->altura = altura_maxima(altura(x->esquerda),altura(x->direita))+1;

    return x; // Novo node
}

Raiz* rotacao_esquerda(Raiz* x) {
    Raiz* y = x->direita;
    Raiz* T2 = y->esquerda;

    //rotação
    y->esquerda = x;
    x->direita = T2;

    //altura
    x->altura = altura_maxima(altura(x->esquerda),altura(x->direita))+1;
    y->altura = altura_maxima(altura(y->esquerda),altura(y->direita))+1;

    return y;
}

Raiz* inserir(Raiz* node, char* nome, int dia, int mes, int ano, int cod,int quant,float preco) {

    if (node == NULL) {
        return criar_raiz(nome, dia, mes, ano, cod, quant, preco);
    }

    // Comparar as datas
    int data_comp = compara_data(dia, mes, ano, node->produto.data.dia, node->produto.data.mes, node->produto.data.ano);

    if (data_comp < 0) {
        node->esquerda = inserir(node->esquerda, nome, dia, mes, ano, cod, quant, preco);
    } else if (data_comp > 0) {
        node->direita = inserir(node->direita, nome, dia, mes, ano, cod, quant, preco);
    } else {

        if (cod < node->produto.codigo) {
            node->esquerda = inserir(node->esquerda, nome, dia, mes, ano, cod, quant, preco);
        } else if (cod > node->produto.codigo) {
            node->direita = inserir(node->direita, nome, dia, mes, ano, cod, quant, preco);
        } else {
            //se data e codigo for igual soma a quantidade
            node->produto.quant += quant;
            return node;
        }
    }

    // Atualiza a altura do nó
    node->altura = 1 + altura_maxima(altura(node->esquerda), altura(node->direita));

    // Balanceamento da árvore
    int balance = balanceamento(node);

    // Correção do balanceamento

    // Rotação simples à direita
    if (balance > 1 && compara_data(dia, mes, ano, node->esquerda->produto.data.dia, node->esquerda->produto.data.mes, node->esquerda->produto.data.ano) < 0) {
        return rotacao_direita(node);
    }

    // Rotação simples à esquerda
    if (balance < -1 && compara_data(dia, mes, ano, node->direita->produto.data.dia, node->direita->produto.data.mes, node->direita->produto.data.ano) > 0) {
        return rotacao_esquerda(node);
    }

    // Rotação dupla à direita
    if (balance > 1 && compara_data(dia, mes, ano, node->esquerda->produto.data.dia, node->esquerda->produto.data.mes, node->esquerda->produto.data.ano) > 0) {
        node->esquerda = rotacao_esquerda(node->esquerda);
        return rotacao_direita(node);
    }

    // Rotação dupla à esquerda
    if (balance < -1 && compara_data(dia, mes, ano, node->direita->produto.data.dia, node->direita->produto.data.mes, node->direita->produto.data.ano) < 0) {
        node->direita = rotacao_direita(node->direita);
        return rotacao_esquerda(node);
    }

    return node;
}

typedef struct Lista {
    Raiz* produto;
    struct Lista* proximo;
} Lista;

// Função de busca para retornar uma lista de produtos com a mesma data
Lista* buscar_produto(Raiz* node, int dia, int mes, int ano, int* vendas) {

    if (node == NULL) {
        return NULL;
    }

    int comparacao = compara_data(dia, mes, ano, node->produto.data.dia, node->produto.data.mes, node->produto.data.ano);
    Lista* lista_resultados = NULL;

    // verifica se a data é igual para adicinar a lista
    if (comparacao == 0) {
        Lista* novo_produto = (Lista*) malloc(sizeof(Lista));
        novo_produto->produto = node;
        novo_produto->proximo = lista_resultados;
        lista_resultados = novo_produto;

        *vendas += node->produto.quant;
    }

    // Procura os filhos
    if (comparacao <= 0) {
        Lista* esquerda_resultados = buscar_produto(node->esquerda, dia, mes, ano, vendas);
        if (esquerda_resultados) {

            Lista* temp = esquerda_resultados;
            while (temp->proximo) {
                temp = temp->proximo;
            }
            temp->proximo = lista_resultados;
            lista_resultados = esquerda_resultados;
        }
    }

    if (comparacao >= 0) {
        Lista* direita_resultados = buscar_produto(node->direita, dia, mes, ano, vendas);
        if (direita_resultados) {
            Lista* temp = direita_resultados;
            while (temp->proximo) {
                temp = temp->proximo;
            }
            temp->proximo = lista_resultados;
            lista_resultados = direita_resultados;
        }
    }

    return lista_resultados;
}

void menu(char prod[][50], int iprod,FILE *compras, Raiz* produto) {

    int resp;
    printf("1-Listagem de produtos\n");
    printf("2-Listagem de vendas\n");
    printf("3-Pesquisar produtos\n");
    printf("4-Sair\n");
    scanf("%d", &resp);
    getchar();

    switch(resp) {
    case 1:
        system("cls");
        visualizar_produtos(prod, iprod);
        system("pause");
        system("cls");
        menu(prod, iprod, compras, produto);
        break;

    case 2:
        system("cls");
        visualizar_vendas(compras);
        system("pause");
        system("cls");
        menu(prod, iprod, compras, produto);
        break;

    case 3: {
        int dia, mes, ano;
        char data[11];
        int vendas = 0;

        system("cls");
        printf("________________________________________________\n");
        printf("Pesquisar\n");
        printf("Data: ");
        fgets(data, sizeof(data), stdin);
        data[strcspn(data, "\n")] = 0;
        sscanf(data, "%d %d %d", &dia, &mes, &ano);
        Lista* encontrados = buscar_produto(produto, dia, mes, ano, &vendas);

        if(encontrados != NULL) {
            Lista* temp = encontrados;
            while(temp != NULL) {
                Raiz* node = encontrados->produto;
                printf("Nome: %s\n", node->produto.nome);
                printf("Código: %d\n", node->produto.codigo);
                printf("Data: %d/%d/%d\n", node->produto.data.dia, node->produto.data.mes, node->produto.data.ano);
                printf("Preço: %.2f\n", node->produto.preco);
                printf("Quantidade: %.0f\n", node->produto.quant);

                temp = temp->proximo;
            }
            printf("Vendas do dia: %d\n", vendas);
        } else
        {
            printf("Produto não encontrado");
        }

        system("pause");
        system("cls");
        menu(prod, iprod, compras, produto);
    }
    break;

    case 4:
        printf("SAINDO...");
        exit(0);
        break;

    default:
        printf("Opção inválida");
        system("pause");
        system("cls");
        menu(prod, iprod, compras, produto);
        break;
    }
}


int main(int argc, char *argv[]) {

    setlocale(LC_ALL, "portuguese");

    Raiz* produto = NULL;

    FILE *i, *compras;
    char prod[150][50], d[ 6 ];
    float v;
    int cod = 1, iprod=0, cont = 0, quant = QUANT;
    char linha[ 200 ];

    if( (i = fopen( "nome_produtos.txt", "r") ) == NULL )
        perror("ERRO abertura entrada\n");
    if( (compras = fopen( "venda_produtos.txt", "w")) == NULL )
        perror( "ERRO abertura saida\n");

    while( ! feof( i ) ) {
        fgets( prod[ iprod ], sizeof(prod), i );
        prod[ iprod ][ strlen(prod[ iprod ])-1 ] = 0;
        printf( "%s\r ", prod[ iprod ] );
        iprod++;
    }

    fclose( i );
    srand( time( NULL ) );

    if( argc > 1 )
        quant = atoi( argv[ 1 ] );
    while( cont < quant ) {

        int dia = 0;
        int mes = 0;
        int ano = 0;
        float preco = (rand()%9999)/100.0;
        double quantidade = (rand()%999)/100.0 +1;

        verifica_data(&dia, &mes, &ano);

        cod = rand()%iprod;

        produto = inserir(produto,prod[cod], dia, mes, ano,cod, quantidade, preco);

        sprintf( linha, "%02d/%02d/%04d;\t %3d;\t %s; %6.0f; %6.2f\n", dia, mes,
                 ano, cod, prod[cod],quantidade, preco );
        printf( "%s\r ", linha );
        fprintf(compras,"%s", linha );
        cont++;
    }

    fclose(compras);

    system("cls");
    compras = fopen("venda_produtos.txt", "r");
    menu(prod, iprod, compras, produto);

    fclose( i );
    fclose( compras );
}
