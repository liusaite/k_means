/* File:  serial_k.c
 * Comple: gcc -g -Wall -fopenmp -o serial_k serial_k.c
 * Run  :  ./a.out <#of data generated> <# of cluster> <# of threads>
 * Output:   Time for k means and result
 *
 *  Minghao Sun
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <time.h>

typedef struct{
	double x;
	double y;
}Point;

double threshold=0.0001;	//k_mean converege threshold
int cluster_time=0;				//iter number
int data_size;						//data size
int cluster_k;						// k value
Point* p;									// point data
Point* mean;							// k cluster center coordinate
int* cluster;							//classfier for date size
int flag=1;								//loop sign
int thread_count;         // number of thread.

void Initilize_Data();
void Initilize_Mean();
void UpdateCenter();
void UpdateCluster();
double getDistance(Point point1, Point point2);
void MeanCopy(Point* mean_new);
int isEqual(Point* p1,Point* p2);
void Print(Point* p1,int size,int number );

int main(int argc,char* argv[]){
	//command line input
	data_size=strtol( argv[1],NULL,10);
	cluster_k=strtol( argv[2],NULL,10);
  thread_count=strtol( argv[3],NULL,10);
	//initilize p mean and cluster
	p=malloc(data_size*sizeof(Point));
	mean=malloc(cluster_k*sizeof(Point));
	cluster=malloc(data_size*sizeof(int));
	//intilize data and cluster center
	Initilize_Data();
	Initilize_Mean();
	// Print(p,data_size,100);
	//compute k_means by update rule
	while(flag){
		UpdateCluster();
		UpdateCenter();
		cluster_time++;
	}
	return 0;
}
/*	Initilize_Data()
 *	Initilize data size is data_size in x-y coordinate.
 */

void Initilize_Data(){
	srand((unsigned)time(NULL));
	int i;
	for(i=0;i<data_size;i++){
		p[i].x=(rand()/(double)RAND_MAX);
		p[i].y=(rand()/(double)RAND_MAX);

	}
}
/*	Initilize_Mean()
 *	get the intilize center for k cluster.
 * 	we can random or just pick several points of data set.
 */
void Initilize_Mean(){
	int i;
	for(i=0;i<cluster_k;i++){
		mean[i].x=p[i].x;
		mean[i].y=p[i].y;
	}
}
/* UpdateCluster()
 * Based on the distance function to compute the closest center for each point.
 */
void UpdateCluster(){
	int i,j;
	double min;
	double distance[data_size][cluster_k];

  #pragma omp parallel num_threads(thread_count) default(none) \
	private(i, j, min) shared(data_size,cluster_k,p,mean,distance,cluster)
	{
      #pragma omp for
    	for(i=0;i<data_size;i++){
    		for(j=0;j<cluster_k;j++){
    			distance[i][j]=getDistance(p[i],mean[j]);
    		}
    		min=distance[i][0];

    		for(j=0;j<cluster_k;j++){
    			if (min>=distance[i][j]){
                    min = distance[i][j];
                    cluster[i] = j;
          }
    		}
    	  // printf("cluster is %d\n",cluster[i] );
    		 // printf("(%.3f,%.3f) \t in cluster-%d\n", p[i].x, p[i].y, cluster[i] + 1);
    	}

  }
}
/*
 *compute the distance of each two points
 */
double getDistance(Point point1, Point point2)
{
    double d;
    d = sqrt((point1.x - point2.x)*(point1.x - point2.x) + (point1.y - point2.y)*(point1.y - point2.y));
    return d;
}

/*
* updata center based on the new cluster array.
*/
void UpdateCenter(){
	int i,j;
  int* count=calloc(cluster_k,sizeof(int));
	double* x_sum=(double*)calloc(cluster_k,sizeof(double));
  double* y_sum=(double*)calloc(cluster_k,sizeof(double));
	Point* mean_new=malloc(cluster_k*sizeof(Point));

  #pragma omp parallel num_threads(thread_count)
	{
		int* count_temp=calloc(cluster_k,sizeof(int));
		double* x_sum_temp=(double*)calloc(cluster_k,sizeof(double));
		double* y_sum_temp=(double*)calloc(cluster_k,sizeof(double));
		#pragma omp for
    for(i=0;i<data_size;i++){
      x_sum_temp[cluster[i]]+=p[i].x;
      y_sum_temp[cluster[i]]+=p[i].y;
      count_temp[cluster[i]]++;
    }
		#pragma omp critical
		for(i=0;i<cluster_k;i++){
			x_sum[i]+=x_sum_temp[i];
			y_sum[i]+=y_sum_temp[i];
			count[i]+=count_temp[i];
		}
		free(x_sum_temp);
		free(y_sum_temp);
		free(count_temp);
  }


		for(j=0;j<cluster_k;j++){
      if(count[j]==0){
  			mean_new[j].x=(rand()/(double)RAND_MAX);
  			mean_new[j].y=(rand()/(double)RAND_MAX);
  			// printf("case count ==0 !!!!n" );
  		}else{
  			// printf("count is %d\n",count[j]);
  			mean_new[j].x=x_sum[j]/count[j];
  			mean_new[j].y=y_sum[j]/count[j];
  			// printf("mean x is %f\n",mean_new[j].x);
  			// printf("mean y is %f\n",mean_new[j].y);
  		}
		}



	// printf("is equal result is %d \n",isEqual(mean_new,mean) );
	// Print(mean,cluster_k,cluster_time);
	// Print(mean_new,cluster_k,cluster_time);
	if(isEqual(mean_new,mean)==1){
		flag=0;
		printf("equal now.\n");
	}else {
		MeanCopy(mean_new);
	}

}
/*
 * copy center
 */
void MeanCopy(Point* mean_new){
	int j;
	for(j=0;j<cluster_k;j++){
		mean[j].x=mean_new[j].x;
		mean[j].y=mean_new[j].y;
	}
}
/*
 * Judge two center is close enough
 */
int isEqual(Point* p1,Point* p2){
	int i;
	for(i=0;i<cluster_k;i++){
		if((fabs(p1[i].x-p2[i].x)>threshold)||(fabs(p1[i].y-p2[i].y)>threshold)) {
			return 0;
		}
	}
	return 1;
}
/*
 * print all point data set with data size and iter number.
 */
void Print(Point* p1,int size,int number ){
	int i;
	for (i = 0; i < size; i++) {
		 printf("(%.5f,%.5f) \t in iter %d\n", p1[i].x, p1[i].y,number);
	}

}
