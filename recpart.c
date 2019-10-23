#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <strings.h>//to use "bzero"
#include <time.h>//to estimate the runing time

#include "partition.h"
#include "struct.h"

#define NLINKS 100000000 //maximum number of edges of the input graph: used for memory allocation, will increase if needed. //NOT USED IN THE CURRENT VERSION
#define NNODES 10000000 //maximum number of nodes in the input graph: used for memory allocation, will increase if needed

//compute the maximum of three unsigned long
inline unsigned long max3(unsigned long a,unsigned long b,unsigned long c){
	a=(a>b) ? a : b;
	return (a>c) ? a : c;
}

//reading the edgelist from file
//NOT USED IN THE CURRENT VERSION
edgelist* readedgelist(char* input){
	unsigned long long e1=NLINKS;
	edgelist *el=malloc(sizeof(edgelist));
	FILE *file;

	el->n=0;
	el->e=0;
	file=fopen(input,"r");
	el->edges=malloc(e1*sizeof(edge));
	while (fscanf(file,"%lu %lu", &(el->edges[el->e].s), &(el->edges[el->e].t))==2) {
		el->n=max3(el->n,el->edges[el->e].s,el->edges[el->e].t);
		if (++(el->e)==e1) {
			e1+=NLINKS;
			el->edges=realloc(el->edges,e1*sizeof(edge));
		}
	}
	fclose(file);
	el->n++;

	el->edges=realloc(el->edges,el->e*sizeof(edge));

	return el;
}

//freeing memory
//NOT USED IN THE CURRENT VERSION
void free_edgelist(edgelist *el){
	free(el->edges);
	free(el);
}

//building the adjacency matrix
//NOT USED IN THE CURRENT VERSION
adjlist* mkadjlist(edgelist* el){
	unsigned long long i;
	unsigned long u,v;
	unsigned long *d=calloc(el->n,sizeof(unsigned long));
	adjlist *g=malloc(sizeof(adjlist));

	g->n=el->n;
	g->e=el->e;

	for (i=0;i<el->e;i++) {
		d[el->edges[i].s]++;
		d[el->edges[i].t]++;
	}

	g->cd=malloc((g->n+1)*sizeof(unsigned long long));
	g->cd[0]=0;
	for (i=1;i<g->n+1;i++) {
		g->cd[i]=g->cd[i-1]+d[i-1];
		d[i-1]=0;
	}

	g->adj=malloc(2*g->e*sizeof(unsigned long));
	for (i=0;i<g->e;i++) {
		u=el->edges[i].s;
		v=el->edges[i].t;
		g->adj[ g->cd[u] + d[u]++ ]=v;
		g->adj[ g->cd[v] + d[v]++ ]=u;
	}

	g->weights = NULL;
	g->totalWeight = 2*g->e;
	g->map=NULL;

	free(d);
	free_edgelist(el);

	return g;
}

//reading the list of edges and building the adjacency array
adjlist* readadjlist(char* input){
	unsigned long n1=NNODES,n2,u,v,i;
	unsigned long *d=calloc(n1,sizeof(unsigned long));
	adjlist *g=malloc(sizeof(adjlist));
	FILE *file;

	g->n=0;
	g->e=0;
	file=fopen(input,"r");//first reading to compute the degrees
	while (fscanf(file,"%lu %lu", &u, &v)==2) {
		g->e++;
		g->n=max3(g->n,u,v);
		if (g->n+1>=n1) {
			n2=g->n+NNODES;
			d=realloc(d,n2*sizeof(unsigned long));
			bzero(d+n1,(n2-n1)*sizeof(unsigned long));
			n1=n2;
		}
		d[u]++;
		d[v]++;
	}
	fclose(file);

	g->n++;
	d=realloc(d,g->n*sizeof(unsigned long));

	g->cd=malloc((g->n+1)*sizeof(unsigned long long));
	g->cd[0]=0;
	for (i=1;i<g->n+1;i++) {
		g->cd[i]=g->cd[i-1]+d[i-1];
		d[i-1]=0;
	}

	g->adj=malloc(2*g->e*sizeof(unsigned long));

	file=fopen(input,"r");//secong reading to fill the adjlist
	while (fscanf(file,"%lu %lu", &u, &v)==2) {
		g->adj[ g->cd[u] + d[u]++ ]=v;
		g->adj[ g->cd[v] + d[v]++ ]=u;
	}
	fclose(file);

	g->weights = NULL;
	g->totalWeight = 2*g->e;
	g->map=NULL;

	free(d);

	return g;
}

//freeing memory
void free_adjlist(adjlist *g){
	free(g->cd);
	free(g->adj);
	free(g->weights);
	free(g->map);
	free(g);
}

