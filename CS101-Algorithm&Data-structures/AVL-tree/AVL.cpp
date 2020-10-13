#include <iostream>
#include <cstdlib>
#include <string>

#define 这_是_一_个_整_型_变_量 int
#define 我_打_字_带_下_划_线 main

constexpr auto MAX = 100000;

template <class T>
class queue{
private:
    T q[MAX];
    int h = 0;
    int t = 0;
public:
    void push_back(T num);
    void pop_back();
    bool is_empty();
    T getHead();
};

template <class T>
void queue<T>::push_back(T num){
    q[t] = num;
    t++;
}

template <class T>
void queue<T>::pop_back(){
    q[h] = NULL;
    h++;
}


template <class T>
bool queue<T>::is_empty(){
    if (h == t) return true;
    else return false;
}

template <class T>
T queue<T>::getHead(){
    return q[h];
}

class BST{
private:
    class treeNode{
    public:
        friend class BST;
        int data; //储存的数据
        int height; //高度
        treeNode* rchild;
        treeNode* lchild; //俩孩子
        treeNode(const int& key, const int& height = 0);
        ~treeNode();
    };

private:
    treeNode* root; //好像需要一个私密的根
    void lRotate(treeNode* &R); //左旋
    void rRotate(treeNode* &R); //右旋
    void lrRotate(treeNode* &R); //左右旋
    void rlRotate(treeNode* &R); //右左旋
    int setheight(treeNode* R); //最大值
    int getHeight(treeNode* R); //高度，空树的高度是-1
    void balance(treeNode* &R);
    
private:
    bool search(const int& key, treeNode* R); //搜索一个key
    void insert(const int& key, treeNode*& R); //插入一个节点
    treeNode* remove(const int& key, treeNode*& R); //删除这个东西
    treeNode* findSuccesor(treeNode* R){
        treeNode* temp = R->rchild;
        while (temp->lchild != nullptr){
            temp = temp->lchild;
        }
        return temp;
    };
    void bfs(treeNode* R);
    
public:
    BST();
    ~BST();
    int getHeight();
    bool search(const int& key);
    void remove(const int& key);
    void insert(const int& key);
    void bfs();
};

BST::treeNode::treeNode(const int& key, const int& height)
{
    this->data = key;
    this->height = height;
    this->lchild = nullptr;
    this->rchild = nullptr;
}

int BST::getHeight(treeNode* R){
    if (R != nullptr){
        return R->height;
    }
    else return 0;
}

void BST::balance(treeNode* &R){
    if ((getHeight(R->lchild) - getHeight(R->rchild)) >= 2){
        treeNode* temp = R->lchild;
        if (getHeight(temp->lchild) >= getHeight(temp->rchild)){ rRotate(R);}
        else {lrRotate(R); }
    }
    else if (getHeight(R->rchild) - getHeight(R->lchild) >= 2){
        treeNode* temp = R->rchild;
        if (getHeight(temp->rchild) >= getHeight(temp->lchild)){lRotate(R);}
        else {rlRotate(R);}
    }
}

int BST::getHeight(){
    return getHeight(root);
}

BST::treeNode::~treeNode(){}

BST::BST()
{
    root = nullptr;
} //新建一个根节点 如果你的根节点是(2,5) 则需要写BST(2,5)

BST::~BST(){};

int BST::setheight(treeNode* R){
    if (R == nullptr) return 0;
    else{
        int lheight = getHeight(R->lchild);
        int rheight = getHeight(R->rchild);
        if (lheight > rheight) return (lheight + 1);
        else return (rheight + 1);
    }
} //重新为一个节点设置高度

void BST::rRotate(treeNode* &R){
    treeNode* temp = R->lchild;
    R->lchild = temp->rchild;
    temp->rchild = R;
    R->height = setheight(R);
    temp->height = setheight(temp);
    R = temp;
} //右旋

void BST::lRotate(treeNode* &R){
    treeNode* temp = R->rchild;
    R->rchild = temp->lchild;
    temp->lchild = R;
    R->height = setheight(R);
    temp->height = setheight(temp);
    R = temp;
}


void BST::lrRotate(treeNode*& R){
    lRotate(R->lchild);
    rRotate(R);
}


void BST::rlRotate(treeNode*& R){
    rRotate(R->rchild);
    lRotate(R);
}

bool BST::search(const int& key, treeNode* R){
    while(R != nullptr){
        if (key < R->data) R = R->lchild;
        else if (key > R->data) R = R->rchild;
        else {return true;}
    }
    return false;
}

bool BST::search(const int& key){
    return search(key, root);
}

void BST::insert(const int& key, treeNode*& R){
    if (R == nullptr){
        R = new treeNode(key);
    }
    else{
//        右子树插入
        if (key > R->data) insert(key, R->rchild);
        else insert(key, R->lchild);
    }
    balance(R);
    R->height = setheight(R);
}

void BST::insert(const int& key){
    insert(key, root);
}

BST::treeNode* BST::remove(const int& key, treeNode*& R){
    if (R == nullptr){return R;}
    else{
        if (key > R->data){
            R->rchild = remove(key, R->rchild);
        }
        else if (key < R->data){
            R->lchild = remove(key, R->lchild);
        }
        else{
            if (R->lchild != nullptr && R->rchild != nullptr){
                treeNode* s = findSuccesor(R);
                R->data = s->data;
                remove(s->data, R->rchild);
            }
            else{
                treeNode* tmp = R;
                if (R->lchild != nullptr){
                    R = R->lchild;
                }
                else if (R->rchild != nullptr) R = R->rchild;
                else R = nullptr;
                delete tmp;
                tmp = nullptr;
            }
        }
    }
    if (R != nullptr) balance(R);
    if (R != nullptr) R->height = setheight(R);
    return R;
}


void BST::remove(const int& key){
    remove(key, root);
}

void BST::bfs(treeNode* R){
    if (R != nullptr){
        queue<treeNode*> bq;
        bq.push_back(R);
        while (!bq.is_empty()) {
            treeNode* temp = bq.getHead();
            std::cout << temp->data << " ";
            bq.pop_back();
            if (temp->lchild != nullptr){
                bq.push_back(temp->lchild);
            }
            if (temp->rchild != nullptr) {
                bq.push_back(temp->rchild);
            }
        }
    }
}

void BST::bfs(){
    bfs(root);
}


这_是_一_个_整_型_变_量 我_打_字_带_下_划_线(int argc, const char * argv[]) {
    BST tree;
    int n;
    std::cin >> n;
    int i = 0;
    while(i <= n) {
        std::string temp;
        std::getline(std::cin, temp);
        if (temp[0] == 'a') {
            temp = temp.erase(0,4);
            tree.insert(atoi(temp.c_str()));
        }
        else if (temp[0] == 'd'){
            temp = temp.erase(0,4);
            int dkey = atoi(temp.c_str());
            bool flag = tree.search(dkey);
            if (flag == 0){
                std::cout << "No card " << dkey << std::endl;
            }
            else{
                tree.remove(dkey);
            }
        }
        i++;
        tree.bfs();
        printf("\n");
    }
    tree.bfs();
    printf("\n");
    return 0;
}

