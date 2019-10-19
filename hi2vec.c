/*
Info:
Feel free to use these lines as you wish.
Conver a hierachy into vectors

To compile:
"gcc hi2vec.c -O3 -o hi2vec -lm".

To execute:
"./hi2vec k a hierarchy.txt vec.txt".
- "hierarchy.txt" should contain the hierarchy (in the format of the output of the recpart program).
- k is the number of dimensions
- a is the damping factor
- "vec.txt" will contain the vectors, node id follow by k coordinates on each line.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define HMAX 100000 //upper-bound on the depth of the tree. Has to be increased manually in case of segfault! Usually, the deapth of the hierarchy is around 10...

inline double rand1(){//random double between -1 and 1
	return 2* ( (rand()+1.) / (RAND_MAX+1.)) - 1;
}

void* recvec(FILE* file1, FILE* file2, unsigned k, double a, double *vec){
	unsigned h;
	unsigned long i,j,u,n,c;
	double ah;

	if (fscanf(file1,"%u %lu", &h, &c)!=2){
		printf("file reading error 1\n");
	}
	ah=pow(a,h);
	if (vec==NULL){
		vec=malloc(HMAX*k*sizeof(double));
		bzero(vec,k*sizeof(double));
	}
	if (c==1){//print vector to file
		if (fscanf(file1,"%lu", &n)!=1){
			printf("file reading error 2\n");
		}
		for (i=0;i<n;i++){
			if (fscanf(file1,"%lu",&u)!=1){
				printf("file reading error 3\n");
			}
			fprintf(file2,"%lu",u);
			for (j=0;j<k;j++){
				fprintf(file2," %le",vec[h*k+j]+rand1()*ah);
			}
			fprintf(file2,"\n");
		}
	}
	else{
		for (i=0;i<c;i++){
			for (j=0;j<k;j++){
				vec[(h+1)*k+j]=vec[h*k+j]+rand1()*ah;
			}
			recvec(file1, file2, k, a, vec);
		}
	}
}

int main(int argc,char** argv){
	FILE *file1,*file2;
	unsigned k;
	double a;

	time_t t1,t2;
	t1=time(NULL);

	srand(time(NULL));
	printf("Number of dimensions: %s\n",argv[1]);
	k=atoi(argv[1]);
	printf("Damping factor: %s\n",argv[2]);
	a=atof(argv[2]);

	printf("Reading hierarchy from file: %s\n",argv[3]);
	file1=fopen(argv[3],"r");
	printf("Writing vectors to file: %s\n",argv[4]);
	file2=fopen(argv[4],"w");

	recvec(file1, file2, k, a, NULL);
	
	fclose(file1);
	fclose(file2);

	t2=time(NULL);
	printf("- Overall Time = %ldh%ldm%lds\n",(t2-t1)/3600,((t2-t1)%3600)/60,((t2-t1)%60));

	return 0;
}


