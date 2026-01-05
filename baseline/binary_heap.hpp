#ifndef BINARY_HEAP_HPP
#define BINARY_HEAP_HPP

#include <vector>
#include <stdexcept>
#include <iostream>

template <typename T>
class BinaryHeap {
private:
    // vector base
    std::vector<T> data;

    // Percolate Up / Sift Up
    void siftUp(int index);

    // Percolate Down / Sift Down
    void siftDown(int index);

public:
    BinaryHeap() = default;
    ~BinaryHeap() = default;

    bool empty() const;
    size_t size() const;
    
    const T& top() const;

    void push(const T& value);

    void pop();

    void clear();
};


#include "binary_heap.ipp"

#endif