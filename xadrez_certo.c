#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "malloc.h"
#include "unistd.h"

#define PEAO    1
#define CAVALO  2
#define BISPO   3
#define TORRE   4
#define RAINHA  5
#define REI     6
const int MAX_NIVEL=6;

typedef struct Posicao{
    int qtdBrancas;
    struct Peca *brancas;
    int qtdPretas;
    struct Peca *pretas;
    int jogVez;
    struct Peca *tab[8][8];
}Posicao;

struct Peca{
int codigo;
int linha,coluna;
int ataques;
struct Peca *prox,*ant;
};

struct Peca *CriaListaPeca(){
    struct Peca *lista = (struct Peca*) malloc(sizeof(struct Peca));
    lista->prox = lista->ant = lista;
    return lista;
}

struct Jogada{
int deLinha,deColuna,paraLinha,paraColuna;
struct Jogada *prox,*ant;
};

struct Jogada *CriaListaJogada(){
    struct Jogada *lista = (struct Jogada*) malloc(sizeof(struct Jogada));
    lista->prox = lista->ant = lista;
    return lista;
}

struct Jogada *CriaJogada(int deLinha,int deColuna, int paraLinha, int paraColuna){
    struct Jogada *aux = (struct Jogada*) malloc(sizeof(struct Jogada));
    aux->deLinha = deLinha;
    aux->deColuna = deColuna;
    aux->paraLinha = paraLinha;
    aux->paraColuna = paraColuna;
    aux->prox = aux->ant = NULL;
    return aux;
}

void InsereJogada(struct Jogada *lista, int deLinha, int deColuna, int paraLinha, int paraColuna){
    struct Jogada *novo = CriaJogada(deLinha, deColuna, paraLinha, paraColuna);

    novo->prox = lista->prox;
    novo->ant = lista;
    lista->prox->ant = novo;
    lista->prox = novo;
}


void DestroiJogadas(struct Jogada *lista){
    struct Jogada *aux = lista;
    lista->ant->prox = NULL;
    while (aux!=NULL){
        lista = lista->prox;
        free(aux);
        aux = lista;
    }
}

struct Peca *CriaPeca(int codigo,int linha, int coluna){
    struct Peca *aux = (struct Peca*) malloc(sizeof(struct Peca));
    aux->codigo = codigo;
    aux->linha = linha;
    aux->coluna = coluna;
    aux->ataques = 0;
    aux->prox = aux->ant = NULL;
    return aux;
}

struct Peca *InserePeca(struct Peca *lista, int codigo, int linha, int coluna){
    struct Peca *novo = CriaPeca(codigo,linha,coluna);

    novo->prox = lista->prox;
    novo->ant = lista;
    lista->prox->ant = novo;
    lista->prox = novo;

    return novo;
}

void RemovePeca(struct Peca *ponteiro){
    ponteiro->prox->ant = ponteiro->ant;
    ponteiro->ant->prox = ponteiro->prox;
    free(ponteiro);
}

void DestroiPecas(struct Peca *lista){
    struct Peca *aux = lista;
    lista->ant->prox = NULL;
    while (aux!=NULL){
        lista = lista->prox;
        free(aux);
        aux = lista;
    }
}

void ZeraAtaques(struct Peca *lista){
    struct Peca *aux = lista->prox;
    while (aux!=lista){
        aux->ataques = 0;
        aux = aux->prox;
    }
}

void ResetCor(){
    printf("\033[0m");
}

void AjustaCor(int peca){
    switch(abs(peca)){
    case PEAO: printf("\033[0;36m");break;
    case CAVALO: printf("\033[1;33m");break;
    case BISPO: printf("\033[0;35m");break;
    case TORRE: printf("\033[0;32m");break;
    case RAINHA: printf("\033[0;34m");break;
    case REI: printf("\033[1;31m");break;
    }
}

