#ifndef PAIRING_HEAP_HPP
#define PAIRING_HEAP_HPP

#include <iostream>
#include <cstddef>
#include <stdexcept>
#include <string>

#include "memory_pool.hpp"

template<typename T>
struct Node{
    T key;
    Node<T> *child;
    Node<T> *sibling;
    Node<T> *prev;

    explicit Node(T k)
        : key(k), child(nullptr), sibling(nullptr), prev(nullptr) {}
};

template<typename T>
class PairingHeap {
private:
    Node<T> *root;
    std::size_t sz;

    MemoryPool<Node<T>> pool;

    // meld two heaps rooted at a and b, return new root
    static Node<T> *merge(Node<T> *a, Node<T> *b);

    // delete-min helper: two-pass pairing merge on sibling list
    static Node<T> *twoPassMerge(Node<T> *firstSibling);

    // decrease-key helper: cut x from its current position (x becomes a standalone root)
    static void cut(Node<T> *x);

    // destructor/clear helper: delete all nodes in subtree
    void deleteAll(Node<T>* x);
public:
    PairingHeap() : root(nullptr), sz(0) {}
    ~PairingHeap() { clear(); }

    bool empty() const { return root == nullptr; }
    std::size_t size() const { return sz; }

    // get-min
    T getMin() const {
        if(!root) throw std::runtime_error("PairingHeap::getMin(): empty heap");
        return root->key;
    }

    // insert: return handle for decreaseKey
    Node<T> *insert(T key);

    // meld: consume other (other becomes empty)
    void meld(PairingHeap& other);

    // decrease-key: newKey must be <= node->key
    void decreaseKey(Node<T> *node, T newKey);

    // delete-min: remove root and return min value
    T deleteMin();

    // free all nodes
    void clear() {
        deleteAll(root);
        root = nullptr;
        sz = 0;
    }
};

#include "pairing_heap.ipp"

#endif