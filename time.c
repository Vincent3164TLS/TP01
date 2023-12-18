#include <stdio.h>
#include <time.h>

void printElapsedTime() {
    clock_t start_time = clock();

    while (1) {
        clock_t current_time = clock();
        double elapsed_time = (double)(current_time - start_time) / CLOCKS_PER_SEC;

        printf("Elapsed Time: %.2f seconds\n", elapsed_time);

        while ((double)(clock() - current_time) / CLOCKS_PER_SEC < 1.0) {}
    }
}

int main() {
    printElapsedTime();

    return 0;
}

