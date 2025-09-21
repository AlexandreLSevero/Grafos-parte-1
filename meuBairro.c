//  Nome: Alexandre Luppi Severo e Silva	RA: 10419724
//  Nome: Enrico Minto Spanier		        RA: 10419775
//  Nome: Guilherme Vecchi Bonotti Freitas  RA: 10418517

#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<memory.h>
#include<string.h>

// Definição de cores usadas na travessia do grafo (BFS/DFS)
#define BRANCO 0
#define CINZA  1
#define PRETO  2

typedef struct a{ /* Celula de uma lista de arestas */
    int    extremo2;
    struct a *prox;
}Arest;

typedef struct v{  /* Cada vertice tem um ponteiro para uma lista de arestas incidentes nele */
    int nome;
    int cor;
    Arest *prim;
}Vert;

// Protótipos das funções para manipulação do grafo
void criaGrafo(Vert **G, int ordem);
void destroiGrafo(Vert **G, int ordem);
int  acrescentaAresta(Vert G[], int ordem, int v1, int v2);
void imprimeGrafo(Vert G[], int ordem);

#define MAX_NOME_LOCAL 100
typedef struct {
    char nome[MAX_NOME_LOCAL];
    int v1;
    int v2;   // se == -1, localidade está em vértice único
} Localidade;

// Protótipos de funções para cálculo de distâncias
void calculaDistancias(Vert G[], int ordem, int origem, int dist[]);
int distanciaAteLocalidade(int dist[], Localidade loc);

// Função que cria um grafo com 'ordem' vértices
void criaGrafo(Vert **G, int ordem){
    int i;
    *G= (Vert*) malloc(sizeof(Vert)*ordem);
    for(i=0; i<ordem; i++){
        (*G)[i].nome= i;
        (*G)[i].cor= BRANCO;
        (*G)[i].prim= NULL;
    }
}

// Função que libera a memória do grafo e de suas listas de arestas
void destroiGrafo(Vert **G, int ordem){
    int i;
    Arest *a, *n;
    for(i=0; i<ordem; i++){
        a= (*G)[i].prim;
        while (a!= NULL){
              n= a->prox;
              free(a);
              a= n;
        }
    }
    free(*G);
}

// Adiciona aresta não direcionada entre v1 e v2
int acrescentaAresta(Vert G[], int ordem, int v1, int v2){
    Arest * A1, *A2;
    if (v1<0 || v1 >= ordem) return 0;
    if (v2<0 || v2 >= ordem) return 0;
    A1= (Arest *) malloc(sizeof(Arest));
    A1->extremo2= v2;
    A1->prox= G[v1].prim;
    G[v1].prim= A1;
    if (v1 == v2) return 1;
    A2= (Arest *) malloc(sizeof(Arest));
    A2->extremo2= v1;
    A2->prox= G[v2].prim;
    G[v2].prim= A2;
    return 1;
}

// Imprime o grafo em formato de lista de adjacência
void imprimeGrafo(Vert G[], int ordem){
    int i;
    Arest *aux;
    printf("\nOrdem:   %d",ordem);
    printf("\nLista de Adjacencia:\n");
    for (i=0; i<ordem; i++){
        printf("\n    v%d: ", i);
        aux= G[i].prim;
        for( ; aux != NULL; aux= aux->prox)
            printf("  v%d", aux->extremo2);
    }
    printf("\n\n");
}

// Calcula distâncias de origem a todos os vértices usando BFS
void calculaDistancias(Vert G[], int ordem, int origem, int dist[]) {
    int *fila = (int*) malloc(ordem * sizeof(int));
    int ini = 0, fim = 0;
    for (int i = 0; i < ordem; i++) dist[i] = INT_MAX;
    dist[origem] = 0;
    fila[fim++] = origem;
    while (ini < fim) {
        int v = fila[ini++];
        Arest *a = G[v].prim;
        while (a != NULL) {
            int u = a->extremo2;
            if (dist[u] == INT_MAX) {
                dist[u] = dist[v] + 1;
                fila[fim++] = u;
            }
            a = a->prox;
        }
    }
    free(fila);
}

