/*                                                                */
/*    訪問順制約付きTSP用プログラム                                    */
/*    C code written by K. Ando and K. Sekitani (Shizuoka Univ.)  */
/*                                                                */
/*                                                                */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define MAX_N 10000   // 点の数の最大値
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

void read_tsp_data(char *filename, struct point p[MAX_N],int *np, int prec[MAX_N], int *mp) {
  FILE *fp;
  char buff[500];
  int i;

  if ((fp=fopen(filename,"rt")) == NULL) {// 指定ファイルを読み込み用に開く
    fprintf(stderr,"Error: File %s open failed.\n",filename);
    exit(EXIT_FAILURE);
  }   

  while((fgets(buff,sizeof(buff),fp)!=NULL)   // PRECEDENCE_CONSTRAINTS:で始まる行に出会う
	&&(strncmp("PRECEDENCE_CONSTRAINTS:",buff,23)!=0)) ; // まで読み飛ばす. 
  if(strncmp("PRECEDENCE_CONSTRAINTS:",buff,23)==0)  {
    sscanf(buff+24,"%d",mp);
    for(i=0;i<*mp;i++) fscanf(fp,"%d ",&prec[i]);
  } else {
    fprintf(stderr,"Error: There is no precedence constraint in file %s.\n",filename);
    exit(EXIT_FAILURE);
  }

  while((fgets(buff,sizeof(buff),fp)!=NULL)   // DIMENSION で始まる行に出会う
	&&(strncmp("DIMENSION",buff,9)!=0)) ; // まで読み飛ばす. 
  sscanf(buff,"DIMENSION: %d",np);           // 点の数 *np を読み込む

  while((fgets(buff,sizeof(buff),fp)!=NULL)   // NODE_COORD_SECTIONで始まる
	&&(strncmp("NODE_COORD_SECTION",buff,18)!=0)) ; // 行に出会うまで, 
                                                        // 読み飛ばす. 
  for(i=0;i<*np;i++) {                       // i=0 から i=(*np)-1まで
    if(fgets(buff,sizeof(buff),fp)!=NULL) 
      sscanf(buff,"%*d %d %d",&(p[i].x),&(p[i].y)); // i番目の点の座標を読み込む
  }                                 

  fclose(fp);
}
void show_array(int a[MAX_N], int len) {
  int i;
  printf("[ ");
  for(i=0;i<len-1;i++) {
    printf("%2d ",a[i]);
  }
 printf("%2d ]\n",a[i]);
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
void ci(struct point p[MAX_N], int n, int tour[MAX_N],int m, int prec[MAX_N]){
  FILE *fp;
  int i,j,a,b,c,r;
  int visited[MAX_N]; // 都市iが訪問済みならば1そうでなければ0
  double d[MAX_N]; // 未訪問点 r から現在の部分巡回路までの最短距離を d[r] に保存
  int nearest[MAX_N]; /* 未訪問点 r を現在の部分巡回路内の枝(i,i+1)に挿入する
                        ときに最も距離の増加が小さい i を nearest[r]に保存*/
  double dist1,dist2, min_dist;
  int min_i,min_j,min_r;
  int sbtlen=m;

  for(a=0;a<n;a++) visited[a]=0; // 最初は全都市は未訪問

	for(int i = 0; i < m; i++){
		tour[i] = prec[i];
		visited[prec[i]] = 1; 
	}
	
  //printf("tour   :"); show_array(tour,sbtlen);
  //printf("visited:"); show_array(visited,n);
	
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
    //printf("r,i,j,d[r] = %d %d %d %lf\n", r,i,j,d[r]);
    
    //printf("tour   :"); show_array(tour,sbtlen);
    //printf("visited:"); show_array(visited,n);
  	
  }
}
// 2-opt
int TwoOpt_array(struct point p[MAX_N], int n, int tour[MAX_N],int m, int prec[MAX_N]){
  int a,b,c,d;
  int i,j,k,l,g,h;
  int x, y;
  int chk=0;
  int count;
  for(i=0; i < n-2; i++) {
    for(k=i+2; k < n; k++) {
      j = i + 1;
      l = (k + 1) % n;
      a = tour[i]; b = tour[j];
      c = tour[k]; d = tour[l];
      
      if(dist(p[a], p[b]) + dist(p[c], p[d]) > dist(p[a], p[c]) + dist(p[b], p[d])) {
      	// 選んだ枝の2-opt改善が制約にかかるかどうか
		    count = 0;
		    for (x = j; x <=k; x++){
		    	for(y = 0; y < m; y++){
		    		if(tour[x] == prec[y]) count++;
		    	}
		    }
		    if(count>=2 && count < m) continue;
      	
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
// TwoOpt焼きなまし
void TwoOpt_with_SA_array(struct point p[MAX_N], int n, int tour[MAX_N], int m, int prec[MAX_N]){
  int a,b,c,d;
  int i,j,k,l,g,h;
  double Ei, Ej;
  double prob;
  double temp_max = 200;
  double temp=temp_max; 
  double tempfac = 0.99;
  double freezelim = 0.1;
  double r;
  double min = INF;
  int max = -1;
  int chk = 0;
  int count;
  int x, y;
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
					// 選んだ枝の2-opt改善が制約にかかるかどうか
				  count = 0;
				  for (x = j; x <=k; x++){
				  	for(y = 0; y < m; y++){
				  		if(tour[x] == prec[y]) count++;
				  	}
				  }
				  if(count>=2 && count < m) continue;
				  while(g < h) {
						SWAP(tour[g], tour[h]);
		 			 	g++; h--;
					}
				}
				else {
					prob = exp(-(Ej-Ei)/temp);
//	      	prob = exp(Ej/temp)/(exp(Ei/temp)+exp(Ej/temp));
					r = (double)rand()/RAND_MAX;
					if(r <= prob){
						// 選んだ枝の2-opt改善が制約にかかるかどうか
						count = 0;
						for (x = j; x <=k; x++){
							for(y = 0; y < m; y++){
								if(tour[x] == prec[y]) count++;
							}
						}
				  	if(count>=2 && count < m) continue;
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
		printf("temp = %lf\n",temp);
		printf("%lf\n",tour_length_array(p,n,tour));
		if(temp <= freezelim) return;
	}
}

void write_tour_data(char *filename, int n, int tour[MAX_N]){
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
  int n;                   // 点の数 
  int m;                   // 順序制約に現れる点の数
  struct point p[MAX_N];   // 各点の座標を表す配列 
  int tour[MAX_N];   // 巡回路を表現する配列
  int prec[MAX_N];   // 順序制約を表現する配列
  int i;

  if(argc != 2) {
    fprintf(stderr,"Usage: %s <tsp_filename>\n",argv[0]);
    exit(EXIT_FAILURE);
  }

  // 点の数と各点の座標を1番目のコマンドライン引数で指定されたファイルから読み込む
  read_tsp_data(argv[1],p,&n,prec,&m);

  //順序制約の確認
  //for(i=0;i<m;i++) printf("%d\n",prec[i]);

  // 最安挿入法による巡回路構築
  ci(p,n,tour,m,prec);
  printf("%5.1lf\n",tour_length_array(p,n,tour)); // 巡回路tourの長さ
  // 2-opt焼きなまし法による改善
  //TwoOpt_with_SA_array(p,n,tour,m,prec);
  //printf("%5.1lf\n",tour_length_array(p,n,tour)); // 巡回路tourの長さ
  // 2-optによる改善
	TwoOpt_array(p,n,tour,m,prec);
  // ファイルに出力
  write_tour_data("tour1.dat",n,tour);
  printf("%5.1lf\n",tour_length_array(p,n,tour)); // 巡回路tourの長さ

  exit(EXIT_SUCCESS);
}
