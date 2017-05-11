#include <stdio.h> 
#include <stdlib.h> 
#include "Puzzle.h" 
#include "Output.h"

/*run instructions: in the same directory, include files puzzle.h and output.h. Then, execute 

gcc -g -std=c99 -Wall -I . WangJerry.c; ./a.out > output

*/

/******************************************************************************/
/*Position                                                                    */
/******************************************************************************/
/*position struct definition*/
typedef struct pnode { 
	/*next pointer for the hash bucket*/ 
	struct pnode *next; 
	/*position from which this one came*/
	struct pnode *back;
	/*number of moves to get to this position*/
	int cost;
	/*piece that moved to this position*/
	char piece;
	/*direction moved to enter this position*/
	int dir;
	/*this position's board*/
	char board[BoardSize];
    
} PositionBody;

/*Allocate memory for a new position and initialize fields within position struct.*/
PositionBody *NEWPOSITION() {
  PositionBody *p = (PositionBody *) malloc(sizeof(PositionBody));
  if (p==NULL) {	
	exit(1);
  }  
  p->next=NULL;
  p->back=NULL;
  p->cost=0;
  p->dir=0;
  p->piece='.';
	return p; 
}
/******************************************************************************/
/*Queue                                                                      */
/******************************************************************************/
/*queue struct definition*/
typedef struct queue {
	int front;
	int rear;
	int size;
	int maxsize;
	PositionBody *queueArray[QueueArraySize];

} QueueBody;
/*allocate memory for a new queue and initialize all queue fields*/
QueueBody *newQueue(){
	QueueBody *qp = (QueueBody *)malloc(sizeof(QueueBody));
	qp->front=0;
	qp->rear=0;
	qp->size=0;	
	qp->maxsize=0;
	return qp;
}
/*insert element into a queue. If queue is full, ignore the element*/
void enqueue(PositionBody *item, QueueBody *qp){	
		
	if(qp->size == QueueArraySize){		
		return;
	}	
	else{				
		qp->queueArray[qp->rear]=item;								
		
		qp->rear++;
		if(qp->rear==QueueArraySize){			
			qp->rear=0;
		}
		qp->size++;		
		if(qp->size > qp->maxsize){
			qp->maxsize=qp->size;
		}		
	}	
	
}
/*remove and return an element from the head of the queue. If queue is empty, return nothing*/
PositionBody *dequeue(QueueBody *qp){	
	if(qp->size==0){		
		return NULL;
	}
	else{
		PositionBody *temp = qp->queueArray[qp->front];
		qp->front++;
		if(qp->front>=QueueArraySize){
			qp->front=0;
		}
		qp->size--;		
		return temp;
	}
}

/******************************************************************************/
/*Hashtable                                                                   */
/******************************************************************************/
/*power function to aid in hash code calculations*/
int power(int base, int exponent){
	int count=1;
	int i;
	for(i=0; i<exponent; i++){
		count*=base;
	}
	return count;
}
/*hashtable struct definition*/
typedef struct HashTable {
	int elementsCount;
	int maxbucketsize;
	PositionBody *HashArray[HashArraySize];

} HashTableBody;
/*allocate space for new hash table, initialize fields in hashtable struct*/
HashTableBody *newHashTable(){
	HashTableBody * htptr=(HashTableBody *)malloc(sizeof(HashTableBody));
	htptr->elementsCount=0;
	htptr->maxbucketsize=0;
	return htptr;
}
/*hashfunction that takes the polynomial sum of each char's ascii value and multiplies with 17 ^ i:
hashcode = (s[i](17^i)+s[i-1](17^i-1)+....s[0]) % hasharraysize
*/
int hashfunction(char *string){
	int hashcode=0;
	int i;
	for(i=BoardSize-1; i>=0; i--){
		hashcode+=string[i]*power(17,i);
		hashcode%=HashArraySize;
	}
	return hashcode;
}

/*given a bucket in the hashtable, count the number of elements in the bucket and return the count*/
int printBucket(PositionBody *head, int k){
	int count = 0;
	PositionBody *current = head;	
	while(current!=NULL){				
		current = current->next;
		count++;	
	}		
	return count;
}
/*aggregate hashtable statistics to print out after the solution is found.*/
void printHashTable(HashTableBody *htptr){			
	
	int bucketsizes[20]={0};
	for(int k=0; k<HashArraySize; k++){	
		if(htptr->HashArray[k]!=NULL ){			
			int bucketCount = printBucket(htptr->HashArray[k],k);
			if(bucketCount>htptr->maxbucketsize){
				htptr->maxbucketsize=bucketCount;
			}
			htptr->elementsCount+=bucketCount;
			bucketsizes[bucketCount]+=1;
		}
		else{
			bucketsizes[0]+=1;
		}
	}	
	
	HashStats(HashArraySize,htptr->elementsCount,htptr->maxbucketsize);
	for(int i=0; i<htptr->maxbucketsize+1; i++){
		BucketStat(i,bucketsizes[i]);
	}
}
/*given two strings, return whether they are the same strings or not. Useful to compare boards.*/
/*1 -> strings do not match 	0->strings match */
int stringCMP(char *string1, char*string2){
	
	for(int i=0; i<BoardSize; i++){				
		if((char) string1[i] != (char) string2[i]){
			return 1;
		}
	}
	return 0;
}
/*put an element at the end of a given bucket, the head, in the hashtable.*/
void putAtEndOfLinkedList(PositionBody *head, PositionBody *element){
	PositionBody *current = head;		
	while(current->next!=NULL){							
		current = current->next;		
	}		
	current->next=element;		
	
}
/*return whether a position's board exists already in the hash table.*/
/*1: k not found	0:k found*/
int member(PositionBody *k, HashTableBody *hashtable){
	int index=hashfunction(k->board);
	if(hashtable->HashArray[index]==NULL){
		return 1;	
	}
	else{
		PositionBody *current = hashtable->HashArray[index];		
		if(stringCMP(current->board, k->board)==0){
			return 0;
		}
		while(current->next!=NULL){							
			if(stringCMP(current->board, k->board)==0){
				return 0;
			}
			current = current->next;		
		}				
		return 1;		
	}	
}
/*insert an element into a hashtable. I don't include a MEMBER check inside the insert function, 
but I execute MEMBER before calling insert in the outer function*/
void insert(PositionBody *k, HashTableBody *hashtable){
	int index=hashfunction(k->board);
	
	if(hashtable->HashArray[index]==NULL){
		hashtable->HashArray[index]=k;
	}
	else{		
		putAtEndOfLinkedList(hashtable->HashArray[index],k);		
	}	
	
}

