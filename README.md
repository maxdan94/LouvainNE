
## Info:

Feel free to use these lines as you wish.
Computes graph cut recursively in order to obtain an ordering of the nodes

## To compile:

"make"

## To execute:

"./recpart edgelist.txt res.txt [bisection]"
- "edgelist.txt" should contain an undirected unweighted graph: one edge on each line (two unsigned long (nodes' ID)) separated by a space.
- "res.txt": will contain the resulting tree.

option "bisection": the bisection algorithm to use, default is 0.
- 0: random bisection

## Initial contributor:
Maximilien Danisch  
April 2019  
http://bit.ly/danisch  
maximilien.danisch@gmail.com
