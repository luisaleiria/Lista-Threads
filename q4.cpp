#include <iostream>
#include <vector>
#include <queue>
#include <pthread.h>

using namespace std;
//u  //v
typedef pair<long long, pair<int, int> > pli;
long long inf = 1e18;
const int Maximo = 2e5 + 5;
int n, m;
//int x, y;
int ds[Maximo], siz[Maximo], matriz[Maximo][Maximo];
int par[Maximo];
int T; //num de threads
int counter = 0; //num de ilhas
pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;
int contadorx; //contador global linhas para as threads
int contadory; //contador global colunas para as thread


//tem varios conjuntos disjuntos e pergunto a qual conjunto x  elemento pertence
//basicamente é pegar o representante do conjunto
int dsFind(int a){
    
    if(ds[a]!=a){
        ds[a] = dsFind(ds[a]);
    }
    return ds[a];
}

//tem a e b que pertencem a conjuntos que não sei e quero juntar eles
//pega o representante de dois conjuntos e une eles, escolhendo o novo representante
void dsUnion(int a, int b){
    int temp;
    if(a>b){
        //swap a e b
        temp = a;
        a = b;
        b = temp;
    }
    a = dsFind(a);
    b = dsFind(b);
    ds[b] = a;
}

//percorre a matriz e checa se tem 1 nos arredores 
 void functeste (int x, int y, int threadID){
    if (x-1 >= 0 && matriz[x-1][y] == matriz[x][y]){
        pthread_mutex_lock(&mymutex); 
        dsUnion (x*m + y, (x-1)*m + y);
        printf("A thread %d descobriu o uma extensão de ilha na posição %d %d\n", threadID, x, y);
        pthread_mutex_unlock(&mymutex);
    }
    if (y-1 >= 0 && matriz[x][y-1] == matriz[x][y]){
        pthread_mutex_lock(&mymutex); 
        dsUnion (x*m + y, x*m + y - 1);
        printf("A thread %d descobriu o uma extensão de ilha na posição %d %d\n", threadID, x, y);
        pthread_mutex_unlock(&mymutex);
    }
    if (x+1 < n && matriz[x+1][y] == matriz[x][y]){
        pthread_mutex_lock(&mymutex); 
        dsUnion (x*m + y, (x+1)*m + y);
        printf("A thread %d descobriu o uma extensão de ilha na posição %d %d\n", threadID, x, y);
        pthread_mutex_unlock(&mymutex);
    }
    if (y+1 < m && matriz[x][y+1] == matriz[x][y]){
        pthread_mutex_lock(&mymutex); 
        dsUnion (x*m + y, x*m + y + 1);
        printf("A thread %d descobriu o uma extensão de ilha na posição %d %d\n", threadID, x, y);
        pthread_mutex_unlock(&mymutex);
    }
    if (x+1 < n && y+1 < m && matriz[x+1][y+1] == matriz[x][y]){
        pthread_mutex_lock(&mymutex); 
        dsUnion (x*m + y, (x+1)*m + y + 1);
        printf("A thread %d descobriu o uma extensão de ilha na posição %d %d\n", threadID, x, y);
        pthread_mutex_unlock(&mymutex);
    }
    if (x-1 >= 0 && y+1 < m && matriz[x-1][y+1] == matriz[x][y]){
        pthread_mutex_lock(&mymutex); 
        dsUnion (x*m + y, (x-1)*m + y + 1);
        printf("A thread %d descobriu o uma extensão de ilha na posição %d %d\n", threadID, x, y);
        pthread_mutex_unlock(&mymutex);
    }
    if (x+1 < n && y-1 >= 0 && matriz[x+1][y-1] == matriz[x][y]){
        pthread_mutex_lock(&mymutex); 
        dsUnion (x*m + y, (x+1)*m + y - 1);
        printf("A thread %d descobriu o uma extensão de ilha na posição %d %d\n", threadID, x, y);
        pthread_mutex_unlock(&mymutex);
    }
    if (x-1 >= 0 && y-1 >= 0 && matriz[x-1][y-1] == matriz[x][y]){
        pthread_mutex_lock(&mymutex); 
        dsUnion (x*m + y, (x-1)*m + y - 1);
        printf("A thread %d descobriu o uma extensão de ilha na posição %d %d\n", threadID, x, y);
        pthread_mutex_unlock(&mymutex);
    }
 }

void *verificando(void *thread){
    int threadID = *((int*) thread);
    int c = 0;

    // faz com que as threads comecem em linhas difrentes 
    if (threadID < n){
        c = threadID;
    }

    // chamando a funcao
    for (int i = c; i < n; i++){
        for (int j = 0; j < m; j++){
            // só vai para a função se for ilha (igual a 1)
            if (matriz[i][j] == 1) {
                functeste(i, j, threadID);
            }
        }
    }
    pthread_exit(NULL);
}

int main (){
    int cont = 0;
    int resp = 0;
    
    
    cout << "Digite o numero de linhas:" << '\n';
    cin >> n;
    cout << "Digite o numero de colunas:" << '\n';
    cin >> m;

    cout << "Digite a matriz:" << '\n';


    //recebendo a matriz
    for (int i = 0; i < n; i++){
        for (int j = 0; j < m; j++){
            cin >> matriz[i][j];
        }
    }

    int c=0;
    //montando o ds (quando mar = -1) 
    for (int i = 0; i < n; i++){
        for (int j = 0; j < m; j++){
            if (matriz[i][j] == 0){
                ds[c] = -1;
            }
            else {
                ds[c] = c;
            }
            c++;
        }
    }
    
    int total = n*m;

    printf ("Digite o numero de threads: ");
    cin >> T;
    
    
    pthread_mutex_init(&mymutex, NULL);

    //criando as threads e mandando pra funcao
    pthread_t threads[T];
    int *taskids[T];
    int rc;
    int t;

    for(t=0; t<T; t++){
        taskids[t] = (int *) malloc(sizeof(int)); 
        *taskids[t] = t;
        rc = pthread_create(&threads[t], NULL , verificando, (void *)taskids[t]);
        if (rc != 0){
            cout << "Failed to create threads" << '\n';
        }
    }
    
    for(t=0; t<T; t++){
        pthread_join(threads[t], NULL);
        
    }

    cout << '\n';
    cout << "o numero de ilhas e: ";

    // resposta
    for (int i = 0; i<total; i++){
        if (ds[i]==i){
            resp++;
        }
    }
    
    cout << resp << '\n';
    cout << '\n';
    
    for(int k=0; k<T; k++){ //liberando as threads
        pthread_join(threads[k], NULL);
        printf("Liberando thread %d\n", k);
    }

}