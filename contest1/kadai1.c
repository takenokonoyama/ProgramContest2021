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

double tour_length(struct point p[MAX_N], int n, int tour[MAX_N]) {
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

void nn(struct point p[MAX_N], int n, int tour[MAX_N]){
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
    //    printf("tour   :"); show_array(tour,n);
    //    printf("visited:"); show_array(visited,n);
  }
}

void TwoOpt(struct point p[MAX_N], int n, int tour[MAX_N]){
  int a,b,c,d;
  int i,j,k,l,g,h,sa;
  double max;
  while(1) {
    max=-1;
    sa=-1;
    for(i=0; i < n-2; i++) {
      j = i + 1;
      for(k=i+2; k < n; k++) {
        l = (k + 1) % n;
        a = tour[i]; b = tour[j];
        c = tour[k]; d = tour[l];
        if(dist(p[a], p[b]) + dist(p[c], p[d]) > dist(p[a], p[c]) + dist(p[b], p[d])) {
          sa = (dist(p[a], p[b]) + dist(p[c], p[d])) - (dist(p[a], p[c]) + dist(p[b], p[d]));
        }
        if(max < sa) {
          g = j; h = k;
          max = sa;
        }
      }
    }
    while(g < h && max > 0) {
      SWAP(tour[g], tour[h]);
      g++; h--;
    }
    if(max == -1) break;
  }
}

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
  int  n;                   // 点の数 
  struct point  p[MAX_N];   // 各点の座標を表す配列 
  int tour[MAX_N];   // 巡回路を表現する配列

  if(argc != 2) {
    fprintf(stderr,"Usage: %s <tsp_filename>\n",argv[0]);
    exit(EXIT_FAILURE);
  }

  // 点の数と各点の座標を1番目のコマンドライン引数で指定されたファイルから読み込む
  read_tsp_data(argv[1],p,&n);
  // 最近近傍法による巡回路構築
  nn(p,n,tour);
  // 巡回路をテキストファイルとして出力
  write_tour_data("tour1.dat",n,tour);
  // 巡回路長を画面に出力
  printf("%lf\n",tour_length(p,n,tour));
  // 2opt による改善
  TwoOpt(p,n,tour);
  OrOpt1(p,n,tour);
  //TwoOpt(p,n,tour);
  // 巡回路をテキストファイルとして出力
  write_tour_data("tour2.dat",n,tour);
  // 巡回路長を画面に出力
  printf("%lf\n",tour_length(p,n,tour));

  exit(EXIT_SUCCESS);
  return 0;
}
