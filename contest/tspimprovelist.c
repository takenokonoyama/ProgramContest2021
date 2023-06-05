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
#include <time.h>
#define MAX_N 30000   // 点の数の最大値
#define INF 100000000 // 無限大の定義
#define EPSILON 0.00000001 //ε 小さい正の値
#define SWAP(a,b){int temp; temp=(a); (a)=(b); (b)=temp; }

//list.h
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
//list.h 終わり

struct point {
  int x;
  int y;
};

double dist(struct point p, struct point q) { // pとq の間の距離を計算
  return sqrt((p.x-q.x)*(p.x-q.x)+(p.y-q.y)*(p.y-q.y));
}

double tour_length_list(struct point p[MAX_N], int n, struct list* visited) {
  int i;
  double sum=0.0;
  for(struct cell* i = visited->head->next; i->next != visited->tail; i = i->next) sum+=dist(p[i->data],p[i->next->data]);
  sum += dist(p[visited->tail->prev->data], p[visited->head->next->data]);
  return sum;// 総距離が関数の戻り値
}

double tour_length_array(struct point p[MAX_N], int n, int tour[MAX_N]) {
  int i;
  double sum=0.0;
  for(i=0;i<n;i++) sum+=dist(p[tour[i]],p[tour[(i+1)%n]]);
  return sum;// 総距離が関数の戻り値
}
void insert(int tour[MAX_N], int* len, int k, int value) {
  int i;
  
  if(k<0 || k > *len) {
    printf("Error in insert: out of range\n");
  }
  
  for(i=*len;i>k;i--) {
    tour[i]=tour[i-1];
  }
  tour[k]=value;
  (*len)++;
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
// nearest neigbor
void nn_array(struct point p[MAX_N], int n, int tour[MAX_N]){
  int i,j,r,nearest=-1;
  int a;
  int visited[MAX_N]; // 都市iが訪問済みならば1そうでなければ0
  double min;

  for(r=0;r<n;r++) visited[r]=0; // 最初は全都市は未訪問
  tour[0]=0;         // 最初に訪問する都市は 0 
  visited[0]=1;      // 都市0は訪問済み

  for(i=0;i<n-1;i++) {
    a = tour[i];
    //最後に訪問した都市 a == tour[i]から最短距離にある未訪問都市nearestを
    //見つける
    min = INF;          
    for(r=1;r<n;r++) { 
      if(!visited[r] && dist(p[a],p[r])<min){ 
        nearest=r; //都市tour[i]から暫定的に最短距離にある未訪問都市をnearestとする
        min = dist(p[a],p[r]); // その暫定最短距離をminとする
      }
    }
    tour[i+1]=nearest; // i+1 番目に訪問する都市を nearest にして, 
    visited[nearest]=1;// nearest を訪問済みとする. 
  }
}
int TwoOpt_list(struct point p[MAX_N], int n, struct list* visited){
  int a,b,c,d;
  struct cell* tmpk;
  int chk=0;
  
  for(struct cell *i = visited->head->next; i->next->next != visited->tail; i = i->next) {
    for(struct cell *k = i->next; k != visited->tail; k = tmpk) {
    	tmpk = k->next;
    	struct cell *j = i->next;
      struct cell *l = (k->next != visited->tail) ? k->next : visited->head->next;
      a = i->data; b = j->data;
      c = k->data; d = l->data;
      if(dist(p[a], p[b]) + dist(p[c], p[d]) > dist(p[a], p[c]) + dist(p[b], p[d])) {
      	printf("if done\n");
		    struct cell* tmp;
		    for(struct cell* p = k->prev; p != i; p = tmp){
				  tmp = p->prev;
				  insertBefore(l, p->data);
				  erase(p);
					}
		      chk = 1;
		    }
		  }
  	}
  return chk;
}
int TwoOpt_array(struct point p[MAX_N], int n, int tour[MAX_N]){
  int a,b,c,d;
  int i,j,k,l,g,h;
  int chk=0;
  
  for(i=0; i < n-2; i++) {
    for(k=i+2; k < n; k++) {
      j = i + 1;
      l = (k + 1) % n;
      a = tour[i]; b = tour[j];
      c = tour[k]; d = tour[l];
      if(dist(p[a], p[b]) + dist(p[c], p[d]) > dist(p[a], p[c]) + dist(p[b], p[d])) {
        g = j; h = k;
        while(g < h) {
          SWAP(tour[g], tour[h]);
          g++; h--;
        }
        chk=1;
      }
    }
  }
  return chk;
}
int ThreeOpt_list(struct point p[MAX_N], int n, struct list* visited){
	int a, b, c, d, e, f;
	double distance[5];
	struct cell* i;
	struct cell *j;
	struct cell* k;
	struct cell *l;
	struct cell* m;
	struct cell *o;
	struct cell* endi = visited->tail->prev->prev->prev->prev;
	struct cell* endk = visited->tail->prev->prev;
	struct cell* endm = visited->tail;
	int chk = 0;
	int ni = 0, nk, nm;
	for(i=visited->head->next; i->next->next->next->next != visited->tail; i = i->next) {
		for(k = i->next; k->next->next != visited->tail; k = k->next) {
	 		for(m = k->next; m != visited->tail; m = m->next) {
				j = i->next;
				l = k->next;
				o = (m->next != visited->tail) ? m->next : visited->head->next;
	 			a = i->data; b = j->data; c = k->data;
	 			d = l->data; e = m->data; f = o->data;
	 			distance[0] = dist(p[a], p[b]) + dist(p[c], p[d]) + dist(p[e], p[f]);
	 			distance[1] = dist(p[a], p[e]) + dist(p[d], p[b]) + dist(p[c], p[f]);
	 			distance[2] = dist(p[a], p[d]) + dist(p[e], p[c]) + dist(p[b], p[f]);
	 			distance[3] = dist(p[a], p[d]) + dist(p[e], p[b]) + dist(p[c], p[f]);
	 			distance[4] = dist(p[a], p[c]) + dist(p[b], p[e]) + dist(p[d], p[f]);
	 			int min_index = 0;
	 			for(int x = 1; x < 5; x++){
	 				if(distance[x] < distance[min_index]) min_index = x;
	 			}
	 			if(min_index == 0) {
	 				continue;
	 			}
	 			else {
	 				struct list newvisited;
	 				struct list* nvis = &newvisited;
	 				initialize(nvis);
	 				/*ae+db+cf*/
		 			if(min_index == 1){
		 				for(struct cell* q = visited->head->next; q != i->next; q=q->next){
	 						insertBefore(nvis->tail, q->data);
	 					}
	 					for(struct cell* q = m; q != l->prev; q=q->prev){
	 						insertBefore(nvis->tail, q->data);
	 					}
	 					for(struct cell* q = j; q != k->next; q=q->next){
	 						insertBefore(nvis->tail, q->data);
	 					}
	 					for(struct cell* q = o; q != visited->tail; q=q->next){
	 						insertBefore(nvis->tail, q->data);
	 					}
	 					chk = 1;
		 			}
		 			/*ad+ec+bf*/
		 			else if(min_index == 2){
	 					for(struct cell* q = visited->head->next; q != i->next; q=q->next){
	 						insertBefore(nvis->tail, q->data);
	 					}
	 					for(struct cell* q = l; q != m->next; q = q->next){			
	 						insertBefore(nvis->tail, q->data);
	 					}
	 					for(struct cell* q = k; q != j->prev; q=q->prev){
	 						insertBefore(nvis->tail, q->data);	
		 				}
		 				for(struct cell* q = o; q != visited->tail; q=q->next){
	 						insertBefore(nvis->tail, q->data);	
		 				}
		 				chk = 1;
		 			}
		 			/*ad+eb+cf*/
		 			else if(min_index == 3){
		 				for(struct cell* q = visited->head->next; q != i->next; q=q->next){
	 						insertBefore(nvis->tail, q->data);
	 					}
	 					for(struct cell* q = l; q != m->next; q=q->next){
	 						insertBefore(nvis->tail, q->data);
	 					}
	 					for(struct cell* q = j; q != k->next; q=q->next){
	 						insertBefore(nvis->tail, q->data);
		 				}
		 				for(struct cell* q = o; q != visited->tail; q=q->next){
	 						insertBefore(nvis->tail, q->data);			
		 				}
		 				chk = 1;
		 			}
		 			/*ac+be+df*/
		 			else if(min_index == 4){
	 					for(struct cell* q = visited->head->next; q != i->next; q = q->next){
	 						insertBefore(nvis->tail, q->data);			
		 				}				
	 					for(struct cell* q = k; q != j->prev; q=q->prev){
	 						insertBefore(nvis->tail, q->data);			
		 				}
	 					for(struct cell* q = m; q != l->prev; q=q->prev){
	 						insertBefore(nvis->tail, q->data);			
		 				}
	 					for(struct cell* q = o; q != visited->tail; q=q->next){
	 						insertBefore(nvis->tail, q->data);			
		 				}
	 				chk = 1;
	 				}
	 				struct cell*r = nvis->head->next;
	 				for(struct cell* q = visited->head->next; q != visited->tail; q = q->next){
	 					q->data = r->data;
	 					r = r->next;
	 				}
	 			}
	 		}
	 	}
	}
	return chk;
}


int OrOpt1_list(struct point p[MAX_N], int n, struct list* visited) {
  int a,b,c,d,e;
  int chk = 0;
  struct cell* tmp;
	clock_t time_start, time_end;
	time_start = clock();
  for(struct cell* i=visited->head->next; i != visited->tail; i = i->next) {
		struct cell *j = (i->next != visited->tail) ? i->next : visited->head->next;
		for(struct cell* k = visited->head->next; k != visited->tail; k = tmp) {
	 		time_end = clock();
	 		if(time_end-time_start > 15000000) return 2;
			tmp = k->next;
			if(k->data == i->data || k->data == j->data) continue;
			struct cell *l = (k->prev != visited->head) ? k->prev : visited->tail->prev;
			struct cell *m = (k->next != visited->tail) ? k->next : visited->head->next;
			a = i->data; b = j->data; c = l->data; d = k->data; e = m->data;
			if(dist(p[a], p[b]) + dist(p[d], p[c]) + dist(p[d], p[e]) > dist(p[a], p[d]) + dist(p[b], p[d]) + dist(p[c], p[e])) {
		  	insertAfter(i, d);
		  	erase(k);
		  	chk=1;
		  	break;
			}
    }
  }
  return chk;
}

int OrOptk_list(int q, struct point p[MAX_N], int n, struct list* visited){
	int a, b, c, e;
	int d[q];
	int chk = 0;
	struct cell* tmp;
	struct cell* k[q];
	struct cell* end = visited->tail;
	clock_t time_start, time_end;
	time_start = clock();
	for(int x = 0; x < q-2; x++) end = end->prev;
	for(struct cell* i=visited->head->next; i != visited->tail; i = i->next) {
	 struct cell *j = (i->next != visited->tail) ? i->next : visited->head->next;
	 for(struct cell* k0 = visited->head->next; k0 != end; k0 = tmp) {
	  time_end = clock();
	  //printf("ts-te=%ld\n",time_end-time_start);
	 	if(time_end-time_start > 15000000) return 2;
	  tmp = k0->next;
	  k[0] = k0;
	  for(int x = 1; x < q-1; x++) k[x] = k[x-1]->next;
		k[q-1] = (k[q-2]->next != visited->tail) ? k[q-2]->next : visited->head->next;
		// continue
		int flag = 0;
		for(int x = 0; x < q; x++)
			if(k[x]->data == i->data || k[x]->data == j->data) flag = 1;
		if(flag) continue;
		
		struct cell *l = (k[0]->prev != visited->head) ? k[0]->prev : visited->tail->prev;
		struct cell *m = (k[q-1]->next != visited->tail) ? k[q-1]->next : visited->head->next;		
		a = i->data; b = j->data; 
		c = l->data; 
		for(int x = 0; x < q; x++)
			d[x] = k[x]->data;
		e = m->data;
		
		if(dist(p[a], p[b])+dist(p[d[0]],p[c])+dist(p[d[q-1]],p[e])>dist(p[a],p[d[q-1]])+dist(p[b], p[d[0]])+dist(p[c], p[e])) {
			struct cell* point = i;
			for(int x = q-1; x >= 0 ; x--){
				insertAfter(point, d[x]);
				erase(k[x]);
				point = point->next;
		  }
		  end = visited->tail;
	 		for(int x = 0; x < q-2; x++) end = end->prev;
		  chk=1;
		  break;
			}
    }
  }
  return chk;
}

void write_tour_data_list(char *filename, int n, struct list* visited){
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

void write_tour_data_array(char *filename, int n, int tour[MAX_N]){
  FILE *fp; 
  int i;
 
 // 構築した巡回路をfilenameという名前のファイルに書き出すためにopen
  if((fp=fopen(filename,"wt"))==NULL){ 
    fprintf(stderr,"Error: File %s open failed.\n",filename);
    exit(EXIT_FAILURE);
  }
  fprintf(fp,"%d\n",n);
  for(i=0;i<n; i++){
   fprintf(fp,"%d ",tour[i]);
  }
  fprintf(fp,"\n");
  fclose(fp);
}

int main(int argc, char *argv[]) {
  int  n, chk2opt = 1, chkk=1, chk1 = 1;
  int chk;
  struct point p[MAX_N];
  struct list visited; 
  struct list* vis = &visited;
  int tour[MAX_N];
  int num = 0;
  char tourFileName[20];
  if(argc != 2) {
    fprintf(stderr,"Usage: %s <tsp_filename>\n",argv[0]);
    exit(EXIT_FAILURE);
  }
  
  // 点の数と各点の座標を1番目のコマンドライン引数で指定されたファイルから読み込む
  read_tsp_data(argv[1],p,&n);
  // 最近近傍法による巡回路構築
 	nn_array(p,n,tour);
 	 //配列をリストに変換
 	sprintf(tourFileName, "tour%08d.dat", ++num);
  write_tour_data_array(tourFileName,n,tour);
  initialize(vis);
  for(int i=0; i<n; i++) {
    insertBefore(visited.tail, tour[i]);
  }
  chk = 1;
  // TwoOpt check
  while(chk){
  	//chk = TwoOpt_array(p, n, tour);
  	//printf("%lf\n",tour_length_array(p,n,tour));
		chk = TwoOpt_list(p, n, vis);
		printf("%lf\n",tour_length_list(p,n,vis));
	}
  /*
 	// ThreeOpt check
 	int a;
  chk = 1;
  while(chk){
  	//chk = ThreeOpt_array(p, n, tour);
  	//printf("%lf\n",tour_length_array(p,n,tour));
		chk = ThreeOpt_list(p, n, vis);
		printf("%lf\n",tour_length_list(p,n,vis));
	}
  printf("%lf\n",tour_length_list(p,n,vis));
 	*/ 
  exit(EXIT_SUCCESS);
  return 0;
}
