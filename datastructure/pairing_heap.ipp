template <typename T>
Node<T> *PairingHeap<T>::merge(Node<T> *a, Node<T> *b) {
    if(!a) return b;
    if(!b) return a;

    if(a->key > b->key) {
        Node<T> *tmp = a;
        a = b; b = tmp;
    }
    
    b->prev = a;
    b->sibling = a->child;
    if (a->child) a->child->prev = b;
    a->child = b;

    return a;
}

template <typename T>
Node<T> *PairingHeap<T>::insert(T key) {
    Node<T> *node = new Node<T>(key);
    root = merge(root, node);
    sz++;
    return node;
}

template <typename T>
void PairingHeap<T>::meld(PairingHeap<T>& other) {
    if (other.empty()) return;

    root = merge(root, other.root);
    sz += other.sz;

    other.root = nullptr;
    other.sz = 0;
}

template <typename T>
Node<T> *PairingHeap<T>::twoPassMerge(Node<T> *firstSibling) {
    if(!firstSibling) return nullptr;
    if(!firstSibling->sibling) return firstSibling;

    Node<T> *a = firstSibling;
    Node<T> *b = firstSibling->sibling;
    Node<T> *remaining = b->sibling;

    a->sibling = nullptr;
    b->sibling = nullptr;

    return merge(merge(a, b), twoPassMerge(remaining));
}

template <typename T>
T PairingHeap<T>::deleteMin() {
    if(this->empty()) throw std::runtime_error("PairingHeap::deleteMin(): empty heap");

    Node<T> *oldRoot = root;
    T result = root->key;
    Node<T> *children = root->child;

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

template <typename T>
void PairingHeap<T>::cut(Node<T> *x) {
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

template <typename T>
void PairingHeap<T>::decreaseKey(Node<T> *node, T newKey) {
    if (newKey > node->key) throw std::runtime_error("PairingHeap::decreaseKey: newKey must be <= current key");

    node->key = newKey;

    if (node == root) return;

    cut(node);

    root = merge(root, node);
}

template <typename T>
void PairingHeap<T>::deleteAll(Node<T> *x) {
    if (!x) return;
    
    Node<T> *current = x;

    while (current) {
        Node<T> *next = current->sibling;
        deleteAll(current->child);
        delete current;
        current = next;  
    }
}
