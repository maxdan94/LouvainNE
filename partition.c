#include "partition.h"


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
	if (strcmp(c,"1")==0){
		printf("Louvain partition\n");
		return louvain;
	}
	printf("unknown\n");
	exit(1);
}


// -----------------------------------------------------------
// START Louvain utility functions

/*
int myCompare (const void * a, const void * b, void * array2) {
  long diff = ((unsigned long *)array2)[*(unsigned long *)a] - ((unsigned long *)array2)[*(unsigned *)b];
  int res = (0 < diff) - (diff < 0);
  return  res;
}

unsigned long * mySort(unsigned long *part, unsigned long size) {
  unsigned long *nodes = (unsigned long *)malloc(size * sizeof(unsigned long));
  for (unsigned long i = 0; i < size; i++) {
    nodes[i]=i;
  }

  qsort_r(nodes, size, sizeof(unsigned long), myCompare, (void *)part);

  return nodes;
}
*/


inline long double degreeWeighted(adjlist *g, unsigned long node) {
  unsigned long long i;
  if (g->weights == NULL) {
    return 1.*(g->cd[node + 1] - g->cd[node]);
  }

  long double res = 0.0L;
  for (i = g->cd[node]; i < g->cd[node + 1]; i++) {
    res += g->weights[i];
  }
  return res;
}

inline long double selfloopWeighted(adjlist *g, unsigned long node) {
  unsigned long long i;

  for (i = g->cd[node]; i < g->cd[node + 1]; i++) {
    if (g->adj[i] == node) {
      return (g->weights == NULL)?1.0:g->weights[i];
    }
  }

  return 0.0;
}

inline void removeNode(louvainPartition *p, adjlist *g, unsigned long node, unsigned long comm, long double dnodecomm) {
  p->in[comm]  -= 2.0L * dnodecomm + selfloopWeighted(g, node);
  p->tot[comm] -= degreeWeighted(g, node);
}

inline void insertNode(louvainPartition *p, adjlist *g, unsigned long node, unsigned long comm, long double dnodecomm) {
  p->in[comm]  += 2.0L * dnodecomm + selfloopWeighted(g, node);
  p->tot[comm] += degreeWeighted(g, node);
  
  p->node2Community[node] = comm;
}

inline long double gain(louvainPartition *p, adjlist *g, unsigned long comm, long double dnc, long double degc) {
  long double totc = p->tot[comm];
  long double m2   = g->totalWeight;
  
  return (dnc - totc*degc/m2);
}



void freeLouvainPartition(louvainPartition *p) {
  free(p->in);
  free(p->tot);
  free(p->neighCommWeights);
  free(p->neighCommPos);
  free(p->node2Community);
  free(p);
}


louvainPartition *createLouvainPartition(adjlist *g) {
  unsigned long i;

  louvainPartition *p = malloc(sizeof(louvainPartition));

  p->size = g->n;

  p->node2Community = malloc(p->size * sizeof(unsigned long));
  p->in = malloc(p->size * sizeof(long double));
  p->tot = malloc(p->size * sizeof(long double));

  p->neighCommWeights = malloc(p->size * sizeof(long double));
  p->neighCommPos = malloc(p->size * sizeof(unsigned long));
  p->neighCommNb = 0;

  for (i = 0; i < p->size; i++) {
    p->node2Community[i] = i;
    p->in[i]  = selfloopWeighted(g, i);
    p->tot[i] = degreeWeighted(g, i);
    p->neighCommWeights[i] = -1;
    p->neighCommPos[i] = 0;
  }

  return p;
}

long double modularity(louvainPartition *p, adjlist *g) {
  long double q  = 0.0L;
  long double m2 = g->totalWeight;
  unsigned long i;

  for (i = 0; i < p->size; i++) {
    if (p->tot[i] > 0.0L)
      q += p->in[i] - (p->tot[i] * p->tot[i]) / m2;
  }

  return q / m2;
}

void neighCommunitiesInit(louvainPartition *p) {
  unsigned long i;
  for (i = 0; i < p->neighCommNb; i++) {
    p->neighCommWeights[p->neighCommPos[i]] = -1;
  }
  p->neighCommNb = 0;
}

/*
Computes the set of neighbor communities of a given node (excluding self-loops)
*/
void neighCommunities(louvainPartition *p, adjlist *g, unsigned long node) {
  unsigned long long i;
  unsigned long neigh, neighComm;
  long double neighW;
  p->neighCommPos[0] = p->node2Community[node];
  p->neighCommWeights[p->neighCommPos[0]] = 0.;
  p->neighCommNb = 1;

  // for all neighbors of node, add weight to the corresponding community
  for (i = g->cd[node]; i < g->cd[node + 1]; i++) {
    neigh  = g->adj[i];
    neighComm = p->node2Community[neigh];
    neighW = (g->weights == NULL)?1.0:g->weights[i];

    // if not a self-loop
    if (neigh != node) {
      // if community is new (weight == -1)
      if (p->neighCommWeights[neighComm] == -1) {
	p->neighCommPos[p->neighCommNb] = neighComm;
	p->neighCommWeights[neighComm] = 0.;
	p->neighCommNb++;
      }
      p->neighCommWeights[neighComm] += neighW;
    }
  }
}

