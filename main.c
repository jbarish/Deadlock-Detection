#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int ** allocation; /*allocation matrix, read in from file */
int** request; /*request matrix, read in from file */
int procs; /*number of processes*/
int resources; /*number of resources*/
int* finOrder; /*order the processes finish */
int* inOrder; /*a sorted array of the procs in  finish order */

struct stringArray {
	int* ints;
	int n;
}; typedef struct stringArray* StringsInfo;


int compare(int* allocation, int* request, int s);
void append(int* a, int* b, int s);
int appendCompare(int* start, int* allocation, int* request, int s);

/*
 * Given a string, count the number of times a given character appears
 *
 * Param fullName   the string to count in
 * Param c          the character to count
 * Return           the number of occurances of c in fullName
 */
int countOccurance(char* fullName, char c){
	int counter = 0;
	for(int i = 0; i<strlen(fullName); i++){
		if(fullName[i]==c){
			counter++;
		}
	}
	return counter;
}

/*
 * Given a string, split it on a given char
 *
 * Param fullName   the string to split
 * Return           a struct containing the array of ints
 */
StringsInfo splitString(char* fullName){
	int numNames = countOccurance(fullName, ' ') +1; 
	int* names = (int*)malloc(sizeof(int)*numNames);
	int i=0;
	char* tempName = (char*)malloc(sizeof(char)*strlen(fullName)+1);
	strcpy(tempName, fullName);
	char* s = strtok(tempName," ");
	
	while(s!=NULL){
		names[i++] = atoi(s);
		s = strtok(NULL," ");
	}
	StringsInfo si = malloc(sizeof(struct stringArray));
	si->n = numNames;
	si->ints = names;
	free(tempName);
	return si;
}


/*  
 * Read in a matrix from a file
 *
 * Param fName  the file name
 * Return       the matrix (integers)
 */
int** readMat(char* fName){
	FILE* fp;
	char str[2048];
	char newStr[2048];
	newStr[0] = '\0';
	fp = fopen(fName, "r");
	int** mat = malloc(sizeof(int*)*1);
	int numProcs = 0;
	
	/*read from file */
	while(fgets(str, 2048, fp) != NULL){
		if(strlen(str) >= 1){/* ensure no blank lines at end of file */ 
			StringsInfo si = splitString(str); /*split string on space */
			mat[numProcs] = si->ints;
			numProcs++;
			resources = si->n;
			free(si);
			mat = (int**)realloc(mat, sizeof(int*)*(numProcs+1)); /*reallocate length of matrix*/
		}
	}
	fclose(fp);
	procs = numProcs;
	
	return mat;
	
}

/* detect if a deadlock occurs
 *
 * Param start   A struct containing the avaiable vector 
 * Param n       The index to start considering procs at (0 for first run)
 */
int detection(StringsInfo start, int n){
	int found = 0;
	
	/*go through each process that we need to resolve */
	for(int i = n ; i< procs; i++){

		/*go through each available process to see if we can run it */
		for(int j = 0; j< procs; j++){
			if(finOrder[j] == -1 && compare(start->ints, request[j], resources)){
				append(start->ints, allocation[j], resources);
				finOrder[j] = i;
				inOrder[i] =j;
				found = 1;
				break;
			}
		}
		
		/*if we have left over procs that have not yet been run, there is deadlock*/
		if(!found){
			return 0;	
		}else {
			found = 0;
		}
	}
	return 1;
}

