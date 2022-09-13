### Binary Search Tree
While a perfect binary search tree has O(logn) complexity for all operations, inserts and removes may degenerate the tree into a doubly linked list. Which means all operations take O(n) in the worst case.

- Search:
	- Average: O(logn)
	- Worst: O(n)	
- Insert:
	- Average: O(logn)
	- Worst: O(n)	
- Delete: 
	- Average: O(logn)
	- Worst: O(n)
* * *
### AVL Tree
Height balanced binary search tree. Guarantees |left.h - right.h| < 2 for each node. Because of this, an AVL three has O(logn) time for all operations.
Needs to store an additional integer for each node to store either height or balance factor of each node, so it has a negligable space overhead compared to BSTs.
In my implementation I chose to deal with balance factors instead of heights since, fundemantally, that's what the algorithm actually aims to keep track of. 

- Search:
	- Average: O(logn)	
- Insert:
	- Average: O(logn)
- Delete: 
	- Average: O(logn)
* * *
### Tracked Array
Array that keeps track of empty indices. 

- Search
	- Average: O(n)
- Insert
	- Average: O(n)
- Delete
	- O(1)