void CalculaPeao(Posicao pos,int linha,int coluna,struct Jogada *lista){
    int peca = pos.tab[linha][coluna]->codigo;

    if (linha+peca>=0 && linha+peca<=7){//não está na primeira nem na última linha
        if (pos.tab[linha+peca][coluna]==NULL){ //pode avancar
            InsereJogada(lista,linha,coluna,linha+peca,coluna);
        }
        if (coluna>0 && pos.tab[linha+peca][coluna-1]!=NULL && pos.tab[linha+peca][coluna-1]->codigo*peca<0){ //verificar captura na esquerda
            InsereJogada(lista,linha,coluna,linha+peca,coluna-1);
            pos.tab[linha+peca][coluna-1]->ataques++;
        }
        if (coluna<7 && pos.tab[linha+peca][coluna+1]!=NULL && pos.tab[linha+peca][coluna+1]->codigo*peca<0){
            InsereJogada(lista,linha,coluna,linha+peca,coluna+1);
            pos.tab[linha+peca][coluna+1]->ataques++;
        }
    }
}

void CalculaCavalo(Posicao pos, int linha, int coluna, struct Jogada *lista){
    int peca = pos.tab[linha][coluna]->codigo;

    if (linha<6){ //duas linhas acima
        if (coluna<7 && (pos.tab[linha+2][coluna+1]==NULL || pos.tab[linha+2][coluna+1]->codigo*peca<0)){ //direita
            InsereJogada(lista,linha,coluna,linha+2,coluna+1);
            if (pos.tab[linha+2][coluna+1]!=NULL)
                pos.tab[linha+2][coluna+1]->ataques++;
        }
        if (coluna>0 && (pos.tab[linha+2][coluna-1]==NULL || pos.tab[linha+2][coluna-1]->codigo*peca<0)){ //esquerda
            InsereJogada(lista,linha,coluna,linha+2,coluna-1);
            if (pos.tab[linha+2][coluna-1]!=NULL)
                pos.tab[linha+2][coluna-1]->ataques++;
        }
    }
    if (linha<7){//uma linha acima
        if (coluna<6 && (pos.tab[linha+1][coluna+2]==NULL || pos.tab[linha+1][coluna+2]->codigo*peca<0)){ //duas casas à direita
            InsereJogada(lista,linha,coluna,linha+1,coluna+2);
            if (pos.tab[linha+1][coluna+2]!=NULL)
                pos.tab[linha+1][coluna+2]->ataques++;
        }
        if (coluna>1 && (pos.tab[linha+1][coluna-2]==NULL || pos.tab[linha+1][coluna-2]->codigo*peca<0)){ //duas casas à esquerda
            InsereJogada(lista,linha,coluna,linha+1,coluna-2);
            if (pos.tab[linha+1][coluna-2]!=NULL)
                pos.tab[linha+1][coluna-2]->ataques++;
        }
    }
    if (linha>1){ //duas linhas abaixo
        if (coluna<7 && (pos.tab[linha-2][coluna+1]==NULL || pos.tab[linha-2][coluna+1]->codigo*peca<0)){ //direita
            InsereJogada(lista,linha,coluna,linha-2,coluna+1);
            if (pos.tab[linha-2][coluna+1]!=NULL)
                pos.tab[linha-2][coluna+1]->ataques++;
        }
        if (coluna>0 && (pos.tab[linha-2][coluna-1]==NULL || pos.tab[linha-2][coluna-1]->codigo*peca<0)){ //esquerda
            InsereJogada(lista,linha,coluna,linha-2,coluna-1);
            if (pos.tab[linha-2][coluna-1]!=NULL)
                pos.tab[linha-2][coluna-1]->ataques++;
        }
    }
    if (linha>0){//uma linha abaixo
        if (coluna<6 && (pos.tab[linha-1][coluna+2]==NULL || pos.tab[linha-1][coluna+2]->codigo*peca<0)){ //duas casas à direita
            InsereJogada(lista,linha,coluna,linha-1,coluna+2);
            if (pos.tab[linha-1][coluna+2]!=NULL)
                pos.tab[linha-1][coluna+2]->ataques++;
        }
        if (coluna>1 && (pos.tab[linha-1][coluna-2]==NULL || pos.tab[linha-1][coluna-2]->codigo*peca<0)){ //duas casas à esquerd
            InsereJogada(lista,linha,coluna,linha-1,coluna-2);
            if (pos.tab[linha-1][coluna-2]!=NULL)
                pos.tab[linha-1][coluna-2]->ataques++;
        }
    }
}

