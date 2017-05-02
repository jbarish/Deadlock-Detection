#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int ** allocation;
int** request;
int procs;
int resources;
int* finOrder;
int* inOrder;

struct stringArray {
	int* ints;
	int n;
}; typedef struct stringArray* StringsInfo;


int compare(int* allocation, int* request, int s);
void append(int* a, int* b, int s);

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
	return si;
}



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
			StringsInfo si = splitString(str);
			mat[numProcs] = si->ints;
			numProcs++;
			resources = si->n;
			mat = (int**)realloc(mat, sizeof(int*)*(numProcs+1));
		}
	}
	fclose(fp);
	procs = numProcs;
	
	return mat;
	
}

int detection(StringsInfo start){
	int found = 0;
	for(int i = 0 ; i< procs; i++){
		for(int j = 0; j< procs; j++){
			if(finOrder[j] == -1 && compare(start->ints, request[j], resources)){
				append(start->ints, allocation[j], resources);
				finOrder[j] = i;
				inOrder[i] =j;
				found = 1;
				break;
			}
		}
		if(!found){
			return 0;	
		}else {
			found = 0;
		}
	}
	return 1;
}

int compare(int* allocation, int* request, int s){
	for(int i = 0; i< s; i++){
		if(allocation[i] < request[i]){
			return 0;
		}
	}
	return 1;
}

void append(int* a, int* b, int s){
	for(int i = 0; i< s; i++){
		a[i]+= b[i];
	}
}

int main(int argc, char* argv[] ){
	
	allocation = readMat(argv[1]);
	request= readMat(argv[2]);
	StringsInfo start = splitString(argv[3]);
	finOrder = malloc(sizeof(int)*procs);
	if(resources != start->n){
		printf("Improper starting vector. Must have %i elements\n", resources);
		exit(1);
	}
	for(int i = 0; i<procs; i++){
		finOrder[i] =-1;
	}
	
	inOrder = malloc(sizeof(int)*procs);
	for(int i = 0; i<procs; i++){
		inOrder[i] =-1;
	}
	
	if(detection(start)){
		printf("No deadlock! Order processes ran:\n");
		for(int i = 0; i< procs; i++){
			printf("\tProccess %i\n", inOrder[i] );
		}
	}else{
		printf("Deadlock with procs: \n");
		for(int i = 0; i< procs; i++){
			if(finOrder[i]==-1){
				printf("\tProccess %i\n",i );
			}
		}
	}
	free(request);
	free(allocation);
	free(finOrder);
	free(inOrder);
	return 0;
}