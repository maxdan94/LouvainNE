#include "partition.h"

#define K 5

//generating n random labels (boolean values)
unsigned long init(adjlist *g,unsigned long *lab){
	unsigned long i,n=g->n;
	unsigned long nlab=(K>n)?n:K;

	//random side for each node:
	for (i=0;i<n;i++){
		lab[i]=rand()%nlab;
	}

	return nlab;
}

partition choose_partition(char *c){
	printf("Chosen partition algorithm: ");
	if (strcmp(c,"0")==0){
		printf("Random partition\n");
		return init;
	}
	//if (strcmp(c,"1")==0){
	//	printf("Greedy Sparsest Cut\n");
	//	return greedySparsestcut;
	//}
	printf("unknown\n");
	exit(1);
}


