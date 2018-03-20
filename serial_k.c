
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct{
	double x;
	double y;
}Point;

double threshold=0.0001;
int cluster_time=0;
int data_size;
int cluster_k;
Point* p;
Point* mean;
int* cluster;
int flag=1;

void Initilize_Data();
void Initilize_Mean();
void UpdateCenter();
void UpdateCluster();
double getDistance(Point point1, Point point2);
void MeanCopy(Point* mean_new);
int isEqual(Point* p1,Point* p2);
void Print(Point* p1,int size,int number );
int main(int argc,char* argv[]){
	data_size=strtol( argv[1],NULL,10);
	cluster_k=strtol( argv[2],NULL,10);

	p=malloc(data_size*sizeof(Point));
	mean=malloc(cluster_k*sizeof(Point));
	cluster=malloc(data_size*sizeof(int));

	Initilize_Data();
	Initilize_Mean();
	Print(p,data_size,100);
	while(flag){
		UpdateCluster();
		UpdateCenter();
		cluster_time++;
	}
	return 0;
}

void Initilize_Data(){
	srand((unsigned)time(NULL));
	int i;
	for(i=0;i<data_size;i++){
		p[i].x=(rand()/(double)RAND_MAX);
		p[i].y=(rand()/(double)RAND_MAX);

	}
}

void Initilize_Mean(){
	int i;
	for(i=0;i<cluster_k;i++){
		mean[i].x=p[i].x;
		mean[i].y=p[i].y;
	}
}

void UpdateCluster(){
	int i,j;
	double min;
	double distance[data_size][cluster_k];
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

double getDistance(Point point1, Point point2)
{
    double d;
    d = sqrt((point1.x - point2.x)*(point1.x - point2.x) + (point1.y - point2.y)*(point1.y - point2.y));
    return d;
}


void UpdateCenter(){
	Point temp;
	int i,j,count;
	Point* mean_new=malloc(cluster_k*sizeof(Point));
	for(j=0;j<cluster_k;j++){
		temp.x=0.0;
		temp.y=0.0;
		count=0;
		for(i=0;i<data_size;i++){
			if (cluster[i] == j){
                temp.x += p[i].x;
                temp.y += p[i].y;
                count++;
            }
		}
		if(count==0){
			mean_new[j].x=(rand()/(double)RAND_MAX);
			mean_new[j].y=(rand()/(double)RAND_MAX);
			printf("case count ==0 !!!!n" );
		}else{
			printf("count is %d\n",count);
			mean_new[j].x=temp.x/count;
			mean_new[j].y=temp.y/count;
			printf("mean x is %f\n",mean_new[j].x);
			printf("mean y is %f\n",mean_new[j].y);
		}
	}
	printf("is equal result is %d \n",isEqual(mean_new,mean) );
	Print(mean,cluster_k,cluster_time);
	Print(mean_new,cluster_k,cluster_time);
	if(isEqual(mean_new,mean)==1){
		flag=0;
		printf("equal now.\n");
	}else {
		MeanCopy(mean_new);
	}

}

void MeanCopy(Point* mean_new){
	int j;
	for(j=0;j<cluster_k;j++){
		mean[j].x=mean_new[j].x;
		mean[j].y=mean_new[j].y;
	}
}

int isEqual(Point* p1,Point* p2){
	int i;
	for(i=0;i<cluster_k;i++){
		if((fabs(p1[i].x-p2[i].x)>threshold)||(fabs(p1[i].y-p2[i].y)>threshold)) {
			return 0;
		}
	}
	return 1;
}
void Print(Point* p1,int size,int number ){
	int i;
	for (i = 0; i < size; i++) {
		 printf("(%.5f,%.5f) \t in iter %d\n", p1[i].x, p1[i].y,number);
	}

}
