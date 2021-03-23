#include <stdio.h>
int colorMap(float minVal, float maxVal, int maxValColor, float val)
{
    // YOUR CODE STARTS HERE
    float r = (val - minVal) / (maxVal - minVal);
    float R =  r * ((maxValColor >> 16) & 0xff);
    float G =  r * ((maxValColor >> 8) & 0xff);
    float B =  r * (maxValColor & 0xff);
    return ((int)R<<16) + ((int)G<<8) + (int)B;
    // YOUR CODE ENDS HERE
}

int main()
{
    float minVal, maxVal, val;
    int maxValColor;
    scanf("%f %f %x %f", &minVal, &maxVal, &maxValColor, &val);
    int color = colorMap(minVal, maxVal, maxValColor, val);
    printf ("0x%x\n", color);
    return 0;
}
