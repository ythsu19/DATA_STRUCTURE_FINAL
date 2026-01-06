#include "pairing_heap.hpp"

#include <iostream>
#include <vector>
#include <string>

int main() {
    try {
        // 1. 測試 int
        std::cout << "--- Int Test ---" << std::endl;
        Opt::PairingHeap<int> ph;
        ph.insert(10);
        ph.insert(5);
        ph.insert(20);
        std::cout << "Min: " << ph.getMin() << " (Expected: 5)" << std::endl;
        ph.deleteMin();
        std::cout << "Min after delete: " << ph.getMin() << " (Expected: 10)" << std::endl;

        // 2. 測試 double
        std::cout << "\n--- Double Test ---" << std::endl;
        Opt::PairingHeap<double> dph;
        dph.insert(3.14);
        dph.insert(1.1);
        std::cout << "Min: " << dph.getMin() << " (Expected: 1.1)" << std::endl;

        // 3. 測試 string (證明 Template 運作正常)
        std::cout << "\n--- String Test ---" << std::endl;
        Opt::PairingHeap<std::string> sph;
        sph.insert("Banana");
        sph.insert("Apple");
        sph.insert("Cherry");
        std::cout << "Min: " << sph.getMin() << " (Expected: Apple)" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}