void CalculaBispo(Posicao pos, int linha, int coluna, struct Jogada *lista){
    int peca = pos.tab[linha][coluna]->codigo;

    int k=0;

    do{//diagonal superior direita
        k++;
        if (linha+k<8 && coluna+k<8 && (pos.tab[linha+k][coluna+k]==NULL||pos.tab[linha+k][coluna+k]->codigo*peca<0)){
            InsereJogada(lista,linha,coluna,linha+k,coluna+k);
            if (pos.tab[linha+k][coluna+k]!=NULL)
                pos.tab[linha+k][coluna+k]->ataques++;
        }
    }while (linha+k<8 && coluna+k<8 && pos.tab[linha+k][coluna+k]==NULL);

    k=0;
    do{//diagonal superior esquerda
        k++;
        if (linha+k<8 && coluna-k>=0 && (pos.tab[linha+k][coluna-k]==NULL||pos.tab[linha+k][coluna-k]->codigo*peca<0)){
            InsereJogada(lista,linha,coluna,linha+k,coluna-k);
            if (pos.tab[linha+k][coluna-k]!=NULL)
                pos.tab[linha+k][coluna-k]->ataques++;
        }
    }while (linha+k<8 && coluna-k>=0 && pos.tab[linha+k][coluna-k]==NULL);

    k=0;
    do{//diagonal inferior direita
        k++;
        if (linha-k>=0 && coluna+k<8 && (pos.tab[linha-k][coluna+k]==NULL||pos.tab[linha-k][coluna+k]->codigo*peca<0)){
            InsereJogada(lista,linha,coluna,linha-k,coluna+k);
            if (pos.tab[linha-k][coluna+k]!=NULL)
                pos.tab[linha-k][coluna+k]->ataques++;
        }
    }while (linha-k>=0 && coluna+k<8 && pos.tab[linha-k][coluna+k]==NULL);

    k=0;
    do{//diagonal inferior direita
        k++;
        if (linha-k>=0 && coluna-k>=0 && (pos.tab[linha-k][coluna-k]==NULL||pos.tab[linha-k][coluna-k]->codigo*peca<0)){
            InsereJogada(lista,linha,coluna,linha-k,coluna-k);
            if (pos.tab[linha-k][coluna-k]!=NULL)
                pos.tab[linha-k][coluna-k]->ataques++;
        }
    }while (linha-k>=0 && coluna+k>=0 && pos.tab[linha-k][coluna-k]==NULL);
}

void CalculaTorre(Posicao pos, int linha, int coluna, struct Jogada *lista){
    int peca=pos.tab[linha][coluna]->codigo;

    int k=0;

    do{//coluna acima
        k++;
        if (linha+k<8 && (pos.tab[linha+k][coluna]==NULL||pos.tab[linha+k][coluna]->codigo*peca<0)){
            InsereJogada(lista,linha,coluna,linha+k,coluna);
            if (pos.tab[linha+k][coluna]!=NULL)
                pos.tab[linha+k][coluna]->ataques++;
        }
    }while(linha+k<8 && pos.tab[linha+k][coluna]==NULL);

    k=0;
    do{//coluna abaixo
        k++;
        if (linha-k>=0 && (pos.tab[linha-k][coluna]==NULL||pos.tab[linha-k][coluna]->codigo*peca<0)){
            InsereJogada(lista,linha,coluna,linha-k,coluna);
            if (pos.tab[linha-k][coluna]!=NULL)
                pos.tab[linha-k][coluna]->ataques++;
        }
    }while(linha-k>=0 && pos.tab[linha-k][coluna]==NULL);

    k=0;
    do{//linha direita
        k++;
        if (coluna+k<8 && (pos.tab[linha][coluna+k]==NULL||pos.tab[linha][coluna+k]->codigo*peca<0)){
            InsereJogada(lista,linha,coluna,linha,coluna+k);
            if (pos.tab[linha][coluna+k]!=NULL)
                pos.tab[linha][coluna+k]->ataques++;
        }
    }while(coluna+k<8 && pos.tab[linha][coluna+k]==NULL);

    k=0;
    do{//linha esquerda
        k++;
        if (coluna-k>=0 && (pos.tab[linha][coluna-k]==NULL||pos.tab[linha][coluna-k]->codigo*peca<0)){
            InsereJogada(lista,linha,coluna,linha,coluna-k);
            if (pos.tab[linha][coluna-k]!=NULL)
                pos.tab[linha][coluna-k]->ataques++;
        }
    }while(coluna-k>=0 && pos.tab[linha][coluna-k]==NULL);
}

