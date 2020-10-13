#include <stdio.h>
//#include <math.h>
#include <stdlib.h>

int factorial(int a){
    int b = 1;
    for (int i = 1; i <= a; i++){
        b *= i;
    }
    return b;
}

float pownum(float x, int times){
    double r = 1;
    for (int i = 0; i < times; i++){
        r *= x;
    }
    return r;
}

int main(){
    float x;
    int n;
    printf("Please enter the value of n: ");
    scanf("%d", &n);
    getchar();
    printf("Please enter the value of x: ");
    scanf("%f", &x);
    float ex = 1;
    for (int i = 1; i <= n; i++){
        ex += pownum(x,i)/factorial(i);
    }
    printf("Result = %f\n", ex);
    system("pause");
    return 0;
}
