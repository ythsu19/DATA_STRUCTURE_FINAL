#include <queue>
#include <vector>
#include <functional>

class BinaryHeapWrapper {
private:
    // min-heap
    std::priority_queue<int, std::vector<int>, std::greater<int>> pq;

public:
    void insert(int x) {
        pq.push(x);
    }

    int extractMin() {
        int ret = pq.top();
        pq.pop();
        return ret;
    }

    bool empty() const {
        return pq.empty();
    }
};
