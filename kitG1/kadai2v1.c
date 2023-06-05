/*                                                                */
/*    訪問順制約付きTSP用プログラム                                    */
/*    C code written by K. Ando and K. Sekitani (Shizuoka Univ.)  */
/*                                                                */
/*                                                                */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gd.h>
#include <gdfonts.h>
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

void copyarray(int dst[MAX_N], int src[MAX_N], int n) {
  int i;
  for(i=0;i<n;i++) dst[i]=src[i];
}

void reversearray(int dst[MAX_N], int src[MAX_N], int n) {
  int i;
  for(i=0;i<n;i++) dst[i]=src[n-1-i];
}

void shiftarray(int k, int arr[MAX_N], int n) {
  int i, tmp[MAX_N];
  copyarray(tmp,arr,n);
  for (i=0;i<n;i++) arr[(n+i-k)%n]=tmp[i];
}

int check_validity(int tour[MAX_N], int n, int prec[MAX_N], int m) {
  int i,j,start=-1;
  int check[MAX_N];
  int order[MAX_N];
  int revtour[MAX_N];
  int error_flag_forward=0,error_flag_backward=0;
  char messf[50],messb[50];

  for(i=0;i<n;i++) check[i]=0;
  for(j=0;j<n;j++) revtour[j]=0;
  for(j=0;j<m;j++) order[j]=0;

  for(i=0;i<n;i++) check[tour[i]]+=1; //tour に含まれている都市は何個あるか計数

  for(i=0;i<n;i++) 
    if(check[i]!=1) {
      return 0;
    }

  for(i=0;start==-1 && i<n;i++) {
    for(j=0;j<m;j++) {
      if(tour[i]==prec[j]) {
	      start=j;
	      break;
      }
    }
  }

  shiftarray(start,prec,m);

  for(j=0;j<m;j++) 
    for(i=0;i<n;i++) {
      if(tour[i]==prec[j]) order[j]=i;
    }

  for(j=1;j<m;j++) 
    if(order[j-1]>order[j]) {
      error_flag_forward=1;
      break;
   }

  reversearray(revtour,tour,n);
  
  start = -1;
  for(i=0;start==-1 && i<n;i++) {
    for(j=0;j<m;j++) {
      if(revtour[i]==prec[j]) {
	      start=j;
	      break;
      }
    }
  }

  shiftarray(start,prec,m);

  for(j=0;j<m;j++) 
    for(i=0;i<n;i++) {
      if(revtour[i]==prec[j]) order[j]=i;
    }

  for(j=1;j<m;j++) 
    if(order[j-1]>order[j]) {
      error_flag_backward=1;
      break;
    }

  if(error_flag_forward*error_flag_backward) {
    return 1;
  }
  return 0;
}

