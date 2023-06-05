/*                                                                */
/*    訪問順制約付きTSP用プログラム                                    */
/*    C code written by K. Ando and K. Sekitani (Shizuoka Univ.)  */
/*                                                                */
/*                                                                */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
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

void ni(struct point p[MAX_N], int n, int tour[MAX_N], int m, int prec[MAX_N]){
  FILE *fp;
  int h,i,j,a,b,c,r;
  int visited[MAX_N]; // 都市iが訪問済みならば1そうでなければ0
  double d[MAX_N]; // 未訪問点 r から現在の部分巡回路までの最短距離を d[r] に保存
  double min_dist;
  int min_i,min_r;
  int sbtlen=0;

  for(a=0;a<n;a++) visited[a]=0; // 最初は全都市は未訪問
	/*
	a = prec[0];
	tour[0] = a; 
	visited[a] = 1;
  // a=prec[0] に最も近い点を探す
	min_dist = INF;
  for(r=0;r<n;r++) {
    if(!visited[r] && dist(p[a],p[r])<min_dist) {
      min_r=r;
      min_dist=dist(p[a],p[r]);
    }
  }
	
  tour[0]=a;tour[1]=min_r;sbtlen=2;
  visited[a]=visited[min_r]=1;      // 都市a,min_rは訪問済み
	*/

	for(int i = 0; i < m; i++){
		tour[i] = prec[i];
		visited[prec[i]] = 1; 
		sbtlen++;
	}

  while(sbtlen<n) {
    min_dist=INF;
    for(r=0;r<n;r++) {
      if(!visited[r]) {
				for(i=0;i<sbtlen;i++) {
					d[r]=dist(p[tour[i]],p[r]);	
					if (d[r]<min_dist) {
						min_i = i;
						min_dist = d[r];
						min_r=r;
					}
				}
      }
    }
		r=min_r;
		d[r] = min_dist;
		i = min_i;
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
// 2-opt
int TwoOpt_array(struct point p[MAX_N], int n, int tour[MAX_N],int m, int prec[MAX_N]){
  int a,b,c,d;
  int i,j,k,l,g,h;
  int x, y;
  int chk=0;
  int count;
  int count2;
  double Ei, Ej;
  for(i=0; i < n-2; i++) {
    for(k=i+2; k < n; k++) {
      j = i + 1;
      l = (k + 1) % n;
      a = tour[i]; b = tour[j];
      c = tour[k]; d = tour[l];
      Ei = dist(p[a], p[b]) + dist(p[c], p[d]);
      Ej = dist(p[a], p[c]) + dist(p[b], p[d]);
      if(Ei - Ej > EPSILON ) {
      	// 選んだ枝の2-opt改善が制約にかかるかどうか
		    count = 0;
		    for (x = j; x <=k; x++){
		    	for(y = 0; y < m; y++){
		    		if(tour[x] == prec[y]){
		    			count++;
		    			break;
		    		}
		    	}
		    	if(count == 2) break;
		    }
				if(count >= 2) continue;
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

// TwoOpt焼きなまし 制約対応
void TwoOpt_with_SA_array(struct point p[MAX_N], int n, int tour[MAX_N], int m, int prec[MAX_N], double temp_max){
  int a,b,c,d;
  int i,j,k,l,g,h;
  double Ei, Ej;
  double prob;
  //double temp_max = 100;
  double temp=temp_max; 
  double tempfac = 0;
  double freezelim = 100;
  double r;
  double min = INF;
  int max = -1;
  int chk = 0;
  int count;
  int x, y;
  double sumcost = 0;
  while(1){
		for(i=0; i < n-2; i++) {
			j = i + 1;
			for(k=i+2; k < n; k++) {
				l = (k + 1) % n;
				a = tour[i]; b = tour[j];
				c = tour[k]; d = tour[l];
				Ei = (dist(p[a], p[b]) + dist(p[c], p[d]));
				Ej = (dist(p[a], p[c]) + dist(p[b], p[d]));
				
				if(Ei-Ej > EPSILON) {
					g = j; h = k;
					// 選んだ枝の2-opt改善が制約にかかるかどうか
				  count = 0;
				  for (x = j; x <=k; x++){
				  	for(y = 0; y < m; y++){
				  		if(tour[x] == prec[y]) {
				  			count++;
				  			break;
				  		}
				  	}
				  	if(count == 2) break;
				  }
				  if(count>=2) continue;
				  sumcost += Ej-Ei;
				  while(g < h) {
						SWAP(tour[g], tour[h]);
		 			 	g++; h--;
					}
				}
				else {
					
					prob = exp(-(Ej-Ei)/temp);
					//else prob = exp(-(Ej-Ei)*(Ej-Ei)/temp);
					//prob = exp(-(Ej-Ei)/temp);
					/*
					//printf("cost = %f\n", Ej-Ei);
					if(Ej-Ei < 1) prob = exp(-(Ej-Ei)/temp);
					else if(Ej - Ei >= 1 && Ej -Ei <= 20) prob = exp(-(Ej-Ei)*(Ej-Ei)/temp);
					else prob = exp(-(Ej-Ei)*(Ej-Ei)*(Ej-Ei)/temp);
					*/
//	      	prob = exp(Ej/temp)/(exp(Ei/temp)+exp(Ej/temp));
					
					r = (double)rand()/RAND_MAX;
					//printf("prob = %f r = %f\n", prob, r);
					if(r <= prob){
						// 選んだ枝の2-opt改善が制約にかかるかどうか
						count = 0;
						for (x = j; x <=k; x++){
							for(y = 0; y < m; y++){
								if(tour[x] == prec[y]) {
									count++;
									break;
								}
							}
							if(count == 2) break;
						}
				  	if(count>=2) continue;
				  	sumcost += Ej-Ei;
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
		//printf("temp = %lf\n",temp);
		//printf("sumcost = %lf\n",sumcost);
		//printf("len = %lf\n",tour_length_array(p,n,tour));
		printf("sumcost = %lf\n",sumcost);
		if(temp <= freezelim) return;
	}
	
}

// 3Opt(高速化)
int ThreeOpt_array_n2(struct point p[MAX_N], int n, int tour[MAX_N], int m, int prec[MAX_N]) {
  int a, b, c, d, e, f;
  int I;
  int fImproved = 0;
  int NewTour[MAX_N];
  int count;
  int count_p1, count_p2, count_p3;
  int x, y;
  for (int i = 0; i < n-4; i++) {
    for (int j = i+1; j < n-2; j++) {
      double D[5]; int min = 0;
      int k = (int)(rand() % (n-j-1) + j+1);
      a = tour[i]; b = tour[i+1]; c = tour[j];
      d = tour[j+1]; e = tour[k]; f = tour[(k+1)%n];
      D[0] = dist(p[a],p[b]) + dist(p[c],p[d]) + dist(p[e],p[f]);  // 元の距離
      D[1] = dist(p[a],p[e]) + dist(p[d],p[b]) + dist(p[f],p[c]);
      D[2] = dist(p[d],p[a]) + dist(p[e],p[c]) + dist(p[f],p[b]);
      D[3] = dist(p[c],p[f]) + dist(p[a],p[d]) + dist(p[e],p[b]);
      D[4] = dist(p[a],p[c]) + dist(p[d],p[f]) + dist(p[e],p[b]);
      for (int m = 1; m <= 4; m++) {
        if (D[min] - D[m] > EPSILON) min = m;    // ４通りのつなぎ替えを比較
      }
      if (min == 0) continue;
      // 繋ぎ替えを実行
			if (min == 1) { // D(i,k) + D(j+1,i+1) + D(k+1,j)
        // 選んだ枝の3-2opt改善が制約にかかるかどうか
		    count_p1 = 0; count_p2 = 0; 
		    for (x = j+1; x <=k; x++){
		  	  for(y = 0; y < m; y++){
		  	  	if(tour[x] == prec[y]) {
		  	  		count_p1++;
		  	  		break;
		    		}
		    	}
		    	if(count_p1 == 2) break;
		    }
		    
		    for (x = i+1; x <=j; x++){
		  	  for(y = 0; y < m; y++){
		  	  	if(tour[x] == prec[y]) {
		  	  		count_p2++;
		  	  		break;
		    		}
		    	}
		    	if(count_p2 == 2) break;
		    }
		    if(count_p1 >= 2 || (count_p1 == 1 && count_p2 != 0) || count_p1 != 0) continue;
		    //if(1) continue;
		    //printf("min = %d\n", min);
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
        for(I = 0; I < n; I++) {
             tour[I] = NewTour[I];
        }
        return 1;
      } 
      else if (min == 2) { // D(j+1,i) + D(k,j) + D(k+1,i+1);
        // 選んだ枝の3-2opt改善が制約にかかるかどうか
		    count_p1 = 0; count_p2 = 0; 
		    for (x = j+1; x <=k; x++){
		  	  for(y = 0; y < m; y++){
		  	  	if(tour[x] == prec[y]) {
		  	  		count_p1++;
		  	  		break;
		    		}
		    	}
		    	if(count_p1 == 2) break;
		    }
		    for (x = i+1; x <=j; x++){
		  	  for(y = 0; y < m; y++){
		  	  	if(tour[x] == prec[y]) {
		  	  		count_p2++;
		  	  		break;
		    		}
		    	}
		    	if(count_p2 == 2) break;
		    }
        if(count_p2 >= 2 || (count_p2 == 1 && count_p1 != 0) || (count_p2 != 0)) continue;
        //if(1) continue;
        //printf("min = %d\n", min);
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
        for(I = 0; I < n; I++) {
           tour[I] = NewTour[I];
         }
        return 1;
      } 
      else if (min == 3) { // D(j,k+1) + D(i,j+1) + D(k,i+1)
        // 選んだ枝の3-2opt改善が制約にかかるかどうか
		    count_p1 = 0; count_p2 = 0; 
		    for (x = j+1; x <=k; x++){
		  	  for(y = 0; y < m; y++){
		  	  	if(tour[x] == prec[y]) {
		  	  		count_p1++;
		  	  		break;
		    		}
		    	}
		    	if(count_p1 == 2) break;
		    }
		    for (x = i+1; x <=j; x++){
		  	  for(y = 0; y < m; y++){
		  	  	if(tour[x] == prec[y]) {
		  	  		count_p2++;
		  	  		break;
		  	  	}
		    	}
		    	if(count_p2 == 2) break;
		    }
		    if(count_p1 >= 1 && count_p2 >= 1) continue;
		    //if(1) continue;
		    //printf("min = %d\n", min);
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
        
        for(I = 0; I < n; I++) {
          tour[I] = NewTour[I];
        }
        return 1;
       } 
       else if (min == 4) { // D(i,j) + D(j+1,k+1) + D(k,i+1)
        // 選んだ枝の3-2opt改善が制約にかかるかどうか
	      count_p1 = 0; count_p2 = 0;
	      for (x = j+1; x <= k; x++){
	    	  for(y = 0; y < m; y++){
	    	  	if(tour[x] == prec[y]) {
	    	  		count_p1++;
	    	  		break;
	      		}
	      	}
	      	if(count_p1 == 2) break;
	      }
	       for (x = i+1; x <=j; x++){
	    	  for(y = 0; y < m; y++){
	    	  	if(tour[x] == prec[y]) {
	    	  		count_p2++;
	    	  		break;
	      		}
	      	}
	      	if(count_p2 == 2) break;
	      }
	      if(!((count_p1 + count_p2 == 1) && (count_p1*count_p2 == 0))) continue;
	      
	      //if(count_p1 >= 2 || (count_p1 == 1 && count_p2 != 0) || count_p2 >= 2 || (count_p2 == 1 && count_p1 != 0)) continue;
	      //if(1) continue;
	      //printf("min = %d\n", min);
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
        
        for(I = 0; I < n; I++) {
          tour[I] = NewTour[I];
        }
        return 1;
      } 
       //printf("3-opt improved! min=%d\n", min);
    }
  }
  return fImproved;
}
// 3-2Opt(高速化)
int ThreeTwoOpt_array_n2(struct point p[MAX_N], int n, int tour[MAX_N], int m, int prec[MAX_N]) {
  int a, b, c, d, e, f;
  int I;
  int fImproved = 0;
  int NewTour[MAX_N];
  int count;
  int flag1 = 0, flag2 = 0, flag3 = 0;
  int count_p1, count_p2, count_p3;
  int x, y;
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
        if (D[min] - D[J] > EPSILON) min = J;    // ４通りのつなぎ替えを比較
      }
      if (min == 0) continue;
      // 繋ぎ替えを実行
      if (min == 1) { // D(i,k) + D(j+1,i+1) + D(k+1,j)
        // 選んだ枝の3-2opt改善が制約にかかるかどうか
		    count_p1 = 0; count_p2 = 0; 
		    for (x = j+1; x <=k; x++){
		  	  for(y = 0; y < m; y++){
		  	  	if(tour[x] == prec[y]) {
		  	  		count_p1++;
		  	  		break;
		    		}
		    	}
		    	if(count_p1 == 2) break;
		    }
		    
		    for (x = i+1; x <=j; x++){
		  	  for(y = 0; y < m; y++){
		  	  	if(tour[x] == prec[y]) {
		  	  		count_p2++;
		  	  		break;
		    		}
		    	}
		    	if(count_p2 == 2) break;
		    }
		    if(count_p1 >= 2 || (count_p1 == 1 && count_p2 != 0) || count_p1 != 0) continue;
		    //if(1) continue;
		    //printf("min = %d\n", min);
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
        for(I = 0; I < n; I++) {
             tour[I] = NewTour[I];
        }
        return 1;
      } 
      else if (min == 2) { // D(j+1,i) + D(k,j) + D(k+1,i+1);
        // 選んだ枝の3-2opt改善が制約にかかるかどうか
		    count_p1 = 0; count_p2 = 0; 
		    for (x = j+1; x <=k; x++){
		  	  for(y = 0; y < m; y++){
		  	  	if(tour[x] == prec[y]) {
		  	  		count_p1++;
		  	  		break;
		    		}
		    	}
		    	if(count_p1 == 2) break;
		    }
		    for (x = i+1; x <=j; x++){
		  	  for(y = 0; y < m; y++){
		  	  	if(tour[x] == prec[y]) {
		  	  		count_p2++;
		  	  		break;
		    		}
		    	}
		    	if(count_p2 == 2) break;
		    }
        if(count_p2 >= 2 || (count_p2 == 1 && count_p1 != 0) || (count_p2 != 0)) continue;
        //if(1) continue;
        //printf("min = %d\n", min);
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
        for(I = 0; I < n; I++) {
           tour[I] = NewTour[I];
         }
        return 1;
      } 
      else if (min == 3) { // D(j,k+1) + D(i,j+1) + D(k,i+1)
        // 選んだ枝の3-2opt改善が制約にかかるかどうか
		    count_p1 = 0; count_p2 = 0; 
		    for (x = j+1; x <=k; x++){
		  	  for(y = 0; y < m; y++){
		  	  	if(tour[x] == prec[y]) {
		  	  		count_p1++;
		  	  		break;
		    		}
		    	}
		    	if(count_p1 == 2) break;
		    }
		    for (x = i+1; x <=j; x++){
		  	  for(y = 0; y < m; y++){
		  	  	if(tour[x] == prec[y]) {
		  	  		count_p2++;
		  	  		break;
		  	  	}
		    	}
		    	if(count_p2 == 2) break;
		    }
		    if(count_p1 >= 1 && count_p2 >= 1) continue;
		    //if(1) continue;
		    //printf("min = %d\n", min);
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
        
        for(I = 0; I < n; I++) {
          tour[I] = NewTour[I];
        }
        return 1;
       } 
       else if (min == 4) { // D(i,j) + D(j+1,k+1) + D(k,i+1)
          // 選んだ枝の3-2opt改善が制約にかかるかどうか
		      count_p1 = 0; count_p2 = 0;
		      for (x = j+1; x <= k; x++){
		    	  for(y = 0; y < m; y++){
		    	  	if(tour[x] == prec[y]) {
		    	  		count_p1++;
		    	  		break;
		      		}
		      	}
		      	if(count_p1 == 2) break;
		      }
		       for (x = i+1; x <=j; x++){
		    	  for(y = 0; y < m; y++){
		    	  	if(tour[x] == prec[y]) {
		    	  		count_p2++;
		    	  		break;
		      		}
		      	}
		      	if(count_p2 == 2) break;
		      }
		      if(!((count_p1 + count_p2 == 1) && (count_p1*count_p2 == 0))) continue;
		      
		      //if(count_p1 >= 2 || (count_p1 == 1 && count_p2 != 0) || count_p2 >= 2 || (count_p2 == 1 && count_p1 != 0)) continue;
		      //if(1) continue;
		      //printf("min = %d\n", min);
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
          
          for(I = 0; I < n; I++) {
            tour[I] = NewTour[I];
          }
          return 1;
        } 
        else if (min == 5) { // D(i,j) + D(j+1,k+1) + D(k,i+1)
          // 選んだ枝の3-2opt改善が制約にかかるかどうか
	        count_p1 = 0; count_p2 = 0; 
	        for (x = j+1; x <=k; x++){
	      	  for(y = 0; y < m; y++){
	      	  	if(tour[x] == prec[y]) {
	      	  		count_p1++;
	      	  		break;
	        		}
	        	}
	        	if(count_p1 == 2) break;
	        }
	        if(count_p1 >= 2) continue;
	        //printf("min = %d\n", min);
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
          for(I = 0; I < n; I++) {
            tour[I] = NewTour[I];
          }
          return 1;
      } 
      else if (min == 6) { // D(i,j) + D(j+1,k+1) + D(k,i+1)
        // 選んだ枝の3-2opt改善が制約にかかるかどうか
	      count_p1 = 0; count_p2 = 0; 
	      for (x = i+1; x <=k; x++){
	    	  for(y = 0; y < m; y++){
	    	  	if(tour[x] == prec[y]) {
	    	  		count_p1++;
	    	  		break;
	      		}
	      	}
	        	if(count_p1 == 2) break;
	     	}
	      if(count_p1 >= 2) continue;
	      //printf("min = %d\n", min);
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
        for(I = 0; I < n; I++) {
          tour[I] = NewTour[I];
        }
        return 1;
      } 
      else if (min == 7) { // D(i,j) + D(j+1,k+1) + D(k,i+1)
        // 選んだ枝の3-2opt改善が制約にかかるかどうか
	      count_p1 = 0; count_p2 = 0; 
	      for (x = i+1; x <=j; x++){
	    	  for(y = 0; y < m; y++){
	    	  	if(tour[x] == prec[y]) {
	    	  		count_p1++;
	    	  		break;
	      		}
	      	}
	      	if(count_p1 == 2) break;
	      }
	      if(count_p1 >= 2) continue;
	      //printf("min = %d\n", min);
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

        for(I = 0; I < n; I++) {
          tour[I] = NewTour[I];
        }
        return 1;
      }
      //printf("3-opt improved! min=%d\n", min);
      //printf("3-opt improved! min=%d\n", min);
    }
  }
 	return 0;
}

// Oropt1近傍
int OrOpt1_list(struct point p[MAX_N], int n, struct list* visited, int pm, int prec[MAX_N]) {
  int a,b,c,d,e;
  int chk = 0;
  int x;
  int flag;
  double Ei, Ej;
  struct cell* tmp;
  int count = 0;
  for(struct cell* i=visited->head->next; i != visited->tail; i = i->next) {
		struct cell *j = (i->next != visited->tail) ? i->next : visited->head->next;
		for(struct cell* k = visited->head->next; k != visited->tail; k = tmp) {
			tmp = k->next;
			if(k->data == i->data || k->data == j->data) continue;
			struct cell *l = (k->prev != visited->head) ? k->prev : visited->tail->prev;
			struct cell *m = (k->next != visited->tail) ? k->next : visited->head->next;
			a = i->data; b = j->data; c = l->data; d = k->data; e = m->data;
			Ei = dist(p[a], p[b]) + dist(p[d], p[c]) + dist(p[d], p[e]);
			Ej = dist(p[a], p[d]) + dist(p[b], p[d]) + dist(p[c], p[e]);
			if(Ei-Ej > EPSILON) {
				flag = 0;
				for(x = 0; x < pm; x++){
					if(d == prec[x]) {
						flag = 1;
						break;	
					}
				}
				/*
				count = 0;
				if(flag) {
					for(strcut cell* g = i; g != k; g = g->next){
						for(int h = 0; h < pm; h++){
							if(g->data == prec[h]) {
								count++
								break;
							}
						}
						if(count == 1) break;
					}
				}
				if(count == 1) continue;
		  	*/
		  	if(flag) continue;
		  	//printf("a:%d, b:%d, c:%d, d:%d, e:%d\n", a, b, c, d, e);
		  	//printf("Ei = %f\n", dist(p[a], p[b]) + dist(p[d], p[c]) + dist(p[d], p[e]));
		  	//printf("Ej = %f\n", dist(p[a], p[d]) + dist(p[b], p[d]) + dist(p[c], p[e]));
		  	insertAfter(i, d);
		  	erase(k);
		  	chk=1;
		  	//printf("1\n");
		  	break;
			}
    }
  }
  return chk;
}
 
// OrOptk近傍
int OrOptk_list(int q, struct point p[MAX_N], int n, struct list* visited, int pm, int prec[MAX_N]){
	int a, b, c, e;
	int d[10];
	int chk = 0;
	int prec_chk;
	double Ei;
	double Ej;
	struct cell* tmp;
	struct cell* k[10];
	struct cell* end = visited->tail;
	clock_t time_start, time_end;
	time_start = clock();
	if(q==1){
		chk = OrOpt1_list(p, n, visited, pm, prec);
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
			Ei = dist(p[a], p[b])+dist(p[d[0]],p[c])+dist(p[d[q-1]],p[e]);
			Ej = dist(p[a],p[d[q-1]])+dist(p[b], p[d[0]])+dist(p[c], p[e]);
			if(Ei - Ej > EPSILON) {
				// 制約条件を満たすかどうか
				prec_chk = 0;
				for(int x = 0; x < q; x++){
					for(int y = 0; y < pm; y++){
						if(d[x] == prec[y]) {
							prec_chk = 1;
							break;
						}
					}
					if(prec_chk == 1) break;
				}
				if(prec_chk == 1) continue;
				
				// 改善
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
void exchangeprec(int n, struct list* visited, int m, int prec[MAX_N]) {
	struct cell* tmp;
	struct cell* tmp2;
	struct cell* tmp3;
	int x = rand()%2 +1;
  for(struct cell* i=visited->head->next; i != visited->tail; i=i->next) {
	  for(int j = 0; j < m; j++) {
	    if(i->data == prec[j]) {
	      //printf("visited->head->next->data:%d\n", visited->head->next->data);
	      //printf("i->data:%d\n", i->data);
	     
	      tmp = i->next;
	      tmp2 = i->next->next;
	      tmp3 = i->next->next->next;
	      
	      if(x == 1){
	      	insertBefore(visited->tail, i->data);
					erase(i);
	      }
	     	else if(x == 2){
	     		insertBefore(visited->tail, i->data);
					erase(i);
					 
					insertBefore(visited->tail, tmp->data);
					erase(tmp);
	     	}
	     	else if(x == 3){
	     		insertBefore(visited->tail, i->data);
					erase(i);
					 
					insertBefore(visited->tail, tmp->data);
					erase(tmp);
					  
					insertBefore(visited->tail, tmp2->data);
					erase(tmp2);
	     	}
	     	/*
			  else if(x == 4){ 
					insertBefore(visited->tail, i->data);
					erase(i);
					  
					insertBefore(visited->tail, tmp->data);
					erase(tmp);
					  
					insertBefore(visited->tail, tmp2->data);
					erase(tmp2);
					  
					insertBefore(visited->tail, tmp3->data);
					erase(tmp3);
			  }
			  */
			    return;
	    }
	  }
	}
}

double loop_array(struct point p[MAX_N], int n, int tour[MAX_N], int pm, int prec[MAX_N], int nic, int exflag, double best){
	int newtour[MAX_N];
	int chk1 = 0;
	int chk2 = 1;
	int chk3 = 1;
	int chk32 = 1;
	int chkk = 1;
	int chk2or;
	int chk_array[10];
	int count=0;
	struct list visited;
	
	// コピー
	for(int i = 0; i < n; i++) newtour[i] = tour[i];
	double before = tour_length_array(p,n,newtour);
	// 配列をリストに変換
	initialize(&visited);
	for(int i=0; i<n; i++) {
	  insertBefore(visited.tail, newtour[i]);
	}
	for(int i=0; i<nic; i++) exchangeprec(n, &visited, pm, prec);
  //exchangePrec(n, &visited, pm, prec);
  // リストから配列に変換
	struct cell* j = visited.head->next;
	for(int i = 0; i < n; i++){
		newtour[i] = j->data;
		j = j->next;
	}
  
  	TwoOpt_with_SA_array(p, n, newtour, pm, prec, 100+nic*10);
  	printf("bad done \n");
  	printf("%5.1lf\n",tour_length_array(p,n,newtour)-before); // 巡回路tourの長さ
  
  switch(rand()%2+1) {
    case 1:
      chk2 = 1;
      while(chk2){
      	// 2-optによる改善
	      chk2 = TwoOpt_array(p,n,newtour,pm,prec);
      }
      //printf("2opt done\n");
      //printf("%5.1lf\n",tour_length_array(p,n,newtour));
      
      // 配列をリストに変換
      initialize(&visited);
      for(int i=0; i<n; i++) {
        insertBefore(visited.tail, newtour[i]);
      }
      
      // chk配列を初期化
      for(int i = 0; i < 10; i++) chk_array[i] = 1;
      chkk = 1;
      // OrOptkによる改善
      while(chkk == 1) {
	      chkk = 0;
	      for(int k = 6; k >= 1; k--){
		      if(chk_array[k] == 1){
			      chk_array[k] = OrOptk_list(k, p, n, &visited, pm, prec);
			      if(chk_array[k] == 2) {
				      chkk = 2;
				      break;
			      }
		      }
		      if(chk_array[k] == 1) chkk = 1;
	      }
	      if(chkk == 2) break;
      }
      //printf("oropt done\n");
      //printf("%5.1lf\n",tour_length_list(p,n,&visited));
      
      // リストから配列に変換
      j = visited.head->next;
      for(int i = 0; i < n; i++){
	      newtour[i] = j->data;
	      j = j->next;
      }
      
      chk32 = 1;
      while(chk32){
	      // 3-2Optによる改善
	      chk32 = ThreeTwoOpt_array_n2(p, n, newtour, pm, prec);
      }
      //printf("32opt done \n");
      //printf("%5.1lf\n",tour_length_array(p,n,newtour));
      break;
    
    case 2:
      chk32 = 1;
      while(chk32){
	      // 3-2Optによる改善
	      chk32 = ThreeTwoOpt_array_n2(p, n, newtour, pm, prec);
      }
      //printf("32opt done \n");
      //printf("%5.1lf\n",tour_length_array(p,n,newtour));
      // 配列をリストに変換
      initialize(&visited);
   	  for(int i=0; i<n; i++) {
        insertBefore(visited.tail, newtour[i]);
      }
      
      // 配列をリストに変換
      initialize(&visited);
      for(int i=0; i<n; i++) {
        insertBefore(visited.tail, newtour[i]);
      }
      
      // chk配列を初期化
      for(int i = 0; i < 10; i++) chk_array[i] = 1;
      chkk = 1;
      // OrOptkによる改善
      while(chkk == 1) {
	      chkk = 0;
	      for(int k = 6; k >= 1; k--){
		      if(chk_array[k] == 1){
			      chk_array[k] = OrOptk_list(k, p, n, &visited, pm, prec);
			      if(chk_array[k] == 2) {
				      chkk = 2;
				      break;
			      }
		      }
		      if(chk_array[k] == 1) chkk = 1;
	      }
	      if(chkk == 2) break;
      }
      //printf("oropt done\n");
      //printf("%5.1lf\n",tour_length_list(p,n,&visited));
      
      // リストから配列に変換
      j = visited.head->next;
      for(int i = 0; i < n; i++){
	      newtour[i] = j->data;
	      j = j->next;
      }
      
      while(chk2){
      	// 2-optによる改善
	      chk2 = TwoOpt_array(p,n,newtour,pm,prec);
      }
      //printf("2opt done\n");
      //printf("%5.1lf\n",tour_length_array(p,n,newtour));
      
      break;
  }
	
	//for(int i = 0; i < n; i++) tour[i] = newtour[i];
	
	if(best > tour_length_array(p,n,newtour) /*|| exflag == 1*/){
	  for(int i = 0; i < n; i++) tour[i] = newtour[i];
	}
	
	printf("loop done\n");
	printf("%5.1lf\n",tour_length_array(p,n,newtour));
	return tour_length_array(p,n,newtour);
}

int main(int argc, char *argv[]) {
  int n;                   // 点の数 
  int m;                   // 順序制約に現れる点の数
  struct point p[MAX_N];   // 各点の座標を表す配列 
  int tour[MAX_N];   // 巡回路を表現する配列
  int prec[MAX_N];   // 順序制約を表現する配列
  int i;
  struct list visited;
  struct list* vis = &visited;
	int chk2, chk3, chk32;
	int chkk;
	int chk_array[10];
  if(argc != 2) {
    fprintf(stderr,"Usage: %s <tsp_filename>\n",argv[0]);
    exit(EXIT_FAILURE);
  }

  // 点の数と各点の座標を1番目のコマンドライン引数で指定されたファイルから読み込む
  read_tsp_data(argv[1],p,&n,prec,&m);

  //順序制約の確認
  //for(i=0;i<m;i++) printf("%d\n",prec[i]);

  // 最近挿入法による巡回路構築
  ni(p, n, tour, m, prec);
  
  chk2 = 1;
  while(chk2){
  	// 2-optによる改善
		chk2 = TwoOpt_array(p,n,tour,m,prec);
	}
	write_tour_data_array("tour.dat",n,tour);
	//printf("%5.1lf\n",tour_length_array(p,n,tour));
	
	//配列をリストに変換
	initialize(vis);
	for(int i=0; i<n; i++) {
	  insertBefore(visited.tail, tour[i]);
	}
	// chk配列を初期化
	for(int i = 0; i < 10; i++) chk_array[i] = 1;
	chkk = 1;
	// OrOptkによる改善
	while(chkk == 1) {
		chkk = 0;
		for(int k = 6; k >= 1; k--){
			if(chk_array[k] == 1){
				chk_array[k] = OrOptk_list(k, p, n, vis, m, prec);
				write_tour_data_list("tour.dat",n,vis);
				//printf("oroptk done\n");
				//printf("%5.1lf\n",tour_length_list(p,n,vis));
				if(chk_array[k] == 2) {
					chkk = 2;
					break;
				}
			}
			if(chk_array[k] == 1) chkk = 1;
		}
		if(chkk == 2) break;
	}
	// リストから配列に変換
	struct cell* j = visited.head->next;
	for(int i = 0; i < n; i++){
		tour[i] = j->data;
		j = j->next;
	}
	chk32 = 1;
	while(chk32){
		// 3-2Optによる改善
		chk32 = ThreeTwoOpt_array_n2(p, n, tour, m, prec);
		write_tour_data_array("tour.dat",n,tour);
		//printf("32opt done\n");
		//printf("%5.1lf\n",tour_length_array(p,n,tour));
	}
  
  //配列をリストに変換
  initialize(vis);
	for(int i=0; i<n; i++) {
	  insertBefore(visited.tail, tour[i]);
	}
  double best = tour_length_array(p,n,tour);
	
  int cm = 0;
  int exflag = 0;
  int nic = 1;
  int nicmax = 10;
  while(1) {
    cm++;
    printf("loop:%d\n", cm);
    printf("nic:%d\n", nic);
  	double ND = loop_array(p, n, tour, m, prec, nic, exflag, best);
  	//flag = 0;
  	if(best > ND) {
  	  //flag = 1;
  	  best = ND;
  	  write_tour_data_array("tour.dat",n,tour);
  	  nic = 0;
  	} else if(best < ND && exflag == 1) {
  	    nic = 1;
  	    exflag = 0;
  	} else if(best < ND && exflag == 0){
  	    nic++;
  	    exflag = 0;
  	    if(nic > nicmax) {
  	     // nic = rand()%2+3;
          nic = 9;
  	      //nic = 1;    
		      //exflag = 1;
  	    }
  	}
  	printf("best\n");
  	printf("%5.1lf\n\n",best);
  }
  
  exit(EXIT_SUCCESS);
}
