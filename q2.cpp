//#include <bits/stdc++.h>
#include<vector>
#include<iostream>
#include<map>
#include<list>
#include <pthread.h>
using namespace std;

//pthread_mutex_t lock0;
pthread_mutex_t mutex0=PTHREAD_MUTEX_INITIALIZER;

map<int, list<int> > adj;
vector<vector<int> > visits(1);  
vector<vector<int> > path(1);
vector<int> flag(1);
int nv;
vector<int> prints;

void addEdge(int v, int w, map<int, list<int> > &adj){
	// adiciona o vertice w ao vector de adjacencias
	adj[v].push_back(w);
}

void DFS(int v, vector<vector<int> > visits, map<int, list<int> > &adj, int threadID){
	    
	    visits[threadID][v] = 1; //marca como visitado
	    
	    //coloco o mutex para adicionar em path os vertices
	    pthread_mutex_lock(&mutex0);
	    
	    if (flag[threadID] != 1) path[threadID].push_back(v); //coloco no array da thread 0 os vertices do path quando o source eh 0, por exemplo
    	
    	pthread_mutex_unlock(&mutex0);
    	
    	list<int>::iterator i;
    	for (i = adj[v].begin(); i != adj[v].end(); ++i){
    		
    		//se o vertice ainda nao foi visitado, chama o DFS
    		if (visits[threadID][*i] == 0) DFS(*i, visits, adj, threadID);
    		
    		//entra no else se identificar o ciclo
    		else{
    		    for(int j = 0; j < path[threadID].size(); ++j){
    		        if(path[threadID][j] == *i){
    		          flag[threadID] = 1;
    		        } 
    		    }
    		}
        }
}
void *multithread_dfs(void *thread) {
    
    int threadID = *((int*) thread); 
    
    DFS(threadID, visits, adj, threadID); //uma thread para cada vertice
    
    //mutex pra poder printar corretamente e nao dar confusao
    pthread_mutex_lock(&mutex0);
    cout << "A thread " << threadID << " entrou na funcao" << endl;
    if(flag[threadID] == 1) cout << "A thread " << threadID << " achou um ciclo. Logo, tem deadlock!\n" ;
    else cout << "A thread " << threadID << " nÃ£o achou deadlock\n";
    cout << "\n";
    pthread_mutex_unlock(&mutex0);
    pthread_exit(NULL);
}

int main(){
    
	int v1, v2, na;
	
	cout << "Digite o numero total de processos e de recursos:\n";
	cin >> nv;
	cout << "Digite o numero de arestas do grafo:\n";
	cin >> na;
	
	int NUM_THREADS = nv;
	
	visits.resize(nv);
    path.resize(nv);
    flag.resize(nv, 0);
	
	//vetor de visitados comeca com 0 (nenhum ainda foi visitado)
	for(int i = 0; i< nv; i++){
	    visits[i].resize(nv);
        for(int j = 0; j< nv; j++){
            visits[i][j] = 0;
        }    
    }
	
	cout << "Entre com 2 vertices por vez para formarmos as arestas do grafo, sendo o primeiro a origem e o segundo o destino:\n";
	for(int j=0; j<na; j++){
	    cin >> v1 >> v2;
	    addEdge(v1, v2, adj);
	}
	
	pthread_t threads[NUM_THREADS];
    int *taskids[NUM_THREADS];

    //criando as threads e mandando pra funcao
    int rc; 
    int t;
    for(t=0; t<NUM_THREADS; t++){
        taskids[t] = (int *) malloc(sizeof(int)); 
        *taskids[t] = t;
        rc = pthread_create(&threads[t], NULL , multithread_dfs, (void *)taskids[t]);
    }
    int u;
    //espera as threads executarem e depois liberando
    for(u=0; u<NUM_THREADS;u++) {
        int *res;
        pthread_join(threads[u], (void **) &res);
        //printf("%d\n",*res);
        cout << "Liberando thread " << u << "\n";
    }   
    pthread_exit(NULL);
	
	free(taskids);
    return 0;
}