/******************************************************************************/
/*Generate all possible moves given a position                                */
/******************************************************************************/
/*given two strings, copy the contents of string 2 into string 1. Useful to generate new boards*/
void stringCopy(char *string1, char *string2){
	for(int i=0; i<BoardSize; i++){
		string1[i]=string2[i];
	}
}
/*given a board and a new direction to move to, make the new board and try to insert it to the hashtable*/
PositionBody *makeNewBoard(int spaceindex, int swapindex, PositionBody *p, HashTableBody *htptr, int direction){	
	if(p->board[swapindex]!=SymbolFixed && p->board[swapindex]!=SymbolBlank){
		char temp = p->board[swapindex];
		PositionBody *k = NEWPOSITION();
		stringCopy(k->board,p->board);
		k->board[swapindex]='.';
		k->board[spaceindex]=temp;
				
		if(member(k, htptr)==1){
			k->back=p;
			k->dir=direction;
			k->cost=p->cost+1;
			k->piece=temp;
			insert(k, htptr);			
			return k;
		}
		
		else{
			free(k);
			return NULL;
		}			
	}	
	return NULL;
}
/*given the location of a space (the spaceindex) and a position, 
try to exchange the space with the piece residing in the immediate left, right, up and down space. If it's possible, try to make a new board*/
void tryAllDirections(int spaceindex, PositionBody *p, HashTableBody *htptr, QueueBody *q){
	
	int left = spaceindex-1;
	int right = spaceindex+1;
	int up = spaceindex-BoardWidth; 
	int down = spaceindex+BoardWidth;
	PositionBody *m=NULL;
	/*left*/	
	if((left+1)%4!=0){		
		m = makeNewBoard(spaceindex, left, p, htptr, 1);
		if(m!=NULL){
			enqueue(m,q);
		}	
	}
	/*right*/
	if(right%4!=0){		
		m = makeNewBoard(spaceindex, right, p, htptr, 3);
		if(m!=NULL){
			enqueue(m,q);
		}	
	}
	/*up*/
	if(up>=0){		
		m = makeNewBoard(spaceindex, up, p, htptr, 2);
		if(m!=NULL){
			enqueue(m,q);		
		}		
	}
	/*down*/
	if(down<BoardSize){		
		m = makeNewBoard(spaceindex, down, p, htptr, 0);
		if(m!=NULL){
			enqueue(m,q);
		}
	}	
}
/*given a position, try to generate all possible positions resulting from a single move.
I test whether the space pieces can be exchanged with any valid letter pieces rather than the other way around.*/
void findMovesAndInsert(HashTableBody * htptr, PositionBody *p, QueueBody *q){
	char *start=p->board;
	int indices[3];
	int counter = 0;
	/*given a start board, I assume there are only 3 space pieces and I get the index of all three to generate further boards*/
	for(int i=0; i<BoardSize; i++){
		if(start[i]=='.'){
			indices[counter]=i;
			counter++;
		}
	}	
	
	for(int f=0; f<3; f++){
		tryAllDirections(indices[f], p, htptr,q);
	}
}
/*once a solution has been found we must reverse the back pointers to output the solution from start to goal.*/
PositionBody *reverseLinkedList(PositionBody *p){
	PositionBody *prev=p;
	PositionBody *current=p->back;
	PositionBody *n = current->back;
	prev->back = NULL;
	while(n!=NULL){
		current->back=prev;
		prev=current;
		current=n;
		n=n->back;
	}
	current->back=prev;
	return current;	
}
/*entry point of our program: initialize new queue and hashtable. Initialize the starting position.
enqueue the first position, then keep generating new positions and dequeing to check whether the goal 
position has been found. when the goal position has been found, reverse the back pointers and print the solution 
as well as the hashtable and queue statistics*/
int main(){
	
	QueueBody *qp = newQueue();	
	HashTableBody *htptr = newHashTable();

	PositionBody *first= NEWPOSITION();
	stringCopy(first->board, StartBoard);	
	enqueue(first,qp);
	
	char *board = first->board;
	char *goal = GoalBoard;
	
	PositionBody *current=NULL;
	
	while(stringCMP(board,goal)==1){
		current = dequeue(qp);
		findMovesAndInsert(htptr,current,qp);
		board = current->board;				
	}
	
	PositionBody *solutionlist = current;
	solutionlist = reverseLinkedList(current);
	while(solutionlist->back!=NULL){
		OutputPosition(solutionlist->board,solutionlist->cost,solutionlist->piece,solutionlist->dir);
		solutionlist=solutionlist->back;
	}
	OutputPosition(solutionlist->board,solutionlist->cost,solutionlist->piece,solutionlist->dir);
	QueueStats(QueueArraySize,qp->maxsize,qp->front,qp->rear);
	printHashTable(htptr);

}

