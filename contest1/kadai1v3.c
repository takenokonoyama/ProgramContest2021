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
#define MAX_N 30000   // 点の数の最大値
#define INF 100000000 // 無限大の定義
#define EPSILON 0.00000001 //ε 小さい正の値
#define SWAP(a,b){int temp; temp=(a); (a)=(b); (b)=temp; }   

struct cell {
  int data;
  struct cell* prev;
  struct cell* next;
};

struct list {
  struct cell* head;
  struct cell* tail;
};

void initialize(struct list* l) {
  l->head = malloc(sizeof(struct cell));
  l->tail = malloc(sizeof(struct cell));
  l->head->prev = NULL;
  l->head->next = l->tail;
  l->tail->prev = l->head;
  l->tail->next = NULL;
}

void insertBefore(struct cell* p, int v) {
  struct cell* newcell = malloc(sizeof(struct cell));
  newcell->data = v;
  newcell->next = p;
  p->prev->next = newcell;
  newcell->prev = p->prev;
  p->prev = newcell;
}

void insertAfter(struct cell* p, int v) {
  struct cell* newcell = malloc(sizeof(struct cell));
  newcell->data = v;
  newcell->prev = p;
  p->next->prev = newcell;
  newcell->next = p->next;
  p->next = newcell;
}

void erase(struct cell* p) {
  p->prev->next = p->next;
  p->next->prev = p->prev;
  free(p);
}

void printNumbers(struct list* l) {
  for (struct cell* p = l->head->next; p->next != NULL; p = p->next)
    printf("%d ", p->data);
  printf("\n");
}

void printString(struct list* l) {
  for (struct cell* p = l->head->next; p->next != NULL; p = p->next)
    printf("%c ", p->data);
  printf("\n");
}

void clea1(struct list* l){
	while(l->head->next != l->tail)
		erase(l->head->next);
	free(l->head);
	free(l->tail);	
}

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
void fi(struct point p[MAX_N], int n, int tour[MAX_N]){
  FILE *fp;
  int h,i,j,a,b,c,r;
  int visited[MAX_N]; // 都市iが訪問済みならば1そうでなければ0
  double d[MAX_N]; // 未訪問点 r から現在の部分巡回路までの最短距離を d[r] に保存
  double min;
  double max;
  double max_dist;
  int min_i[MAX_N],min_r[MAX_N];
  double min_dist[MAX_N];
  int sbtlen=0;
	double min_max;
	int min_max_r, min_max_i;
  for(a=0;a<n;a++) visited[a]=0; // 最初は全都市は未訪問
	
  // a= 0 に最も点を探す
	min_dist = 0;
	i = 0;
  for(r=i+1;r<n;r++) {
    if(dist(p[a],p[r])>max_dist) {
      max=r;
      max_dist=dist(p[a],p[r]);
    }
  }
	
  tour[0]=a;tour[1]=max;sbtlen=2;
  visited[a]=visited[max]=1;      // 都市a,は訪問済み
	
  while(sbtlen<n) {
    for(r=0;r<n;r++) {
    	min = INF;
      if(!visited[r]) {
				for(i=0;i<sbtlen;i++) {
					d[r]=dist(p[tour[i]],p[r]);	
					if (d[r]<min) {
						min_i[r] = i;
	  				min_dist[r] = d[r];
	  				min = d[r];
	  				min_r[r]=r;
	  			}
				}
      }
    }
    
    min_max = 0;
    for(r = 0; r < n; r++){
    	if(!visited[r]){
    		if(min_max < min_dist[r]){
    			min_max = min_dist[r];
    			min_max_r = min_r[r];
    			min_max_i = min_i[r];
    		}
    	
    	}
    }
    
		r=min_max_r;
		i = min_max_i;
		h = (i-1 == -1) ? sbtlen-1 : i-1;
		j = (i+1) % sbtlen;
		a = tour[i];
		b = tour[h];
		c = tour[j];
		
    if(dist(p[r],p[a])+dist(p[r],p[b])-dist(p[a],p[b])< dist(p[r],p[c])+dist(p[r],p[a])-dist(p[a],p[c])){
    	insert(tour,&sbtlen,i,r);	
    	visited[r]=1;
    }
    else {
    	insert(tour,&sbtlen,j,r);	
    	visited[r]=1;    	
    }

  }
}

