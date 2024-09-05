#include <stdio.h>

int main(void){
        int i = 1;
        while (i<11){
                printf("%d", i);
                if (i < 10){
                        printf(", ");
                }
                i++;
        }
		printf("\n");
        return 0;
}