void CalculaRei(Posicao pos, int linha, int coluna,struct Jogada *lista){
    int peca=pos.tab[linha][coluna]->codigo;

    if (linha<7){//linha de cima
        if (pos.tab[linha+1][coluna]==NULL||pos.tab[linha+1][coluna]->codigo*peca<0){//mesma coluna
            InsereJogada(lista,linha,coluna,linha+1,coluna);
            if (pos.tab[linha+1][coluna]!=NULL)
                pos.tab[linha+1][coluna]->ataques++;
        }
        if (coluna<7 && (pos.tab[linha+1][coluna+1]==NULL||pos.tab[linha+1][coluna+1]->codigo*peca<0)){//superior direita
            InsereJogada(lista,linha,coluna,linha+1,coluna+1);
            if (pos.tab[linha+1][coluna+1]!=NULL)
                pos.tab[linha+1][coluna+1]->ataques++;
        }
        if (coluna>0 && (pos.tab[linha+1][coluna-1]==NULL||pos.tab[linha+1][coluna-1]->codigo*peca<0)){//superior esquerda
            InsereJogada(lista,linha,coluna,linha+1,coluna-1);
            if (pos.tab[linha+1][coluna-1]!=NULL)
                pos.tab[linha+1][coluna-1]->ataques++;
        }
    }

    if (linha>0){//linha de baixo
        if (pos.tab[linha-1][coluna]==NULL||pos.tab[linha-1][coluna]->codigo*peca<0){//mesma coluna
            InsereJogada(lista,linha,coluna,linha-1,coluna);
            if (pos.tab[linha-1][coluna]!=NULL)
                pos.tab[linha-1][coluna]->ataques++;
        }
        if (coluna<7 && (pos.tab[linha-1][coluna+1]==NULL||pos.tab[linha-1][coluna+1]->codigo*peca<0)){//inferior direita
            InsereJogada(lista,linha,coluna,linha-1,coluna+1);
            if (pos.tab[linha-1][coluna+1]!=NULL)
                pos.tab[linha-1][coluna+1]->ataques++;
        }
        if (coluna>0 && (pos.tab[linha-1][coluna-1]==NULL||pos.tab[linha-1][coluna-1]->codigo*peca<0)){//inferior esquerda
            InsereJogada(lista,linha,coluna,linha-1,coluna-1);
            if (pos.tab[linha-1][coluna-1]!=NULL)
                pos.tab[linha-1][coluna-1]->ataques++;
        }
    }

    if (coluna<7 && (pos.tab[linha][coluna+1]==NULL||pos.tab[linha][coluna+1]->codigo*peca<0)){ //mesma linha, casa da direita
        InsereJogada(lista,linha,coluna,linha,coluna+1);
            if (pos.tab[linha][coluna+1]!=NULL)
                pos.tab[linha][coluna+1]->ataques++;
    }

    if (coluna>0 && (pos.tab[linha][coluna-1]==NULL||pos.tab[linha][coluna-1]->codigo*peca<0)){ //mesma linha, casa da esquerda
        InsereJogada(lista,linha,coluna,linha,coluna-1);
            if (pos.tab[linha][coluna-1]!=NULL)
                pos.tab[linha][coluna-1]->ataques++;
    }
}

//inicia o tabuleiro, colocando as peças e criando as listas
struct Posicao IniciaTabuleiro(){
    struct Posicao pos;
    pos.qtdBrancas = pos.qtdPretas = 16;
    pos.jogVez = 1;
    pos.brancas = CriaListaPeca();
    pos.pretas = CriaListaPeca();

    int i,j;
    for (i=2;i<6;i++)
        for (j=0;j<8;j++)
            pos.tab[i][j] = NULL;

    for (j=0;j<8;j++){
        pos.tab[1][j] = InserePeca(pos.brancas,PEAO,1,j);
        pos.tab[6][j] = InserePeca(pos.pretas,-PEAO,6,j);
    }

    pos.tab[0][0] = InserePeca(pos.brancas,TORRE,0,0);
    pos.tab[0][7] = InserePeca(pos.brancas,TORRE,0,7);

    pos.tab[7][0] = InserePeca(pos.pretas,-TORRE,7,0);
    pos.tab[7][7] = InserePeca(pos.pretas,-TORRE,7,7);

