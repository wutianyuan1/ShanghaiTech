#include <iostream>
#include <string>
#define len 5000

char stack[len];

class Opr
{
    public:
        Opr();
        bool isempty();
        void in(const char* item);
        bool pop();
        char gettop();
        int getlength();
        void traverse();
    private:
        int top;
};

Opr::Opr()
{
    top = 0;
    for(int i = 0; i < len; i++)
    {
        stack[i] = 0;
    }
}

bool Opr::isempty()
{
    if (top == 0){return true;}
    else{return false;}
}

int Opr::getlength()
{
    return (top);
}

void Opr::in(const char* item)
{
    stack[top] = *item;
    top++;
}

char Opr::gettop()
{
    return stack[top];
}

bool Opr::pop()
{
    if (top > 0)
    {
        stack[top] = 0;
        top--;
        return true;
    }
    else{return false;}
}

void Opr::traverse()
{
    int i = 0;
    while(stack[i] != 0)
    {
        std::cout << stack[i] << " ";
        i++;
    }
    std::cout << "" << std::endl;
}

int main()
{   Opr ll;
	std::string sread;
	std::cin >> sread;
	while (sread != "*end*") {
		if (sread == "*isEmpty*") {
			std::cout << "Stack is empty: " << std::boolalpha << ll.isempty() << std::endl;
		}

		if (sread == "*getLength*") {
			std::cout << "length: " <<ll.getlength() << std::endl;
		}

		if (sread == "*gettop*") {
			if (ll.gettop() == 0) {
				std::cout << "empty!" << std::endl;
			} else {
				std::cout << "first data: " << ll.gettop() << std::endl;
			}
		}
		if (sread == "*in*") {
			std::cin >> sread;
			const char* item = sread.c_str();
			ll.in(item);
			std::cout << "in item " << item << std::endl;
		}
		if (sread == "*traverse*") {
			ll.traverse();
		}
		if (sread == "*pop*")
        {
            std::cout << "pop item "<< ll.gettop() << " : "<< std::boolalpha << ll.pop() << std::endl;
        }
		std::cin >> sread;
	}
	return 0;
}
