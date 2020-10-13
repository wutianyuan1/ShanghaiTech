#include <stdio.h>
#include <stdlib.h>

int main(){
    printf("Enter the temperature in degree F: ");
    float F;
    scanf("%f", &F);
    float C;
    C = (5.0 / 9.0)*(F - 32);
    printf("Converted degree in C: %f\n", C);
    system("pause");
    return 0;
}
