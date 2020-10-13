#include <iostream>
#include <ostream>
#include <string>
#include <stdlib.h>
using namespace std;
struct _listnode {
	int data;
	_listnode *next;
};

class Linkedlist {
private:
	_listnode *header;
	int length;

public:
	Linkedlist();
	~Linkedlist();
	void insert(const int& item);
	void traverse();
	void iterative();
};


Linkedlist::Linkedlist() {
    header = new _listnode();
    header->data = -1;
    header->next = nullptr;
    length = 0;
}

Linkedlist::~Linkedlist() {
    _listnode *qianyige = header;
    _listnode *zhege = header->next;
    _listnode *shan;
    while(zhege != nullptr)
        {qianyige->next=zhege->next;
        shan=zhege;
        zhege=zhege->next;
        free(shan);}
    length = 0;
}


void Linkedlist::iterative(){
cout << "qqqq" <<endl;
}


void Linkedlist::insert(const int& item) {
    if(item>0)
    {_listnode * needinsert = new _listnode();
    needinsert->data = item;
    needinsert->next = header->next;
    header->next = needinsert;
    length++;}
}


void Linkedlist::traverse() {
    _listnode *emm = new _listnode();
    emm = header->next;
    if(emm == nullptr){std::cout << "" ;}
    else{
    while(emm != nullptr)
    {
        int p = (emm->data);
        std::cout << p << " ";
        emm = emm->next;
    }
    std::cout << " " << std::endl;}
}

int main(void) {
	Linkedlist ll;
	std::string sread;
	std::cin >> sread;
	while (sread != "*end*") {
		if (sread == "*insert*") {
			std::cin >> sread;
			int item = atoi(sread.c_str());
			ll.insert(item);
			std::cout << "insert item " << item << std::endl;
		}
		if (sread == "*traverse*") {
			ll.traverse();
		}
        if (sread == "***"){
            ll.iterative();
        }
		std::cin >> sread;
	}

	return 0;
}


