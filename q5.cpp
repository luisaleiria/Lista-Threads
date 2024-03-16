#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <iostream>
#include <vector>

using namespace std;

int N;
int T;
int *vet;

void *primos(void *thread){
    int threadID = *((int*) thread); 
    printf("thread %d entrou na funcao\n", threadID); //marcador de threads entrando na funcao
    for(int i=2+threadID; i<= N; i = i+T){ //para fazer a rotatividade 
        if(vet[i]==1){
            for(int j = i*2; j<= N; j = j + i){ //todas as threads entram e pegam multiplos diferentes para apagar
                //comecou com o i*2 para nao apagar o primo (que é o primeiro)
                if(vet[j] == 1){
                    vet[j]=0;
                    printf("A thread %d apagou o numero %d\n", threadID, j); //marcando que thread apagou o que 
                }
            }
        }
    }
    pthread_exit(NULL);
}

int main(){

    printf("Numero T:");
    scanf("%d", &T);
    printf("Numero N:");
    scanf("%d", &N);

    vet = (int*) malloc(N * sizeof(int)); //alocando o vetor dinamicamente

    //inicializando todo o vetor  com 1 menos  a pos0 e a pos1 (que nao sao  primos)
    for(int i=2; i<N; i++){
        vet[i]=1;
    }
    vet[0]=0;
    vet[1]=0;

    //declarando as threads e os ids
    pthread_t threads[T];
    int *taskids[T];

    //criando as threads e mandando pra funcao
    for(int i=0; i<T; i++){
        int rc; 
        int t;
        for(t=0; t<T; t++){
            taskids[t] = (int *) malloc(sizeof(int)); *taskids[t] = t;
            rc = pthread_create(&threads[t], NULL, primos, (void *)taskids[t]);
        }
    }

    printf("\n\n");

    for(int k=0; k<T; k++){ //liberando as threads
        pthread_join(threads[k], NULL);
        printf("Liberando thread %d\n", k);
    }

    printf("\n\n");
    printf("NUMEROS PRIMOS NO INTERVALO\n\n");

    for(int k=2; k<N; k++){ //printando vetor dos primos
        if(vet[k]!=0){
            printf("%d, ", k);
        }
    }
    free(vet); //liberando vetor
}