    pos.tab[0][1] = InserePeca(pos.brancas,CAVALO,0,1);
    pos.tab[0][6] = InserePeca(pos.brancas,CAVALO,0,6);

    pos.tab[7][1] = InserePeca(pos.pretas,-CAVALO,7,1);
    pos.tab[7][6] = InserePeca(pos.pretas,-CAVALO,7,6);

    pos.tab[0][2] = InserePeca(pos.brancas,BISPO,0,2);
    pos.tab[0][5] = InserePeca(pos.brancas,BISPO,0,5);

    pos.tab[7][2] = InserePeca(pos.pretas,-BISPO,7,2);
    pos.tab[7][5] = InserePeca(pos.pretas,-BISPO,7,5);

    pos.tab[0][3] = InserePeca(pos.brancas,RAINHA,0,3);
    pos.tab[7][3] = InserePeca(pos.pretas,-RAINHA,7,3);

    pos.tab[0][4] = InserePeca(pos.brancas,REI,0,4);
    pos.tab[7][4] = InserePeca(pos.pretas,-REI,7,4);

    return pos;
}

void CalculaJogadas(Posicao pos,int linha, int coluna, struct Jogada *lista){
    int peca = pos.tab[linha][coluna]->codigo;
    switch (abs(peca)){
    case PEAO:   CalculaPeao(pos,linha,coluna,lista);break;//peoes
    case CAVALO: CalculaCavalo(pos,linha,coluna,lista);break;//cavalos
    case BISPO:  CalculaBispo(pos,linha,coluna,lista);break;//bispos
    case TORRE:  CalculaTorre(pos,linha,coluna,lista);break;//torres
    case RAINHA: CalculaBispo(pos,linha,coluna,lista);
                 CalculaTorre(pos,linha,coluna,lista);break;
    case REI:    CalculaRei(pos,linha,coluna,lista);break;//rei
    }
}

struct Jogada *CalculaMovimentosPossiveis(Posicao pos){
    struct Jogada *jogadas = CriaListaJogada();
    struct Peca *aux;

    ZeraAtaques(pos.brancas);
    ZeraAtaques(pos.pretas);

    if (pos.jogVez==1){
        aux = pos.brancas->prox;
        while (aux!=pos.brancas){
            CalculaJogadas(pos,aux->linha,aux->coluna,jogadas);
            aux=aux->prox;
        }
    }else{
        aux = pos.pretas->prox;
        while (aux!=pos.pretas){
            CalculaJogadas(pos,aux->linha,aux->coluna,jogadas);
            aux=aux->prox;
        }
    }
    return jogadas;
}

struct Jogada EscolheJogada(struct Jogada *jogadas){
    int deLinha,deColuna,paraLinha, paraColuna;
    struct Jogada *jog=NULL;
    do{
        printf("Digite a linha e a coluna da peca a ser movida:");
        scanf("%d %d",&deLinha,&deColuna);
        deLinha--;deColuna--;//na matriz a contagem começa com 0

        printf("Digite a linha e a coluna para onde a peca deve ser movida:");
        scanf("%d %d",&paraLinha,&paraColuna);
        paraLinha--;paraColuna--;//na matriz a contagem começa com 0

        jog = jogadas->prox;
        while (jog!=jogadas&&(jog->deLinha!=deLinha||jog->deColuna!=deColuna||jog->paraLinha!=paraLinha||jog->paraColuna!=paraColuna)){
            jog = jog->prox;
        }
    }while (jog==jogadas);
    struct Jogada jogAux;
    jogAux = *jog;

    return jogAux;
}

int ExecutaJogada(struct Posicao *pos,struct Jogada jog){
    double resultado;
    int resp=0;
    struct Peca *peca = pos->tab[jog.deLinha][jog.deColuna];

    pos->tab[jog.deLinha][jog.deColuna] = NULL; //retira a peça do ligar

    if (pos->tab[jog.paraLinha][jog.paraColuna]!=NULL){ //a peça fará uma captura
        if (abs(pos->tab[jog.paraLinha][jog.paraColuna]->codigo)==REI) //rei capturado
            resp=1;

        RemovePeca(pos->tab[jog.paraLinha][jog.paraColuna]); //retira a peça que foi capturada da sua lista
        if (pos->jogVez==1)
            pos->qtdPretas--; //capturou peça preta
        else pos->qtdBrancas--;//capturou peça branca
    }

    pos->tab[jog.paraLinha][jog.paraColuna] = peca;
    peca->linha = jog.paraLinha;
    peca->coluna = jog.paraColuna;

    pos->jogVez = -pos->jogVez;

    return resp;
}

