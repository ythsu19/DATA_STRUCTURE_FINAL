#ifndef MEMORY_POOL_HPP
#define MEMORY_POOL_HPP

#include <vector>
#include <cstddef>

template <typename T>
class MemoryPool {
private:

    static const size_t BLOCK_SIZE = 4096;

    std::vector<T*> blocks;

    std::vector<T*> freeList;

    size_t currentBlockIndex;

    void expand() {
        T* newBlock = static_cast<T*>(::operator new(BLOCK_SIZE * sizeof(T)));
        blocks.push_back(newBlock);
        currentBlockIndex = 0;
    }

public:
    MemoryPool() {
        currentBlockIndex = BLOCK_SIZE; // firstime allocate expand
    }

    ~MemoryPool() {
        // release all
        for (T* block : blocks) {
            ::operator delete(block);
        }
    }

    // exchange new
    template <typename... Args>
    T* allocate(Args&&... args) {
        T* ptr = nullptr;

        if (!freeList.empty()) {
            ptr = freeList.back();
            freeList.pop_back();
        } 
        else {
            if (currentBlockIndex >= BLOCK_SIZE) {
                expand();
            }
            ptr = blocks.back() + currentBlockIndex;
            currentBlockIndex++;
        }

        new(ptr) T(std::forward<Args>(args)...);
        return ptr;
    }

    void deallocate(T* ptr) {
        if (!ptr) return;
        
        ptr->~T();

        freeList.push_back(ptr);
    }
    
    void reserve(size_t n) {
        while (blocks.size() * BLOCK_SIZE < n) {
            expand();
        }
    }
};

#endif