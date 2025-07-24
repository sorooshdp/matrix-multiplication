#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <psapi.h>
#endif

constexpr int MATRIX_SIZE = 500; 

using Matrix = std::vector<int>;  

size_t get_memory_usage() {
    #ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    return pmc.PrivateUsage / 1024;
    #else
    return 0;
    #endif
}

Matrix matrix_mul(const Matrix& a, const Matrix& b) {
    Matrix c(MATRIX_SIZE * MATRIX_SIZE, 0); 
    
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            int sum = 0;
            for (int k = 0; k < MATRIX_SIZE; k++) {
                sum += a[i * MATRIX_SIZE + k] * b[k * MATRIX_SIZE + j];
            }
            c[i * MATRIX_SIZE + j] = sum;
        }
    }
    
    return c;
}

int main() {
    Matrix a(MATRIX_SIZE * MATRIX_SIZE, 456); 
    Matrix b(MATRIX_SIZE * MATRIX_SIZE, 456); 

    size_t initial_memory = get_memory_usage();
    std::cout << "Initial memory: " << initial_memory << " KB\n";

    double min_duration = std::numeric_limits<double>::max();
    size_t peak_memory = initial_memory;

    for (int i = 0; i < 50; i++) {
        size_t before_mem = get_memory_usage();
        std::cout << "Run " << (i+1) << " - Memory before: " << before_mem << " KB\n";

        auto start = std::chrono::high_resolution_clock::now();
        Matrix c = matrix_mul(a, b);
        auto end = std::chrono::high_resolution_clock::now();
        
        size_t after_mem = get_memory_usage();
        peak_memory = std::max(peak_memory, after_mem);

        std::chrono::duration<double> duration = end - start;
        std::cout << "Memory delta: " << (after_mem - before_mem) << " KB\n";
        std::cout << "Time: " << duration.count() << " s\n";
        
        min_duration = std::min(min_duration, duration.count());
    }

    std::cout << "\nPeak memory: " << peak_memory << " KB\n";
    std::cout << "Total increase: " << (peak_memory - initial_memory) << " KB\n";
    std::cout << "Min time: " << min_duration << " s\n";

    std::cout << "First 10 elements of result matrix: ";
    Matrix c = matrix_mul(a, b);
    for (int i = 0; i < 10 && i < MATRIX_SIZE * MATRIX_SIZE; i++) {
        std::cout << c[i] << " ";
    }

    std::cout << "Press Enter to exit...\n";
    std::cin.get();
}