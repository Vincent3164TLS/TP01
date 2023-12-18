#include <stdio.h>

int main() {
    // Declare a variable
    int myVariable;

    // Assign a value to the variable
    myVariable = 42;

    // Print the value and memory address
    printf("Value: %d\n", myVariable);
    printf("Memory Address: %p\n", (void*)&myVariable);

    return 0;
}

