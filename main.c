#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <strings.h>//to use "bzero"
#include <time.h>//to estimate the runing time

#include "bisection.h"
#include "struct.h"

#define NLINKS 100000000 //maximum number of edges for memory allocation, will increase if needed

//compute the maximum of three unsigned long
inline unsigned long max3(unsigned long a,unsigned long b,unsigned long c){
	a=(a>b) ? a : b;
	return (a>c) ? a : c;
}

//reading the edgelist from file
adjlist* readedgelist(char* input){
	unsigned long e1=NLINKS;
	adjlist *g=malloc(sizeof(adjlist));
	FILE *file;

	g->n=0;
	g->e=0;
	file=fopen(input,"r");
	g->edges=malloc(e1*sizeof(edge));
	while (fscanf(file,"%lu %lu", &(g->edges[g->e].s), &(g->edges[g->e].t))==2) {
		g->n=max3(g->n,g->edges[g->e].s,g->edges[g->e].t);
		if (g->e++==e1) {
			e1+=NLINKS;
			g->edges=realloc(g->edges,e1*sizeof(edge));
		}
	}
	fclose(file);
	g->n++;

	g->edges=realloc(g->edges,g->e*sizeof(edge));

	return g;
}


//assign new contiguous labels from 0 to n-1
unsigned long* relabel(adjlist* g){
	unsigned long i;
	unsigned long *new=malloc(g->n*sizeof(unsigned long));
	bool *b=calloc(g->n,sizeof(bool));

	g->map=malloc(g->n*sizeof(unsigned long));
	g->n=0;///////////////
	for (i=0;i<g->e;i++) {
		if (b[g->edges[i].s]==0){
			b[g->edges[i].s]=1;
			g->map[g->n]=g->edges[i].s;
			new[g->edges[i].s]=g->n++;
		}
		if (b[g->edges[i].t]==0){
			b[g->edges[i].t]=1;
			g->map[g->n]=g->edges[i].t;
			new[g->edges[i].t]=g->n++;
		}
	}
	g->map=realloc(g->map,g->n*sizeof(unsigned long));
	return new;
}


//building the adjacency matrix
void mkadjlist(adjlist* g,unsigned long* new){
	unsigned long i,u,v;
	unsigned long *d=calloc(g->n,sizeof(unsigned long));

	for (i=0;i<g->e;i++) {
		g->edges[i].s=new[g->edges[i].s];
		g->edges[i].t=new[g->edges[i].t];
	}

	for (i=0;i<g->e;i++) {
		d[g->edges[i].s]++;
		d[g->edges[i].t]++;
	}

	g->cd=malloc((g->n+1)*sizeof(unsigned long));
	g->cd[0]=0;
	for (i=1;i<g->n+1;i++) {
		g->cd[i]=g->cd[i-1]+d[i-1];
		d[i-1]=0;
	}

	g->adj=malloc(2*g->e*sizeof(unsigned long));

	for (i=0;i<g->e;i++) {
		u=g->edges[i].s;
		v=g->edges[i].t;
		g->adj[ g->cd[u] + d[u]++ ]=v;
		g->adj[ g->cd[v] + d[v]++ ]=u;
	}

	free(d);
}



//freeing memory
void free_adjlist(adjlist *g){
	free(g->edges);
	free(g->cd);
	free(g->adj);
	free(g->map);
	free(g);
}