void nn(struct point p[MAX_N],int n,int tour[MAX_N], int m, int prec[MAX_N]){
  int i,j,r,nearest=-1;
  int a;
  int visited[MAX_N]; // 都市iが訪問済みならば1そうでなければ0
  double min;
  int precchk = -1;
  int precpos = 0;
  int R;
  R = (int)(rand() % m);
  for(r=0;r<n;r++) visited[r]=0; // 最初は全都市は未訪問
  tour[0]=prec[R];         // 最初に訪問する都市は 0 
  visited[prec[R]]=1;      // 都市0は訪問済み
  precpos = (R+1) % m;
  printf("%d\n", precpos);
  for(i=0;i<n-1;i++) {
    a = tour[i];
    //最後に訪問した都市 a == tour[i]から最短距離にある未訪問都市nearestを
    //見つける
    min = INF;   
    for(r=0;r<n;r++) { 
      if(!visited[r] && dist(p[a],p[r])<min){
        precchk = -1;
        if(r == prec[precpos]) precchk = 1;
        printf("r:%d prec[precpos]:%d\n", r, prec[precpos]);
        if(!visited[r] && precchk != -1) {
          if(prec[precpos] == prec[precchk]) {
            nearest=r; //都市tour[i]から暫定的に最短距離にある未訪問都市をnearestとする
            min = dist(p[a],p[r]); // その暫定最短距離をminとする
            printf("%d\n", precpos);
          } else {
          	continue;
          }
        } else if(!visited[r] && precchk == -1) {
            nearest=r; //都市tour[i]から暫定的に最短距離にある未訪問都市をnearestとする
            min = dist(p[a],p[r]); // その暫定最短距離をminとする
        }
      }
    }
    if(nearest == prec[precpos]) precpos = (R+1) % m;
    tour[i+1]=nearest; // i+1 番目に訪問する都市を nearest にして, 
    visited[nearest]=1;// nearest を訪問済みとする. 
  }
}
void nn2(struct point p[MAX_N],int n,int tour[MAX_N], int m, int prec[MAX_N]){
  int i,j,r,nearest=-1;
  int a;
  int visited[MAX_N]; // 都市iが訪問済みならば1そうでなければ0
  double min;
  int precchk = -1;
  int precpos = 0;
  int R;
  R = (int)(rand() % m);
  for(r=0;r<n;r++) visited[r]=0; // 最初は全都市は未訪問
  tour[0]=prec[R];         // 最初に訪問する都市は 0 
  visited[prec[R]]=1;      // 都市0は訪問済み
  precpos = (R+1) % m;
  printf("%d\n", precpos);
  for(i=0;i<n-1;i++) {
    a = tour[i];
    //最後に訪問した都市 a == tour[i]から最短距離にある未訪問都市nearestを
    //見つける
    min = INF;   
    for(r=0;r<n;r++) { 
      if(!visited[r] && dist(p[a],p[r])<min){
        precchk = -1;
        if(r == prec[precpos]) precchk = 1;
        printf("r:%d prec[precpos]:%d\n", r, prec[precpos]);
        if(!visited[r] && precchk != -1) {
          if(prec[precpos] == prec[precchk]) {
            nearest=r; //都市tour[i]から暫定的に最短距離にある未訪問都市をnearestとする
            min = dist(p[a],p[r]); // その暫定最短距離をminとする
            printf("%d\n", precpos);
          } else {
          	continue;
          }
        } else if(!visited[r] && precchk == -1) {
            nearest=r; //都市tour[i]から暫定的に最短距離にある未訪問都市をnearestとする
            min = dist(p[a],p[r]); // その暫定最短距離をminとする
        }
      }
    }
    if(nearest == prec[precpos]) precpos = (R+1) % m;
    tour[i+1]=nearest; // i+1 番目に訪問する都市を nearest にして, 
    visited[nearest]=1;// nearest を訪問済みとする. 
  }
}
// 3-2opt
int ThreeTwoOpt_array(struct point p[MAX_N], int n, int tour[MAX_N], int m, int prec[MAX_N]) {
  int a, b, c, d, e, f;
  int I;
  int fImproved = 0;
  int NewTour[MAX_N];
  int count;
  for (int i = 0; i < n-4; i++) {
    for (int j = i+1; j < n-2; j++) {
      for (int k = j+1; k < n; k++) {
        int D[8], min = 0, precchk = 0;
        a = tour[i]; b = tour[i+1]; c = tour[j];
        d = tour[j+1]; e = tour[k]; f = tour[(k+1)%n];
        D[0] = dist(p[a],p[b]) + dist(p[c],p[d]) + dist(p[e],p[f]);  // 元の距離
        D[1] = dist(p[a],p[e]) + dist(p[d],p[b]) + dist(p[f],p[c]);
        D[2] = dist(p[d],p[a]) + dist(p[e],p[c]) + dist(p[f],p[b]);
        D[3] = dist(p[c],p[f]) + dist(p[a],p[d]) + dist(p[e],p[b]);
        D[4] = dist(p[a],p[c]) + dist(p[d],p[f]) + dist(p[e],p[b]);
        D[5] = dist(p[a],p[b]) + dist(p[c],p[e]) + dist(p[d],p[f]);
        D[6] = dist(p[a],p[e]) + dist(p[c],p[d]) + dist(p[b],p[f]);
        D[7] = dist(p[a],p[c]) + dist(p[b],p[d]) + dist(p[e],p[f]);
        for (int J = 1; J <= 7; J++) {
          if (D[J] < D[min]) min = J;    // 7通りのつなぎ替えを比較
        }
        if (min == 0) continue;
        // 繋ぎ替えを実行
        if (min == 1) { // D(i,k) + D(j+1,i+1) + D(k+1,j)
          count = 0;
          for(I = 0; I <= i; I++) {
            NewTour[count] = tour[I];
            count++;
          }
          for(I = k; I >= j+1; I--) {
            NewTour[count] = tour[I];
            count++;
          }
          for(I = i+1; I <= j; I++) {
            NewTour[count] = tour[I];
            count++;
          }
          for(I = k+1; I < n; I++) {
            NewTour[count] = tour[I];
            count++;
          }
          if(check_validity(NewTour,n,prec,m)) continue;
          for(I = 0; I < n; I++) {
               tour[I] = NewTour[I];
          }
          return 1;
        } else if (min == 2) { // D(j+1,i) + D(k,j) + D(k+1,i+1);
            count = 0;
            for(I = 0; I <= i; I++) {
              NewTour[count] = tour[I];
              count++;
            }
            for(I = j+1; I <= k; I++) {
              NewTour[count] = tour[I];
              count++;
            }
            for(I = j; I >= i+1; I--) {
              NewTour[count] = tour[I];
              count++;
            }
            for(I = k+1; I < n; I++) {
              NewTour[count] = tour[I];
              count++;
            }
            if(check_validity(NewTour,n,prec,m)) continue;
            for(I = 0; I < n; I++) {
               tour[I] = NewTour[I];
             }
            return 1;
        } else if (min == 3) { // D(j,k+1) + D(i,j+1) + D(k,i+1)
            count = 0;
            for(I = 0; I <= i; I++) {
                NewTour[count] = tour[I];
                count++;
            }
            for(I = j+1; I <= k; I++) {
              NewTour[count] = tour[I];
              count++;
            }
            for(I = i+1; I <= j; I++) {
              NewTour[count] = tour[I];
              count++;
            }
            for(I = k+1; I < n; I++) {
              NewTour[count] = tour[I];
              count++;
            }
            if(check_validity(NewTour,n,prec,m)) continue;
            for(I = 0; I < n; I++) {
              tour[I] = NewTour[I];
            }
            return 1;
        } else if (min == 4) { // D(i,j) + D(j+1,k+1) + D(k,i+1)
            count = 0;
            for(I = 0; I <= i; I++) {
              NewTour[count] = tour[I];
              count++;
            }
            for(I = j; I >= i+1; I--) {
              NewTour[count] = tour[I];
              count++;
            }
            for(I = k; I >= j+1; I--) {
              NewTour[count] = tour[I];
              count++;
            }
            for(I = k+1; I < n; I++) {
              NewTour[count] = tour[I];
              count++;
            }
            if(check_validity(NewTour,n,prec,m)) continue;
            for(I = 0; I < n; I++) {
              tour[I] = NewTour[I];
            }
            return 1;
        } else if (min == 5) { // D(i,j) + D(j+1,k+1) + D(k,i+1)
            count = 0;
            for(I = 0; I <= j; I++) {
              NewTour[count] = tour[I];
              count++;
            }
            for(I = k; I >= j+1; I--) {
              NewTour[count] = tour[I];
              count++;
            }
            for(I = k+1; I < n; I++) {
              NewTour[count] = tour[I];
              count++;
            }
            if(check_validity(NewTour,n,prec,m)) continue;
            for(I = 0; I < n; I++) {
              tour[I] = NewTour[I];
            }
            return 1;
        } else if (min == 6) { // D(i,j) + D(j+1,k+1) + D(k,i+1)
            count = 0;
            for(I = 0; I <= i; I++) {
              NewTour[count] = tour[I];
              count++;
            }
            for(I = k; I >= i+1; I--) {
              NewTour[count] = tour[I];
              count++;
            }
            for(I = k+1; I < n; I++) {
              NewTour[count] = tour[I];
              count++;
            }
            if(check_validity(NewTour,n,prec,m)) continue;
            for(I = 0; I < n; I++) {
              tour[I] = NewTour[I];
            }
            return 1;
        } else if (min == 7) { // D(i,j) + D(j+1,k+1) + D(k,i+1)
            count = 0;
            for(I = 0; I <= i; I++) {
              NewTour[count] = tour[I];
              count++;
            }
            for(I = j; I >= i+1; I--) {
              NewTour[count] = tour[I];
              count++;
            }
            for(I = j+1; I < n; I++) {
              NewTour[count] = tour[I];
              count++;
            }
            if(check_validity(NewTour,n,prec,m)) continue;
            for(I = 0; I < n; I++) {
              tour[I] = NewTour[I];
            }
            return 1;
        }
         //printf("3-opt improved! min=%d\n", min);
      }
    }
  }
  return 0;
}

