#include <stdio.h>
#include <unistd.h>

#define COMMON_PERIOD_SEC 1 // Global common period for all tasks

void task1() {
    printf("Task 1 execute\n");
}

void task2() {
    printf("Task 2 execute\n");
}

void task3() {
    printf("Task 3 execute\n");
}

int main() {
    while (1) {

        task1();
        sleep(COMMON_PERIOD_SEC);


        task2();
        sleep(COMMON_PERIOD_SEC);


        task3();
        sleep(COMMON_PERIOD_SEC);
    }

    return 0;
}

