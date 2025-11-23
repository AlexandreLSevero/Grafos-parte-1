// Nome: Alexandre Luppi Severo e Silva	RA: 10419724
// Nome: Enrico Minto Spanier		RA: 10419775
// Nome: Guilherme Vecchi Bonotti Freitas RA: 10418517

#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<memory.h>
#include<string.h>
#include<float.h> // Para usar FLT_MAX

// Definição de cores
#define BRANCO 0
#define CINZA 1
#define PRETO 2

// Constante para representar o infinito no cálculo de distâncias (float)
#define F_INFINITY FLT_MAX

/*
 * Estrutura da aresta (célula de lista de adjacência)
 * Inclui o peso (distância em metros) da aresta.
 */
typedef struct a{
	int extremo2;
    float peso; // Peso da aresta (distância em metros)
	struct a *prox;
}Arest;

/*
 * Estrutura do vértice.
 * 'pred' (predecessor) é essencial para a reconstrução do caminho.
 */
typedef struct v{
	int nome;
	int cor;
	Arest *prim;
	int pred; // Predecessor para reconstruir o caminho (usado no Dijkstra)
}Vert;

/*
 * Estrutura da Localidade.
 * Inclui a posição exata da localidade na aresta.
 */
#define MAX_NOME_LOCAL 100
typedef struct {
	char nome[MAX_NOME_LOCAL];
	int v1;
	int v2;        // Se == -1, localidade está em vértice único.
    float dist_v1; // Distância em metros de v1 até a Localidade.
    float dist_v2; // Distância em metros de v2 até a Localidade.
} Localidade;

// Protótipos das funções de manipulação do grafo
void criaGrafo(Vert **G, int ordem);
void destroiGrafo(Vert **G, int ordem);
int acrescentaAresta(Vert G[], int ordem, int v1, int v2, float peso);
void imprimeGrafo(Vert G[], int ordem);

// Protótipos de funções de cálculo de distâncias (Dijkstra) e caminhos
void calculaDistanciasDijkstra(Vert G[], int ordem, int origem, float dist[]);
float distanciaAteLocalidade(float dist[], Localidade loc, int *vertice_mais_proximo);
void imprimeCaminho(Vert G[], int origem, int destino);
// **NOVAS FUNÇÕES PARA O TSP**
void resetaPredecessores(Vert G[], int ordem);
float calculaMatrizDistancias(Vert G[], int ordem, Localidade locais[], int idx_loc_origem, int idx_loc_destino, int *vert_origem, int *vert_destino, int *vert_prox_destino);
void tspVizinhoMaisProximo(Vert G[], int ordem, Localidade locais[], int qtdLocal, int locais_a_visitar_idx[], int qtdVisitar);


/*
 * Cria um grafo com 'ordem' vértices.
 */
void criaGrafo(Vert **G, int ordem){
	int i;
	*G= (Vert*) malloc(sizeof(Vert)*ordem);
	for(i=0; i<ordem; i++){
		(*G)[i].nome= i;
		(*G)[i].cor= BRANCO;
		(*G)[i].prim= NULL;
		(*G)[i].pred= -1;
	}
}

/*
 * Libera a memória do grafo e de suas listas de arestas.
 */
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

/*
 * Acrescenta uma aresta não direcionada entre os vértices v1 e v2 com o peso fornecido.
 */
int acrescentaAresta(Vert G[], int ordem, int v1, int v2, float peso){
	Arest * A1, *A2;
	if (v1<0 || v1 >= ordem) return 0;
	if (v2<0 || v2 >= ordem) return 0;

    // Aresta 1 (v1 -> v2)
	A1= (Arest *) malloc(sizeof(Arest));
	A1->extremo2= v2;
    A1->peso = peso;
	A1->prox= G[v1].prim;
	G[v1].prim= A1;

	if (v1 == v2) return 1;

    // Aresta 2 (v2 -> v1)
	A2= (Arest *) malloc(sizeof(Arest));
	A2->extremo2= v1;
    A2->peso = peso;
	A2->prox= G[v2].prim;
	G[v2].prim= A2;
	return 1;
}

