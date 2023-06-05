/*                                                             */
/*    経路表示用プログラム                                      */
/*    C code written by K. Ando (Shizuoka Univ.)               */
/*                                                             */
/*              Version 2008.12.5                              */
/*              Version 2013.04.23                             */
/*              Version 2015.06.04                             */
/*                                                             */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gd.h>
#include <gdfonts.h>
#include <math.h>
#define MAX_N 10000   // 点の数の最大値
#define INF 100000000 // 無限大の定義

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

void read_tsp_data(char *filename, struct point p[MAX_N],int *np, int prec[MAX_N], int *mp) {
  FILE *fp;
  char buff[10000];
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

void draw(struct point p[MAX_N],int n,int tour[MAX_N], int m, int prec[MAX_N]) {
  int i, j;
  gdImagePtr im;
  FILE *fp;
  int white,black,red,yellow;
  char number[100];
  char number2[100];
	int flag;
	int mm;
  im = gdImageCreate (4000, 3000);  /* 700x700 のイメージを作成する */

  /* 変数 yellow に黄色を割り当てる. 黄色が背景色になる */
  yellow = gdImageColorAllocate (im,255,255,0);

  /* 変数 black に黒色を, 変数 red に赤色を割り当てる */
  black = gdImageColorAllocate(im,   0, 0, 0);
  red   = gdImageColorAllocate(im, 255, 0, 0);

  // 描画画面に点 i を半径7pxの黒丸でプロットし, 
  // さらに, 黒丸の横に番号iを黒字で添える.
  for(i=0;i<n;i++) {
  	
  	flag = 0;
  	for(j = 0; j < m; j++){
  		if(tour[i] == prec[j]) {
  			flag = 1;
  			mm = i;
  			break;
  		}
  	}
  	if(flag) {
  		gdImageArc(im, p[i].x, p[i].y, 7, 7, 0, 360, red);
  		sprintf(number,"%2d",mm);
    	gdImageString(im, gdFontSmall, p[i].x,p[i].y+5,number,red);
    }
    
    else gdImageArc(im, p[i].x, p[i].y, 7, 7, 0, 360, black);
  	
  	//gdImageArc(im, p[i].x, p[i].y, 7, 7, 0, 360, black);
  	gdImageLine(im,p[tour[i]].x,p[tour[i]].y,p[tour[(i+1)%n]].x,p[tour[(i+1)%n]].y,black);
  } 
 /*   
  // 描画画面に巡回路の総距離を印字する
  sprintf(number2,"%5.1lf",tour_length(p,n,tour));
  gdImageString(im, gdFontSmall, 500, 0,"total_length = ",black);
  gdImageString(im, gdFontSmall, 600, 0,number2,black);
	*/
  /* 標準出力にイメージを png フォーマットで出力する */
  gdImagePng(im, stdout);

  /* 作成したイメージを破棄する */
  gdImageDestroy(im);
}

int main(int argc, char *argv[]) {
  int n,m,i;                // 点の数 
  int pm;
  int prec[MAX_N];
  struct point p[MAX_N];   // 各点の座標を表す配列 
  int tour[MAX_N];
  scanf("%d",&m);
  //printf("%d", m);
  for(i=0;i<m;i++) 
    scanf("%d",&tour[i]);

  // 点の数と各点の座標を1番目のコマンドライン引数で指定されたファイルから読み込む
   read_tsp_data(argv[1],p,&n,prec,&pm);
  
  // 点の座標を2番目のコマンドライン引数で指定されたファイルに出力
  draw(p,n,tour, pm, prec);
}

