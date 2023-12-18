#include <stdio.h>

int main() {

    int myArray[] = {10, 20, 30, 40, 50};
    int arraySize = sizeof(myArray) / sizeof(myArray[0]);

    printf("Array: ");
    for (int i = 0; i < arraySize; i++) {
        printf("%d ", myArray[i]);
    }
    printf("\n");

    int searchValue;
    printf("Enter a value to search: ");
    scanf("%d", &searchValue);

    int found = 0; 

    for (int i = 0; i < arraySize; i++) {
        if (myArray[i] == searchValue) {
            printf("Value %d found at index %d.\n", searchValue, i);
            found = 1;
            break; 
    }
    }

    if (!found) {
        printf("Value %d not found in the array.\n", searchValue);
    }

    return 0;
}