void PromocaoPeaoRainha(struct Posicao *pos) {
  for (int i = 0; i < 8; i++) {
    if (pos->tab[7][i] != NULL && pos->tab[7][i]->codigo == 1) {
      RemovePeca(pos->tab[7][i]);
      pos->tab[7][i] = InserePeca(pos->brancas, RAINHA, 7, i);
    } else if (pos->tab[0][i] != NULL && pos->tab[0][i]->codigo == -1) {
      RemovePeca(pos->tab[0][i]);
      pos->tab[0][i] = InserePeca(pos->pretas, -RAINHA, 0, i);
    }
  }
}

double AvaliaPosicao(struct Posicao *pos) {
  double resultado;
  double soma_brancas = 0;
  double soma_pretas = 0;
  struct Jogada *listaDeJogadas = CalculaMovimentosPossiveis(*pos);
  struct Jogada *aux = listaDeJogadas->prox;
  while (aux != listaDeJogadas) {
    if (pos->tab[aux->deLinha][aux->deColuna]->codigo == 1) {
      soma_brancas = soma_brancas + 2;
    } else if (pos->tab[aux->deLinha][aux->deColuna]->codigo == 3) {
      soma_brancas = soma_brancas + 0.30;
    } else if (pos->tab[aux->deLinha][aux->deColuna]->codigo == 2)  {
      soma_brancas = soma_brancas + 0.80;
    } else if (pos->tab[aux->deLinha][aux->deColuna]->codigo == 4) {
      soma_brancas = soma_brancas + 0.40;
    } else if (pos->tab[aux->deLinha][aux->deColuna]->codigo == 5) {
      soma_brancas = soma_brancas + 0.10;
    }  else if (pos->tab[aux->deLinha][aux->deColuna]->codigo == -1) {
      soma_pretas = soma_pretas + 2;
    } else if (pos->tab[aux->deLinha][aux->deColuna]->codigo == -2 ) {
      soma_pretas = soma_pretas + 0.80;
    } 
    else if (pos->tab[aux->deLinha][aux->deColuna]->codigo == -3) {
      soma_pretas = soma_pretas + 0.30;
    }else if (pos->tab[aux->deLinha][aux->deColuna]->codigo == -4) {
      soma_pretas = soma_pretas + 0.40;
    } else if (pos->tab[aux->deLinha][aux->deColuna]->codigo == -5) {
      soma_pretas = soma_pretas + 0.10;
    } 

    aux = aux->prox;
  }

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (pos->tab[i][j] == NULL) {
        soma_brancas = soma_brancas;
        soma_pretas = soma_pretas;
      } else if (pos->tab[i][j]->codigo == 1) {
        soma_brancas = soma_brancas + 1.00 * (i + 1);
      } else if (pos->tab[i][j]->codigo == 2 || pos->tab[i][j]->codigo == 3) {
        soma_brancas = soma_brancas + 3.00 ;
      } else if (pos->tab[i][j]->codigo == 4) {
        soma_brancas = soma_brancas + 5.00 ;
      } else if (pos->tab[i][j]->codigo == 5) {
        soma_brancas = soma_brancas + 9.00 ;
      } else if (pos->tab[i][j]->codigo == 6) {
        soma_brancas = soma_brancas + 1000.00 ;
      } else if (pos->tab[i][j]->codigo == -1) {
        soma_pretas = soma_pretas + 1.00 * (8 - i);
      } else if (pos->tab[i][j]->codigo == -2 || pos->tab[i][j]->codigo == -3) {
        soma_pretas = soma_pretas + 3.00 ;

      } else if (pos->tab[i][j]->codigo == -4) {
        soma_pretas = soma_pretas + 5.00 ;

      } else if (pos->tab[i][j]->codigo == -5) {
        soma_pretas = soma_pretas + 9.00 ;

      } else if (pos->tab[i][j]->codigo == -6) {
        soma_pretas = soma_pretas + 1000.00 ;
      }
    }
  }
  resultado = soma_pretas - soma_brancas;
  return resultado;
}

