#include <stdio.h>
#include <stdlib.h>

void findSecondMaxAndMin(int* secondMax, int* secondMin)
{
    // YOUR CODE STARTS HERE
    int n;
    scanf("%d", &n);
    int* arr = malloc(n*sizeof(int));
    for (int i=0; i<n; i++)
        scanf("%d", &arr[i]);
    int max = -10001, min = 10001;
    for (int i=0; i<n; i++)
    {
        max = (arr[i] > max) ? arr[i] : max;
        min = (arr[i] < min) ? arr[i] : min;
    }
    *secondMax = -10001, *secondMin = 10001;
    for (int i=0; i<n; i++)
    {
        if (arr[i] == max || arr[i] == min)
            continue;
        (*secondMax) = (arr[i] > (*secondMax)) ? arr[i] : (*secondMax);
        (*secondMin) = (arr[i] < (*secondMin)) ? arr[i] : (*secondMin);
    }
    free(arr);
    // YOUR CODE ENDS HERE
}

int main(int argc, char *argv[])
{
    int secondMax, secondMin;
    findSecondMaxAndMin(&secondMax, &secondMin);
    printf ("%d %d\n", secondMax, secondMin);
    return 0;
}