//Make the 2 subgraphs of graph g using the labels "lab"
adjlist** mkkids(adjlist* g, unsigned* lab, unsigned nlab){
	unsigned long i,j;

	adjlist** clust=malloc(nlab*sizeof(adjlist*));
	for (i=0;i<nlab;i++){
		clust[i]=malloc(sizeof(adjlist));
		clust[i]->edges=malloc(NLINKS*sizeof(edge));
		clust[i]->e=0;
		clust[i]->emax=NLINKS;
	}

	for (i=0;i<g->e;i++) {
		if (lab[g->edges[i].s]==lab[g->edges[i].t]){
			j=lab[g->edges[i].s];
			clust[j]->edges[clust[j]->e]=g->edges[i];
			if (clust[j]->e++==clust[j]->emax) {
				clust[j]->emax+=NLINKS;
				clust[j]->edges=realloc(clust[j]->edges,clust[j]->emax*sizeof(edge));
			}
		}
	}
	for (i=0;i<nlab;i++){
		clust[i]->edges=realloc(clust[i]->edges,clust[i]->e*sizeof(edge));
		clust[i]->emax=clust[i]->e;
		clust[i]->n=0;
	}

	for (i=0;i<g->n;i++){
		(clust[lab[i]]->n)++;
	}

	unsigned long *new=malloc(g->n*sizeof(unsigned long));
	for (i=0;i<nlab;i++){
		clust[i]->map=malloc(clust[i]->n*sizeof(unsigned long));
		clust[i]->n=0;
	}

	for (i=0;i<g->n;i++){
		j=lab[i];
		clust[j]->map[clust[j]->n]=g->map[i];
		new[i]=(clust[j]->n)++;
	}

	for (i=0;i<nlab;i++){
		mkadjlist(clust[i],new);
	}
	free(new);
	return clust;
}

//printing line (corresponding to one tree's node and associated set of nodes) in file
void printres(unsigned long n, unsigned long *map, char* output){
	unsigned long u;
	FILE* file=fopen(output,"w");
	for (u=0;u<n;u++){
		fprintf(file,"%lu %lu\n",u,map[u]);
	}
	fclose(file);
}

//recursive function
void recurs(bisection bisec, adjlist* g, unsigned h, FILE* file){
	unsigned nlab,i;
	if (g->e==0){
		fprintf(file,"%u 1 %lu",h,g->n);
		for (i=0;i<g->n;i++){
			fprintf(file," %lu",g->map[i]);
		}
		fprintf(file,"\n");
		free_adjlist(g);
	}
	else{
		unsigned *lab=malloc(g->n*sizeof(unsigned));
		nlab=bisec(g,lab);
		if (nlab==1){
			fprintf(file,"%u 1 %lu",h,g->n);
			for (i=0;i<g->n;i++){
				fprintf(file," %lu",g->map[i]);
			}
			fprintf(file,"\n");
			free_adjlist(g);
			free(lab);
		}
		else{
			adjlist** clust=mkkids(g,lab,nlab);
			fprintf(file,"%u %u\n",h,nlab);
			free_adjlist(g);
			free(lab);
			for (i=0;i<nlab;i++){
				recurs(bisec, clust[i],h+1,file);
			}
		}
	}
}

//main function
int main(int argc,char** argv){
	adjlist* g;
	bisection bisec;
	unsigned long n;
	unsigned long *map,*new;

	time_t t1=time(NULL),t2;
	srand(time(NULL));

	if (argc==3)
		bisec=choosebisection("0");
	else if (argc==4)
		bisec=choosebisection(argv[3]);
	else{
		printf("Command line arguments are not valid.\n");
		exit(1);
	}

	printf("Reading edgelist from file %s\n",argv[1]);
	g=readedgelist(argv[1]);

	printf("Building adjacency array\n");
	new=relabel(g);
	mkadjlist(g,new);
	free(new);
	n=g->n;

	printf("Number of nodes: %lu\n",g->n);
	printf("Number of edges: %lu\n",g->e);

	printf("Starting recursive bisections\n");
	printf("Prints resulting order in file %s\n",argv[2]);
	FILE* file=fopen(argv[2],"w");
	recurs(bisec, g, 0, file);
	fclose(file);
	t2=time(NULL);

	printf("- Overall time = %ldh%ldm%lds\n",(t2-t1)/3600,((t2-t1)%3600)/60,((t2-t1)%60));

	return 0;
}

