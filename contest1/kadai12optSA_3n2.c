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
// ceapest insertion
void ci(struct point p[MAX_N], int n, int tour[MAX_N]){
  FILE *fp;
  int i,j,a,b,c,r;
  int visited[MAX_N]; // 都市iが訪問済みならば1そうでなければ0
  double d[MAX_N]; // 未訪問点 r から現在の部分巡回路までの最短距離を d[r] に保存
  int nearest[MAX_N]; /* 未訪問点 r を現在の部分巡回路内の枝(i,i+1)に挿入する
                        ときに最も距離の増加が小さい i を nearest[r]に保存*/
  double dist1,dist2, min_dist;
  int min_i,min_j,min_r;
  int sbtlen=0;

  for(a=0;a<n;a++) visited[a]=0; // 最初は全都市は未訪問

  // a = 0 に最も近い点を探す
  a=0;
  min_dist=INF;
  for(r=i+1;r<n;r++) {
    if(dist(p[a],p[r])<min_dist) {
      min_r=r;
      min_dist=dist(p[a],p[r]);
    }
  }

  tour[0]=a;tour[1]=min_r;sbtlen=2;
  visited[a]=visited[min_r]=1;      // 都市a,min_rは訪問済み

  while(sbtlen<n) {
    min_dist=INF;
    for(r=0;r<n;r++) {
      if(!visited[r]) {
	d[r]=INF;
	for(i=0;i<sbtlen;i++) {
	  a=tour[i];
	  j=(i+1)%sbtlen; // j== i+1 ただし i=sbtlen-1 のときは, j==0
	  b=tour[j];
	  if (dist(p[a],p[r])+dist(p[b],p[r])-dist(p[a],p[b])<d[r]) {
	    nearest[r]=i;
	    d[r]=dist(p[a],p[r])+dist(p[b],p[r])-dist(p[a],p[b]);
	  }
	}
	if (d[r]<min_dist) {
	  min_dist = d[r];
	  min_r=r;
	}
      }
    }
    r=min_r;
    i=nearest[r];
    j=(i+1)%sbtlen;
    insert(tour,&sbtlen,j,r); 
    visited[r]=1;
  }
}