//Make the nlab subgraphs of graph g using the labels "lab"
adjlist** mkkids(adjlist* g, unsigned long* lab, unsigned long nlab){
	unsigned long i,li,k;
	unsigned long long j;
	static unsigned long *new=NULL;

	if (new==NULL){
		new=malloc(g->n*sizeof(unsigned long));
	}

	adjlist** clust=malloc(nlab*sizeof(adjlist*));
	for (i=0;i<nlab;i++){
		clust[i]=malloc(sizeof(adjlist));
		clust[i]->e=0;
		clust[i]->n=0;
	}

	for (i=0;i<g->n;i++){
		(clust[lab[i]]->n)++;
	}

	for (i=0;i<nlab;i++){
		clust[i]->map=malloc(clust[i]->n*sizeof(unsigned long));
		clust[i]->cd=malloc((clust[i]->n+1)*sizeof(unsigned long long));
		clust[i]->n=0;
	}

	for (i=0;i<g->n;i++){
		li=lab[i];
		clust[li]->map[clust[li]->n]=(g->map==NULL)?i:g->map[i];
		new[i]=(clust[li]->n)++;
		for (j=g->cd[i];j<g->cd[i+1];j++) {
			k=g->adj[j];
			if (li==lab[k]){
				clust[li]->e++;
			}
		}
	}

	for (i=0;i<nlab;i++){
		clust[i]->adj=malloc(clust[i]->e*sizeof(unsigned long));
		clust[i]->e=0;
	}

	for (i=0;i<g->n;i++){
		li=lab[i];
		clust[li]->cd[new[i]]=clust[li]->e;
		for (j=g->cd[i];j<g->cd[i+1];j++) {
			k=g->adj[j];
			if (li==lab[k]){
				clust[li]->adj[clust[li]->e++]=new[k];
			}
		}
	}

	for (i=0;i<nlab;i++){
		clust[i]->cd[clust[i]->n]=clust[i]->e;
		clust[i]->e/=2;
		clust[i]->weights = NULL;
		clust[i]->totalWeight = 2*g->e;
	}

	return clust;
}

//recursive function
void recurs(partition part, adjlist* g, unsigned h, FILE* file){
	time_t t0,t1,t2;
	unsigned long nlab;
	unsigned long i;
	static unsigned long *lab=NULL;
	if (lab==NULL){
		lab=malloc(g->n*sizeof(unsigned long));
	}

	if (h==0){
		t0=time(NULL);
	}


	if (g->e==0){
		fprintf(file,"%u 1 %lu",h,g->n);
		for (i=0;i<g->n;i++){
			fprintf(file," %lu",g->map[i]);
		}
		fprintf(file,"\n");
		free_adjlist(g);
	}
	else{
		nlab=part(g,lab);
		if (h==0) {
			t1=time(NULL);
			printf("First level partition computed: %lu parts\n", nlab);
			printf("- Time to compute first level partition = %ldh%ldm%lds\n",(t1-t0)/3600,((t1-t0)%3600)/60,((t1-t0)%60));
		}
		  
		if (nlab==1){
			fprintf(file,"%u 1 %lu",h,g->n);
			for (i=0;i<g->n;i++){
				fprintf(file," %lu",g->map[i]);
			}
			fprintf(file,"\n");
			free_adjlist(g);
		}
		else{
			adjlist** clust=mkkids(g,lab,nlab);
			if (h==0) {
				t2=time(NULL);
				printf("- Time to build first level subgraphs = %ldh%ldm%lds\n",(t2-t1)/3600,((t2-t1)%3600)/60,((t2-t1)%60));
			}
			fprintf(file,"%u %lu\n",h,nlab);
			free_adjlist(g);
			for (i=0;i<nlab;i++){
				recurs(part, clust[i],h+1,file);
			}
			free(clust);
		}
	}
}


//main function
int main(int argc,char** argv){
	adjlist* g;
	partition part;

	time_t t0=time(NULL),t1,t2;
	srand(time(NULL));

	if (argc==3)
		part=choose_partition("1");
	else if (argc==4)
		part=choose_partition(argv[3]);
	else{
		printf("Command line arguments are not valid.\n");
		exit(1);
	}

	printf("Reading edgelist from file %s and building adjacency array\n",argv[1]);
	g=readadjlist(argv[1]);
	printf("Number of nodes: %lu\n",g->n);
	printf("Number of edges: %llu\n",g->e);

	/*
	//using more memory but reading the input text file only once
	edgelist* el;
	printf("Reading edgelist from file %s\n",argv[1]);
	el=readedgelist(argv[1]);

	printf("Number of nodes: %lu\n",el->n);
	printf("Number of edges: %llu\n",el->e);

	t1=time(NULL);
	printf("- Time = %ldh%ldm%lds\n",(t1-t0)/3600,((t1-t0)%3600)/60,((t1-t0)%60));

	printf("Building adjacency array\n");
	g=mkadjlist(el);
	*/

	t1=time(NULL);
	printf("- Time to load the graph = %ldh%ldm%lds\n",(t1-t0)/3600,((t1-t0)%3600)/60,((t1-t0)%60));

	printf("Starting recursive bisections\n");
	printf("Prints result in file %s\n",argv[2]);
	FILE* file=fopen(argv[2],"w");
	recurs(part, g, 0, file);
	fclose(file);

	t2=time(NULL);
	printf("- Time to compute the hierarchy = %ldh%ldm%lds\n",(t2-t1)/3600,((t2-t1)%3600)/60,((t2-t1)%60));
	printf("- Overall time = %ldh%ldm%lds\n",(t2-t0)/3600,((t2-t0)%3600)/60,((t2-t0)%60));

	return 0;
}

