#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <strings.h>//to use "bzero"
#include <time.h>//to estimate the runing time
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>

#include "partition.h"
#include "struct.h"

#define NLINKS 100000000 //maximum number of edges of the input graph: used for memory allocation, will increase if needed. //NOT USED IN THE CURRENT VERSION
#define NNODES 10000000 //maximum number of nodes in the input graph: used for memory allocation, will increase if needed
#define HMAX 100 //maximum depth of the tree: used for memory allocation, will increase if needed

//compute the maximum of three unsigned long
inline unsigned long max3(unsigned long a,unsigned long b,unsigned long c){
  a = (a > b) ? a : b;
  return (a > c) ? a : c;
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

//reading the list of edges and building the adjacency array
//NOT USED IN THE CURRENT VERSION
#define BUFFER_SIZE (16 * 1024)

int
read_two_integers(int fd, unsigned long *u, unsigned long *v) {
  static char buf[BUFFER_SIZE];
  static ssize_t how_many = 0;
  static int pos = 0;
  unsigned long node_number=0;
  int readu = 0;

  while (1) {
    while(pos < how_many) {
      if (buf[pos] == ' ') {
	*u = node_number;
	readu=1;
	node_number = 0;
	pos++;
      } else if (buf[pos] == '\n') {
	*v = node_number;
	node_number = 0;
	readu=0;
	pos++;
	return 2;
      } else {
	node_number = node_number * 10 + buf[pos] - '0';
	pos++;
      }
    }

    how_many = read (fd, buf, BUFFER_SIZE);
    pos = 0;
    if (how_many == 0) {
      if(readu==1) {
	*v = node_number;
	return 2;
      }	    
      return 0;;
    }
  } 
}

adjlist *
readadjlist_v2(char* input_filename){
  unsigned long n1 = NNODES, n2, u, v, i;
  unsigned long *d = calloc(n1,sizeof(unsigned long));
  adjlist *g = malloc(sizeof(adjlist));

  g->n = 0;
  g->e = 0;

  // first read of the file to compute degree of each node
  int fd = open(input_filename, O_RDONLY);
  posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL);
  while (read_two_integers(fd, &u, &v) == 2) {
    g->e++;
    g->n = max3(g->n, u, v);
    if (g->n + 1 >= n1) {
      n2 = g->n+NNODES;
      d = realloc(d, n2 * sizeof(unsigned long));
      bzero(d + n1, (n2 - n1) * sizeof(unsigned long));
      n1 = n2;
    }
    d[u]++;
    d[v]++;
  }
  close(fd);

  g->n++;
  d = realloc(d, g->n * sizeof(unsigned long));

  // computation of cumulative degrees
  g->cd = malloc((g->n + 1) * sizeof(unsigned long long));
  g->cd[0] = 0;
  for (i = 1; i < g->n + 1; i++) {
    g->cd[i] = g->cd[i - 1] + d[i - 1];
    d[i - 1] = 0;
  }

  g->adj = malloc(2 * g->e * sizeof(unsigned long));

  // second read to create adjlist
  fd = open(input_filename, O_RDONLY);
  posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL);
  while (read_two_integers(fd, &u, &v) == 2) {
    g->adj[g->cd[u] + d[u]] = v;
    d[u]++;
    g->adj[g->cd[v] + d[v]] = u;
    d[v]++;
  }
  close(fd);

  g->weights = NULL;
  g->totalWeight = 2*g->e;
  g->map = NULL;

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
//NOT USED IN THE CURRENT VERSION
adjlist** mkchildren(adjlist* g, unsigned long* lab, unsigned long nlab){
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
    clust[i]->totalWeight = 2*clust[i]->e;
  }

  return clust;
}


//Make the nlab subgraphs of graph g using the labels "lab". Make the subgraphs ne by one...
adjlist* mkchild(adjlist* g, unsigned long* lab, unsigned long nlab, unsigned h, unsigned long clab){
  unsigned long i,u,v,lu;
  unsigned long long j,k,tmp;

  static unsigned hmax=0;
  static unsigned long **nodes;
  static unsigned long **new;
  static unsigned long long **cd;
  static unsigned long long **e;
  static unsigned long *d;
  adjlist* sg;

  if (hmax==0){
    hmax=HMAX;
    nodes=malloc(HMAX*sizeof(unsigned long *));
    new=malloc(HMAX*sizeof(unsigned long *));
    cd=malloc(HMAX*sizeof(unsigned long long *));
    e=malloc(HMAX*sizeof(unsigned long long *));
  }
  if (h==hmax){
    hmax+=HMAX;
    nodes=realloc(nodes,hmax*sizeof(unsigned long *));
    new=realloc(new,hmax*sizeof(unsigned long *));
    cd=realloc(cd,hmax*sizeof(unsigned long long *));
    e=realloc(e,hmax*sizeof(unsigned long long *));
  }

  if (clab==0){
    d=calloc(nlab,sizeof(unsigned long));
    cd[h]=malloc((nlab+1)*sizeof(unsigned long long));
    e[h]=calloc(nlab,sizeof(unsigned long long));
    for (i=0;i<g->n;i++){
      d[lab[i]]++;
    }
    cd[h][0]=0;
    for (i=0;i<nlab;i++){
      cd[h][i+1]=cd[h][i]+d[i];
      d[i]=0;
    }
    nodes[h]=malloc(g->n*sizeof(unsigned long));
    new[h]=malloc(g->n*sizeof(unsigned long));
    for (u=0;u<g->n;u++){
      lu=lab[u];
      nodes[h][cd[h][lu]+d[lu]]=u;
      new[h][u]=d[lu]++;
      for (j=g->cd[u];j<g->cd[u+1];j++){
	v=g->adj[j];
	if (lu==lab[v]){
	  e[h][lu]++;
	}
      }
    }
    free(d);
  }

  sg=malloc(sizeof(adjlist));
  sg->n=cd[h][clab+1]-cd[h][clab];
  sg->e=e[h][clab]/2;
  sg->cd=malloc((sg->n+1)*sizeof(unsigned long long));
  sg->cd[0]=0;
  sg->adj=malloc(2*sg->e*sizeof(unsigned long));
  sg->map=malloc(sg->n*sizeof(unsigned long));
  sg->weights = NULL;
  sg->totalWeight = 2*sg->e;
  tmp=0;
  for (k=cd[h][clab];k<cd[h][clab+1];k++){
    u=nodes[h][k];
    sg->map[new[h][u]]=(g->map==NULL)?u:g->map[u];//new[h][u] is equal to i-cd[h][clab]...
    for (j=g->cd[u];j<g->cd[u+1];j++){
      v=g->adj[j];
      if (clab==lab[v]){//clab is equal to lab[u]
	sg->adj[tmp++]=new[h][v];
      }
    }
    sg->cd[new[h][u]+1]=tmp;
  }

  if (clab==nlab-1){
    free(nodes[h]);
    free(new[h]);
    free(cd[h]);
    free(e[h]);
  }

  return sg;
}


//recursive function
void recurs(partition part, adjlist* g, unsigned h, FILE* file){
  time_t t0,t1,t2;
  unsigned long nlab;
  unsigned long i;
  adjlist* sg;
  unsigned long *lab;

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
    lab=malloc(g->n*sizeof(unsigned long));
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
    }
    else{
      fprintf(file,"%u %lu\n",h,nlab);
      for (i=0;i<nlab;i++){
	sg=mkchild(g,lab,nlab,h,i);
	recurs(part,sg,h+1,file);
      }
    }
    free_adjlist(g);
    free(lab);
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

