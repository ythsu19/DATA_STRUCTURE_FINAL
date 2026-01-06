
import pandas as pd
import matplotlib.pyplot as plt
import sys

try:
    # 1. 讀取 CSV
    df = pd.read_csv('../benchmark/benchmark_result.csv')
    
    # 2. 設定畫布大小與風格
    plt.figure(figsize=(12, 7))
    plt.style.use('ggplot') # 使用漂亮的風格
    
    # 3. 繪製折線圖
    # X軸: Density, Y軸: 各個方法的時間
    
    # Linear Scan (通常很慢，用虛線表示)
    if 'Linear(ms)' in df.columns:
        plt.plot(df['Density(%)'], df['Linear(ms)'], 
                 label='Linear Scan', color='grey', linestyle='--', alpha=0.5)

    # Standard Priority Queue (Baseline)
    if 'Std_PQ(ms)' in df.columns:
        plt.plot(df['Density(%)'], df['Std_PQ(ms)'], 
                 label='std::priority_queue', color='blue', marker='o', linewidth=2)

    # Hand-written Binary Heap
    if 'Binary(ms)' in df.columns:
        plt.plot(df['Density(%)'], df['Binary(ms)'], 
                 label='Binary Heap (Hand)', color='green', marker='^', linestyle='-.')

    # Pairing Heap (No Pool)
    if 'Pairing_NoPool(ms)' in df.columns:
        plt.plot(df['Density(%)'], df['Pairing_NoPool(ms)'], 
                 label='Pairing Heap (No Pool)', color='orange', linestyle=':')

    # Pairing Heap (Optimized / Pool) - 這是我們的主角
    # 注意：根據你提供的數據，最後一欄可能是 Pairing_OPT(ms) 或 Pairing_Pool(ms)
    # 這裡做一個檢查
    col_opt = 'Pairing_OPT(ms)' if 'Pairing_OPT(ms)' in df.columns else 'Pairing_Pool(ms)'
    if col_opt in df.columns:
        plt.plot(df['Density(%)'], df[col_opt], 
                 label='Pairing Heap (Memory Pool)', color='red', marker='*', linewidth=2.5, markersize=10)

    # 4. 圖表裝飾
    plt.title('Dijkstra Algorithm Performance: Density vs Time', fontsize=16)
    plt.xlabel('Graph Density (%)', fontsize=12)
    plt.ylabel('Time (ms)', fontsize=12)
    plt.legend(fontsize=10)
    plt.grid(True, which='both', linestyle='--', linewidth=0.5)
    
    # 設定 Y 軸從 0 開始，這樣差距看起來才真實
    plt.ylim(bottom=0)
    
    # 5. 標註 Pairing Heap 的優勢區間 (Annotation)
    # 找出 Pairing Pool 贏過 Binary Heap 的點
    if 'Binary(ms)' in df.columns and col_opt in df.columns:
        wins = df[df[col_opt] < df['Binary(ms)']]
        if not wins.empty:
            # 標註最大贏面
            max_win_idx = (df['Binary(ms)'] - df[col_opt]).idxmax()
            win_density = df.loc[max_win_idx, 'Density(%)']
            win_diff = df.loc[max_win_idx, 'Binary(ms)'] - df.loc[max_win_idx, col_opt]
            
            plt.annotate(f'Best Performance Gap\n(-{win_diff:.2f}ms)', 
                         xy=(win_density, df.loc[max_win_idx, col_opt]), 
                         xytext=(win_density, df.loc[max_win_idx, col_opt] - 5),
                         arrowprops=dict(facecolor='black', shrink=0.05))

    # 6. 儲存圖片
    plt.savefig('benchmark_analysis.png', dpi=300)
    print("Success: Plot saved to 'benchmark_analysis.png'")

except Exception as e:
    print(f"Error in Python script: {e}")
