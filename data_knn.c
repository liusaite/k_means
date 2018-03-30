/* File:  serial_k.c
 * Comple: gcc -g -Wall -o data_knn.out data_knn.c -lm
 * Run  :  ./data_knn.out <#of data generated> <# of cluster>
 * Output:   Time for k means and result
 *
 *  Minghao Sun
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>


const char* split=",";
#define data_size 47
#define numattributes 35
#define cluster_k 4
char name[data_size][100];
double parameters[data_size][numattributes];

double threshold=0.01;	//k_mean converege threshold
int cluster_time=0;				//iter number
// int data_size;						//data size
				// k value
// Point* p;									// point data
double mean[cluster_k][numattributes];							// k cluster center coordinate
int* cluster;							//classfier for date size
int flag=1;								//loop sign

void read_ints (const char* file_name);
// void Initilize_Data();
void Initilize_Mean();
void UpdateCenter();
void UpdateCluster();
double getDistance(double* point1, double* point2);
void MeanCopy(double (*mean_new)[numattributes]);
int isEqual(double (*mean_new)[numattributes]);
double PrintAccuracy();
// void Print(Point* p1,int size,int number );

int main(int argc,char* argv[]){
  // read_ints("data.txt");
  read_ints("small_soybean.txt");
  //
  // int i,j;
  // for (j=0; j<data_size;j++){
  // for (i=0;i<numattributes;i++){
  //   printf("%2.2lf ",parameters[j][i] );
  // }
  //   printf("\n" );
  // }
	// //command line input
	// data_size=strtol( argv[1],NULL,10);
	// cluster_k=strtol( argv[2],NULL,10);
  //
  //
  //
	// //initilize p mean and cluster
	// p=malloc(data_size*sizeof(Point));
	// mean=malloc(cluster_k*sizeof(Point));

  double time_average=0.0;
  double accuracy=0.0;
  int count=100;
  int i;
  for(i=0;i<count;i++){
    cluster=malloc(data_size*sizeof(int));
    Initilize_Mean();
    flag=1;
    cluster_time=0;
    clock_t cpu_begin=clock();
	// Print(p,data_size,10000);
	//compute k_means by update rule
  	while(flag){
  		UpdateCluster();
  		UpdateCenter();
      // printf("cluster %d\n",cluster_time );
  		cluster_time++;
  	}
  	clock_t cpu_end=clock();
    time_average+=(double)(cpu_end-cpu_begin)*1000.0/CLOCKS_PER_SEC;
  	// printf("Run time for serial algorithm is %f ms.\n",(double)(cpu_end-cpu_begin)*1000.0/CLOCKS_PER_SEC);
    accuracy+=PrintAccuracy();
  }
  printf("Overall accuracy is %2.2lf\n",accuracy/count );
  printf("Overall runtime is %2.2lf\n",time_average/count );
	return 0;
}
/*	Initilize_Data()
 *	Initilize data size is data_size in x-y coordinate.
 */

// void Initilize_Data(){
// 	srand((unsigned)time(NULL));
// 	int i;
// 	for(i=0;i<data_size;i++){
// 		p[i].x=(rand()/(double)RAND_MAX);
// 		p[i].y=(rand()/(double)RAND_MAX);
//
// 	}
// }
/*	Initilize_Mean()
 *	get the intilize center for k cluster.
 * 	we can random or just pick several points of data set.
 */
void Initilize_Mean(){
	int count=0;
  int i;
  int temp;
  srand((unsigned)time(NULL));
  while(count<=cluster_k){
    temp=rand()%data_size;
    // printf("temp is %d\n",temp );
    for(i=0;i<numattributes;i++){
      mean[count][i]=parameters[temp][i];
      // printf("%2.2lf ",parameters[temp][i] );
    }
    count++;
    // printf("\n" );
  }
}
/* UpdateCluster()
 * Based on the distance function to compute the closest center for each point.
 */
void UpdateCluster(){
	int i,j;
	double min;
	double distance[data_size][cluster_k];
	for(i=0;i<data_size;i++){
		for(j=0;j<cluster_k;j++){
			distance[i][j]=getDistance(parameters[i],mean[j]);
		}
		min=distance[i][0];
		for(j=0;j<cluster_k;j++){
			if (min>=distance[i][j]){
                min = distance[i][j];
                cluster[i] = j;
            }
            // printf("%lf  ",distance[i][j] );
		}
    // printf("\n" );
    // for(i=0;i<data_size;i++){
    //   for(j=0;j<cluster_k;j++){
        // printf("%lf \n",distance[i][j] );
    //   }
    // }
	  // printf("%d cluster is %d\n",i,cluster[i] );
		 // printf("(%.3f,%.3f) \t in cluster-%d\n", p[i].x, p[i].y, cluster[i] + 1);
	}
}
/*
 *compute the distance of each two points
 */
