# data-structures-project


Data Structures Project under Jim Storer

This program performs a simple breadth-first-search using a queue and hashtable of my own implementation. 
The hash function converts a string into an integer using a polynomial function such that for the i-th char in string s, the hash code equals s[i](17^i)+s[i-1](17^i-1)+....s[0]

The BFS returns the shortest solution to the puzzle of turning a 2-d board with starting position:

N I T E
$ . $ .
D A Y .


into final position 


D A Y .
$ . $ .
N I T E

The rules are that '.' represents a space for each letter to move. 1 move of 1 letter is a turn. And letters cannot move onto spaces containing '$' or other letters. 



Running instructions:

/*

in the same directory, include files puzzle.h and output.h. Then, execute 

gcc -g -std=c99 -Wall -I . WangJerry.c; ./a.out > output

and the final output will be saved for viewing in the file named 'output'

*/