/*
 * Imprime o grafo em formato de lista de adjacência (com pesos em metros).
 */
void imprimeGrafo(Vert G[], int ordem){
	int i;
	Arest *aux;
	printf("\nOrdem: %d",ordem);
	printf("\nLista de Adjacencia (v -> v [peso m]):\n");
	for (i=0; i<ordem; i++){
		printf("\n v%d: ", i);
		aux= G[i].prim;
		for( ; aux != NULL; aux= aux->prox)
			printf(" v%d [%.2fm]", aux->extremo2, aux->peso);
	}
	printf("\n\n");
}

/*
 * Reinicia o array de predecessor. Essencial antes de cada execução do Dijkstra.
 */
void resetaPredecessores(Vert G[], int ordem) {
    for (int i = 0; i < ordem; i++) {
        G[i].pred = -1;
    }
}

/*
 * Implementação do Algoritmo de Dijkstra para calcular a menor distância em metros
 * (Grafo Ponderado).
 * dist[] deve ser inicializado antes de chamar a função.
 */
void calculaDistanciasDijkstra(Vert G[], int ordem, int origem, float dist[]) {
    // Implementação O(V^2) de Dijkstra sem Fila de Prioridade
    int *S = (int*) calloc(ordem, sizeof(int));
    
	for (int i = 0; i < ordem; i++) {
		dist[i] = F_INFINITY;
        G[i].pred = -1; // Reset do pred para a busca atual
	}
	dist[origem] = 0.0f;

    for (int count = 0; count < ordem - 1; count++) {
        float min_dist = F_INFINITY;
        int u = -1;
        
        for (int i = 0; i < ordem; i++) {
            if (S[i] == 0 && dist[i] <= min_dist) {
                min_dist = dist[i];
                u = i;
            }
        }

        if (u == -1) break;
        S[u] = 1;

        Arest *a = G[u].prim;
        while (a != NULL) {
            int v = a->extremo2;
            float peso = a->peso;
            
            if (S[v] == 0 && dist[u] != F_INFINITY && dist[u] + peso < dist[v]) {
                dist[v] = dist[u] + peso;
                G[v].pred = u;
            }
            a = a->prox;
        }
    }
    free(S);
}

/*
 * Retorna a menor distância em metros a uma localidade e o vértice mais próximo.
 * O cálculo usa os pesos exatos (dist_v1 e dist_v2).
 */
float distanciaAteLocalidade(float dist[], Localidade loc, int *vertice_mais_proximo) {
	if (loc.v2 == -1) {
		*vertice_mais_proximo = loc.v1;
		return dist[loc.v1];
	} else {
		// Distância = D(origem -> v1) + d(v1 -> local)
		float d1 = (dist[loc.v1] == F_INFINITY) ? F_INFINITY : dist[loc.v1] + loc.dist_v1;
		// Distância = D(origem -> v2) + d(v2 -> local)
		float d2 = (dist[loc.v2] == F_INFINITY) ? F_INFINITY : dist[loc.v2] + loc.dist_v2;
		
		if (d1 == F_INFINITY && d2 == F_INFINITY) {
			*vertice_mais_proximo = -1;
			return F_INFINITY;
		}
		
		if (d1 <= d2) {
			*vertice_mais_proximo = loc.v1; // Vértice do grafo que minimiza o caminho até a localidade
			return d1;
		} else {
			*vertice_mais_proximo = loc.v2;
			return d2;
		}
	}
}

/*
 * Implementação auxiliar para imprimir o caminho entre dois vértices (não localidades).
 * Retorna o último vértice do caminho (o vértice pred do destino).
 */
int encontraCaminhoEntreVertices(Vert G[], int origem, int destino) {
	if (destino == origem) {
		printf("v%d", origem);
        return origem;
	} else if (G[destino].pred == -1) {
		printf("Caminho inacessível (erro na busca).");
        return -1;
	} else {
		// Imprime recursivamente o caminho, exceto o destino (que será impresso na main)
		int pred_destino = G[destino].pred;
        int penultimo = encontraCaminhoEntreVertices(G, origem, pred_destino);
        if (penultimo != -1) {
            printf(" -> v%d", destino);
        }
        return destino; // Retorna o destino
	}
}

