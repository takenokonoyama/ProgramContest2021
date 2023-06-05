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

void dnn_array(struct point p[MAX_N], int n, int tour[MAX_N]){
  int i,j,r,nearest=-1;
  int a;
  int s1, s2;
  int visited[MAX_N]; // 都市iが訪問済みならば1そうでなければ0
  double min;
	double sum;
	double maxsum = 0;
	int n1, n2; //group1,group2の都市数
	int count_group1 = 0; // 訪問したgroup1の都市数
	for(r=0;r<n;r++) visited[r]=0; // 最初は全都市は未訪問
	// s1とs2を決める(s1とｓ2は最も離れた点)
	for (i = 0; i < n-1; i++){
		for(j = i+1;j < n; j++){
			sum = dist(p[i],p[j]);
			if(maxsum < sum){
				maxsum = sum;
				s1 = i;
				s2 = j;
			}
		}
	}
	tour[0]= s1;         // 最初に訪問する都市はs1
	visited[s1]=1;		  // 都市s1は訪問済み
	count_group1++;
	n1 = n / 2;
	n2 = n-n1;
	for(i=0;i<n-1;i++) {
	  a = tour[i];
	  //最後に訪問した都市 a == tour[i]から最短距離にある未訪問都市nearestを
	  //見つける
	  min = INF;          
	  for(r=0;r<n;r++) { 
	    if(r != s2 && !visited[r] && dist(p[a],p[r])<min){ 
	      nearest=r; //都市tour[i]から暫定的に最短距離にある未訪問都市をnearestとする
	      min = dist(p[a],p[r]); // その暫定最短距離をminとする
	    }
	  }
	  
	  tour[i+1]=nearest; // i+1 番目に訪問する都市を nearest にして, 
	  visited[nearest]=1;// nearest を訪問済みとする. 
	  count_group1++;
	  if(count_group1 == n1) break;
	}
	//グループ1の末尾の点とs2をつなげる
	tour[n1] = s2;
	visited[s2] = 1;
	for(i=n1; i < n-1;i++) {
	  a = tour[i];
	  //最後に訪問した都市 a == tour[i]から最短距離にある未訪問都市nearestを見つける
	  min = INF;          
	  for(r=0;r<n;r++) { 
	    if(!visited[r] && dist(p[a],p[r])<min){ 
	      nearest=r; //都市tour[i]から暫定的に最短距離にある未訪問都市をnearestとする
	      min = dist(p[a],p[r]); // その暫定最短距離をminとする
	    }
	  }
	  tour[i+1]=nearest; // i+1 番目に訪問する都市を nearest にして, 
	  visited[nearest]=1;// nearest を訪問済みとする. 
	}
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

int OrOpt1_list(struct point p[MAX_N], int n, struct list* visited) {
  int a,b,c,d,e;
  int chk = 0;
  struct cell* tmp;

  for(struct cell* i=visited->head->next; i != visited->tail; i = i->next) {
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
		  chk=1;
		  break;
		}
    }
  }
  return chk;
}

int OrOpt2_list(struct point p[MAX_N], int n, struct list* visited) {
  int a,b,c,d1,d2,e;
  int chk = 0;
  struct cell* tmp;
  
  for(struct cell* i=visited->head->next; i->next != visited->tail; i = i->next) {
	 struct cell *j = (i->next != visited->tail) ? i->next : visited->head->next;
	 for(struct cell* k1 = visited->head->next; k1 != visited->tail; k1 = tmp) {
	   tmp = k1->next;
		struct cell* k2 = (k1->next != visited->tail) ? k1->next : visited->head->next;
		if(k1->data == i->data || k1->data == j->data || k2->data == i->data || k2->data == j->data) continue;
		struct cell *l = (k1->prev != visited->head) ? k1->prev : visited->tail->prev;
		struct cell *m = (k2->next != visited->tail) ? k2->next : visited->head->next;
		a = i->data; b = j->data; c = l->data; d1 = k1->data; d2 = k2->data; e = m->data;
		if(dist(p[a], p[b]) + dist(p[d1], p[c]) + dist(p[d2], p[e]) > dist(p[a], p[d2]) + dist(p[b], p[d1]) + dist(p[c], p[e])) {
		  insertAfter(i, d2);
		  erase(k2);
		  insertBefore(j, d1);
		  erase(k1);
		  chk=1;
		  break;
		}
    }
  }
  return chk;
}

int OrOpt3_list(struct point p[MAX_N], int n, struct list* visited) {
  int a,b,c,d1,d2,d3,e;
  int chk = 0;
  struct cell* tmp;
  
  for(struct cell* i=visited->head->next; i->next != visited->tail; i = i->next) {
	 struct cell *j = (i->next != visited->tail) ? i->next : visited->head->next;
	 for(struct cell* k1 = visited->head->next; k1->next->next != visited->tail; k1 = tmp) {
	  tmp = k1->next;
		struct cell* k2 = k1->next;
		struct cell* k3 = (k2->next != visited->tail) ? k2->next : visited->head->next;
		if(k1->data == i->data || k1->data == j->data || k2->data == i->data || k2->data == j->data || k3->data == j->data || k3->data == i->data) continue;
		struct cell *l = (k1->prev != visited->head) ? k1->prev : visited->tail->prev;
		struct cell *m = (k3->next != visited->tail) ? k3->next : visited->head->next;
		a = i->data; b = j->data; c = l->data; d1 = k1->data; d2 = k2->data; d3 = k3->data; e = m->data;
		if(dist(p[a], p[b]) + dist(p[d1], p[c]) + dist(p[d3], p[e]) > dist(p[a], p[d3]) + dist(p[b], p[d1]) + dist(p[c], p[e])) {
		  insertAfter(i, d3);
		  erase(k3);
		  insertAfter(i->next, d2);
		  erase(k2);
		  insertAfter(i->next->next, d1);
		  erase(k1);
		  chk=1;
		  break;
		}
    }
  }
  return chk;
}