// 2-opt
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
void TwoOpt_with_SA_array(struct point p[MAX_N], int n, int tour[MAX_N]){
  int a,b,c,d;
  int i,j,k,l,g,h;
  double Ei, Ej;
  double prob;
  int itr = 0;
  double temp_max = 100;
  double temp=temp_max; 
  double tempfac = 0.99;
  double freezelim = 0.1;
  double r;
  double min = INF;
  int max = -1;
  int chk = 0;
  while(1){
		for(i=0; i < n-2; i++) {
			j = i + 1;
			for(k=i+2; k < n; k++) {
				l = (k + 1) % n;
				a = tour[i]; b = tour[j];
				c = tour[k]; d = tour[l];
				Ei = (dist(p[a], p[b]) + dist(p[c], p[d]));
				Ej = (dist(p[a], p[c]) + dist(p[b], p[d]));
				
				if(Ej-Ei < 0) {
					g = j; h = k;
				  while(g < h) {
						SWAP(tour[g], tour[h]);
		 			 	g++; h--;
					}
					//printf("%lf\n",tour_length_array(p,n,tour));
				}
				else {
					prob = exp(-(Ej-Ei)/temp);
//	      	prob = exp(Ej/temp)/(exp(Ei/temp)+exp(Ej/temp));
					r = (double)rand()/RAND_MAX;
					if(r <= prob){
						g = j; h = k;
					  while(g < h){
							SWAP(tour[g], tour[h]);
			 			 	g++; h--;
						}
					}
				}
			}
		}
		temp = tempfac*temp;
//		printf("temp = %lf\n",temp);
		if(temp <= freezelim) return;
	}
}
int ThreeOpt_array(struct point p[MAX_N], int N, int Tour[MAX_N]) {
  int a, b, c, d, e, f;
  int I;
  int fImproved = 0;
  int NewTour[MAX_N];
  int count;
  for (int i = 0; i < N-4; i++) {
  	for (int j = i+1; j < N-2; j++) {
      for (int k = j+1; k < N; k++) {
      	int D[5], min = 0;
        a = Tour[i]; b = Tour[i+1]; c = Tour[j];
        d = Tour[j+1]; e = Tour[k]; f = Tour[(k+1)%N];
        D[0] = dist(p[a],p[b]) + dist(p[c],p[d]) + dist(p[e],p[f]);  // 元の距離
        D[1] = dist(p[a],p[e]) + dist(p[d],p[b]) + dist(p[f],p[c]);
        D[2] = dist(p[d],p[a]) + dist(p[e],p[c]) + dist(p[f],p[b]);
        D[3] = dist(p[c],p[f]) + dist(p[a],p[d]) + dist(p[e],p[b]);
        D[4] = dist(p[a],p[c]) + dist(p[d],p[f]) + dist(p[e],p[b]);
        for (int m = 1; m <= 4; m++) {
          if (D[m] < D[min]) min = m;    // ４通りのつなぎ替えを比較
        }
        if (min == 0) continue;
        // 繋ぎ替えを実行
        if (min == 1) { // D(i,k) + D(j+1,i+1) + D(k+1,j)
          count = 0;
          for(I = 0; I <= i; I++) {
            NewTour[count] = Tour[I];
            count++;
          }
          for(I = k; I >= j+1; I--) {
            NewTour[count] = Tour[I];
            count++;
          }
          for(I = i+1; I <= j; I++) {
            NewTour[count] = Tour[I];
            count++;
          }
          for(I = k+1; I <= N; I++) {
            NewTour[count] = Tour[I];
            count++;
          }
          for(I = 0; I < N; I++) {
               Tour[I] = NewTour[I];
          }
          return 1;
        } else if (min == 2) { // D(j+1,i) + D(k,j) + D(k+1,i+1);
            count = 0;
            for(I = 0; I <= i; I++) {
              NewTour[count] = Tour[I];
              count++;
            }
            for(I = j+1; I <= k; I++) {
              NewTour[count] = Tour[I];
              count++;
            }
            for(I = j; I >= i+1; I--) {
              NewTour[count] = Tour[I];
              count++;
            }
            for(I = k+1; I < N; I++) {
              NewTour[count] = Tour[I];
              count++;
            }
            for(I = 0; I < N; I++) {
               Tour[I] = NewTour[I];
             }
            return 1;
         } else if (min == 3) { // D(j,k+1) + D(i,j+1) + D(k,i+1)
             count = 0;
              for(I = 0; I <= i; I++) {
                NewTour[count] = Tour[I];
                count++;
              }
              for(I = j+1; I <= k; I++) {
                NewTour[count] = Tour[I];
                count++;
              }
              for(I = i+1; I <= j; I++) {
                NewTour[count] = Tour[I];
                count++;
              }
              for(I = k+1; I < N; I++) {
                NewTour[count] = Tour[I];
                count++;
              }
              for(I = 0; I < N; I++) {
               Tour[I] = NewTour[I];
             }
              return 1;
         } else if (min == 4) { // D(i,j) + D(j+1,k+1) + D(k,i+1)
             count = 0;
             for(I = 0; I <= i; I++) {
               NewTour[count] = Tour[I];
               count++;
             }
             for(I = j; I >= i+1; I--) {
               NewTour[count] = Tour[I];
               count++;
             }
             for(I = k; I >= j+1; I--) {
               NewTour[count] = Tour[I];
               count++;
             }
             for(I = k+1; I <= N; I++) {
               NewTour[count] = Tour[I];
               count++;
             }
             for(I = 0; I < N; I++) {
               Tour[I] = NewTour[I];
             }
             return 1;
         }
      }
    }
  }
  return 0;
}
int ThreeOpt_array_n2(struct point p[MAX_N], int N, int Tour[MAX_N]) {
  int a, b, c, d, e, f;
  int I;
  int fImproved = 0;
  int NewTour[MAX_N];
  int count;
  for (int i = 0; i < N-4; i++) {
    for (int j = i+1; j < N-2; j++) {
      double D[5]; int min = 0;
      int k = (int)(rand() % (N-j-1) + j+1);
      a = Tour[i]; b = Tour[i+1]; c = Tour[j];
      d = Tour[j+1]; e = Tour[k]; f = Tour[(k+1)%N];
      D[0] = dist(p[a],p[b]) + dist(p[c],p[d]) + dist(p[e],p[f]);  // 元の距離
      D[1] = dist(p[a],p[e]) + dist(p[d],p[b]) + dist(p[f],p[c]);
      D[2] = dist(p[d],p[a]) + dist(p[e],p[c]) + dist(p[f],p[b]);
      D[3] = dist(p[c],p[f]) + dist(p[a],p[d]) + dist(p[e],p[b]);
      D[4] = dist(p[a],p[c]) + dist(p[d],p[f]) + dist(p[e],p[b]);
      for (int m = 1; m <= 4; m++) {
        if (D[m] < D[min]) min = m;    // ４通りのつなぎ替えを比較
      }
      if (min == 0) continue;
      // 繋ぎ替えを実行
      if (min == 1) { // D(i,k) + D(j+1,i+1) + D(k+1,j)
        count = 0;
        for(I = 0; I <= i; I++) {
          NewTour[count] = Tour[I];
          count++;
        }
        for(I = k; I >= j+1; I--) {
          NewTour[count] = Tour[I];
          count++;
        }
        for(I = i+1; I <= j; I++) {
          NewTour[count] = Tour[I];
          count++;
        }
        for(I = k+1; I <= N; I++) {
          NewTour[count] = Tour[I];
          count++;
        }
        for(I = 0; I < N; I++) {
           Tour[I] = NewTour[I];
        }
          return 1;
      } else if (min == 2) { // D(j+1,i) + D(k,j) + D(k+1,i+1);
          count = 0;
          for(I = 0; I <= i; I++) {
            NewTour[count] = Tour[I];
            count++;
          }
          for(I = j+1; I <= k; I++) {
            NewTour[count] = Tour[I];
            count++;
          }
          for(I = j; I >= i+1; I--) {
            NewTour[count] = Tour[I];
            count++;
          }
          for(I = k+1; I < N; I++) {
            NewTour[count] = Tour[I];
            count++;
          }
          for(I = 0; I < N; I++) {
             Tour[I] = NewTour[I];
           }
          return 1;
      } else if (min == 3) { // D(j,k+1) + D(i,j+1) + D(k,i+1)
          count = 0;
          for(I = 0; I <= i; I++) {
             NewTour[count] = Tour[I];
             count++;
          }
          for(I = j+1; I <= k; I++) {
            NewTour[count] = Tour[I];
            count++;
          }
          for(I = i+1; I <= j; I++) {
            NewTour[count] = Tour[I];
            count++;
          }
          for(I = k+1; I < N; I++) {
            NewTour[count] = Tour[I];
            count++;
          }
          for(I = 0; I < N; I++) {
            Tour[I] = NewTour[I];
          }
          return 1;
      } else if (min == 4) { // D(i,j) + D(j+1,k+1) + D(k,i+1)
          count = 0;
          for(I = 0; I <= i; I++) {
            NewTour[count] = Tour[I];
            count++;
          }
          for(I = j; I >= i+1; I--) {
            NewTour[count] = Tour[I];
            count++;
          }
          for(I = k; I >= j+1; I--) {
            NewTour[count] = Tour[I];
            count++;
          }
          for(I = k+1; I <= N; I++) {
            NewTour[count] = Tour[I];
            count++;
          }
          for(I = 0; I < N; I++) {
            Tour[I] = NewTour[I];
          }
          return 1;
       }
       //printf("3-opt improved! min=%d\n", min);
    }
  }
  return 0;
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
	if(q==1){
		chk = OrOpt1_list(p, n, visited);
	}
	else{
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

int main(int argc, char *argv[]){
  int n;
  int tour[MAX_N];
  struct point p[MAX_N];
  struct list visited;
  struct list* vis = &visited;
  
  int chk_array[10];
  int chk2opt, chkk, chk1, chk3opt;
  
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
  printf("%lf\n",tour_length_array(p,n,tour));
	/*
	// 2opt による改善
	chk2opt = 1;
	while(chk2opt) {
	  chk2opt = TwoOpt_array(p,n,tour);
	}
	*/
	// 焼きなまし法による改善
	TwoOpt_with_SA_array(p, n, tour);
	printf("%lf\n",tour_length_array(p,n,tour));
	sprintf(tourFileName, "tour%08d.dat", ++num);
	write_tour_data_array(tourFileName,n,tour);
	
	// 3opt(n^2)による改善
	chk3opt = 1;
	while(chk3opt==1) {
	  //chk3opt = ThreeOpt_array(p, n, tour);
	  chk3opt = ThreeOpt_array_n2(p, n, tour);
	  sprintf(tourFileName, "tour%08d.dat", ++num);
		write_tour_data_array(tourFileName,n,tour);
		printf("3opt done\n");
		printf("%lf\n",tour_length_array(p,n,tour));
	}
	//配列をリストに変換
	initialize(vis);
	for(int i=0; i<n; i++) {
	  insertBefore(visited.tail, tour[i]);
	}
	/*
	// chk配列を初期化
	chkk = 1;
	while(chkk == 1) {
		for(int k = 8; k >= 1; k--){
			chkk = OrOptk_list(k, p, n, vis);
			sprintf(tourFileName, "tour%08d.dat", ++num);
			write_tour_data_list(tourFileName,n,vis);
			printf("%lf\n",tour_length_list(p,n,vis));
			if(chkk == 2) break;
		}
		if(chkk == 2) break;
	}
	*/
	// chk配列を初期化
	for(int i = 0; i < 10; i++) chk_array[i] = 1;
	chkk = 1;
	// OrOptkによる改善
	while(chkk == 1) {
		chkk = 0;
		for(int k = 8; k >= 1; k--){
			if(chk_array[k] == 1){
				chk_array[k] = OrOptk_list(k, p, n, vis);
				sprintf(tourFileName, "tour%08d.dat", ++num);
				write_tour_data_list(tourFileName,n,vis);
				printf("OrOpt%d done\n",k);
				printf("%lf\n",tour_length_list(p,n,vis));
				if(chk_array[k] == 2) {
					chkk = 2;
					break;
				}
			}
			if(chk_array[k] == 1) chkk = 1;
		}
		if(chkk == 2) break;
	}
	
	/*
	// リストから配列に変換
	struct cell* j = visited.head->next;
	for(int i = 0; i < n; i++){
		tour[i] = j->data;
		j = j->next;
	}
	*/
	
  exit(EXIT_SUCCESS);
  return 0;
}
