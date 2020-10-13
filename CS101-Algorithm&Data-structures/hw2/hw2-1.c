//
//  main.c
//  hw2_2
//
//  Created by 吴天元 on 2018/10/19.
//  Copyright © 2018 吴天元. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

void extOddDigits(long n, long* nd){
    long counter = 1;
    long flag = 0;
    while (n != 0){
        long digit = n % 10;
        if (digit < 0){digit *= -1;}
        if (digit % 2 == 1){
            flag = 1;
            *nd += (counter * digit);
            counter *= 10;
        }
        n /= 10;
    }
    if (flag == 0){
        *nd = -1;
    }
    
}

int main(int argc, const char * argv[]) {
    long n, nd = 0;
    scanf("%ld",&n);
    extOddDigits(n, &nd);
    printf("%ld\n",nd);
    return 0;
}