int OrOpt4_list(struct point p[MAX_N], int n, struct list* visited) {
  int a,b,c,d1,d2,d3,d4,e;
  int chk = 0;
  struct cell* tmp;
  
  for(struct cell* i=visited->head->next; i->next != visited->tail; i = i->next) {
	 struct cell *j = (i->next != visited->tail) ? i->next : visited->head->next;
	 for(struct cell* k1 = visited->head->next; k1->next->next->next != visited->tail; k1 = tmp) {
	  tmp = k1->next;
		struct cell* k2 = k1->next;
		struct cell* k3 = k2->next;
		struct cell* k4 = (k3->next != visited->tail) ? k3->next : visited->head->next;
		if(k1->data == i->data || k1->data == j->data || k2->data == i->data || k2->data == j->data || k3->data == j->data || k3->data == i->data || k4->data == j->data || k4->data == i->data) continue;
		struct cell *l = (k1->prev != visited->head) ? k1->prev : visited->tail->prev;
		struct cell *m = (k4->next != visited->tail) ? k4->next : visited->head->next;
		a = i->data; b = j->data; c = l->data; d1 = k1->data; d2 = k2->data; d3 = k3->data; d4 = k4->data; e = m->data;
		if(dist(p[a], p[b]) + dist(p[d1], p[c]) + dist(p[d4], p[e]) > dist(p[a], p[d4]) + dist(p[b], p[d1]) + dist(p[c], p[e])) {
			insertAfter(i, d4);
		  erase(k4);
		  insertAfter(i->next, d3);
		  erase(k3);
		  insertAfter(i->next->next, d2);
		  erase(k2);
		  insertAfter(i->next->next->next, d1);
		  erase(k1);
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
  int  n, chk, chk1, chk2, chk3, chk4;
  struct point p[MAX_N];
  struct list visited; 
  struct list* vis = &visited;
  int tour[MAX_N];
  int num=0;
  char tourFileName[20];
  if(argc != 2) {
    fprintf(stderr,"Usage: %s <tsp_filename>\n",argv[0]);
    exit(EXIT_FAILURE);
  }
  
  // 点の数と各点の座標を1番目のコマンドライン引数で指定されたファイルから読み込む
  read_tsp_data(argv[1],p,&n);
  // 最近近傍法による巡回路構築
  dnn_array(p,n,tour);
  // 2opt による改善
  chk = TwoOpt_array(p,n,tour);
  while(chk) {
    chk = TwoOpt_array(p,n,tour);
  }
  // 巡回路をテキストファイルとして出力
  sprintf(tourFileName, "tour%08d.dat", ++num);
  write_tour_data_array(tourFileName,n,tour);
  //配列をリストに変換
  initialize(vis);
  for(int i=0; i<n; i++) {
    insertBefore(visited.tail, tour[i]);
  }
  //リストを配列に変換
  /*
  struct cell* j = visited.head->next;
  for(int i=0; i<n; i++) {
    tour[i] = j->data;
    j->next;
  }
  */
  // oroptによる改善
 	chk4 = OrOpt4_list(p,n,vis);
  sprintf(tourFileName, "tour%08d.dat", ++num);
  write_tour_data_list(tourFileName,n,vis);
  chk3 = OrOpt3_list(p,n,vis);
  sprintf(tourFileName, "tour%08d.dat", ++num);
  write_tour_data_list(tourFileName,n,vis);	
  chk2 = OrOpt2_list(p,n,vis);
  sprintf(tourFileName, "tour%08d.dat", ++num);
  write_tour_data_list(tourFileName,n,vis);
  chk1 = OrOpt1_list(p,n,vis);
  sprintf(tourFileName, "tour%08d.dat", ++num);
  write_tour_data_list(tourFileName,n,vis);
 
  while(chk1 == 1 || chk2 == 1 || chk3 == 1 || chk4 == 1) {
  	if(chk4) {
  		chk4 = OrOpt4_list(p,n,vis);
  		sprintf(tourFileName, "tour%08d.dat", ++num);
  		write_tour_data_list(tourFileName,n,vis);
  	}
  	if(chk3) {
			chk3 = OrOpt3_list(p,n,vis);
		  sprintf(tourFileName, "tour%08d.dat", ++num);
  		write_tour_data_list(tourFileName,n,vis);
    }
    if(chk2) {
		  chk2 = OrOpt2_list(p,n,vis);
		  sprintf(tourFileName, "tour%08d.dat", ++num);
  		write_tour_data_list(tourFileName,n,vis);
   	}
    if(chk1) {
		  chk1 = OrOpt1_list(p,n,vis);
		  sprintf(tourFileName, "tour%08d.dat", ++num);
  		write_tour_data_list(tourFileName,n,vis);
    }
  }
  exit(EXIT_SUCCESS);
  return 0;
}