/*use preemption to try and recover from our deadlock */
int recover(StringsInfo start){
	
	/*figure out where we left off to see what procs are deadlocked */
	int max = -1;
	for(int i = 0 ; i< procs; i++){
		if(finOrder[i] > max){
			max = finOrder[i];
		}
	}
	max+=1;
	
	for(int i = 0; i<procs; i++ ){
		if(finOrder[i] == -1){
			/*process we try to preempt */
			for(int j = 0; j < procs; j++){
				
				/*if the j process has not yet run, and if we take away its resources, can we run process i? */
				if(finOrder[j] == -1 && appendCompare(start->ints,allocation[j], request[i], resources)){
					
					printf("\t\tProcess %i completed with process %i preempted \n", i, j);
					append(start->ints, allocation[i], resources);
					inOrder[max]=i;
					finOrder[i] = max++;
					
					/*try to see if we can finish running all procs */
					if(detection(start, max)){
						/*if we can, we are done*/
						printf("\t\tDeadlock Resolved! Order processes finished:\n");
						for(int index = max; index< procs && inOrder[index] != -1; index++){
							printf("\t\tProccess %i\n", inOrder[index] );
						}
						return 1;
					}else{
						/*if not, then keep going with preemption. */
						
						
						int flag = 1;
						/*print how many procs we could run since last preemption */
						for(int index = max; index< procs && inOrder[index] != -1; index++){
							if(flag){
								printf("\t\t---The following processes were able to continue running:---\n");
								flag = 0;
							}
							printf("\t\t   Process %i\n", inOrder[index] );
						}
					
						/*figure out how far we got in fixing it up */
						for(int l = max-1 ; l< procs; l++){
							if(finOrder[l] > max){
								max = finOrder[l];
							}
						}
						max+=1;
						break;
						
					}
				}
			}
		}
	}
	return 0;
}



/* check if all elements of the allocation array are greater than the elements in the request array */
int compare(int* allocation, int* request, int s){
	for(int i = 0; i< s; i++){
		if(allocation[i] < request[i]){
			return 0;
		}
	}
	return 1;
}

/*add two vectors together */
void append(int* a, int* b, int s){
	for(int i = 0; i< s; i++){
		a[i]+= b[i];
	}
}

/*check if we add a specific allocation vector to our available vector, we can successfully run a proc */
int appendCompare(int* start, int* allocation, int* request, int s){
	int* temp = malloc(sizeof(int)*s);
	for(int i = 0; i< s; i++){
		temp[i] = allocation[i]+start[i];
	}
	int res = compare(temp, request, s);
	free(temp);
	return res;
	
}

int main(int argc, char* argv[] ){
	
	allocation = readMat(argv[1]); /*read allocation matrix */
	request= readMat(argv[2]); /* read request matrix */
	StringsInfo start = splitString(argv[3]); /*get available vector */
	
	finOrder = malloc(sizeof(int)*procs);
	inOrder = malloc(sizeof(int)*procs);
	
	if(resources != start->n){
		printf("Improper starting vector. Must have %i elements\n", resources);
		exit(1);
	}
	
	for(int i = 0; i<procs; i++){
		finOrder[i] =-1;
		inOrder[i] =-1;
	}
	
	/*run detection algorithm */
	if(detection(start, 0)){
		/*no deadlock*/
		printf("---No deadlock! Order processes ran:---\n");
		for(int i = 0; i< procs; i++){
			printf("\tProcess %i\n", inOrder[i] );
		}
	}else{
		/*deadlock occured */
		printf("Deadlock!\nAble to run the following processes:\n");
		for(int i = 0; i< procs && inOrder[i] != -1; i++){
			printf("\tProccess %i\n", inOrder[i] );
		}
		printf("Deadlock with processes: \n");
		for(int i = 0; i< procs; i++){
			if(finOrder[i]==-1){
				printf("\tProcess %i\n",i );
			}
		}
		printf("\t---Attempting to recover through preemption:---\n");
		int res = recover(start);
		if(!res){
			printf("\t\t---Could not resolve deadlock with procs:--- \n");
			for(int i = 0; i< procs; i++){
				if(finOrder[i]==-1){
					printf("\t\t   Proccess %i\n",i );
				}
			}
		}
		
	}
	
	for(int i = 0; i< procs; i++){
		free(allocation[i]);
		free(request[i]);
	}
	free(start->ints);
	free(start);
	free(request);
	free(allocation);
	free(finOrder);
	free(inOrder);
	return 0;
}