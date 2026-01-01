struct Node{
    int key;
    Node* child;
    Node* sibling;
    Node* parent;
};

class PairingHeap {
private:
    Node* root;
    Node* merge(Node*a, Node*b);
public:
    PairingHeap();
    bool empty() const;
    void insert(int key);
    int extractMin();
};

Node* PairingHeap::merge(Node*a, Node*b) {
    if(!a) return b;
    if(!b) return a;

    if(a->key > b->key) {
        Node *tmp = a;
        a = b; b = tmp;
    }
    
    b->parent = a;
    b->sibling = a->child;
    a->child = b;

    return a;
}