/*
Same behavior as neighCommunities except:
- self loop are counted
- data structure if not reinitialised
*/
void neighCommunitiesAll(louvainPartition *p, adjlist *g, unsigned long node) {
  unsigned long long i;
  unsigned long neigh, neighComm;
  long double neighW;

  for (i = g->cd[node]; i < g->cd[node + 1]; i++) {
    neigh  = g->adj[i];
    neighComm = p->node2Community[neigh];
    neighW = (g->weights == NULL)?1.0:g->weights[i];
    
    // if community is new
    if (p->neighCommWeights[neighComm] == -1) {
      p->neighCommNb++;
      p->neighCommPos[p->neighCommNb] = neighComm;
      p->neighCommWeights[neighComm] = 0.;      
    }
    p->neighCommWeights[neighComm] += neighW;
  }
}


unsigned long updatePartition(louvainPartition *p, unsigned long *part, unsigned long size) {
  // Renumber the communities in p
  unsigned long *renumber = calloc(p->size, sizeof(unsigned long));
  unsigned long i, last = 1;
  for (i = 0; i < p->size; i++) {
    if (renumber[p->node2Community[i]] == 0) {
      renumber[p->node2Community[i]] = last++;
    }
  }

  // Update part with the renumbered communities in p
  for (i = 0; i < size; i++) {
    part[i] = renumber[p->node2Community[part[i]]] - 1;
  }

  free(renumber);
  return last-1;
}


// Compute one pass of Louvain and returns the improvement
long double louvainOneLevel(louvainPartition *p, adjlist *g) {
  unsigned long nbMoves;
  long double startModularity = modularity(p, g);
  long double newModularity = startModularity;
  long double curModularity;
  unsigned long i,j,node;
  unsigned long oldComm,newComm,bestComm;
  long double degreeW, bestCommW, bestGain, newGain;


  // generate a random order for nodes' movements
  /*
  unsigned long *randomOrder = (unsigned long *)malloc(p->size * sizeof(unsigned long));
  for (unsigned long i = 0; i < p->size; i++)
    randomOrder[i] = i;
  for (unsigned long i = 0; i < p->size - 1; i++) {
    unsigned long randPos = rand()%(p->size - i) + i;
    unsigned long  tmp = randomOrder[i];
    randomOrder[i] = randomOrder[randPos];
    randomOrder[randPos] = tmp;
  }
  */
  
  // repeat while 
  //   there are some nodes moving
  //   or there is an improvement of quality greater than a given epsilon 
  do {
    curModularity = newModularity;
    nbMoves = 0;

    // for each node:
    //   remove the node from its community
    //   compute the gain for its insertion in all neighboring communities
    //   insert it in the best community with the highest gain
    for (i = 0; i < g->n; i++) {
      node = i;//randomOrder[nodeTmp];
      oldComm = p->node2Community[node];
      degreeW = degreeWeighted(g, node);

      // computation of all neighboring communities of current node
      neighCommunitiesInit(p);
      neighCommunities(p, g, node);

      // remove node from its current community
      removeNode(p, g, node, oldComm, p->neighCommWeights[oldComm]);
      //      printf("remove %lu from %lu\n", node, oldComm);

      // compute the gain for all neighboring communities
      // default choice is the former community
      bestComm = oldComm;
      bestCommW  = 0.0L;
      bestGain = 0.0L;
      for (j = 0; j < p->neighCommNb; j++) {
	newComm = p->neighCommPos[j];
	newGain = gain(p, g, newComm, p->neighCommWeights[newComm], degreeW);

	//printf("try %lu %lu in %lu : %Lf\n", j, node, newComm, newGain);

	if (newGain > bestGain) {
	  bestComm = newComm;
	  bestCommW = p->neighCommWeights[newComm];
	  bestGain = newGain;
	}
      }

      // insert node in the nearest community
      insertNode(p, g, node, bestComm, bestCommW);
      // printf("insert %lu to %lu\n", node, bestComm);

      if (bestComm != oldComm) {
	nbMoves++;
      }
    }

    newModularity = modularity(p, g);
  } while (nbMoves>0 && 
    	   newModularity - curModularity > MIN_IMPROVEMENT);
  
  //  free(randomOrder);

  return newModularity - startModularity;
}

unsigned long louvain(adjlist *g, unsigned long *lab) {
  unsigned long i,n;
  long double improvement;
  // Initialize partition with trivial communities
  for (i = 0; i < g->n; i++) {
    lab[i] = i;
  }

  // Execution of Louvain method
  //  while(1) {
    louvainPartition *gp = createLouvainPartition(g);

    improvement = louvainOneLevel(gp, g);

    //    if (improvement < MIN_IMPROVEMENT) {
    //      freeLouvainPartition(gp);
    //      break;
    //    }
    
    n = updatePartition(gp, lab, g->n);
    
    //adjlist *g2 = partition2Graph(gp, g);
    //    if (g->n < size) {
    //      freeGraph(g);
    //    }
    freeLouvainPartition(gp);
    //    g = g2;
    //  }

  return n;
}



// END Louvain utility functions
// -----------------------------------------------------------