// Retorna a menor distância a uma localidade
int distanciaAteLocalidade(int dist[], Localidade loc) {
    if (loc.v2 == -1) {
        return dist[loc.v1];
    } else {
        int d1 = dist[loc.v1];
        int d2 = dist[loc.v2];
        if (d1 == INT_MAX && d2 == INT_MAX) return INT_MAX;
        int menor = (d1 < d2) ? d1 : d2;
        if (menor == INT_MAX) return INT_MAX;
        return menor + 1;
    }
}

// Função principal
int main(int argc, char *argv[]) {
    Vert *G;
    int ordemG = 34;
    criaGrafo(&G, ordemG);

    // Arestas do graco e seus vértices adjacentes
    acrescentaAresta(G,ordemG,0,1);
    acrescentaAresta(G,ordemG,0,3);
    acrescentaAresta(G,ordemG,0,4);
    acrescentaAresta(G,ordemG,1,2);
    acrescentaAresta(G,ordemG,2,3);
    acrescentaAresta(G,ordemG,2,6);
    acrescentaAresta(G,ordemG,3,9);
    acrescentaAresta(G,ordemG,3,10);
    acrescentaAresta(G,ordemG,4,5);
    acrescentaAresta(G,ordemG,4,11);
    acrescentaAresta(G,ordemG,4,13);
    acrescentaAresta(G,ordemG,4,20);
    acrescentaAresta(G,ordemG,4,21);
    acrescentaAresta(G,ordemG,5,6);
    acrescentaAresta(G,ordemG,5,22);
    acrescentaAresta(G,ordemG,6,7);
    acrescentaAresta(G,ordemG,6,24);
    acrescentaAresta(G,ordemG,7,8);
    acrescentaAresta(G,ordemG,7,25);
    acrescentaAresta(G,ordemG,8,9);
    acrescentaAresta(G,ordemG,8,30);
    acrescentaAresta(G,ordemG,9,33);
    acrescentaAresta(G,ordemG,10,11);
    acrescentaAresta(G,ordemG,10,12);
    acrescentaAresta(G,ordemG,12,13);
    acrescentaAresta(G,ordemG,12,33);
    acrescentaAresta(G,ordemG,13,14);
    acrescentaAresta(G,ordemG,13,21);
    acrescentaAresta(G,ordemG,14,15);
    acrescentaAresta(G,ordemG,14,20);
    acrescentaAresta(G,ordemG,15,16);
    acrescentaAresta(G,ordemG,15,19);
    acrescentaAresta(G,ordemG,16,17);
    acrescentaAresta(G,ordemG,17,18);
    acrescentaAresta(G,ordemG,18,19);
    acrescentaAresta(G,ordemG,18,21);
    acrescentaAresta(G,ordemG,19,20);
    acrescentaAresta(G,ordemG,21,22);
    acrescentaAresta(G,ordemG,22,23);
    acrescentaAresta(G,ordemG,23,24);
    acrescentaAresta(G,ordemG,24,25);
    acrescentaAresta(G,ordemG,25,26);
    acrescentaAresta(G,ordemG,25,28);
    acrescentaAresta(G,ordemG,26,27);
    acrescentaAresta(G,ordemG,27,28);
    acrescentaAresta(G,ordemG,28,29);
    acrescentaAresta(G,ordemG,29,30);
    acrescentaAresta(G,ordemG,29,31);
    acrescentaAresta(G,ordemG,30,32);
    acrescentaAresta(G,ordemG,31,32);
    acrescentaAresta(G,ordemG,32,33);

    // Inicializa localidades
    Localidade locais[20];
    int qtdLocal = 0;

    strcpy(locais[qtdLocal].nome,"Minha casa");
    locais[qtdLocal].v1 = 0; locais[qtdLocal].v2 = 3; qtdLocal++;
    strcpy(locais[qtdLocal].nome,"Vanessa Fotografia");
    locais[qtdLocal].v1 = 0; locais[qtdLocal].v2 = 1; qtdLocal++;
    strcpy(locais[qtdLocal].nome,"Centro Educacional Florescer");
    locais[qtdLocal].v1 = 6; locais[qtdLocal].v2 = 7; qtdLocal++;
    strcpy(locais[qtdLocal].nome,"Secretaria Municipal da Saude");
    locais[qtdLocal].v1 = 9; locais[qtdLocal].v2 = 33; qtdLocal++;
    strcpy(locais[qtdLocal].nome,"Canil da Policia Militar");
    locais[qtdLocal].v1 = 14; locais[qtdLocal].v2 = 15; qtdLocal++;
    strcpy(locais[qtdLocal].nome,"Carecas Pizza");
    locais[qtdLocal].v1 = 15; locais[qtdLocal].v2 = 16; qtdLocal++;
    strcpy(locais[qtdLocal].nome,"Tuca's Esfiha");
    locais[qtdLocal].v1 = 16; locais[qtdLocal].v2 = 17; qtdLocal++;
    strcpy(locais[qtdLocal].nome,"Horto Florestal");
    locais[qtdLocal].v1 = 17; locais[qtdLocal].v2 = 18; qtdLocal++;
    strcpy(locais[qtdLocal].nome,"Samurai Brindes");
    locais[qtdLocal].v1 = 15; locais[qtdLocal].v2 = 19; qtdLocal++;
    strcpy(locais[qtdLocal].nome,"Posto Shell");
    locais[qtdLocal].v1 = 18; locais[qtdLocal].v2 = 19; qtdLocal++;
    strcpy(locais[qtdLocal].nome,"Espaco Aya");
    locais[qtdLocal].v1 = 18; locais[qtdLocal].v2 = 21; qtdLocal++;
    strcpy(locais[qtdLocal].nome,"Casa do Repouso Solar da Cantareira");
    locais[qtdLocal].v1 = 21; locais[qtdLocal].v2 = 22; qtdLocal++;
    strcpy(locais[qtdLocal].nome,"4 Patas Pet House");
    locais[qtdLocal].v1 = 24; locais[qtdLocal].v2 = 25; qtdLocal++;
    strcpy(locais[qtdLocal].nome,"Feira Tremembe");
    locais[qtdLocal].v1 = 25; locais[qtdLocal].v2 = 28; qtdLocal++;
    strcpy(locais[qtdLocal].nome,"Padaria Florestal");
    locais[qtdLocal].v1 = 27; locais[qtdLocal].v2 = 28; qtdLocal++;
    strcpy(locais[qtdLocal].nome,"Banco do Brasil");
    locais[qtdLocal].v1 = 28; locais[qtdLocal].v2 = 29; qtdLocal++;
    strcpy(locais[qtdLocal].nome,"Otica Voluntarios");
    locais[qtdLocal].v1 = 29; locais[qtdLocal].v2 = 30; qtdLocal++;
    strcpy(locais[qtdLocal].nome,"Residencial Villa Floresta");
    locais[qtdLocal].v1 = 29; locais[qtdLocal].v2 = 31; qtdLocal++;
    strcpy(locais[qtdLocal].nome,"Tapeceiro");
    locais[qtdLocal].v1 = 31; locais[qtdLocal].v2 = 32; qtdLocal++;
    strcpy(locais[qtdLocal].nome,"Comercio de Ferramentas");
    locais[qtdLocal].v1 = 32; locais[qtdLocal].v2 = 33; qtdLocal++;

    // Calcula distâncias a partir da minha casa (v0)
    int *dist = (int*) malloc(ordemG * sizeof(int));
    int origem = 0; // sempre v0 como Minha Casa
    calculaDistancias(G, ordemG, origem, dist);

    //Imprime distâncias em quadras para cada localidade
    printf("\nDistancias em quadras a partir da Minha Casa (v0):\n");
    for (int i=0;i<qtdLocal;i++){
        int d = distanciaAteLocalidade(dist,locais[i]);
        if (d==INT_MAX){
            printf(" - %s: INACESSIVEL\n",locais[i].nome);
        } else {
            printf(" - %s: %d quadras\n",locais[i].nome,d);
        }
    }

    imprimeGrafo(G,ordemG);

    free(dist);
    destroiGrafo(&G,ordemG);
    return 0;
}
