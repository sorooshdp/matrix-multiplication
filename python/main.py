import time
import psutil
import os
import multiprocessing as mp
from multiprocessing import Pool

SIZE = 1000

def get_memory_usage():
    return psutil.Process(os.getpid()).memory_full_info().uss // 1024

def worker_function(args):
    a, b, start_row, end_row = args
    
    results = []
    for i in range(start_row, end_row):
        row = [0] * SIZE
        for j in range(SIZE):
            total = 0
            for k in range(SIZE):
                total += a[i * SIZE + k] * b[k * SIZE + j]
            row[j] = total
        results.append((i, row))
    
    return results

def matrix_mul_parallel(a, b):
    num_processes = mp.cpu_count()
    rows_per_process = SIZE // num_processes
    remaining_rows = SIZE % num_processes
    
    work_chunks = []
    for process_id in range(num_processes):
        start_row = process_id * rows_per_process
        end_row = start_row + rows_per_process
        if process_id == num_processes - 1:
            end_row += remaining_rows
        
        work_chunks.append((a, b, start_row, end_row))
    
    with Pool(processes=num_processes) as pool:
        all_results = pool.map(worker_function, work_chunks)
    
    c = [0] * (SIZE * SIZE)
    for process_results in all_results:
        for row_index, row_data in process_results:
            for j in range(SIZE):
                c[row_index * SIZE + j] = row_data[j]
    
    return c

def matrix_mul(a, b):
    c = [0] * (SIZE * SIZE)
    
    for i in range(SIZE):
        for j in range(SIZE):
            total = 0
            for k in range(SIZE):
                total += a[i * SIZE + k] * b[k * SIZE + j]
            c[i * SIZE + j] = total
    
    return c

def main():
    a = [456] * (SIZE * SIZE)
    b = [456] * (SIZE * SIZE)
    
    initial = get_memory_usage()
    print(f"Initial memory: {initial} KB")
    print(f"Using {mp.cpu_count()} processes")
    
    min_time = float('inf')
    peak_mem = initial
    
    for i in range(50):
        before_mem = get_memory_usage()
        print(f"Run {i+1} - Before: {before_mem} KB")
        
        start = time.perf_counter()
        # you can switch between matrix_mul and matrix_mul_parallel to test both implementations
        c = matrix_mul_parallel(a, b)
        duration = time.perf_counter() - start
        after_mem = get_memory_usage()
        peak_mem = max(peak_mem, after_mem)
        
        print(f"Delta: {after_mem - before_mem} KB")
        print(f"Time: {duration:.6f} s")
        
        min_time = min(min_time, duration)
    
    print(f"\nPeak: {peak_mem} KB")
    print(f"Total increase: {peak_mem - initial} KB")
    print(f"Min time: {min_time:.6f} s")
    # a view of the first 10 elements of the result matrix
    print("Result (first 10 elements):", c[:10])



if __name__ == "__main__":
    main()