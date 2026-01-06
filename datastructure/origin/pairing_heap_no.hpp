#ifndef PAIRING_HEAP_NO_HPP
#define PAIRING_HEAP_NO_HPP

#include <iostream>
#include <cstddef>
#include <stdexcept>
#include <string>

// #include "memory_pool.hpp" 

namespace Origin {

    template<typename T>
    struct Node {
        T key;
        Node<T> *child;
        Node<T> *sibling;
        Node<T> *prev;

        explicit Node(T k)
            : key(k), child(nullptr), sibling(nullptr), prev(nullptr) {}
    };

    template<typename T>
    class PairingHeap_NO {
    private:
        Node<T> *root;
        std::size_t sz;

        // meld two heaps rooted at a and b, return new root
        Node<T> *merge(Node<T> *a, Node<T> *b) {
            if(!a) return b;
            if(!b) return a;
            if(a->key > b->key) {
                Node<T> *tmp = a;
                a = b;
                b = tmp;
            }
            
            b->prev = a;
            b->sibling = a->child;
            if (a->child) a->child->prev = b;
            a->child = b;

            return a;
        }

        // delete-min helper: two-pass pairing merge on sibling list
        Node<T> *twoPassMerge(Node<T> *firstSibling) {
            if(!firstSibling) return nullptr;
            if(!firstSibling->sibling) return firstSibling;

            Node<T> *a = firstSibling;
            Node<T> *b = firstSibling->sibling;
            Node<T> *remaining = b->sibling;

            a->sibling = nullptr;
            b->sibling = nullptr;

            return merge(merge(a, b), twoPassMerge(remaining));
        }

        // decrease-key helper: cut x from its current position
        void cut(Node<T> *x) {
            Node<T> *previous = x->prev;
            Node<T> *nextSibling = x->sibling;
            if (nextSibling) {
                nextSibling->prev = previous;
            }

            if (previous->child == x){
                previous->child = nextSibling;
            } else {
                previous->sibling = nextSibling;
            }

            x->prev = nullptr;
            x->sibling = nullptr;
        }

        // destructor/clear helper: delete all nodes in subtree
        void deleteAll(Node<T>* x) {
            if (!x) return;
            
            Node<T> *current = x;
            while (current) {
                Node<T> *next = current->sibling;
                deleteAll(current->child);

                // 改回標準 delete
                delete current; 
                // pool.deallocate(current); 

                current = next;
            }
        }

    public:
        PairingHeap_NO() : root(nullptr), sz(0) {}
        ~PairingHeap_NO() { clear(); }

        bool empty() const { return root == nullptr; }
        std::size_t size() const { return sz; }

        // get-min
        T getMin() const {
            if(!root) throw std::runtime_error("PairingHeap_NO::getMin(): empty heap");
            return root->key;
        }

        // insert: return handle for decreaseKey
        Node<T> *insert(T key) {
            // 改回標準 new
            Node<T> *node = new Node<T>(key);
            // Node<T> *node = pool.allocate(key);

            root = merge(root, node);
            sz++;
            return node;
        }

        // meld: consume other (other becomes empty)
        void meld(PairingHeap_NO& other) {
            if (other.empty()) return;

            root = merge(root, other.root);
            sz += other.sz;

            other.root = nullptr;
            other.sz = 0;
        }

        // decrease-key: newKey must be <= node->key
        void decreaseKey(Node<T> *node, T newKey) {
            if (newKey > node->key) throw std::runtime_error("PairingHeap_NO::decreaseKey: newKey must be <= current key");
            node->key = newKey;

            if (node == root) return;

            cut(node);

            root = merge(root, node);
        }

        // delete-min: remove root and return min value
        T deleteMin() {
            if(this->empty()) throw std::runtime_error("PairingHeap_NO::deleteMin(): empty heap");
            Node<T> *oldRoot = root;
            T result = root->key;
            Node<T> *children = root->child;
            if (children) {
                children->prev = nullptr;
            }

            root = twoPassMerge(children);
            
            // 改回標準 delete
            delete oldRoot;
            // pool.deallocate(oldRoot); 

            sz--;
            if (root) {
                root->prev = nullptr;
            }

            return result;
        }

        // free all nodes
        void clear() {
            deleteAll(root);
            root = nullptr;
            sz = 0;
        }
    };

}

#endif