/*
 * Calcula a distância entre DUAS LOCALIDADES (origem e destino)
 * e determina o vértice de chegada (origem) e o vértice de partida (destino) no grafo.
 * Este é o coração do cálculo para o TSP.
 */
float calculaMatrizDistancias(Vert G[], int ordem, Localidade locais[], int idx_loc_origem, int idx_loc_destino, int *vert_origem, int *vert_destino, int *vert_prox_destino) {
    float *dist = (float*) malloc(ordem * sizeof(float));
    float menor_distancia = F_INFINITY;
    
    Localidade loc_origem = locais[idx_loc_origem];
    Localidade loc_destino = locais[idx_loc_destino];

    // O local de origem pode ser o vértice v1 ou v2 da aresta
    int vertices_origem[] = {loc_origem.v1, loc_origem.v2};
    int vertices_destino[] = {loc_destino.v1, loc_destino.v2};
    
    // O local de destino pode ser alcançado por v1 ou v2 da aresta destino
    float dist_local_a_v1[] = {loc_destino.dist_v1, loc_destino.dist_v2};

    // Percorre todas as combinações de partida da origem para chegada no destino
    for (int i = 0; i < 2; i++) { // Vértice de partida no grafo (v1 ou v2 do local_origem)
        int v_partida = vertices_origem[i];
        if (v_partida == -1) continue;
        
        // 1. Roda Dijkstra a partir do Vértice de Partida (v_partida)
        resetaPredecessores(G, ordem);
        calculaDistanciasDijkstra(G, ordem, v_partida, dist);
        
        // 2. Calcula a distância total: (Distância local_origem -> v_partida) + D(v_partida -> v_chegada) + (Distância v_chegada -> local_destino)
        
        // Distância do Local de Origem até o Vértice de Partida (que é o final da primeira perna)
        float dist_origem_a_partida;
        if (v_partida == loc_origem.v1) dist_origem_a_partida = loc_origem.dist_v1;
        else if (v_partida == loc_origem.v2) dist_origem_a_partida = loc_origem.dist_v2;
        else dist_origem_a_partida = 0.0f; // Nunca deveria ocorrer se for bem modelado
        
        // Testa as duas possíveis chegadas ao destino
        for (int j = 0; j < 2; j++) {
            int v_chegada = vertices_destino[j];
            if (v_chegada == -1) continue;
            
            if (dist[v_chegada] == F_INFINITY) continue;

            // Distância do Vértice de Chegada até o Local de Destino
            float dist_chegada_a_local;
            if (v_chegada == loc_destino.v1) dist_chegada_a_local = loc_destino.dist_v1;
            else if (v_chegada == loc_destino.v2) dist_chegada_a_local = loc_destino.dist_v2;
            else dist_chegada_a_local = 0.0f;

            // Distância Total
            float dist_total = dist_origem_a_partida + dist[v_chegada] + dist_chegada_a_local;

            // 3. Atualiza o melhor caminho
            if (dist_total < menor_distancia) {
                menor_distancia = dist_total;
                *vert_origem = v_partida; // Vértice de onde o caminho inicia no grafo
                *vert_destino = v_chegada; // Vértice onde o caminho termina no grafo
                // Armazena o predecessor do *vert_destino* (necessário para reconstruir o caminho)
                *vert_prox_destino = G[v_chegada].pred; 
            }
        }
    }
    
    free(dist);
    return menor_distancia;
}

/*
 * Algoritmo do Caixeiro Viajante (TSP) usando Heurística do Vizinho Mais Próximo.
 * Inicia na Minha Casa (índice 0) e sempre escolhe o próximo local não visitado mais próximo.
 */
