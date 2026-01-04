#include "pairing_heap.hpp"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>

Node *PairingHeap::merge(Node *a, Node *b) {
    if(!a) return b;
    if(!b) return a;

    if(a->key > b->key) {
        Node *tmp = a;
        a = b; b = tmp;
    }
    
    b->prev = a;
    b->sibling = a->child;
    if (a->child) a->child->prev = b;
    a->child = b;

    return a;
}

Node *PairingHeap::insert(int key) {
    Node *node = new Node(key);
    root = merge(root, node);
    sz++;
    return node;
}

void PairingHeap::meld(PairingHeap& other) {
    if (other.empty()) return;

    root = merge(root, other.root);
    sz += other.sz;

    other.root = nullptr;
    other.sz = 0;
}

Node *PairingHeap::twoPassMerge(Node *firstSibling) {
    if(!firstSibling) return nullptr;
    if(!firstSibling->sibling) return firstSibling;

    Node *a = firstSibling;
    Node *b = firstSibling->sibling;
    Node *remaining = b->sibling;

    a->sibling = nullptr;
    b->sibling = nullptr;

    return merge(merge(a, b), twoPassMerge(remaining));
}

int PairingHeap::deleteMin() {
    if(this->empty()) throw std::runtime_error("PairingHeap::deleteMin(): empty heap");

    Node *oldRoot = root;
    int result = root->key;
    Node *children = root->child;

    if (children) {
        children->prev = nullptr;
    }

    root = twoPassMerge(children);
    
    delete oldRoot;
    sz--;

    if (root) {
        root->prev = nullptr;
    }

    return result;
}

void PairingHeap::cut(Node *x) {
    Node *previous = x->prev;
    Node *nextSibling = x->sibling;

    if (nextSibling) {
        nextSibling->prev = previous;
    }

    // Check x is parent or leftSibling
    if (previous->child == x){
        previous->child = nextSibling;
    } else {
        previous->sibling = nextSibling;
    }

    x->prev = nullptr;
    x->sibling = nullptr;
}

void PairingHeap::decreaseKey(Node *node, int newKey) {
    if (newKey > node->key) throw std::runtime_error("PairingHeap::decreaseKey: newKey must be <= current key");

    node->key = newKey;

    if (node == root) return;

    cut(node);

    root = merge(root, node);
}

void PairingHeap::deleteAll(Node *x) {
    if (!x) return;
    
    Node *current = x;

    while (current) {
        Node *next = current->sibling;
        deleteAll(current->child);
        delete current;
        current = next;  
    }
}
