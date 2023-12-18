#include<stdio.h>

int main() {

    int i;

    printf("Entrez la valeur de i : ");
    scanf("%d", &i);

    // Afficher les nombres jusqu'à i
    for (int nombre = 1; nombre <= i; nombre++) {
        printf("%d\n", nombre);
    }

    return 0;
}