void tspVizinhoMaisProximo(Vert G[], int ordem, Localidade locais[], int qtdLocal, int locais_a_visitar_idx[], int qtdVisitar) {
    if (qtdVisitar == 0) return;

    int *visitado = (int*) calloc(qtdVisitar, sizeof(int));
    int atual_idx = 0; // Começa na Minha Casa (índice 0)
    int count_visitados = 1;
    float distancia_total = 0.0f;
    
    printf("\n\n#####################################################\n");
    printf("## SOLUÇÃO DO PROBLEMA DO CAIXEIRO VIAJANTE (TSP) ##\n");
    printf("## Heurística: Vizinho Mais Próximo             ##\n");
    printf("#####################################################\n");
    
    printf("\nConjunto de Locais a Visitar (incluindo Minha Casa no início e fim):\n");
    for (int i = 0; i < qtdVisitar; i++) {
        printf(" - [%d] %s\n", i, locais[locais_a_visitar_idx[i]].nome);
    }
    printf("-----------------------------------------------------\n");

    printf("\nROTEIRO ÓTIMO OBTIDO:\n");
    printf("%s", locais[locais_a_visitar_idx[0]].nome);
    visitado[0] = 1;

    // Roteiro principal
    for (int i = 0; i < qtdVisitar - 1; i++) {
        int proximo_idx = -1;
        float menor_distancia = F_INFINITY;
        
        // Variáveis para a reconstrução do caminho do melhor vizinho
        int melhor_vert_origem = -1;
        int melhor_vert_destino = -1;
        
        // 1. Encontra o Vizinho Mais Próximo
        for (int j = 1; j < qtdVisitar; j++) { // Pula a casa (índice 0)
            if (visitado[j] == 0) {
                int vert_origem, vert_destino, vert_prox_destino;
                
                // Calcula a distância entre o local atual e o próximo não visitado
                float dist = calculaMatrizDistancias(G, ordem, locais, locais_a_visitar_idx[atual_idx], locais_a_visitar_idx[j], &vert_origem, &vert_destino, &vert_prox_destino);
                
                if (dist < menor_distancia) {
                    menor_distancia = dist;
                    proximo_idx = j;
                    melhor_vert_origem = vert_origem;
                    melhor_vert_destino = vert_destino;
                }
            }
        }

        if (proximo_idx == -1) break; // Todos visitados

        // 2. Registra o Trecho
        printf(" -> %s (%.2f metros)\n", locais[locais_a_visitar_idx[proximo_idx]].nome, menor_distancia);
        distancia_total += menor_distancia;
        
        // 3. Imprime o Caminho Detalhado (Recalcula o caminho do trecho ótimo)
        // Isso é necessário porque o cálculo da matriz sobrescreve o pred do grafo
        float *dist_aux = (float*) malloc(ordem * sizeof(float));
        resetaPredecessores(G, ordem);
        calculaDistanciasDijkstra(G, ordem, melhor_vert_origem, dist_aux);
        
        printf("    [Sequência de Vértices: Local -> v%d", melhor_vert_origem);
        encontraCaminhoEntreVertices(G, melhor_vert_origem, melhor_vert_destino);
        printf(" -> Local]\n");

        free(dist_aux);
        
        // 4. Atualiza o estado
        visitado[proximo_idx] = 1;
        atual_idx = proximo_idx;
        count_visitados++;
    }

    // 5. Retorno à Minha Casa (índice 0)
    printf("\nVOLTANDO PARA MINHA CASA:\n");
    
    int vert_origem_final, vert_destino_final, vert_prox_destino_final;
    float dist_final = calculaMatrizDistancias(G, ordem, locais, locais_a_visitar_idx[atual_idx], locais_a_visitar_idx[0], &vert_origem_final, &vert_destino_final, &vert_prox_destino_final);
    
    printf("%s -> %s (%.2f metros)\n", locais[locais_a_visitar_idx[atual_idx]].nome, locais[locais_a_visitar_idx[0]].nome, dist_final);
    distancia_total += dist_final;

    // Imprime o Caminho Detalhado (Volta)
    float *dist_aux_final = (float*) malloc(ordem * sizeof(float));
    resetaPredecessores(G, ordem);
    calculaDistanciasDijkstra(G, ordem, vert_origem_final, dist_aux_final);
    
    printf("    [Sequência de Vértices: Local -> v%d", vert_origem_final);
    encontraCaminhoEntreVertices(G, vert_origem_final, vert_destino_final);
    printf(" -> Local]\n");
    
    free(dist_aux_final);
    
    printf("\n-----------------------------------------------------\n");
    printf("DISTÂNCIA TOTAL PERCORRIDA: %.2f metros\n", distancia_total);
    printf("#####################################################\n\n");
    
    free(visitado);
}

