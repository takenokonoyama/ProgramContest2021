/*                                                                */
/*    経路構築用プログラム                                          */
/*    C code written by K. Ando and K. Sekitani (Shizuoka Univ.)  */
/*                                                                */
/*              Version 2008.12.9                                 */
/*              Version 2013.05.07                                */
/*              Version 2013.05.22 revised                        */
/*              Version 2014.05.13 revised                        */
/*              Version 2015.06.17 revised                        */
/*                                                                */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "list.h"
#define MAX_N 10000   // 点の数の最大値
#define INF 100000000 // 無限大の定義
#define EPSILON 0.00000001 //ε 小さい正の値
#define SWAP(a,b){int temp; temp=(a); (a)=(b); (b)=temp; }   

struct point {
  int x;
  int y;
};

double dist(struct point p, struct point q) { // pとq の間の距離を計算 
  return sqrt((p.x-q.x)*(p.x-q.x)+(p.y-q.y)*(p.y-q.y));
}

double tour_length(struct point p[MAX_N], int n, struct list* visited) {
  int i;
  double sum=0.0;
  for(struct cell* i = visited->head->next; i->next != visited->tail; i = i->next) sum+=dist(p[i->data],p[i->next->data]);
  sum += dist(p[visited->tail->prev->data], p[visited->head->next->data]);
  return sum;// 総距離が関数の戻り値
}

void read_tsp_data(char *filename, struct point p[MAX_N],int *np) {
  FILE *fp;
  char buff[100];
  int i;

  if ((fp=fopen(filename,"rt")) == NULL) {// 指定ファイルを読み込み用に開く
    fprintf(stderr,"Error: File %s open failed.\n",filename);
    exit(0);
  }   

  while((fgets(buff,sizeof(buff),fp)!=NULL)   // DIMENSION で始まる行に出会う
	&&(strncmp("DIMENSION",buff,9)!=0)) ; // まで読み飛ばす. 
  sscanf(buff,"DIMENSION : %d",np);           // 点の数 *np を読み込む

  while((fgets(buff,sizeof(buff),fp)!=NULL)   // NODE_COORD_SECTIONで始まる
	&&(strncmp("NODE_COORD_SECTION",buff,18)!=0)) ; // 行に出会うまで, 
                                                        // 読み飛ばす. 
  for(i=0;i<*np;i++) {                       // i=0 から i=(*np)-1まで
    if(fgets(buff,sizeof(buff),fp)!=NULL) 
      sscanf(buff,"%*d %d %d",&(p[i].x),&(p[i].y)); // i番目の点の座標を読み込む
  }                                 

  fclose(fp);
}

void nn(struct point p[MAX_N], int n, struct list* visited, struct list* unvisited){
  int i,j,r,nearest=-1;
  int a;
  double min;
  struct cell* tmp;

  insertBefore(visited->tail, 0);      // 最初に訪問する都市は 0 
  erase(unvisited->head->next);        // 都市0は訪問済み
  
  for(struct cell *i = visited->head->next; i != visited->tail; i = i->next) {
    a = i->data;
    //最後に訪問した都市 a == tour[i]から最短距離にある未訪問都市nearestを
    //見つける
    min = INF;    
    for(struct cell *j = unvisited->head->next; j != unvisited->tail; j = j->next) { 
      r = j->data;
      if(dist(p[a],p[r])<min){ 
	      nearest=r; //都市tour[i]から暫定的に最短距離にある未訪問都市をnearestとする
	      min = dist(p[a],p[r]); // その暫定最短距離をminとする
      }
    }
    if(min != INF) insertAfter(i, nearest); // i+1 番目に訪問する都市を nearest にして, 
    for(struct cell *j = unvisited->head->next; j != unvisited->tail; j = tmp) {
      tmp = j->next;
      if(j->data==nearest) {
        erase(j);
      }
    }
  }
}

void TwoOpt(struct point p[MAX_N], int n, struct list* visited){
  int a,b,c,d;
  struct cell* tmp;
  int chk;
  
  for(struct cell *i = visited->head->next; i->next->next != visited->tail; i = i->next) {
    chk=0;
    for(struct cell *k = i->next->next; k->next != visited->tail; k = k->next) {
      struct cell *j = i->next;
      struct cell *l = (k->next != visited->tail) ? k->next : visited->head->next;
      a = i->data; b = j->data;
      c = k->data; d = l->data;
      if(dist(p[a], p[b]) + dist(p[c], p[d]) > dist(p[a], p[c]) + dist(p[b], p[d])) {
        for(struct cell *g = j; g != l; g = tmp) {
          tmp = g->next;
          g->next = g->prev;
          g->prev = tmp;
        }
        tmp = i->next;
        i->next = l->prev;
        l->prev = tmp;
        tmp = j->next;
        j->next = k->prev;
        k->prev = tmp;
        chk=1;
      }
      if(chk == 1) {
        i = visited->head->next;
        break;
      }
    }
  }
}

void write_tour_data(char *filename, int n, struct list* visited){
  FILE *fp; 
  int i;
 
 // 構築した巡回路をfilenameという名前のファイルに書き出すためにopen
  if((fp=fopen(filename,"wt"))==NULL){
    fprintf(stderr,"Error: File %s open failed.\n",filename);
    exit(EXIT_FAILURE);
  }
  fprintf(fp,"%d\n",n);
  for(struct cell* i = visited->head->next; i != visited->tail; i=i->next){
    fprintf(fp,"%d ",i->data);
  }
  fprintf(fp,"\n");
  fclose(fp);
}

int main(int argc, char *argv[]) {
  int  n;                   // 点の数 
  struct point p[MAX_N];   // 各点の座標を表す配列 
  struct list visited; // 巡回路を表現する配列
  struct list unvisited;
  struct list* vis = &visited;
  struct list* unvis = &unvisited;
  
  if(argc != 2) {
    fprintf(stderr,"Usage: %s <tsp_filename>\n",argv[0]);
    exit(EXIT_FAILURE);
  }
  
  // 点の数と各点の座標を1番目のコマンドライン引数で指定されたファイルから読み込む
  read_tsp_data(argv[1],p,&n);
  printf("read done\n");
  
  initialize(vis);
  initialize(unvis);

  for(int i = 0; i < n; i++) {
    insertBefore(unvisited.tail, i);
  }

  // 最近近傍法による巡回路構築
  nn(p,n,vis,unvis);
  printf("nn done\n");
  // 巡回路をテキストファイルとして出力
  write_tour_data("tour1.dat",n,vis);
  printf("write done\n");
  // 巡回路長を画面に出力
  printf("%lf\n",tour_length(p,n,vis));
  // 2opt による改善
  TwoOpt(p,n,vis);
  // 巡回路をテキストファイルとして出力
  write_tour_data("tour2.dat",n,vis);
  // 巡回路長を画面に出力
  printf("%lf\n",tour_length(p,n,vis));

  exit(EXIT_SUCCESS);
  return 0;
}
