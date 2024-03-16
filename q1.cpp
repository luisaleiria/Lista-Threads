#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <iostream>
#include <ctime>
#include <time.h>

//para senha "Lista12345"
//Tempo para T=10 -> 0.006314 segundos
//Tempo para T=5 -> 0.002098 segundos
//Tempo para T=1 -> 0.008042 segundos

//para senha "InFrAs0ftw"
//Tempo para T=10 -> 0.006972 segundos
//Tempo para T=5 -> 0.001861 segundos
//Tempo para T=1 -> 0.000196 segundos

//para senha "ABcd123764"
//Tempo para T=10 -> 0.003027 segundos
//Tempo para T=5 -> 0.000530 segundos
//Tempo para T=1 -> 0.000236 segundos

//para senha "Senha12345"
//Tempo para T=10 -> 0.006758 segundos
//Tempo para T=5 -> 0.001741 segundos
//Tempo para T=1 -> 0.000234 segundos


using namespace std;

char senha[] = "Senha12345"; //definindo a string da senha
pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER; //criando o mutex para incrementar o contador 
int T; //num de threads
int verif;
char strGlob[11]; //string que vai receber o que as threads identificaram
int cont=0;

void *verificando(void *thread){

    int threadID = *((int*) thread);
    for(int l=33; l<=125; l++){
        int letra=l;

        if(T==10 && cont<=10){
            if(senha[threadID]==letra){
                printf("A thread %d descobriu o caractere %c\n", threadID, letra);
                strGlob[threadID] = letra;
                pthread_mutex_lock(&mymutex); 
                cont++;
                pthread_mutex_unlock(&mymutex);
            }
        }

        else if(T==5 && cont<=20){
            if(senha[threadID]==letra){
                strGlob[threadID]=letra;
                printf("A thread %d descobriu o caractere %c\n", threadID, letra);
                pthread_mutex_lock(&mymutex); 
                cont++;
                pthread_mutex_unlock(&mymutex); 
                
            } 
            if(senha[threadID + 5]==letra){
                strGlob[threadID + 5] = letra;
                printf("A thread %d descobriu o caractere %c\n", threadID, letra);
                pthread_mutex_lock(&mymutex); 
                cont++;
                pthread_mutex_unlock(&mymutex);
            }
        }

        else if(T==1){
            for(int a=0; a<10; a++){
                if(senha[threadID + a]==letra){
                    strGlob[threadID + a]=letra;
                    printf("A thread %d descobriu o caractere %c\n", threadID, letra);
                }
            } 
        }
    }
     pthread_exit(NULL);
}

int main(){

    printf ("Digite o numero de threads: ");
    scanf("%d", &T);

    pthread_t thread[T];

    //iniciando o contador de tempo
    double time_spent = 0.0; // para armazenar o tempo de execução do código
    clock_t begin = clock();

    if (T == 1) {
        pthread_t thread;
        int taskid = 0;
        pthread_create(&thread, NULL, verificando, (void *)&taskid);
        pthread_join(thread, NULL);
    }

    else{
        
    //criando as threads e mandando pra funcao
    for(int i=0; i<T; i++){
        pthread_t threads[T];
        int *taskids[T];
        int rc; 
        int t;
        for(t=0; t<T; t++){
            taskids[t] = (int *) malloc(sizeof(int)); *taskids[t] = t;
            rc = pthread_create(&threads[t], NULL, funcao, (void *)taskids[t]);
        }
    }
    
        
    for(int k=0; k<T; k++){ //liberando as threads
        pthread_join(thread[k], NULL);
        printf("Liberando thread %d\n", k);
        }
    }

    clock_t end = clock(); //encerrando contador de tempo

    // calcula o tempo decorrido encontrando a diferença (end - begin) e
    // dividindo a diferença por CLOCKS_PER_SEC para converter em segundos
    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;

    //printando a senha descoberta
    printf("A senha e: ");
    printf("%s", strGlob);
    printf("\n");
    printf("O tempo de execucao para T=%d foi de %f segundos", T, time_spent);

    return 0;
}


