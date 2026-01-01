#include <iostream>
#include <stdexcept>
#include <string>

struct Node{
    int key;
    Node* child;
    Node* sibling;

    Node(int k) {
        key = k;
        child = nullptr;
        sibling = nullptr;
    }
};

class PairingHeap {
private:
    Node *root;
    Node *merge(Node *a, Node *b);
public:
    PairingHeap();
    bool empty() const;
    void insert(int key);
    int getMin() const;
    int extractMin();
};

PairingHeap::PairingHeap() : root(nullptr) {}

bool PairingHeap::empty() const {
    return root == nullptr;
}

int PairingHeap::getMin() const {
    if(!root) throw std::runtime_error("empty heap");
    return root->key;
}

Node* PairingHeap::merge(Node*a, Node*b) {
    if(!a) return b;
    if(!b) return a;

    if(a->key > b->key) {
        Node *tmp = a;
        a = b; b = tmp;
    }
    
    b->sibling = a->child;
    a->child = b;

    return a;
}

void PairingHeap::insert(int key){
    Node *node = new Node(key);
    root = merge(root, node);
}