// Função principal (AGORA COM A LÓGICA DO TSP)
int main(int argc, char *argv[]) {
	Vert *G;
	int ordemG = 34;
	criaGrafo(&G, ordemG);

	// --- 1. CONSTRUÇÃO DO GRAFO PONDERADO ---
	acrescentaAresta(G,ordemG,0,1,110.0f);
	acrescentaAresta(G,ordemG,0,3,112.0f);
	acrescentaAresta(G,ordemG,0,4,59.0f);
	acrescentaAresta(G,ordemG,1,2,118.0f);
	acrescentaAresta(G,ordemG,2,3,78.0f);
	acrescentaAresta(G,ordemG,2,6,37.0f);
	acrescentaAresta(G,ordemG,3,9,133.0f);
	acrescentaAresta(G,ordemG,3,10,174.0f);
	acrescentaAresta(G,ordemG,4,5,174.0f);
	acrescentaAresta(G,ordemG,4,11,131.0f);
	acrescentaAresta(G,ordemG,4,13,334.0f);
	acrescentaAresta(G,ordemG,4,20,228.0f);
	acrescentaAresta(G,ordemG,4,21,116.0f);
	acrescentaAresta(G,ordemG,5,6,211.0f);
	acrescentaAresta(G,ordemG,5,22,153.0f);
	acrescentaAresta(G,ordemG,6,7,174.0f);
	acrescentaAresta(G,ordemG,6,24,105.0f);
	acrescentaAresta(G,ordemG,7,8,146.0f);
	acrescentaAresta(G,ordemG,7,25,145.0f);
	acrescentaAresta(G,ordemG,8,9,30.0f);
	acrescentaAresta(G,ordemG,8,30,31.0f);
	acrescentaAresta(G,ordemG,9,33,324.0f);
	acrescentaAresta(G,ordemG,10,11,154.0f);
	acrescentaAresta(G,ordemG,10,12,158.0f);
	acrescentaAresta(G,ordemG,12,13,58.0f);
	acrescentaAresta(G,ordemG,12,33,112.0f);
	acrescentaAresta(G,ordemG,13,14,77.0f);
	acrescentaAresta(G,ordemG,13,21,311.0f);
	acrescentaAresta(G,ordemG,14,15,214.0f);
	acrescentaAresta(G,ordemG,14,20,266.0f);
	acrescentaAresta(G,ordemG,15,16,161.0f);
	acrescentaAresta(G,ordemG,15,19,177.0f);
	acrescentaAresta(G,ordemG,16,17,94.0f);
	acrescentaAresta(G,ordemG,17,18,560.0f);
	acrescentaAresta(G,ordemG,18,19,113.0f);
	acrescentaAresta(G,ordemG,18,21,416.0f);
	acrescentaAresta(G,ordemG,19,20,239.0f);
	acrescentaAresta(G,ordemG,21,22,291.0f);
	acrescentaAresta(G,ordemG,22,23,89.0f);
	acrescentaAresta(G,ordemG,23,24,432.0f);
	acrescentaAresta(G,ordemG,24,25,188.0f);
	acrescentaAresta(G,ordemG,25,26,254.0f);
	acrescentaAresta(G,ordemG,25,28,184.0f);
	acrescentaAresta(G,ordemG,26,27,80.0f);
	acrescentaAresta(G,ordemG,27,28,73.0f);
	acrescentaAresta(G,ordemG,28,29,259.0f);
	acrescentaAresta(G,ordemG,29,30,139.0f);
	acrescentaAresta(G,ordemG,29,31,376.0f);
	acrescentaAresta(G,ordemG,30,32,332.0f);
	acrescentaAresta(G,ordemG,31,32,137.0f);
	acrescentaAresta(G,ordemG,32,33,116.0f);

	// --- 2. INICIALIZAÇÃO DAS LOCALIDADES COM POSIÇÕES EM METROS ---
	Localidade locais[20];
	int qtdLocal = 0;
	
    // 0. Minha Casa
	strcpy(locais[qtdLocal].nome,"Minha Casa");
	locais[qtdLocal].v1 = 0; locais[qtdLocal].v2 = 3;
    locais[qtdLocal].dist_v1 = 46.0f; locais[qtdLocal].dist_v2 = 66.0f; qtdLocal++;
    
    // 1. Vanessa Fotografia
	strcpy(locais[qtdLocal].nome,"Vanessa Fotografia");
	locais[qtdLocal].v1 = 0; locais[qtdLocal].v2 = 1;
    locais[qtdLocal].dist_v1 = 40.0f; locais[qtdLocal].dist_v2 = 70.0f; qtdLocal++;
    
    // 2. Centro Educacional Florescer
	strcpy(locais[qtdLocal].nome,"Centro Educacional Florescer");
	locais[qtdLocal].v1 = 6; locais[qtdLocal].v2 = 7;
    locais[qtdLocal].dist_v1 = 80.0f; locais[qtdLocal].dist_v2 = 94.0f; qtdLocal++;
    
    // 3. Secretaria Municipal da Saude
	strcpy(locais[qtdLocal].nome,"Secretaria Municipal da Saude");
	locais[qtdLocal].v1 = 9; locais[qtdLocal].v2 = 33;
    locais[qtdLocal].dist_v1 = 277.0f; locais[qtdLocal].dist_v2 = 47.0f; qtdLocal++;
    
    // 4. Canil da Policia Militar
	strcpy(locais[qtdLocal].nome,"Canil da Policia Militar");
	locais[qtdLocal].v1 = 14; locais[qtdLocal].v2 = 15;
    locais[qtdLocal].dist_v1 = 134.0f; locais[qtdLocal].dist_v2 = 80.0f; qtdLocal++;
    
    // 5. Carecas Pizza
	strcpy(locais[qtdLocal].nome,"Carecas Pizza");
	locais[qtdLocal].v1 = 16; locais[qtdLocal].v2 = 19;
    locais[qtdLocal].dist_v1 = 161.0f; locais[qtdLocal].dist_v2 = 367.0f; qtdLocal++;
    
    // 6. Tuca's Esfiha
	strcpy(locais[qtdLocal].nome,"Tuca's Esfiha");
	locais[qtdLocal].v1 = 16; locais[qtdLocal].v2 = 17;
    locais[qtdLocal].dist_v1 = 63.0f; locais[qtdLocal].dist_v2 = 31.0f; qtdLocal++;
    
    // 7. Horto Florestal
	strcpy(locais[qtdLocal].nome,"Horto Florestal");
	locais[qtdLocal].v1 = 17; locais[qtdLocal].v2 = 18;
    locais[qtdLocal].dist_v1 = 528.0f; locais[qtdLocal].dist_v2 = 32.0f; qtdLocal++;
    
    // 8. Samurai Brindes
	strcpy(locais[qtdLocal].nome,"Samurai Brindes");
	locais[qtdLocal].v1 = 15; locais[qtdLocal].v2 = 19;
    locais[qtdLocal].dist_v1 = 88.0f; locais[qtdLocal].dist_v2 = 89.0f; qtdLocal++;
    
    // 9. Posto Shell
	strcpy(locais[qtdLocal].nome,"Posto Shell");
	locais[qtdLocal].v1 = 18; locais[qtdLocal].v2 = 19;
    locais[qtdLocal].dist_v1 = 31.0f; locais[qtdLocal].dist_v2 = 82.0f; qtdLocal++;
    
    // 10. Espaco Aya
	strcpy(locais[qtdLocal].nome,"Espaco Aya");
	locais[qtdLocal].v1 = 18; locais[qtdLocal].v2 = 21;
    locais[qtdLocal].dist_v1 = 62.0f; locais[qtdLocal].dist_v2 = 354.0f; qtdLocal++;
    
    // 11. Casa do Repouso Solar da Cantareira
	strcpy(locais[qtdLocal].nome,"Casa do Repouso Solar da Cantareira");
	locais[qtdLocal].v1 = 21; locais[qtdLocal].v2 = 22;
    locais[qtdLocal].dist_v1 = 189.0f; locais[qtdLocal].dist_v2 = 102.0f; qtdLocal++;
    
    // 12. 4 Patas Pet House
	strcpy(locais[qtdLocal].nome,"4 Patas Pet House");
	locais[qtdLocal].v1 = 24; locais[qtdLocal].v2 = 25;
    locais[qtdLocal].dist_v1 = 87.0f; locais[qtdLocal].dist_v2 = 101.0f; qtdLocal++;
    
    // 13. Feira Tremembe
	strcpy(locais[qtdLocal].nome,"Feira Tremembe");
	locais[qtdLocal].v1 = 25; locais[qtdLocal].v2 = 28;
    locais[qtdLocal].dist_v1 = 97.0f; locais[qtdLocal].dist_v2 = 87.0f; qtdLocal++;
    
    // 14. Padaria Florestal
	strcpy(locais[qtdLocal].nome,"Padaria Florestal");
	locais[qtdLocal].v1 = 27; locais[qtdLocal].v2 = 28;
    locais[qtdLocal].dist_v1 = 58.0f; locais[qtdLocal].dist_v2 = 15.0f; qtdLocal++;
    
    // 15. Banco do Brasil
	strcpy(locais[qtdLocal].nome,"Banco do Brasil");
	locais[qtdLocal].v1 = 28; locais[qtdLocal].v2 = 29;
    locais[qtdLocal].dist_v1 = 107.0f; locais[qtdLocal].dist_v2 = 152.0f; qtdLocal++;
    
    // 16. Otica Voluntarios
	strcpy(locais[qtdLocal].nome,"Otica Voluntarios");
	locais[qtdLocal].v1 = 29; locais[qtdLocal].v2 = 30;
    locais[qtdLocal].dist_v1 = 62.0f; locais[qtdLocal].dist_v2 = 77.0f; qtdLocal++;
    
    // 17. Residencial Villa Floresta
	strcpy(locais[qtdLocal].nome,"Residencial Villa Floresta");
	locais[qtdLocal].v1 = 29; locais[qtdLocal].v2 = 31;
    locais[qtdLocal].dist_v1 = 164.0f; locais[qtdLocal].dist_v2 = 212.0f; qtdLocal++;
    
    // 18. Tapeceiro
	strcpy(locais[qtdLocal].nome,"Tapeceiro");
	locais[qtdLocal].v1 = 31; locais[qtdLocal].v2 = 32;
    locais[qtdLocal].dist_v1 = 68.5f; locais[qtdLocal].dist_v2 = 68.5f; qtdLocal++;
    
    // 19. Comercio de Ferramentas
	strcpy(locais[qtdLocal].nome,"Comercio de Ferramentas");
	locais[qtdLocal].v1 = 32; locais[qtdLocal].v2 = 33;
    locais[qtdLocal].dist_v1 = 45.0f; locais[qtdLocal].dist_v2 = 71.0f; qtdLocal++;


	// --- 3. CONJUNTO DE LOCAIS A VISITAR (ENTRADA DO PROBLEMA) ---
    // Fixado conforme exigido: Minha Casa (0) + 4 locais
    // Exemplo: Casa (0), Carecas Pizza (5), Horto Florestal (7), Banco do Brasil (15), Otica (16)
    int locais_a_visitar_idx[] = {0, 5, 7, 15, 16}; 
    int qtdVisitar = sizeof(locais_a_visitar_idx) / sizeof(locais_a_visitar_idx[0]);

	// --- 4. EXECUTA O ALGORITMO TSP ---
    tspVizinhoMaisProximo(G, ordemG, locais, qtdLocal, locais_a_visitar_idx, qtdVisitar);

    // --- (Parte 1: Impressão do Grafo para Documentação) ---
	imprimeGrafo(G,ordemG);

	destroiGrafo(&G,ordemG);
	return 0;
}
