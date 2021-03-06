We’ve learned number expression and bit operation in class. In this problem, we’ll design a “new” floating-point number which is slightly different from the IEEE 754 standard, and implement the conversion between hexadecimal and our floating-point number.

Before the problem description, let’s just recap on what we’ve learned so far.

- Number expression

  The built-in types of C language like int, float, double…are actually bits (a binary sequence, e.g., 0b0100100…) in memory. For example, usually, char contains 8 bits, int and float contains 32 bits, and double contains 64 bits.

- Bit operations
  We’ve learned bitwise operations and (&), or (|), not (~), xor (^) in class. For example, assume A = 60 and B = 13 in binary format, they will be as follows.
  ```
A = 0b00111100
B = 0b00001101
A&B = 0b00001100
A|B = 0b00111101
A^B = 0b00110001
~A = 0b11000011
  ```

Now, let’s design our floating-point number. First, we’ll learn the floating-point encoding in our computer (IEEE 754 standard, https://en.wikipedia.org/wiki/IEEE_754). 

A single-precision floating-point number is usually a 32-bit field in memory, it can be divided into 3 parts: sign, exponent and fraction. A standard float usually contains 23 fraction bits (0-22), 8 exponent bits (23-30) and 1 sign bit (31). For example, the bits 0x3e200000 (in binary, 0b00111110001000000000000000000000) is divided as:

![图片 1.png](https://i.loli.net/2021/03/03/2pRiw4AK6ng1hLd.png)


The formula of converting a floating-point expression to a decimal value (V) is:
![截屏2021-03-03 下午2.27.08.png](https://i.loli.net/2021/03/03/PCd4lF1kiBDz6us.png)

Where E is:

![截屏2021-03-03 下午2.27.16.png](https://i.loli.net/2021/03/03/Lk8qfCxc5hz3AnQ.png)

For example, let’s convert the above number to decimal.
Notice that the width of exponent field is 8 bits, so we can calculate E first:
![截屏2021-03-03 下午2.27.25.png](https://i.loli.net/2021/03/03/2hpezd7irPK8DSw.png)

And the fraction part is 01000000000000000000000binary, so “0.fraction” is 0.01binary. The expression of rational number in binary is similar to which in decimal. A digit to the left is twice as big as a digit to the right. (e.g., 0.1binary = 0.5decimal, 0.01binary = 0.25decimal).

Hence, (1 + 0.fraction)=1.01binary = 1.25decimal. Finally, the value is

![截屏2021-03-03 下午2.27.35.png](https://i.loli.net/2021/03/03/h4Pq18CjE2RrZti.png)

For more details about standard floats, you can refer to this link: https://en.wikipedia.org/wiki/Single-precision_floating-point_format 

But in this problem, we did a slight modification on the standard floating-point number. Our float still holds 32 bits in memory, and the width of sign field is still 1. But the width of exponent is 7 bits, and width of fraction is 32-1-7=24 bits, which is shown in the following figure

![截屏2021-03-03 下午2.30.46.png](https://i.loli.net/2021/03/03/AKrLZmPTf2BCH1l.png)

In this expression, the same bits above can be converted to:

![截屏2021-03-03 下午2.27.43.png](https://i.loli.net/2021/03/03/mAgfuHqwk3CyOBR.png)



You will receive a non-zero<span style="color: red;">hexadecimal number</span>only. It is guaranteed that, after converting into the floating-point number for this problem, its absolute value is greater than 0.00001.



You need to convert it into a floating-point number using the rules given before, then print the converted number to the screen by this format:

```c
    printf("The number converts to %f\n", result);
```

- Please do not use `double` in your intermediate calculations and result, or your correct answer may get judged wrong due to precision problems. Using `float` is safe for this problem.



