#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <strings.h>//to use "bzero"

#define NNODES 100000000 //maximum number of nodes in the input graph: used for memory allocation, will increase if needed

//compute the maximum of three unsigned long
inline unsigned long max3(unsigned long a,unsigned long b,unsigned long c){
	a=(a>b) ? a : b;
	return (a>c) ? a : c;
}

//main function
int main(int argc,char** argv){
	unsigned long u,v,n=0,nmax=NNODES,i=0;
	unsigned long long e=0;
	unsigned long *map=malloc(nmax*sizeof(unsigned long));
	bool *tab=calloc(nmax,sizeof(bool));

	printf("Reading edgelist from file: %s\n",argv[1]);
	FILE *f=fopen(argv[1],"r");
	printf("Writing renumbered edgelist to file: %s\n",argv[2]);
	FILE *g=fopen(argv[2],"w");
	printf("Writing correspondance (new label - old label) to file: %s\n",argv[3]);
	FILE *h=fopen(argv[3],"w");


	while (fscanf(f,"%lu %lu", &u, &v)==2) {
		e++;
		n=max3(n,u+1,v+1);
		if (n>nmax){
			map=realloc(map,(n+NNODES)*sizeof(unsigned long));
			tab=realloc(tab,(n+NNODES)*sizeof(bool));
			bzero(tab+nmax,(n+NNODES-nmax)*sizeof(bool));
			nmax=n+NNODES;
		}
		if (tab[u]==0){
			tab[u]=1;
			map[u]=i++;
			fprintf(h,"%lu %lu\n", map[u], u);
		}
		if (tab[v]==0){
			tab[v]=1;
			map[v]=i++;
			fprintf(h,"%lu %lu\n", map[v], v);
		}
		fprintf(g,"%lu %lu\n", map[u], map[v]);
	}

	fclose(f);
	fclose(g);
	fclose(h);

	printf("Number of nodes (counting 0 degree nodes): %lu\n",n);
	printf("Number of nodes of degree 1 or more: %lu\n",i);
	printf("Number of edges: %llu\n",e);

	free(map);
	free(tab);

	return 0;
}

