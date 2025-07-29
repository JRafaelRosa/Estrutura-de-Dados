#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define tam 1000000

int sequencial(int vet[], int valor){
     for(int i = 0; i < tam; i++){
	if(vet[i] == valor){
	    printf("\nvalor encontrado");
	    return 0;
        }
     }
     
    printf("valor nao encontrado");
    return -1;
}

int binaria(int vet[], int valor, int esq, int dir){

	
	if (esq > dir){
		printf("\nvalor nao encontrado");
		return -1;
	}

   	int md = (esq + dir)/2;
	
    	if(vet[md] == valor){
		printf("\nvalor encontrado");
		return 0;
	}

	else if(vet[md] > valor){
		return binaria(vet, valor,esq, md -1);
    	}
	else{
		return binaria(vet, valor, md + 1, dir);
	}
    
}

int exponencial(int vet[],int valor, int*ind){

	if(valor == vet[*ind]){
	        printf("valor encontrad");
		return 0;
	}
}

int main(){
    long int valor = 0;
    int vet[tam];

    for(int i = 0; i < tam; i++){
	vet[i] = i;
    }

    printf("\nNumero: ");
    scanf("%ld", &valor);


    clock_t ini_seq = clock();
    int res_seq = sequencial(vet, valor);
    clock_t fim_seq = clock();
    double tempo_seq = (double)(fim_seq - ini_seq) / CLOCKS_PER_SEC;
    printf("\nTempo da busca sequencial: %.6f segundos\n", tempo_seq);
   
    int esq = 0;
    int dir = sizeof(vet);

    clock_t ini_bin = clock();
    int res_bin = binaria(vet, valor, 0, tam - 1);
    clock_t fim_bin = clock();
    double tempo_bin = (double)(fim_bin - ini_bin) / CLOCKS_PER_SEC;
    printf("\nTempo da busca binária: %.6f segundos\n", tempo_bin);

    int ind = 0;
    clock_t ini_bin = clock();
    int res_bin = binaria(vet, valor, 0, tam - 1);
    clock_t fim_bin = clock();
    double tempo_bin = (double)(fim_bin - ini_bin) / CLOCKS_PER_SEC;
    printf("\nTempo da busca exponencial: %.6f segundos\n", tempo_bin);
	

   return 0;
}