void DesenhaTabuleiro(Posicao pos){
    double resultado;
    int i,j;
    system("cls");

    printf("    +---+---+---+---+---+---+---+---+\n");
    for (i=7;i>=0;i--){
        printf("%d   |",i+1);
        for (j=0;j<8;j++){
            if (pos.tab[i][j]){ //se tem peça nessa casa
                AjustaCor(pos.tab[i][j]->codigo);
                if (pos.tab[i][j]->codigo>0)
                    printf(" %d",pos.tab[i][j]->codigo);
                else printf("%d",pos.tab[i][j]->codigo);
                ResetCor();
            }else printf("  ");//se não tem peça, deixa em vazio o printf
            printf(" |");
        }
        printf("\n    +---+---+---+---+---+---+---+---+\n");
    }
    printf("      1   2   3   4   5   6   7   8 \n");

    resultado = AvaliaPosicao(&pos);
    // printf("posicao avaliada em: %.3lf\n\n", resultado);
    
}

struct Posicao *CopiaPosicao(Posicao *pos){
  struct Posicao *posCopia = (Posicao *)malloc(sizeof(Posicao));
  posCopia->qtdBrancas = pos->qtdBrancas;
  posCopia->qtdPretas = pos->qtdPretas;
  posCopia->brancas = CriaListaPeca();
  posCopia->pretas = CriaListaPeca();
  posCopia->jogVez = pos->jogVez;

  // Copia lista de pecas
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (pos->tab[i][j]!=NULL){
        if (pos->tab[i][j]->codigo>0){
          posCopia->tab[i][j] = InserePeca(posCopia->brancas, pos->tab[i][j]->codigo, i, j);
        }else if (pos->tab[i][j]->codigo<0){
          posCopia->tab[i][j] = InserePeca(posCopia->pretas, pos->tab[i][j]->codigo, i, j);
        }
      }else{
        posCopia->tab[i][j] = NULL;
      }
    }
  }

  return posCopia;
}

struct Peca* LiberaMemoria(Posicao *posCopia){
    struct Peca* aux = posCopia->pretas;
    posCopia->pretas->ant->prox = NULL;
    while(aux != NULL){
      posCopia->pretas= posCopia->pretas->prox;
      free(aux);
      aux = posCopia->pretas;
    }
    aux = posCopia->brancas;
    posCopia->brancas->ant->prox = NULL;
    while(aux != NULL){
      posCopia->brancas= posCopia->brancas->prox;
      free(aux);
      aux = posCopia->brancas;
    }
    return NULL;
}

struct Jogada ExecutaIA(Posicao posAtual, int nivel,double alfa,double beta){
    struct Jogada *lista,*jogadaAux,*listaBrancas,*listaPretas;

    ///declarar e inicializar as variáveis indicadas (item 1 do exercício)

    double melhorValor;

    if(nivel%2 == 0){
      melhorValor=-INFINITY;
    }else {
      melhorValor=INFINITY;
    }

    int podado = 0;
    struct Jogada melhorJogada,jogadaIA;
    double valorJogada;
    
    ///calcular as possíveis jogadas das peças brancas e pretas (item 2 do exercício)

   struct Posicao copia1=*CopiaPosicao(&posAtual);
        copia1.jogVez=-1;
        listaPretas=CalculaMovimentosPossiveis(copia1);
        copia1.jogVez=-1*copia1.jogVez;
        listaBrancas=CalculaMovimentosPossiveis(copia1);
    if(nivel%2==0){
        lista=listaPretas;
    } else if(nivel%2==1){
        lista=listaBrancas;
    }
    ///fazer com que a variável "lista" receba a lista de jogadas das peças brancas ou das pretas, dependendo do valor de nível (item 3 do exercício)

    if(nivel%2==0){
      lista = listaPretas;
    } else {
      lista = listaBrancas;
    }

    jogadaAux = lista->prox;

