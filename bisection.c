#include "bisection.h"

#define K 5

//generating n random labels (boolean values)
unsigned init(adjlist *g,unsigned *lab){
	unsigned long i,n=g->n;
	unsigned nlab=(K>n)?n:K;

	//random side for each node:
	for (i=0;i<n;i++){
		lab[i]=rand()%nlab;
	}

	return nlab;
}

bisection choosebisection(char *c){
	printf("Chosen bisection algorithm: ");
	if (strcmp(c,"0")==0){
		printf("Random Bisection\n");
		return init;
	}
	//if (strcmp(c,"1")==0){
	//	printf("Greedy Sparsest Cut\n");
	//	return greedySparsestcut;
	//}
	printf("unknown\n");
	exit(1);
}


