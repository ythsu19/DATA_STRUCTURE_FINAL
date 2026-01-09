
// Sift Up - Push
template <typename T>
void BinaryHeap<T>::siftUp(int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        
        if (data[index] < data[parent]) {
            std::swap(data[index], data[parent]);
            index = parent;
        } else {
            break;
        }
    }
}

// 2. Sift Down - Pop
template <typename T>
void BinaryHeap<T>::siftDown(int index) {
    int leftChild = 2 * index + 1;
    int rightChild = 2 * index + 2;
    int smallest = index;

    // check leftChild exist and whether is smaller
    if (leftChild < (int)data.size() && data[leftChild] < data[smallest]) {
        smallest = leftChild;
    }

    // check rightChild exist and whether is smaller
    if (rightChild < (int)data.size() && data[rightChild] < data[smallest]) {
        smallest = rightChild;
    }

    // exchange
    if (smallest != index) {
        std::swap(data[index], data[smallest]);
        siftDown(smallest); // recursion
    }
}

template <typename T>
bool BinaryHeap<T>::empty() const {
    return data.empty();
}

template <typename T>
size_t BinaryHeap<T>::size() const {
    return data.size();
}

template <typename T>
const T& BinaryHeap<T>::top() const {
    if (empty()) {
        throw std::runtime_error("BinaryHeap::top(): emtpy heap");
    }
    return data[0];
}

template <typename T>
void BinaryHeap<T>::push(const T& value) {
    data.push_back(value);
    siftUp(data.size() - 1);
}

template <typename T>
void BinaryHeap<T>::pop() {
    if (empty()) {
        throw std::runtime_error("BinaryHeap::pop(): heap is empty");
    }
    
    data[0] = data.back();
    data.pop_back();
    
    if (!empty()) {
        siftDown(0);
    }
}

template <typename T>
void BinaryHeap<T>::clear() {
    data.clear();
}