double getDistance(double* point1, double* point2)
{
    double d=0.0;
    int i;
    for(i=0;i<numattributes;i++){
      // printf("%2.2lf %2.2lf ",point1[i],point2[i]);
      if(point1[i]!=-1&&point2[i]!=-1){
          d+=(point1[i]-point2[i])*(point1[i]-point2[i]);
      }
      // printf("%2.2lf\n",d );
    }
    return sqrt(d);
}

/*
* updata center based on the new cluster array.
*/
void UpdateCenter(){
	int i,j,k;
  int* count=(int*)calloc(cluster_k,sizeof(int));
  double parameter_sum[cluster_k][numattributes];
	// Point* mean_new=malloc(cluster_k*sizeof(Point));
  double mean_new[cluster_k][numattributes];
  for(i=0;i<data_size;i++){
    for(j=0;j<numattributes;j++){
      if(parameters[i][j]!=-1){
        parameter_sum[cluster[i]][j]+=parameters[i][j];
      }
    }
      count[cluster[i]]++;
      // printf("%d\n",cluster[i]);
  }

  int temp;
  for(i=0;i<cluster_k;i++){
    if (count[i]==0) {
    			// printf("case %d count ==0 !!!!\n",i );
          temp=rand()%data_size;
    }
    for(j=0;j<numattributes;j++){
      if(count[i]==0){
        for(k=0;k<numattributes;k++){
  			     mean_new[i][k]=parameters[temp][k];
        }
  			// printf("case count ==0 !!!!n" );
  		}else{
  			// printf("count is %d\n",count[i]);
  			mean_new[i][j]=parameter_sum[i][j]/count[i];
  			mean_new[i][j]=parameter_sum[i][j]/count[i];
  		}
  			// printf("%2.2lf ",mean_new[i][j]);
    }
    // printf("\n" );
  }
	// printf("is equal result is %d \n",isEqual(mean_new,mean) );
	// Print(mean,cluster_k,cluster_time);
	// Print(mean_new,cluster_k,cluster_time);

	if(isEqual(mean_new)==1){
		flag=0;
		// printf("equal now.\n");
	}else {
		MeanCopy(mean_new);
	}

}
/*
 * copy center
 */
void MeanCopy(double (*mean_new)[numattributes]){
	int i,j;
  for(i=0;i<cluster_k;i++){
    for(j=0;j<numattributes;j++){
		    mean[i][j]=mean_new[i][j];
        // printf("%2.2lf ",mean[i][j] );
      }
      // printf("\n" );
	}
}
/*
 * Judge two center is close enough
 */
int isEqual(double (*mean_new)[numattributes]){
	int i,j;
  for(i=0;i<cluster_k;i++){
    for(j=0;j<numattributes;j++){
		    if((fabs(mean_new[i][j]-mean[i][j])>threshold)) {
			     return 0;
		     }
     }
	}
	return 1;
}
/*
 * print all point data set with data size and iter number.
 */
// void Print(Point* p1,int size,int number ){
// 	int i;
// 	for (i = 0; i < size; i++) {
// 		 printf("(%.5f,%.5f) \t in iter %d\n", p1[i].x, p1[i].y,number);
// 	}
//
// }



void read_ints (const char* file_name)
{
  char line[1000];
  FILE* file = fopen (file_name, "r+");
  char* p;
  int i,j;
  double temp;
  i=0;
  while (fgets(line,1000, file) != NULL)
  {
    j=0;
    p=strtok(line,split);
    // strcpy(name[i],p);
    while(p!=NULL){
      if(p[0]!='?'){
        sscanf(p,"%lf",&temp);
        parameters[i][j]=temp;
        // printf("%2.2lf ", parameters[i][j]);
      }else{
        parameters[i][j]=-1;
        // printf("%2.2lf ", parameters[i][j]);
      }
      p=strtok(NULL,split);
      j++;
    }
    // printf("\n" );
    i++;
    //   /* Where do the numbers end... */
    //   size_t numbers_end = strspn(line, "1234567890. \t");
    //
    //   /* And get the name */
    //   char *name = line + numbers_end;

      /* Do something with the numbers and the name */
  }
  fclose (file);
}
double PrintAccuracy(){
  int diff=0;
  int i,j;
  // for(i=0;i<data_size;i++){
  //   printf("%d ",cluster[i] );
  // }
  // printf("\n" );
  int four[cluster_k]={10,10,10,17};
  for(i=0;i<cluster_k;i++){

      int temp[cluster_k]={0,0,0,0};
      // printf("%d %d %d %d\n", temp[0],temp[1],temp[2],temp[3]);
      for(j=0;j<four[i];j++){
        temp[cluster[j+i*10]]++;
      }
      // printf("four size is %d\n", four[i]);
      int class=0;
      for(j=0;j<cluster_k;j++){
        if(temp[class]<temp[j]) class=j;
      }
      // printf("class is %d and temp is %d \n",class,temp[class] );

      diff+=(four[i]-temp[class]);
  }
  // printf("diff is %d\n",diff );
  return 1.0*diff/data_size;
}