// 3-2Opt(高速化)
int ThreeTwoOpt_array_n2(struct point p[MAX_N], int n, int tour[MAX_N], int m, int prec[MAX_N]) {
  int a, b, c, d, e, f;
  int I;
  int fImproved = 0;
  int NewTour[MAX_N];
  int count;
  for (int i = 0; i < n-4; i++) {
    for (int j = i+1; j < n-2; j++) {
      double D[10];
      int min = 0;
      int k = (int)(rand() % (n-j-1) + j+1);
      a = tour[i]; b = tour[i+1]; c = tour[j];
      d = tour[j+1]; e = tour[k]; f = tour[(k+1)%n];
      D[0] = dist(p[a],p[b]) + dist(p[c],p[d]) + dist(p[e],p[f]);  // 元の距離
      D[1] = dist(p[a],p[e]) + dist(p[d],p[b]) + dist(p[f],p[c]);
      D[2] = dist(p[d],p[a]) + dist(p[e],p[c]) + dist(p[f],p[b]);
      D[3] = dist(p[c],p[f]) + dist(p[a],p[d]) + dist(p[e],p[b]);
      D[4] = dist(p[a],p[c]) + dist(p[d],p[f]) + dist(p[e],p[b]);
      D[5] = dist(p[a],p[b]) + dist(p[c],p[e]) + dist(p[d],p[f]);
      D[6] = dist(p[a],p[e]) + dist(p[c],p[d]) + dist(p[b],p[f]);
      D[7] = dist(p[a],p[c]) + dist(p[b],p[d]) + dist(p[e],p[f]);
      for (int J = 0; J <= 7; J++) {
        if (D[J] < D[min]) min = J;    // ４通りのつなぎ替えを比較
      }
      if (min == 0) continue;
      // 繋ぎ替えを実行
      if (min == 1) { // D(i,k) + D(j+1,i+1) + D(k+1,j)
        count = 0;
        for(I = 0; I <= i; I++) {
          NewTour[count] = tour[I];
          count++;
        }
        for(I = k; I >= j+1; I--) {
          NewTour[count] = tour[I];
          count++;
        }
        for(I = i+1; I <= j; I++) {
          NewTour[count] = tour[I];
          count++;
        }
        for(I = k+1; I < n; I++) {
          NewTour[count] = tour[I];
          count++;
        }
        if(check_validity(NewTour,n,prec,m)) continue;
        for(I = 0; I < n; I++) {
           tour[I] = NewTour[I];
        }
        return 1;
      } else if (min == 2) { // D(j+1,i) + D(k,j) + D(k+1,i+1);
          count = 0;
          for(I = 0; I <= i; I++) {
            NewTour[count] = tour[I];
            count++;
          }
          for(I = j+1; I <= k; I++) {
            NewTour[count] = tour[I];
            count++;
          }
          for(I = j; I >= i+1; I--) {
            NewTour[count] = tour[I];
            count++;
          }
          for(I = k+1; I < n; I++) {
            NewTour[count] = tour[I];
            count++;
          }
          if(check_validity(NewTour,n,prec,m)) continue;
          for(I = 0; I < n; I++) {
             tour[I] = NewTour[I];
          }
          return 1;
      } else if (min == 3) { // D(j,k+1) + D(i,j+1) + D(k,i+1)
          count = 0;
          for(I = 0; I <= i; I++) {
             NewTour[count] = tour[I];
             count++;
          }
          for(I = j+1; I <= k; I++) {
            NewTour[count] = tour[I];
            count++;
          }
          for(I = i+1; I <= j; I++) {
            NewTour[count] = tour[I];
            count++;
          }
          for(I = k+1; I < n; I++) {
            NewTour[count] = tour[I];
            count++;
          }
          if(check_validity(NewTour,n,prec,m)) continue;
          for(I = 0; I < n; I++) {
            tour[I] = NewTour[I];
          }
          return 1;
      } else if (min == 4) { // D(i,j) + D(j+1,k+1) + D(k,i+1)
          count = 0;
          for(I = 0; I <= i; I++) {
            NewTour[count] = tour[I];
            count++;
          }
          for(I = j; I >= i+1; I--) {
            NewTour[count] = tour[I];
            count++;
          }
          for(I = k; I >= j+1; I--) {
            NewTour[count] = tour[I];
            count++;
          }
          for(I = k+1; I < n; I++) {
            NewTour[count] = tour[I];
            count++;
          }
          if(check_validity(NewTour,n,prec,m)) continue;
          for(I = 0; I < n; I++) {
            tour[I] = NewTour[I];
          }
          return 1;
      } else if (min == 5) { // D(i,j) + D(j+1,k+1) + D(k,i+1)
          count = 0;
          for(I = 0; I <= j; I++) {
            NewTour[count] = tour[I];
            count++;
          }
          for(I = k; I >= j+1; I--) {
            NewTour[count] = tour[I];
            count++;
          }
          for(I = k+1; I < n; I++) {
            NewTour[count] = tour[I];
            count++;
          }
          if(check_validity(NewTour,n,prec,m)) continue;
          for(I = 0; I < n; I++) {
            tour[I] = NewTour[I];
          }
          return 1;
      } else if (min == 6) { // D(i,j) + D(j+1,k+1) + D(k,i+1)
          count = 0;
          for(I = 0; I <= i; I++) {
            NewTour[count] = tour[I];
            count++;
          }
          for(I = k; I >= i+1; I--) {
            NewTour[count] = tour[I];
            count++;
          }
          for(I = k+1; I < n; I++) {
            NewTour[count] = tour[I];
            count++;
          }
          if(check_validity(NewTour,n,prec,m)) continue;
          for(I = 0; I < n; I++) {
            tour[I] = NewTour[I];
          }
         	return 1;
      } else if (min == 7) { // D(i,j) + D(j+1,k+1) + D(k,i+1)
          count = 0;
          for(I = 0; I <= i; I++) {
            NewTour[count] = tour[I];
            count++;
          }
          for(I = j; I >= i+1; I--) {
            NewTour[count] = tour[I];
            count++;
          }
          for(I = j+1; I < n; I++) {
            NewTour[count] = tour[I];
            count++;
          }
          if(check_validity(NewTour,n,prec,m)) continue;
          for(I = 0; I < n; I++) {
            tour[I] = NewTour[I];
          }
          return 1;
      }
       //printf("3-opt improved! min=%d\n", min);
    }
  }
 	return 0;
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

  // 最近近傍法による巡回路構築
  nn(p,n,tour,m,prec);
  write_tour_data_array("tour1.dat",n,tour);
  //printf("ci done\n");
  printf("%5.1lf\n",tour_length_array(p,n,tour)); // 巡回路tourの長さ
  /*
  // 3+2optによる改善
  int chk32opt = 1;
  while(chk32opt >= 1) {
    chk32opt = ThreeTwoOpt_array_n2(p,n,tour,m,prec);
    write_tour_data("tour2.dat",n,tour);
    printf("32opt done\n");
    printf("%5.1lf\n",tour_length(p,n,tour)); // 巡回路tourの長さ
  }
  */
  /*
  int newtour[MAX_N];
  for(i = 0; i < n; i++) {
    newtour[i] = tour[n-i-1];
  }
  */

  exit(EXIT_SUCCESS);
}
