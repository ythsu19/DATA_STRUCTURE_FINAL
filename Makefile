# ==========================================
# 設定編譯器與參數
# ==========================================
CXX      := g++
CXXFLAGS := -std=c++17 -O3 -Wall -Wextra

# ==========================================
# 設定 Include 路徑 (關鍵步驟)
# ==========================================
# 這樣做可以讓你在程式碼中直接 #include "binary_heap.hpp" 
# 而不用寫一大串 ../baseline/binary_heap.hpp
INCLUDES := -I./baseline \
            -I./datastructure \
            -I./datastructure/optimize \
            -I./datastructure/origin

# ==========================================
# 定義目標檔案 (執行檔)
# ==========================================
TARGET_BENCH   := benchmark/benchmark
TARGET_MAIN    := datastructure/pairing_heap
TARGET_PLOTTER := plot/plotter

# ==========================================
# 主要規則
# ==========================================
.PHONY: all clean run_bench

# 預設執行 'make' 時會編譯所有目標
all: $(TARGET_BENCH) $(TARGET_MAIN) $(TARGET_PLOTTER)

# 1. 編譯 Benchmark (你指定的需求)
$(TARGET_BENCH): benchmark/benchmark.cpp
	@echo "Compiling Benchmark..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -o $@

# 2. 編譯 Datastructure Main (pairing_heap)
$(TARGET_MAIN): datastructure/main.cpp
	@echo "Compiling Main Pairing Heap..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -o $@

# 3. 編譯 Plotter
$(TARGET_PLOTTER): plot/plotter.cpp
	@echo "Compiling Plotter..."
	$(CXX) $(CXXFLAGS) $< -o $@

# ==========================================
# 工具指令
# ==========================================

# 清除所有產生的執行檔
clean:
	rm -f $(TARGET_BENCH) $(TARGET_MAIN) $(TARGET_PLOTTER)
	rm -f benchmark/*.o datastructure/*.o plot/*.o

# 方便直接跑 benchmark 的指令
run: $(TARGET_BENCH)
	./$(TARGET_BENCH)

# ==========================================
# 自動化實驗流程
# ==========================================
.PHONY: experiment

# 輸入 'make experiment' 就會自動：編譯 -> 跑數據 -> 畫圖
experiment: all
	@echo "--- [1/3] Start Benchmarking ---"
	cd benchmark && ./benchmark
	@echo "--- [2/3] Start Plotting ---"
	cd plot && ./plotter
	@echo "--- [3/3] Done! Check your plot folder. ---"