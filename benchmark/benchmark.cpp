#include <iostream>
#include <vector>
#include <queue>
#include <random>
#include <chrono>
#include <fstream>
#include <iomanip>

#include "../baseline/binary_heap.hpp"
#include "../datastructure/pairing_heap.hpp"

using namespace std;

const int INF = 1e9;
const int V_FIXED = 4000;

struct Edge {
    int to;
    int weight;
};

struct State {
    int dist; // min dist
    int vertex; // index of vertex

    // min-pairing-heap
    bool operator>(const State& other) const {
        return dist > other.dist;
    }

    // min-binary-heap
    bool operator<(const State& other) const {
        return dist < other.dist;
    }
};

vector<vector<Edge>> generate_graph(int V, double density) {
    vector<vector<Edge>> adj(V);

    long long max_edges = (long long)(V) * (V - 1); // maximum number of the graph
    long long target_edges = max_edges * (density / 100.0); // target_edges = max_edges * density

    static mt19937 gen(42);
    uniform_int_distribution<> dis_vertex(0, V - 1);
    uniform_int_distribution<> dis_weight(1, 100); // 1 ~ 100

    for (long long i = 0; i < target_edges; ++i) {
        int u = dis_vertex(gen);
        int v = dis_vertex(gen);
        if(u == v) {
            // avoid self-loop
            i--;
            continue;
        }

        adj[u].push_back({v, dis_weight(gen)});
    }

    return adj;
}

// pure-array method
// pure-array method (Linear Scan) - O(V^2)
void dijkstra_brutal(int V, const vector<vector<Edge>> &adj) {
    vector<int> dist(V, INF);
    vector<bool> vis(V, false);

    dist[0] = 0;

    for (int i = 0; i < V; i++) {
        // 1. Find min node (Linear Scan O(V))
        int u = -1;
        int minDist = INF;
        for (int j = 0; j < V; j++) {
            if (!vis[j] && dist[j] < minDist) {
                u = j;
                minDist = dist[j];
            }
        }

        if (u == -1 || dist[u] == INF) break;
        vis[u] = true;

        // 2. Relax edges
        for (const auto &edge : adj[u]) {
            if (dist[u] + edge.weight < dist[edge.to]) {
                dist[edge.to] = dist[u] + edge.weight;
            }
        }
    }
}

// min-binary-heap
void dijkstra_binary(int V, const vector<vector<Edge>> &adj) {
    BinaryHeap<State> pq;
    vector<int> dist(V, INF);

    dist[0] = 0;
    pq.push({0, 0});

    while (!pq.empty()) {
        State top = pq.top();
        pq.pop();

        int d = top.dist;
        int u = top.vertex;

        if (d > dist[u]) continue;

        for (const auto &edge : adj[u]) {
            int v = edge.to;
            int weight = edge.weight;

            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                pq.push({dist[v], v});
            }
        }
    }
}

// min-pairing-heap
void dijkstra_pairing(int V, const vector<vector<Edge>> &adj) {
    PairingHeap<State> pq;
    vector<int> dist(V, INF);
    // 關鍵：用 handles 陣列紀錄指標，達成 O(1) 存取
    vector<Node<State> *> handles(V, nullptr);
    
    dist[0] = 0;
    handles[0] = pq.insert({0, 0});
    
    while (!pq.empty()) {
        State top = pq.getMin();
        pq.deleteMin();

        int u = top.vertex;
        handles[u] = nullptr;
        
        for (const auto &edge : adj[u]) {
            int v = edge.to;
            int weight = edge.weight;
            int new_dist = dist[u] + weight;
            
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

template<typename Func>
double measure_time(Func func) {
    auto start = chrono::high_resolution_clock::now();
    func();
    auto end = chrono::high_resolution_clock::now();
    
    // return (ms)
    return chrono::duration<double, milli>(end - start).count();
}

int main(int argc, char* argv[]) {  
    if (argc > 1) {
        // --- Perf 測試模式 ---
        string mode = argv[1];
        
        // 設定一個固定的測試環境 (高密度、大點數，讓 Cache Miss 更明顯)
        int V_Perf = 5000; 
        double D_Perf = 20.0;
        
        cout << "Perf Mode: Running " << mode << " (V=" << V_Perf << ", D=" << D_Perf << "%)" << endl;
        
        // 產生圖形
        auto adj = generate_graph(V_Perf, D_Perf);

        // 根據參數決定跑哪一個演算法
        if (mode == "brutal") {
            dijkstra_brutal(V_Perf, adj);
        } else if (mode == "binary") {
            dijkstra_binary(V_Perf, adj);
        } else if (mode == "pairing") {
            dijkstra_pairing(V_Perf, adj);
        } else {
            cout << "Unknown mode. Please use 'brutal', 'binary', or 'pairing'." << endl;
        }
        
        return 0; // 跑完就結束
    }
    
    // testing density 
    vector<double> densities = {0.1, 0.5, 1.0, 2.5, 5.0, 10.0, 20.0, 30.0, 40.0, 50.0, 60.0, 70.0, 80.0, 90.0, 100.0};

    ofstream csv("benchmark_result.csv");
    csv << "Density(%),Std_Heap(ms),Pairing_Heap(ms)\n"; // csv title
    
    cout << "Starting Benchmark (V = " << V_FIXED << ")..." << endl;
    cout << fixed << setprecision(2); // 51.23(45....)

    for (double density : densities) {
        cout << "Running Density: " << density << "% ... " << flush;
        
        // data
        auto adj = generate_graph(V_FIXED, density);
        
        // warm up (not in result)
        dijkstra_brutal(V_FIXED, adj);
        dijkstra_binary(V_FIXED, adj);
        dijkstra_pairing(V_FIXED, adj);

        // pure-array brutal (liner-scan)
        double time_brutal = measure_time([&]() {
            dijkstra_brutal(V_FIXED, adj);
        });
        
        // min-binary-heap
        double time_binary = measure_time([&]() {
            dijkstra_binary(V_FIXED, adj);
        });
        
        // min-pairing-heap
        double time_pairing = measure_time([&]() {
            dijkstra_pairing(V_FIXED, adj);
        });
        
        // output record
        cout << "Array: " << time_brutal << "ms | Binay: " << time_binary << "ms | Pairing: " << time_pairing << "ms" << endl;
        csv << density << "," << time_brutal << "," << time_binary << "," << time_pairing << "\n";
    }
    
    cout << "Benchmark finished! Data saved to 'benchmark_result.csv'" << endl;
    return 0;
}