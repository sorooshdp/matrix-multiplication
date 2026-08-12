#include <iostream>
#include <vector>
#include <chrono>
#include <limits>
#include <cblas.h>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <psapi.h>
#endif

constexpr int MATRIX_SIZE = 1000;

size_t get_memory_usage_kb() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(),
                             (PROCESS_MEMORY_COUNTERS*)&pmc,
                             sizeof(pmc))) {
        /// private usage tracks committed virtual memory, not physical RAM
        return pmc.PrivateUsage / 1024;
    }
#endif
    return 0;
}

size_t get_working_set_size() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(),
                             (PROCESS_MEMORY_COUNTERS*)&pmc,
                             sizeof(pmc))) {
        /// WorkingSetSize is the actual physical RAM currently used by the process
        return pmc.WorkingSetSize / 1024;
    }
#endif
    return 0;
}

int main() {
#ifdef _WIN32
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
#endif

    std::vector<double> a(MATRIX_SIZE * MATRIX_SIZE, 456.0);
    std::vector<double> b(MATRIX_SIZE * MATRIX_SIZE, 456.0);
    std::vector<double> c(MATRIX_SIZE * MATRIX_SIZE, 0.0);

    std::cout << "Matrix size: " << MATRIX_SIZE << "x" << MATRIX_SIZE << "\n";
    std::cout << "Library: OpenBLAS (cblas_dgemm, double precision)\n";

    // Warmup
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                MATRIX_SIZE, MATRIX_SIZE, MATRIX_SIZE,
                1.0, a.data(), MATRIX_SIZE,
                b.data(), MATRIX_SIZE,
                0.0, c.data(), MATRIX_SIZE);

    size_t initial_memory = get_working_set_size();
    std::cout << "Initial memory: " << initial_memory << " KB\n";

    double min_duration = std::numeric_limits<double>::max();
    size_t peak_memory = initial_memory;

    for (int i = 0; i < 50; i++) {
        size_t before_mem = get_working_set_size();
        auto start = std::chrono::high_resolution_clock::now();

        cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                    MATRIX_SIZE, MATRIX_SIZE, MATRIX_SIZE,
                    1.0, a.data(), MATRIX_SIZE,
                    b.data(), MATRIX_SIZE,
                    0.0, c.data(), MATRIX_SIZE);

        auto end = std::chrono::high_resolution_clock::now();
        size_t after_mem = get_working_set_size();
        peak_memory = std::max(peak_memory, after_mem);

        std::chrono::duration<double> duration = end - start;
        if (duration.count() < min_duration) min_duration = duration.count();
        std::cout << "Run " << (i + 1) << ": " << duration.count()
                  << " s, Memory delta: "
                  << (long long)(after_mem - before_mem) << " KB\n";
    }

    std::cout << "\nMin time: " << min_duration << " s\n";
    std::cout << "Peak memory: " << peak_memory << " KB\n";
    std::cout << "Total memory increase: "
              << (peak_memory - initial_memory) << " KB\n";

    std::cout << "First 10 elements of result: ";
    for (int i = 0; i < 10; i++) std::cout << c[i] << " ";
    std::cout << "\nExpected: " << (456.0 * 456.0 * MATRIX_SIZE) << "\n";

    std::cout << "Press Enter to exit...";
    std::cin.get();
    return 0;
}