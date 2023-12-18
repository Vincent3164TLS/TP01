#include <stdio.h>
#include <time.h>

void printTime() {
    clock_t start= clock();

    while (1) {
        clock_t current = clock();
        double time = (double)(current - start) / CLOCKS_PER_SEC;

        printf("temps depuis activation: %.2f seconds\n", time);

        while ((double)(clock() - current) / CLOCKS_PER_SEC < 1.0) {}
    }
}

int main() {
    printTime();

    return 0;
}

