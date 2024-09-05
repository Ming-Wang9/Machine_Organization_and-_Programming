#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s start_integer end_integer where start is less than end\n", argv[0]);
        return 1;
    }

    char *endPtr;
    int start = strtol(argv[1], &endPtr, 10);
    if (*endPtr != '\0') {
        printf("Invalid start value.\n");
        return 1;
    }

    int end = strtol(argv[2], &endPtr, 10);
    if (*endPtr != '\0') {
        printf("Invalid end value.\n");
        return 1;
    }

    if (start >= end) {
        printf("Usage: %s start_integer end_integer where start is less than end\n", argv[0]);
        return 1;
    }

    int i = start;
    while (i <= end) {
        printf("%d", i);
        if (i < end) {
            printf(", ");
        }
        i++;
    }
    printf("\n");

    return 0;
}

