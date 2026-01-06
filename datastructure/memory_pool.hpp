#ifndef MEMORY_POOL_HPP
#define MEMORY_POOL_HPP

#include <vector>
#include <cstddef>

template <typename T>
class MemoryPool {
private:
    // 定義一個區塊 (Block) 大小，例如每次申請 1024 個節點
    // 你可以根據需求調整，Dijkstra 這種 V 很大的，可以設大一點 (例如 4096)
    static const size_t BLOCK_SIZE = 4096;

    // 存放所有區塊的列表
    // 每個元素都是一個指向 "T 陣列" 的指標
    std::vector<T*> blocks;

    // 資源回收桶 (Free List)
    // 當使用者 delete 一個節點時，我們把那個位址存進來
    // 下次要 allocate 時，優先從這裡拿
    std::vector<T*> freeList;

    // 目前區塊用到第幾個位置了
    size_t currentBlockIndex;

    // 擴充一個新區塊
    void expand() {
        // 配置一塊新的記憶體 (大小為 BLOCK_SIZE 個 T)
        // 使用 operator new 來配置原始記憶體，避免呼叫預設建構子
        T* newBlock = static_cast<T*>(::operator new(BLOCK_SIZE * sizeof(T)));
        blocks.push_back(newBlock);
        currentBlockIndex = 0;
    }

public:
    MemoryPool() {
        currentBlockIndex = BLOCK_SIZE; // 初始狀態設為滿，強迫第一次 allocate 時 expand
    }

    ~MemoryPool() {
        // 解構時，釋放所有區塊
        for (T* block : blocks) {
            ::operator delete(block);
        }
    }

    // 配置一個物件 (取代 new)
    // Args... args 是為了支援帶參數的建構子 (Perfect Forwarding)
    template <typename... Args>
    T* allocate(Args&&... args) {
        T* ptr = nullptr;

        // 1. 如果回收桶有東西，優先從回收桶拿
        if (!freeList.empty()) {
            ptr = freeList.back();
            freeList.pop_back();
        } 
        else {
            // 2. 如果目前區塊滿了，就擴充新區塊
            if (currentBlockIndex >= BLOCK_SIZE) {
                expand();
            }
            // 3. 從目前區塊拿一個位置
            ptr = blocks.back() + currentBlockIndex;
            currentBlockIndex++;
        }

        // 在這塊記憶體上呼叫建構子 (Placement New)
        new(ptr) T(std::forward<Args>(args)...);
        return ptr;
    }

    // 釋放一個物件 (取代 delete)
    void deallocate(T* ptr) {
        if (!ptr) return;
        
        // 1. 呼叫解構子
        ptr->~T();

        // 2. 把位址丟回回收桶
        freeList.push_back(ptr);
    }
    
    // 為了 Dijkstra 優化：預先保留空間
    // 如果你知道大概會用多少節點 (例如 V 個)，可以先呼叫這個
    void reserve(size_t n) {
        while (blocks.size() * BLOCK_SIZE < n) {
            expand();
        }
    }
};

#endif // MEMORY_POOL_HPP