
## Info:

Feel free to use these lines as you wish.

## To compile:

"make"

## To execute:

"./recpart edgelist.txt res.txt [partition]"
- "edgelist.txt" should contain an undirected unweighted graph: one edge on each line (two unsigned long (nodes' ID)) separated by a space.
- "res.txt": will contain the resulting tree.

option "partition": the bisection algorithm to use, default is 0.
- 0: random bisection
- 1: Louvain partition  
- 2: Louvain first-level partition
Default is 1

## Initial contributors:  
Jean-Loup Guillaume and Maximilien Danisch  
September 2019  
http://bit.ly/danisch  
maximilien.danisch@gmail.com
