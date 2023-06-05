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
int max(double a, double b) {
    if (a > b)
        return a;
    else
        return b;
}

void ThreeOpt(struct point p[MAX_N], int n, int tour[MAX_N]){
  int a,b,c,d;
  int i,j,k,l,m,o,,g1,g2,h1, ,sa;
  double cost_;
  double cost_1;
  double min_cost;
  double cost[4];
  int 
  for(i=0; i < n-4; i++) {
    j = i + 1;
    for(k=i+2; k < n-2; k++) {
    	l = k+1;
    	for(m=k+2; m < n; m++){
    		o = (m+1) % n;
	      a = tour[i]; b = tour[j];
	      c = tour[k]; d = tour[l];
	      e = tour[m]; f = tour[o]:
	      cost[0] = dist(p[a], p[b]) + dist(p[c], p[d]) dist(p[e], p[f])-(dist(p[a], p[e]) + dist(p[b], p[d])+dist(p[c], p[f]));
	      cost[1] = dist(p[a], p[b]) + dist(p[c], p[d]) dist(p[e], p[f])-(dist(p[a], p[c]) + dist(p[e], p[b])+dist(p[f], p[d]));
	      cost[2] = dist(p[a], p[b]) + dist(p[c], p[d]) dist(p[e], p[f])-(dist(p[a], p[e]) + dist(p[c], p[b])+dist(p[f], p[d]));
	      cost[3] = dist(p[a], p[b]) + dist(p[c], p[d]) dist(p[e], p[f])-(dist(p[a], p[f]) + dist(p[e], p[d])+dist(p[c], p[b]));
	      if(cost1 > cost2){
	      	g1 = b;
	      	h1 = e;
	      }
	      else{
	      	
	      }
	      if(cost3 > cost4){
	      }
	      else{
	      
	      }
	      if()
	      else{
	      
	      }
	      if() {
	        sa = (dist(p[a], p[b]) + dist(p[c], p[d])) - (dist(p[a], p[c]) + dist(p[b], p[d]));
	      }
	      if(max < sa) {
	        g = j; h = k;
	      }
	    }
	  }
	  while(g < h && max > 0) {
	    SWAP(tour[g], tour[h]);
	    g++; h--;
	  }
	}
}

void TwoOpt_with_SA(struct point p[MAX_N], int n, int tour[MAX_N]){
  int a,b,c,d;
  int i,j,k,l,g,h,sa;
  double excost;
  double prob;
  int itr = 0;
  double temp, temp_max = 100;
  double r;
  int max_itr = (n-1)*(n-2)/2;
  double min = INF;
  for(i=0; i < n-2; i++) {
    j = i + 1;
    for(k=i+2; k < n; k++) {
    	temp = temp_max-pow(temp_max,(float)(itr)/max_itr);
 //   	if(temp < 10) return;
    	printf("temp = %f\n", temp);
      l = (k + 1) % n;
      a = tour[i]; b = tour[j];
      c = tour[k]; d = tour[l];
      excost = (dist(p[a], p[b]) + dist(p[c], p[d])) - (dist(p[a], p[c]) + dist(p[b], p[d]));
//      printf("excost = %f\n", excost);
      if(excost >= 0) {
      	g = j; h = k;
        while(g < h) {
    			SWAP(tour[g], tour[h]);
   			 	g++; h--;
  			}
      }
      else {
      	prob = exp(excost/temp);
      	r = (double)rand()/RAND_MAX;
//      	printf("prob = %f rand = %f\n", prob, r);
      	if(r <= prob){
      		g = j; h = k;
		      while(g < h){
		  			SWAP(tour[g], tour[h]);
		 			 	g++; h--;
					}
      	}
      }
//      printf("%f\n", tour_length(p, n, tour));
      itr++;
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
  printf("data wrote\n");
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
  // 焼きなまし法を使った2optの改善
  TwoOpt_with_SA(p,n,tour);
  write_tour_data("tour2.dat",n,tour);
  TwoOpt(p,n,tour);
  // 巡回路をテキストファイルとして出力
  write_tour_data("tour3.dat",n,tour);
  // 巡回路長を画面に出力
  printf("%lf\n",tour_length(p,n,tour));

  exit(EXIT_SUCCESS);
  return 0;
}
