#include <stdio.h>


int sum(int a, int b) {
    return a + b;
}

int main() {
    int num1, num2;

   
    printf("premier nombre");
    scanf("%d", &num1);
    printf("second nombre");
    scanf("%d", &num2);

 
    int result = sum(num1, num2);
    printf("Somme de %d et %d est: %d\n", num1, num2, result);

    return 0;
}

