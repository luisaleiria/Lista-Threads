#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 
#include <string.h>
#include <iostream>
#include <vector>
#include <stack>
#include <cmath>
using namespace std;

#define num_threads 4
pthread_t Threads[num_threads];
pthread_mutex_t Mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int *idsthreads[num_threads];
int flag=1;

typedef struct{
    int inicio;
    int Final;
    int passo;
    void(*func)(int);
    int chunk;
    int taskid;
} estrutura;

stack <estrutura> pilha;
stack <estrutura> pilha1[num_threads];

//definindo os 3 tipos de schedule
#define static 1;
#define dynamic 2;
#define guided 3;


void f(int i){ //funcao so para imprimir a interacao
    cout << i << '\n';
}

void *for_estatico (void *thread){
    int threadID = *((int*) thread);
    estrutura parametro;
    while (!pilha1[threadID].empty()){
        parametro = pilha1[threadID].top();
        pilha1[threadID].pop();
        for (int i = parametro.inicio; i <= parametro.Final; i = i + parametro.passo){
            pthread_mutex_lock(&mymutex);
            printf ("thread %d executando a interação ", threadID);
            f(i);
            pthread_mutex_unlock(&mymutex);
        }
    }
    pthread_exit(NULL);
}

void *for_guided(void *idsthreads){
    int threadID = *((int*) idsthreads); 

    while(flag==1){ //enquanto o fim do for  é menor  que o final dado na entrada
        if(pilha.empty() == 0){ //se a pilha tiver vazia nao pode entrar
            estrutura aux2;
            pthread_mutex_lock(&Mutex);
            aux2 = pilha.top();
            int novochunk = ceil((float)(aux2.Final - aux2.inicio)/(float)num_threads); //calculo do novo chunk utilizando o teto

            if(novochunk<=aux2.chunk){ // novo chunk so pode ser maior ou igual a chunk
                novochunk=aux2.chunk;
            }

            pilha.pop();
            for(int i = aux2.inicio; i < aux2.inicio + novochunk && flag==1; i += aux2.passo){
                printf("Thread %d executando a iteracao ", threadID);
                f(i);
                if(i >= aux2.Final - aux2.passo){ 
                flag = 0;
                }
            }       
            aux2.inicio = aux2.inicio + novochunk; //onde acabou o ultimo for 
            if(aux2.inicio > aux2.Final){ // o aux2.inicio + chunk_size seria o final do loop
                novochunk = (aux2.Final - aux2.inicio + novochunk); //pega o que falta para chegar em final
            }

            if(aux2.inicio >= aux2.Final){
                flag=0;
            }
            pilha.push(aux2); //coloca o novo  inicio na pilha para a prox thread pegar
            pthread_mutex_unlock(&Mutex);
        }
    }
    pthread_exit(NULL);

}

void *for_dynamic(void *idsthreads){
    int threadID = *((int*) idsthreads); 
    while(flag==1){ //enquanto o fim do for  é menor  que o final dado na entrada
        if(pilha.empty() == 0){ //se a pilha tiver vazia nao pode entrar
            estrutura aux2;
            pthread_mutex_lock(&Mutex);
            aux2 = pilha.top();
            pilha.pop();
            for(int i = aux2.inicio; i < aux2.inicio + aux2.chunk && flag==1; i += aux2.passo){
                printf("Thread %d executando a iteracao ", threadID);
                f(i);
                if(i >= aux2.Final - aux2.passo){
                flag = 0;
                }
            }       
            aux2.inicio = aux2.inicio + aux2.chunk; //onde acabou o ultimo for 
            if(aux2.inicio > aux2.Final){ // o aux2.inicio + chunk_size seria o final do loop
                aux2.chunk = (aux2.Final - aux2.inicio + aux2.chunk); //pega o que falta para chegar em final
            }

            if(aux2.inicio >= aux2.Final){
                flag=0;
            }
            pilha.push(aux2); //colocando o novo inicio na pilha para a prox thread utilizar
            pthread_mutex_unlock(&Mutex);
        }
    }
    pthread_exit(NULL);
}