    while (jogadaAux!=lista && podado==0){///aqui vamos percorrer a lista de jogadas possíveis (ou da sbrancas ou das pretas) enquanto ainda for bom continuar avaliando a posição

        ///copiar o parâmetro "posAtual" para "posCopia" (item 4 do exercício)
        struct Posicao *posCopia = CopiaPosicao(&posAtual);

        ///executar a jogada "jogadaAux" em "posCopia" (item 4 do exercício)
        
        ExecutaJogada(posCopia, *jogadaAux);

        ///verificar se "nivel" é menor do que "MAX_NIVEL" (item 5 do exercício)
        if(nivel<MAX_NIVEL)
        {
            ///verificar se "nivel" é par (item 5a do exercício)
            if(nivel%2 == 0)
            {
                ///chamar a função recursivamente e guardar a jogada retornada em "jogadaIA" (item 5a do exercício)
                jogadaIA = ExecutaIA(*posCopia, nivel+1, melhorValor, beta);
            }

            ///verificar se "nivel" é ímpar (item 5b do exercício) - pode ser usado "else" em relação ao item 5a
            else
            {
                ///chamar a função recursivamente e guardar a jogada retornada em "jogadaIA" (item 5b do exercício)
                jogadaIA=ExecutaIA(*posCopia, nivel + 1, alfa , melhorValor);
            }

            ///executar "jogadaIA" sobre "posCopia" (item 5a do exercício)
            ExecutaJogada(posCopia,jogadaIA);
        }

        ///avaliar a posição "posCopia" (item 6 do exercício)
    
        valorJogada = AvaliaPosicao(posCopia);

        ///verificar se houve poda (item 6 do exercício)

        if (valorJogada < alfa || valorJogada > beta)
        {
          podado = 1;
        }
        ///verificar "jogadaIA" é a melhor jogada encontrada até o momento (item 7 do exercício)

        if(nivel%2 == 0 && valorJogada>=melhorValor){
          melhorValor = valorJogada;
          melhorJogada = *(jogadaAux);
        } else if (nivel%2==1 && valorJogada<=melhorValor){
          melhorValor = valorJogada;
          melhorJogada = *(jogadaAux);
        }

        ///liberar a memória de "posCopia" (item 8 do exercício)

        jogadaAux=jogadaAux->prox;
        LiberaMemoria(posCopia);

    }

    ///liberar a memória alocada nas listas de possíveis jogadas das peças brancas e pretas (item 9 do exercício)
      DestroiJogadas(listaBrancas);
      DestroiJogadas(listaPretas);
    ///retornar a melhor jogada encontrada "melhorJogada" (item 9 do exercício).

    return melhorJogada;
}

void SalvaJogada(const char* name,struct Jogada* jogada){
  FILE *arq;
  if (jogada == NULL){
    arq = fopen(name,"wb");
    fclose(arq);
  }else{
    arq = fopen(name,"ab");
    fwrite(&jogada->deLinha,sizeof(int),1,arq);
    fwrite(&jogada->deColuna,sizeof(int),1,arq);
    fwrite(&jogada->paraLinha,sizeof(int),1,arq);
    fwrite(&jogada->paraColuna,sizeof(int),1,arq);
    fclose(arq);
  }
}

int main(){
    int vencedor=0;

    struct Posicao posAux = IniciaTabuleiro();
    struct Jogada *jogadasPossiveis=NULL,jogadaEscolhida;

    ///criar o arquivo de jogadas (vazio) e fechar;
    SalvaJogada("jogadas.bin",NULL);

    while (vencedor==0){
        DesenhaTabuleiro(posAux);
        if (posAux.jogVez==1){
            jogadasPossiveis = CalculaMovimentosPossiveis(posAux);
            jogadaEscolhida = EscolheJogada(jogadasPossiveis); ///eu modifiquei aqui para retornar um struct ao invés de um ponteiro
            DestroiJogadas(jogadasPossiveis);
            
        }else{
            jogadaEscolhida = ExecutaIA(posAux,0,-INFINITY,INFINITY);

        }

        ///salvar a jogada no arquivo
        SalvaJogada("jogadas.bin",&jogadaEscolhida);
        

        vencedor = ExecutaJogada(&posAux,jogadaEscolhida);
        
    }
    if (posAux.jogVez==-1){ //a função ExecutaJogada já trocou o jogador da vez
        printf("\nBrancas venceram!\n\n");
    }else printf("\nPretas venceram!\n\n");

    DestroiPecas(posAux.brancas);
    DestroiPecas(posAux.pretas);

    system("PAUSE");

}