int TwoOpt_list(struct point p[MAX_N], int n, struct list* visited){
  int a,b,c,d;
  struct cell* tmp;
  int chk=0;
  
  for(struct cell *i = visited->head->next; i->next->next != visited->tail; i = i->next) {
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
        chk = 1;
      }
    }
  }
  return chk;
}


/*
void OrOpt1(struct point p[MAX_N], int n, int tour[MAX_N]) {
  int a,b,c,d,e;
  int i,j,k,l,m,h;
  int tmp, chk=0;

	  for(i = 0; i < n-1; i++) {
		 j = i + 1;
		 for(k = 0; k < n; k++) {
		   if(k == i || k == j) continue;
		   l = (k + 1) % n;
		   m = (k + n -1) % n;
		   a = tour[i]; b = tour[j]; c = tour[m]; d = tour[k]; e = tour[l];
		   if(dist(p[a], p[b]) + dist(p[d], p[c]) + dist(p[d], p[e]) > dist(p[a], p[d]) + dist(p[b], p[d]) + dist(p[c], p[e])) {
		     tmp = d;
		     if(k > j) {
		       for(h = k; h > j; h--) tour[h] = tour[h-1];
		       tour[j] = tmp;
		       chk = 1;
		     } else {
		       for(h = k; h < i; h++) tour[h] = tour[h+1];   
		       tour[i] = tmp;
		       chk = 1;
		     }
		     if(chk == 1) {
		       i = 0;
		       chk = 0;
		       break;
		     }
		   }
		 }
	  } 
}
*/

int OrOpt1(struct point p[MAX_N], int n, struct list* visited) {
  int a,b,c,d,e;
  struct cell* tmp;
  int chk = 0;
	for(struct cell* i=visited->head->next; i->next != visited->tail; i = i->next) {
	  struct cell *j = (i->next != visited->tail) ? i->next : visited->head->next;
		for(struct cell* k = visited->head->next; k != visited->tail; k = tmp) {
		  tmp = k->next;
		  if(k->data == i->data || k->data == j->data) continue;
		  struct cell *l = (k->prev != visited->head) ? k->prev : visited->tail->prev;
		  struct cell *m = (k->next != visited->tail) ? k->next : visited->head->next;
		  a = i->data; b = j->data; c = l->data; d = k->data; e = m->data;
		  if(dist(p[a], p[b]) + dist(p[d], p[c]) + dist(p[d], p[e]) > dist(p[a], p[d]) + dist(p[b], p[d]) + dist(p[c], p[e])) {
		    insertAfter(i, d);
		    erase(k);
		    chk = 1;
		  }
		}
	}
	return chk;
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
  int  n, chkor, chktwo;
  int chk;                   // 点の数 
  struct point p[MAX_N];   // 各点の座標を表す配列 
  struct list visited; // 巡回路を表現する配列
  struct list unvisited;
  struct list* vis = &visited;
  struct list* unvis = &unvisited;
  int tour[MAX_N];   // 巡回路を表現する配列
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
  /*
  printf("nn done\n");
  // 巡回路をテキストファイルとして出力
  write_tour_data("tour1.dat",n,vis);
  // 巡回路長を画面に出力
  printf("%lf\n",tour_length(p,n,vis));
  chk = 1;
  chktwo = 0;
  chkor = 0;
  while(chk){
  	// twoopt
  	chktwo = TwoOpt(p,n,vis);
  	printf("twoopt done\n");
  	// 巡回路をテキストファイルとして出力
 	 	write_tour_data("tour2.dat",n,vis);
  	// 巡回路長を画面に出力
  	printf("%lf\n",tour_length(p,n,vis));
  	// oropt
  	chkor = OrOpt1(p,n,vis);
  	printf("oropt done\n");
  	// 巡回路をテキストファイルとして出力
  	write_tour_data("tour3.dat",n,vis);
  	printf("%lf\n",tour_length(p,n,vis));
  	chk = (chktwo == 1 || chkor == 1) ? 1 : 0;
  }
  */
  // 巡回路長を画面に出力
  printf("%lf\n",tour_length(p,n,vis));
  exit(EXIT_SUCCESS);
  return 0;
}