void omp_for( int inicio , int passo , int final , int schedule , int chunk_size , void (*f)(int) ){
    //pthread_t Threads[num_threads];
    if(schedule == 1){ //estatico
        int comeco = 0;
        estrutura argumentos[num_threads];
        //pthread_t Threads[num_threads];
       
        int x = (final-inicio)/(chunk_size*num_threads*passo); //quantidade de ciclos completos que podem ser realizados
       
        int resto = (final-inicio)%(chunk_size*num_threads*passo); //resto depois de realizar os n ciclos completos
        argumentos[0].inicio = inicio;


        //para quando ele realiza n ciclos completos + um resto que tera o chuk_size mudado
        if (resto != 0){
            for (int j = 0; j < x; j++){
                for(int i=0; i<num_threads; i++){
                    if (i == 0 && j == 0){
                        comeco = inicio;
                        argumentos[i].inicio = inicio;
                    }
                    else {
                        argumentos[i].inicio = comeco + (chunk_size*passo);
                        comeco = argumentos[i].inicio;

                    }
                    argumentos[i].passo = passo;
                    argumentos[i].Final = argumentos[i].inicio + ((chunk_size*passo) - 1);

                    pilha1[i].push(argumentos[i]);

                    int rc;
                    argumentos[i].taskid = i;
                    int *numero = (int *)malloc(sizeof(int));
                    *numero = i;
                    rc = pthread_create(&Threads[i], NULL, for_estatico, (void *)numero);
                    if (rc != 0){
                        cout << "Failed to create threads" << '\n';
                    }
                }
            }

            comeco = (x * chunk_size * num_threads * passo) + inicio; //onde os n ciclos completos "pararam" depois do for

            chunk_size = ((final - inicio) - (x * chunk_size * num_threads * passo))/(num_threads*passo); //novo chunk_size
            int resto2 =  ((final - inicio) - (x * chunk_size * num_threads * passo))%(num_threads*passo); 

            // o resto é divisível pelo numero de threads, entao chunk_size é constante para todos
            if (resto2 == 0){
                for(int i=0; i<num_threads; i++){ 
                    if (i==0){
                        argumentos[i].inicio = comeco;
                    }
                    else {
                        argumentos[i].inicio = comeco + (chunk_size*passo);
                        comeco = argumentos[i].inicio;
                    }
                    
                    argumentos[i].passo = passo;
                    argumentos[i].Final = argumentos[i].inicio + ((chunk_size*passo) - 1);

                    pilha1[i].push(argumentos[i]);

                    int rc;
                    argumentos[i].taskid = i;
                    int *numero = (int *)malloc(sizeof(int));
                    *numero = i;
                    rc = pthread_create(&Threads[i], NULL, for_estatico, (void *)numero);
                    if (rc != 0){
                        cout << "Failed to create threads" << '\n';
                    }
                }
            }

            //o resto nao é divisível pelo numero de threads e o novo chunk_size não sera constante para todas as threads
            else {
                int sobra;
                int flag = 0;
                if (resto2 < passo){
                    sobra = 0;
                    flag = 1;
                }
                else {
                    sobra = 1;
                }

                for(int i=0; i<num_threads; i++){ 
                    if (i==0){
                        argumentos[i].inicio = comeco;
                    }
                    else {
                        pthread_mutex_lock(&mutex1);
                        argumentos[i].inicio = comeco + ((chunk_size*passo)+ sobra);
                        comeco = argumentos[i].inicio;
                        pthread_mutex_unlock(&mutex1);
                    }
                    
                    argumentos[i].passo = passo;
                    argumentos[i].Final = argumentos[i].inicio + ((chunk_size*passo) + sobra - 1);

                    pthread_mutex_lock(&mutex1);
                    //resto2--;
                    if (resto2 == 0 ){
                        sobra = 0;
                        argumentos[i].Final =  argumentos[i].Final - 1;
                    }
                    resto2--;
                    pthread_mutex_unlock(&mutex1);

                    pilha1[i].push(argumentos[i]);


                    int rc;
                    argumentos[i].taskid = i;
                    int *numero = (int *)malloc(sizeof(int));
                    *numero = i;
                    rc = pthread_create(&Threads[i], NULL, for_estatico, (void *)numero);
                    if (rc != 0){
                        cout << "Failed to create threads" << '\n';
                    }
                }   
            }
        } 

        // para quando sao n ciclos completos 
        else {
            for (int j = 0; j < x; j++){
                for(int i=0; i<num_threads; i++){
                    if (i == 0 && j == 0){
                        comeco = inicio;
                        argumentos[i].inicio = inicio;
                    }
                    else {
                        argumentos[i].inicio = comeco + (chunk_size*passo);
                        comeco = argumentos[i].inicio;

                    }
                    argumentos[i].passo = passo;
                    argumentos[i].Final = argumentos[i].inicio + ((chunk_size*passo) - 1);

                    pilha1[i].push(argumentos[i]);
                    
                    int rc;
                    argumentos[i].taskid = i;
                    int *numero = (int *)malloc(sizeof(int));
                    *numero = i;
                    rc = pthread_create(&Threads[i], NULL, for_estatico, (void *)numero);
                    if (rc != 0){
                        cout << "Failed to create threads" << '\n';
                    }
                }
            }
        }
    }

    else if(schedule == 2){ //dinamico
        //criando threads
        int rc; 
        int t;
        estrutura auxiliar;
        auxiliar.Final=final;
        auxiliar.inicio= inicio;
        auxiliar.passo = passo;
        auxiliar.func = f;
        auxiliar.chunk = chunk_size;
        pilha.push(auxiliar);
        for(t=0; t<num_threads; t++){
            idsthreads[t] = (int *) malloc(sizeof(int)); *idsthreads[t] = t;
            rc = pthread_create(&Threads[t], NULL, &for_dynamic , (void *)idsthreads[t]);
        }
    }

    else if(schedule == 3){ //guilded
        //criando threads
        int rc; 
        int t;
        estrutura auxiliar;
        auxiliar.Final=final;
        auxiliar.inicio= inicio;
        auxiliar.passo = passo;
        auxiliar.func = f;
        auxiliar.chunk = chunk_size;
        pilha.push(auxiliar);
        for(t=0; t<num_threads; t++){
            idsthreads[t] = (int *) malloc(sizeof(int)); *idsthreads[t] = t;
            rc = pthread_create(&Threads[t], NULL, &for_guided , (void *)idsthreads[t]);
        }
    }
}

int main(){

    int valInicial;
    int valFinal;
    int valPasso;
    int schedule;
    int chunk_size;

    printf("Inicio: ");
    scanf("%d", &valInicial);
    printf("Fim: ");
    scanf("%d", &valFinal);
    printf("Schedule (1-static, 2-dynamic, 3-guided): ");
    scanf("%d", &schedule);
    printf("Passo: ");
    scanf("%d", &valPasso);
    printf("Chunk Size: ");
    scanf("%d", &chunk_size);
    omp_for(valInicial, valPasso, valFinal, schedule, chunk_size, f);

    for(int k=0; k<num_threads; k++){ //liberando as threads
        pthread_join(Threads[k], NULL);
    }
    pthread_exit(NULL);     
}   


