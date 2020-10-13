#include <stdio.h>
#include <stdlib.h>
#include <math.h>


int main(){
    printf("Enter the values for a1, b1, c1, a2, b2, c2: ");
    float a1, a2, b1, b2, c1, c2;
    scanf("%f %f %f %f %f %f", &a1, &b1, &c1, &a2, &b2, &c2);
    float x, y;
    if(fabs(a1*b2 - a2*b1) <= 0.0001){
        printf("denominator too small!\n");
    }
    else{
        x = (b2*c1 - b1*c2)/(a1*b2 - a2*b1);
        y = (a1*c2 - a2*c1)/(a1*b2 - a2*b1);
        printf("x = %f and y = %f\n", x, y);
       }
    system("pause");
    return 0;
 }

