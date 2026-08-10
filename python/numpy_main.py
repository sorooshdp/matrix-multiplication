import time
import os
import psutil
import numpy as np

SIZE = 1000

def get_memory_usage():
    return psutil.Process(os.getpid()).memory_full_info().uss // 1024

def main():
    # float64 is what BLAS libraries natively operate on
    a = np.full((SIZE, SIZE), 456.0, dtype=np.float64)
    b = np.full((SIZE, SIZE), 456.0, dtype=np.float64)

    initial = get_memory_usage()
    print(f"Initial memory: {initial} KB")
    print(f"NumPy version: {np.__version__}")
    print(f"Matrix size: {SIZE}x{SIZE}")
    print("Library: NumPy (uses BLAS backend — check np.show_config())")
    np.show_config()

    # Warmup (first call triggers thread pool / BLAS initialization)
    _ = a @ b

    min_time = float('inf')
    peak_mem = initial

    for i in range(50):
        before = get_memory_usage()
        start = time.perf_counter()
        c = a @ b                  # calls BLAS dgemm internally
        duration = time.perf_counter() - start
        after = get_memory_usage()
        peak_mem = max(peak_mem, after)
        print(f"Run {i+1:2d} - Before: {before} KB, Delta: {after-before} KB, "
              f"Time: {duration:.6f} s")
        min_time = min(min_time, duration)

    print(f"\nPeak memory: {peak_mem} KB")
    print(f"Total increase: {peak_mem - initial} KB")
    print(f"Min time: {min_time:.6f} s")
    print(f"Result[0][0]: {c[0][0]}  (expected {456*456*SIZE})")
    print("First 10 elements:", c.flatten()[:10])

if __name__ == "__main__":
    main()