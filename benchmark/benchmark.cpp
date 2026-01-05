#include <iostream>
#include <vector>
#include <queue>
#include <random>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <algorithm> // for std::sort

// 引用你的 Pairing Heap
#include "../datastructure/pairing_heap.hpp"

using namespace std;

// 定義常數
const int INF = 1e9;
const int NUM_RUNS = 5; // 每個密度重複跑 5 次取平均
const int V_FIXED = 4000; // 固定頂點數 (依電腦效能調整，建議 3000~5000)

struct Edge {
    int to;
    int weight;
};

// 用於 Pairing Heap 的結構
struct State {
    int dist;
    int vertex;
    // Min-Heap 比較運算子
    bool operator>(const State& other) const {
        return dist > other.dist;
    }
};

// --- 1. 圖產生器 (固定種子) ---
vector<vector<Edge>> generate_graph(int V, double density) {
    vector<vector<Edge>> adj(V);
    long long max_edges = (long long)V * (V - 1);
    long long target_edges = max_edges * (density / 100.0);
    
    // 使用固定種子 42，確保每次與 Std Heap 比較的圖都是同一張
    static mt19937 gen(42); 
    uniform_int_distribution<> dis_vertex(0, V - 1);
    uniform_int_distribution<> dis_weight(1, 100);

    // 為了速度，這是一個允許重複邊的簡易生成器
    // 在稠密圖測試中，這對 Dijkstra 效能影響不大，但生成速度快很多
    for (long long i = 0; i < target_edges; ++i) {
        int u = dis_vertex(gen);
        int v = dis_vertex(gen);
        if (u == v) { i--; continue; } // 忽略自環
        adj[u].push_back({v, dis_weight(gen)});
    }
    return adj;
}

// --- 2. 選手 A: Standard Library (Lazy) ---
void dijkstra_std(int V, const vector<vector<Edge>>& adj) {
    // greater 讓它是 Min-Heap
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    vector<int> dist(V, INF);
    
    dist[0] = 0;
    pq.push({0, 0});
    
    while (!pq.empty()) {
        int d = pq.top().first;
        int u = pq.top().second;
        pq.pop();
        
        // Lazy Deletion 檢查：如果是過期的路徑，丟掉
        if (d > dist[u]) continue;
        
        for (const auto& edge : adj[u]) {
            if (dist[u] + edge.weight < dist[edge.to]) {
                dist[edge.to] = dist[u] + edge.weight;
                pq.push({dist[edge.to], edge.to}); // 直接 Push 新的
            }
        }
    }
}

// --- 3. 選手 B: Pairing Heap (Eager DecreaseKey) ---
void dijkstra_pairing(int V, const vector<vector<Edge>>& adj) {
    PairingHeap<State> pq;
    vector<int> dist(V, INF);
    // 關鍵：用 handles 陣列紀錄指標，達成 O(1) 存取
    vector<Node<State>*> handles(V, nullptr);
    
    dist[0] = 0;
    handles[0] = pq.insert({0, 0});
    
    while (!pq.empty()) {
        State top = pq.getMin();
        pq.deleteMin();
        int u = top.vertex;
        handles[u] = nullptr; // 從 Heap 移除了，清空指標
        
        for (const auto& edge : adj[u]) {
            int v = edge.to;
            int new_dist = dist[u] + edge.weight;
            
            if (new_dist < dist[v]) {
                dist[v] = new_dist;
                if (handles[v] == nullptr) {
                    // 該點不在 Heap 中 -> Insert
                    handles[v] = pq.insert({new_dist, v});
                } else {
                    // 該點已在 Heap 中 -> Decrease Key (關鍵優勢)
                    pq.decreaseKey(handles[v], {new_dist, v});
                }
            }
        }
    }
}

// --- 計時器輔助函式 ---
template<typename Func>
double measure_time(Func func) {
    auto start = chrono::high_resolution_clock::now();
    func();
    auto end = chrono::high_resolution_clock::now();
    return chrono::duration<double, milli>(end - start).count();
}

int main() {
    ofstream csv("benchmark_result.csv");
    csv << "Density(%),Std_Heap(ms),Pairing_Heap(ms)\n"; // CSV 標題
    
    // 密度測試點：從稀疏 (0.1%) 到 稠密 (50%)
    // 稠密圖 V=4000 時邊數極多，跑 100% 可能會很久，建議跑道 40-50% 即可看趨勢
    vector<double> densities = {0.1, 0.5, 1.0, 2.5, 5.0, 10.0, 20.0, 30.0, 40.0, 50.0};
    
    cout << "Starting Benchmark (V=" << V_FIXED << ", Runs=" << NUM_RUNS << " per density)...\n";
    cout << fixed << setprecision(2);

    for (double d : densities) {
        cout << "Running Density: " << d << "% ... ";
        
        vector<double> times_std;
        vector<double> times_pairing;
        
        // 為了消除雜訊，每個密度產生一張圖，但跑 NUM_RUNS 次 (或是產生 NUM_RUNS 張不同的圖)
        // 這裡我們選擇：產生一張圖，跑多次取平均 (測試演算法對同一張圖的穩定性)
        auto adj = generate_graph(V_FIXED, d);
        
        // 暖身 (Warm-up) - 跑一次不算分
        dijkstra_std(V_FIXED, adj);
        dijkstra_pairing(V_FIXED, adj);
        
        // 正式測試
        for (int r = 0; r < NUM_RUNS; r++) {
            times_std.push_back(measure_time([&](){ dijkstra_std(V_FIXED, adj); }));
            times_pairing.push_back(measure_time([&](){ dijkstra_pairing(V_FIXED, adj); }));
        }
        
        // 取平均值
        double avg_std = 0, avg_pairing = 0;
        for(double t : times_std) avg_std += t;
        for(double t : times_pairing) avg_pairing += t;
        avg_std /= NUM_RUNS;
        avg_pairing /= NUM_RUNS;
        
        cout << "Std: " << avg_std << "ms | Pairing: " << avg_pairing << "ms" << endl;
        csv << d << "," << avg_std << "," << avg_pairing << "\n";
    }
    
    cout << "Benchmark finished! Data saved to 'benchmark_result.csv'" << endl;
    return 0;
}