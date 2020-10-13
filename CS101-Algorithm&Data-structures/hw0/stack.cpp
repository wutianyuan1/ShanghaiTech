#include <iostream>
#include <stdio.h>
#include <string.h>

using namespace std;

int main()
{
    string str;
    string s;
    getline(cin,str);
    int len = str.size();
    char stack[1000];
    int ptr = 0;
    int re = 20;
    bool flag = true;
    for(int i = 0; i<len; i++)
    {
        char a = str[i];
        if (a == '{')
                {stack[ptr] = '{';
                ptr++;}
        if (a == '(')
                {stack[ptr] = '(';
                ptr++;}
        if (a == '[')
                {stack[ptr] = '[';
                ptr++;}
        if (a == '}')
        {
            if (ptr == 0){flag = false;}
            else {if (stack[ptr-1] == '{'){ptr--;}
                    else{re = 10;}}
        }
        if (a == ']')
        {
            if (ptr == 0){flag = false;}
            else {if (stack[ptr-1] == '['){ptr--;}
                    else{re = 10;}}
        }
        if (a == ')')
        {
            if (ptr == 0){flag = false;}
            else {if (stack[ptr-1] == '('){ptr--;}
                    else{re = 10;}}
        }
        }
    if ((ptr == 0) && (flag == true)){re = 100;}
    if (re == 100){cout << "Match" << endl;}
    else {cout << "Not match" << endl;}
    return 0;
}
