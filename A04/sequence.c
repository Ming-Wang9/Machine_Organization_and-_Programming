#include<stdio.h>
#include<stdlib.h>
#include<errno.h>

typedef struct{
	int *value;
}Sequence;

int main(int argc, char* argv[]){
	if(argc != 5){
		printf("%s", "Usage: ./sequence n x0 m c  ==> where next value in sequence is computed as x_1 = m * x_0 + c"
					"where n is a non-zero positive number of values in the sequence"
					"and x0 is an integer and is the first value in the sequence"
					"and m is an integer and is used as a multiplier of the previous term in the sequence"
					"and c is an integer and is added to the (m*previous) term \n");
	return 1;
	}
	/*
	int num_instances, firstValue, multiplier, constant;
	if (scanf("%d %d %d %d", &num_instances, &firstValue, &multiplier, &constant) != 4){
		printf("Error: Invalid input\n");
		return 1;
	}
	*/

	int num_instances =	atoi(*(argv+1));//strtol(*(argv+1), &endptr, 10); //scanf("%d", &num_instances);
	int	firstValue = atoi(*(argv+2));//strtol(*(argv+2), &endptr, 10); //scanf("%d", &firstValue);
	int multiplier = atoi(*(argv+3));//strtol(*(argv+3), &endptr, 10); //scanf("%d", &multiplier);
	int constant = atoi(*(argv+4));//strtol(*(argv+4), &endptr, 10);//scanf("%d", &constant);
	

	if (num_instances <= 0 ){
		printf("Error, please enter a non-zero positiive number");
		return 1;
	}
	
	Sequence sequence;
	sequence.value = malloc(num_instances * sizeof(int));	
	if (sequence.value == NULL){
		fprintf(stderr, "Memory allocation filaed\n");
		return 1;
	}
		
	*(sequence.value) = (int) firstValue;
	printf("%d", *(sequence.value));
	for (int i = 1; i < num_instances; i++){
		*(sequence.value + i) = multiplier * *(sequence.value + i -1) + constant;
		printf(",%d",*(sequence.value + i));
	}
	printf("\n");
	
	free(sequence.value);
	return 0;
}
	
	



