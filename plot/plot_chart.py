

import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import sys
import os

try:
    # 1. 讀取 CSV
    csv_path = '/home/pkboie/DS/DATA_STRUCTURE_FINAL/benchmark/benchmark_result.csv'

    print(f"Reading data from: {csv_path}")
    df = pd.read_csv(csv_path)
    
    # 2. 設定畫布
    plt.figure(figsize=(14, 9)) # 加大畫布高度
    plt.style.use('ggplot')
    
    # 3. 繪製折線圖
    
    # (A) Linear Scan (用淡灰色虛線，避免視覺干擾)
    if 'Linear(ms)' in df.columns:
        plt.plot(df['Density(%)'], df['Linear(ms)'], 
                 label='Linear Scan', color='black', linestyle='-', alpha=0.3)

    # (B) Standard Priority Queue (Baseline - 藍色圓點)
    if 'Std_PQ(ms)' in df.columns:
        plt.plot(df['Density(%)'], df['Std_PQ(ms)'], 
                 label='std::priority_queue', color='royalblue', marker='o', markersize=5, linewidth=2)

    # (C) Binary Heap (Hand-written - 綠色三角形)
    if 'Binary(ms)' in df.columns:
        plt.plot(df['Density(%)'], df['Binary(ms)'], 
                 label='Binary Heap (Hand)', color='forestgreen', marker='^', markersize=5, linestyle='-.')

    # (D) Pairing Heap No Pool (橘色點線)
    if 'Pairing_NoPool(ms)' in df.columns:
        plt.plot(df['Density(%)'], df['Pairing_NoPool(ms)'], 
                 label='Pairing Heap (No Pool)', color='darkorange', linestyle=':', linewidth=2)

    # (E) Pairing Heap Optimized (紅色星號 - 重點強調)
    # 檢查欄位名稱是 Pool 還是 OPT
    col_opt = 'Pairing_OPT(ms)' if 'Pairing_OPT(ms)' in df.columns else 'Pairing_Pool(ms)'
    
    if col_opt in df.columns:
        plt.plot(df['Density(%)'], df[col_opt], 
                 label='Pairing Heap (Memory Pool)', color='firebrick', marker='*', linewidth=2.5, markersize=10, zorder=10)

    # 4. 精細化軸線設定 (Zoom In)
    ax = plt.gca()

    # --- X 軸: 每 5% 一個刻度 ---
    ax.xaxis.set_major_locator(ticker.MultipleLocator(5))
    
    # --- Y 軸: 自動縮放以觀察 Heap 差異 (忽略 Linear Scan) ---
    # 找出除了 Linear 和 Density 以外的所有數值
    cols_to_check = [c for c in df.columns if 'Linear' not in c and 'Density' not in c]
    
    if cols_to_check:
        # 找出這些欄位中的最大值
        max_val_heap = df[cols_to_check].max().max()
        
        # 設定 Y 軸上限為最大 Heap 時間的 1.25 倍
        # 這樣 Linear Scan 的線會衝出圖表，但下方的細節會非常清楚
        plt.ylim(0, max_val_heap * 1.25)
        print(f"Y-axis scaled to 0 - {max_val_heap * 1.25:.2f} ms (ignoring Linear Scan outliers)")

    # 設定次要刻度 (Minor Ticks)
    ax.yaxis.set_minor_locator(ticker.AutoMinorLocator())
    ax.xaxis.set_minor_locator(ticker.AutoMinorLocator())

    # --- 格線設定 ---
    # 主格線 (白色實線)
    plt.grid(True, which='major', linestyle='-', linewidth=1.0, color='white')
    # 次格線 (淡色虛線，方便讀數)
    plt.grid(True, which='minor', linestyle=':', linewidth=0.5, color='gray', alpha=0.5)

    # 5. 標題與標籤
    plt.title('Dijkstra Algorithm Benchmark: Impact of Graph Density', fontsize=16, pad=20)
    plt.xlabel('Graph Density (%)', fontsize=14)
    plt.ylabel('Time (ms)', fontsize=14)
    plt.legend(fontsize=12, loc='upper left', framealpha=0.9, shadow=True)
    
    # 6. 自動標註 Pairing Heap 的優勢 (Annotation)
    if 'Binary(ms)' in df.columns and col_opt in df.columns:
        # 找出 Pairing Pool 比 Binary 快的地方
        diff_series = df['Binary(ms)'] - df[col_opt]
        
        # 為了避免雜訊，我們只標註差距最大的一點
        if diff_series.max() > 0:
            max_idx = diff_series.idxmax()
            best_density = df.loc[max_idx, 'Density(%)']
            best_time = df.loc[max_idx, col_opt]
            time_diff = diff_series[max_idx]
            
            # 只有當這個點在 Y 軸範圍內才標註
            if best_time < max_val_heap * 1.25:
                text = f"Pairing Heap Lead\n-{time_diff:.2f} ms\n(@{best_density}%)"
                
                plt.annotate(text, 
                             xy=(best_density, best_time), 
                             xytext=(best_density + 5, best_time - (max_val_heap * 0.1)),
                             arrowprops=dict(facecolor='black', shrink=0.05, width=2),
                             fontsize=11, 
                             bbox=dict(boxstyle="round,pad=0.3", fc="white", ec="black", alpha=0.8))

    # 7. 存檔
    output_file = 'benchmark_analysis.png'
    plt.tight_layout()
    plt.savefig(output_file, dpi=300)
    print(f"Success! Plot saved to '{output_file}'")

except Exception as e:
    print("An error occurred in Python script:")
    print(e)
    sys.exit(1)

