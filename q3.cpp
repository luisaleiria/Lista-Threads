#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <vector>

#define tamFrase 1000 //aqui pode variar

void *escritor(void *ThreadEscritor);
void *leitor(void *ThreadLeitor);

int N;
int M;
int frase[tamFrase]; 
int escrevendoAgora=0; //variavel para nao deixar ler ao mesmo tempo que esta escrevendo

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main() {
	
    printf("Digite N: ");
    scanf("%d", &N);
    printf("Digite M: ");
    scanf("%d", &M);

	pthread_t ThreadEscritor[M];
    pthread_t ThreadLeitor[N]; 
    int *idsEscritor[M]; 
    int *idsLeitor[N];
  
    //criando as threads e mandando pra funcao

    for(int i=0; i<N; i++){ //leitoras
        int rc; 
        int t;
        for(t=0; t<N; t++){
            idsLeitor[t] = (int *) malloc(sizeof(int)); *idsLeitor[t] = t;
            rc = pthread_create(&ThreadLeitor[t], NULL, leitor, (void *)idsLeitor[t]);
        } 
    }

    for(int i=0; i<M; i++){ //escritoras
        int rc; 
        int t;
        for(t=0; t<M; t++){
            idsEscritor[t] = (int *) malloc(sizeof(int)); *idsEscritor[t] = t;
            rc = pthread_create(&ThreadEscritor[t], NULL, escritor , (void *)idsEscritor[t]);
        } 
    }

    for(int k=0; k<N; k++){ //liberando as threads leitoras
        pthread_join(ThreadLeitor[k], NULL);
        printf("Liberando thread leitora %d\n", k);
    }

    for(int l=0; l<M; l++){ //liberando as threads escritoras
        pthread_join(ThreadEscritor[l], NULL);
        printf("Liberando thread escritora %d\n", l);
    }
  
    pthread_exit(NULL);
  
    return 0;
}

void *escritor(void *ThreadEscritor) { 
    int threadIDescritor = *((int*) ThreadEscritor); 
    srand(time(NULL)); //iniciando a variavel aleatoria

    while(true) {
        int item = rand()%100; //so escrever nums de 0 a 100
        int posEscrita = rand()%tamFrase;

        pthread_mutex_lock(&mutex); //so uma thread pode escrever de cada vez por isso  to travando aqui
        escrevendoAgora=1; //sinalizar que a operacao de escrita esta sendo feita
        frase[posEscrita]=item; //escrevendo
        printf("Escritor %d escreveu %d na posicao %d \n",threadIDescritor,item, posEscrita);
        escrevendoAgora=0; //sinalizar que a operacao de escrita acabou
        pthread_mutex_unlock(&mutex); //destravando para que outra possa escrever
  }
    pthread_exit(NULL);
}

void *leitor(void *ThreadLeitor) {
    int threadIDleitor = *((int*) ThreadLeitor); 
    srand(time(NULL)); //iniciando a variavel aleatoria

    while(true){
        while(escrevendoAgora==0){ //so deixa ler quando nao esta escrevendo
            int posLeitura = rand()%tamFrase; //decidindo a posicao que vai ser  lida
            printf("Leitor %d esta lendo o valor %d da posicao %d\n", threadIDleitor, frase[posLeitura], posLeitura);
        }
    }

    pthread_exit(NULL);
}