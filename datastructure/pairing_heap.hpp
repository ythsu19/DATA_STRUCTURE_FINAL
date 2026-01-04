#ifndef PAIRING_HEAP_HPP
#define PAIRING_HEAP_HPP

#include <iostream>
#include <cstddef>
#include <stdexcept>
#include <string>

struct Node{
    int key;
    Node *child;
    Node *sibling;
    Node *prev;

    explicit Node(int k)
        : key(k), child(nullptr), sibling(nullptr), prev(nullptr) {}
};

class PairingHeap {
private:
    Node *root;
    std::size_t sz;

    // meld two heaps rooted at a and b, return new root
    static Node* merge(Node *a, Node *b);

    // delete-min helper: two-pass pairing merge on sibling list
    static Node* twoPassMerge(Node* firstSibling);

    // decrease-key helper: cut x from its current position (x becomes a standalone root)
    static void cut(Node* x);

    // destructor/clear helper: delete all nodes in subtree
    static void deleteAll(Node* x);
public:
    PairingHeap() : root(nullptr), sz(0) {}
    ~PairingHeap() { clear(); }

    bool empty() const { return root == nullptr; }
    std::size_t size() const { return sz; }

    // get-min
    int getMin() const {
        if(!root) throw std::runtime_error("PairingHeap::getMin(): empty heap");
        return root->key;
    }

    // insert: return handle for decreaseKey
    Node* insert(int key);

    // meld: consume other (other becomes empty)
    void meld(PairingHeap& other);

    // decrease-key: newKey must be <= node->key
    void decreaseKey(Node* node, int newKey);

    // delete-min: remove root and return min value
    int deleteMin();

    // free all nodes
    void clear() {
        deleteAll(root);
        root = nullptr;
        sz = 0;
    }
};

#endif