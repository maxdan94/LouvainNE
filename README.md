## Reference:

LouvainNE: Hierarchical Louvain Method for High Quality and Scalable Network Embedding.  
WSDM2020.

## To compile:

type "make"

## To execute:

### recpart:

"./recpart edgelist.txt hierarchy.txt [partition]"
- "edgelist.txt" should contain an undirected unweighted graph: one edge on each line (two unsigned long (ID of the 2 nodes) separated by a space).
- "hierarchy.txt": will contain the resulting tree.

option "partition": the partition algorithm to use, default is 1.
- 0: random bisection
- 1: Louvain partition  
- 2: Louvain first-level partition  
- 3: Label Propagation partition

Format of "hierarchy.txt":
- "h c" on each line, where "h" is the depth of the associated tree's node and "c" is its number of children.
- If "c" is one, then the line is of the form "h 1 n node_1 node_2 ... node_n", where "n" is the number of nodes (nodes of the input graph) in that tree's leaf and "node_1 node_2 ... node_n" are the IDs of these "n" nodes.
- The tree-nodes on each line are listed following a DFS ordering.

### hi2vec:

"./hi2vec k a hierarchy.txt vectors.txt"
- "k" is the wished number of dimensions of the output vectors
- "a" is the damping parameter (a=0.01 is a good default value)
- "hierarchy.txt" should contain the input hierarchy in the format described above.
- "vectors.txt" will contain a vector for each node in the graph in the foorm "u v_1 v_2 ... v_k" on each line, where "u" is the ID of a node and "v_1 v_2 ... v_k" are the "k" values of the associated vector.

### renum:

In order to perform a non-mandatory pre-processing step that consists to re-number the nodes from 0 to n-1, where n is the number of nodes of degree one or more in the input graph (that is nodes that belong to at least one edge).

"./renum input_edgelist.txt output_edgelist.txt map.txt"
- "input_edgelist.txt" should contain an undirected unweighted graph: one edge on each line (two unsigned long (ID of the 2 nodes) separated by a space).
- "output_edgelist.txt" will contain the resulting renumbered undirected unweighted graph: one edge on each line (two unsigned long (ID of the 2 nodes) separated by a space).
- "map.txt" will contain the matching "new_label old_label" on each line.

## Initial contributors:  

Jean-Loup Guillaume and Maximilien Danisch  
Technical consultants: Ayan Kumar Bhowmick, Koushik Meneni and Bivas Mitra  
September 2019  
http://bit.ly/danisch  
maximilien.danisch@gmail.com
