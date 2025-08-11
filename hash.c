#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define M 101
#define N 50


typedef struct Nodo
{
    char nome[N];
    char cpf[15];
    struct Nodo* prox;
} Nodo;

Nodo* tabela[M];

int hash(int key,const char* nome){

    unsigned int h = key;
    int len = strlen(nome);

    for(int i = 0; i < len; i++){
        h = (h * 31) + (unsigned char)nome[i];
    }

    return h % M;
}

int converte_nome(char nome[])
{

    int valor = 0;

    for(int i = 0; i < strlen(nome); i++)
    {
        //multiplicar pelo indice da letra ajuda a evitar colisao pois abc vai ser diferente acb
        valor += (int)nome[i] * (i + 1);
    }

    return valor;
}

void armazena(char nome[], char cpf[])
{
    if(buscar(nome)){
        return;
    }

    Nodo* atual = malloc(sizeof(Nodo));
    int valor = converte_nome(nome);
    strcpy(atual->nome, nome);
    strcpy(atual->cpf, cpf);

    int i = hash(valor, atual->nome);

    atual->prox = tabela[i];
    tabela[i] = atual;
}

void imprime()
{

    for(int i = 0; i < M; i++)
    {
        Nodo* atual = tabela[i];

        printf("\n-------------INDICE %d -------------\n", i);

        while(atual != NULL)
        {
            printf("-----PESSOA-----\n");
            printf("Nome: %s\n", atual->nome);
            printf("CPF: %s\n", atual->cpf);
            printf("----------------\n");
            atual = atual->prox;
        }
        printf("NULO\n");
    }
}

int buscar(char* nome)
{
    int comp = 0;
    int i = hash(converte_nome(nome), nome);

    Nodo* atual = tabela[i];

    while(atual != NULL)
    {   comp++;
        if(strcmp(atual->nome, nome) == 0)
        {
            printf("\n-------PESSOA ENCONTRADA----\n");
            printf("Nome: %s\n", atual->nome);
            printf("CPF: %s\n", atual->cpf);
            printf("COMPARACOES: %d", comp);
            return 1;
        }
        atual = atual->prox;
    }

    printf("PESSOA INEXISTENTE\n");
    return 0;
}

void criar_cpf(char* cpf)
{

    for(int i = 0; i < 14; i++)
    {
        cpf[i] = '0' + (rand() % 10);

        if (i == 3 || i == 7)
        {
            cpf[i] = '.';
        }
        else if (i == 11)
        {
            cpf[i] = '-';
        }
    }

    cpf[14] = '\0';
}

void gerar_nome(char* nome)
{
    const char* nomes[10] =
    {
        "Joao", "Rafael", "Pedro", "Matheus", "Cristiane", "Andre", "Heloisa",
        "Luis", "Gabriel", "Lucas"
    };

    const char* sobreN[10] =
    {
        "Santos", "Rosa", "Camargo", "Dias", "Silva", "Rocha", "Spancer",
        "Graysson", "Paker", "Stark"
    };

    const char* pNome[4] = {"da", "de","dos","do"};


    strcpy(nome,nomes[rand() % 10]);

    if(rand() % 2 == 1)
    {
        strcat(nome," ");
        strcat(nome,pNome[rand() % 4]);
    }

    strcat(nome," ");
    strcat(nome,sobreN[rand() % 10]);

}

int main()
{
    srand(time(NULL));

    char nome[N] = "";
    char cpf[15] = "";

    for(int i = 0; i< M; i++)
    {
        gerar_nome(nome);
        criar_cpf(cpf);

        armazena(nome,cpf);
    }

    imprime();

    printf("\n-----BUSCAR----\n");
    printf("Nome: ");
    setbuf(stdin, NULL);
    fgets(nome, sizeof(nome), stdin);
    nome[strcspn(nome, "\n")] = '\0';
    buscar(nome);

    return 0;
}
