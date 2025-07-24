#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <algorithm>
#include <omp.h>
#include <limits>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <psapi.h>
#endif

constexpr int MATRIX_SIZE = 500;
constexpr uint32_t MATRIX_VALUE = 456;  
using Matrix = std::vector<uint32_t>;  

size_t get_memory_usage_kb() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        return pmc.PrivateUsage / 1024;
    }
#endif
    return 0;
}

Matrix create_matrix() {
    return Matrix(MATRIX_SIZE * MATRIX_SIZE, MATRIX_VALUE);
}

Matrix transpose(const Matrix& matrix) {
    Matrix transposed(MATRIX_SIZE * MATRIX_SIZE);
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            transposed[j * MATRIX_SIZE + i] = matrix[i * MATRIX_SIZE + j];
        }
    }
    return transposed;
}

Matrix matrix_mul(const Matrix& a, const Matrix& b) {
    const Matrix b_transposed = transpose(b);
    Matrix c(MATRIX_SIZE * MATRIX_SIZE, 0);
    
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            uint32_t sum = 0;  
            for (int k = 0; k < MATRIX_SIZE; k++) {
                sum += a[i * MATRIX_SIZE + k] * b_transposed[j * MATRIX_SIZE + k];
            }
            c[i * MATRIX_SIZE + j] = sum;
        }
    }
    return c;
}

Matrix matrix_mul_par(const Matrix& a, const Matrix& b) {
    const Matrix b_transposed = transpose(b);
    Matrix c(MATRIX_SIZE * MATRIX_SIZE, 0);
    
    #pragma omp parallel for collapse(2) schedule(static)
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            uint32_t sum = 0;  
            for (int k = 0; k < MATRIX_SIZE; k++) {
                sum += a[i * MATRIX_SIZE + k] * b_transposed[j * MATRIX_SIZE + k];
            }
            c[i * MATRIX_SIZE + j] = sum;
        }
    }
    return c;
}

int main() {
    #ifdef _WIN32
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
    #endif

    Matrix a = create_matrix();
    Matrix b = create_matrix();
    
    const size_t initial_memory = get_memory_usage_kb();
    std::cout << "Initial memory: " << initial_memory << " KB\n";
    
    int num_threads = omp_get_max_threads();
    omp_set_num_threads(num_threads);
    std::cout << "Using " << num_threads << " threads\n";
    std::cout << "Matrix size: " << MATRIX_SIZE << "x" << MATRIX_SIZE << "\n";
    std::cout << "Element size: " << sizeof(uint32_t) << " bytes\n";  
    std::cout << "Total matrix memory: " 
              << (MATRIX_SIZE * MATRIX_SIZE * sizeof(uint32_t) * 3) / (1024.0 * 1024.0) 
              << " MB\n";

    double min_seq = std::numeric_limits<double>::max();
    std::cout << "\nBenchmarking SEQUENTIAL:\n";
    for (int i = 0; i < 50; i++) {
        size_t before_mem = get_memory_usage_kb();
        auto start = std::chrono::high_resolution_clock::now();
        Matrix c = matrix_mul(a, b);
        auto end = std::chrono::high_resolution_clock::now();
        size_t after_mem = get_memory_usage_kb();
        
        double duration = std::chrono::duration<double>(end - start).count();
        if (duration < min_seq) min_seq = duration;
        
        std::cout << "Run " << i+1 << ": " << duration << " s, Memory: " 
                  << (after_mem - before_mem) << " KB\n";
    }

    double min_par = std::numeric_limits<double>::max();
    std::cout << "\nBenchmarking PARALLEL:\n";
    for (int i = 0; i < 50; i++) {
        size_t before_mem = get_memory_usage_kb();
        auto start = std::chrono::high_resolution_clock::now();
        Matrix c = matrix_mul_par(a, b);
        auto end = std::chrono::high_resolution_clock::now();
        size_t after_mem = get_memory_usage_kb();
        
        double duration = std::chrono::duration<double>(end - start).count();
        if (duration < min_par) min_par = duration;
        
        std::cout << "Run " << i+1 << ": " << duration << " s, Memory: " 
                  << (after_mem - before_mem) << " KB\n";
    }

    std::cout << "\nRESULTS:\n";
    std::cout << "Sequential min: " << min_seq << " s\n";
    std::cout << "Parallel min:   " << min_par << " s\n";
    std::cout << "Speedup:        " << (min_seq / min_par) << "x\n";
    std::cout << "Parallel efficiency: " << (min_seq / min_par) / num_threads * 100.0 << "%\n";
    
    const size_t u32_size = sizeof(uint32_t);
    const size_t f64_size = sizeof(double);
    std::cout << "\nMemory savings:\n";
    std::cout << "- Element size: " << u32_size << " bytes (u32) vs "
              << f64_size << " bytes (f64)\n";
    std::cout << "- Total matrix memory: "
              << (MATRIX_SIZE * MATRIX_SIZE * u32_size * 3) / (1024.0 * 1024.0)
              << " MB vs "
              << (MATRIX_SIZE * MATRIX_SIZE * f64_size * 3) / (1024.0 * 1024.0)
              << " MB (3 matrices)\n";

    Matrix c_seq = matrix_mul(a, b);
    Matrix c_par = matrix_mul_par(a, b);
    std::cout << "Sequential result[0]: " << c_seq[0] << "\n";
    std::cout << "Parallel result[0]:   " << c_par[0] << "\n";
    
    std::cout << "\nFirst 10 elements of the result matrix:\n";
    for (int i = 0; i < 10 && i < c_par.size(); i++) {
        std::cout << c_par[i] << " ";
    }
    std::cout << "\n";
    
    getchar();  

    return 0;
}

// #include <iostream>
// #include <vector>
// #include <chrono>
// #include <thread>
// #include <algorithm>  
// #ifdef _WIN32
// #ifndef NOMINMAX
// #define NOMINMAX
// #endif
// #include <windows.h>
// #include <psapi.h>
// #endif

// constexpr int MATRIX_SIZE = 500;
// using Matrix = std::vector<int>;

// size_t get_memory_usage() {
//     #ifdef _WIN32
//     PROCESS_MEMORY_COUNTERS_EX pmc;
//     GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
//     return pmc.PrivateUsage / 1024;
//     #else
//     return 0;
//     #endif
// }

// Matrix matrix_mul_seq(const Matrix& a, const Matrix& b) {
//     Matrix c(MATRIX_SIZE * MATRIX_SIZE, 0);
    
//     for (int i = 0; i < MATRIX_SIZE; i++) {
//         for (int k = 0; k < MATRIX_SIZE; k++) {
//             int temp = a[i * MATRIX_SIZE + k];
//             for (int j = 0; j < MATRIX_SIZE; j++) {
//                 c[i * MATRIX_SIZE + j] += temp * b[k * MATRIX_SIZE + j];
//             }
//         }
//     }
    
//     return c;
// }

// void multiply_thread(int start_row, int end_row, 
//                      const Matrix& a, const Matrix& b, Matrix& c) {
//     for (int i = start_row; i < end_row; i++) {
//         for (int k = 0; k < MATRIX_SIZE; k++) {
//             int temp = a[i * MATRIX_SIZE + k];
//             for (int j = 0; j < MATRIX_SIZE; j++) {
//                 c[i * MATRIX_SIZE + j] += temp * b[k * MATRIX_SIZE + j];
//             }
//         }
//     }
// }

// Matrix matrix_mul_par(const Matrix& a, const Matrix& b) {
//     Matrix c(MATRIX_SIZE * MATRIX_SIZE, 0);
    
//     const int num_threads = std::thread::hardware_concurrency();
//     const int rows_per_thread = MATRIX_SIZE / num_threads;
//     std::vector<std::thread> threads;
    
//     for (int i = 0; i < num_threads; i++) {
//         int start_row = i * rows_per_thread;
//         int end_row = (i == num_threads - 1) ? MATRIX_SIZE : start_row + rows_per_thread;
        
//         threads.emplace_back(
//             multiply_thread, 
//             start_row, end_row, 
//             std::cref(a), std::cref(b), std::ref(c)
//         );
//     }
    
//     for (auto& t : threads) {
//         t.join();
//     }
    
//     return c;
// }

// int main() {
//     Matrix a(MATRIX_SIZE * MATRIX_SIZE, 456);
//     Matrix b(MATRIX_SIZE * MATRIX_SIZE, 456);
    
//     #ifdef _WIN32
//     SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
//     #endif
    
//     const int num_threads = std::thread::hardware_concurrency();
//     std::cout << "Using " << num_threads << " threads\n";
    
//     std::cout << "Warming up...\n";
//     for (int i = 0; i < 3; i++) {
//         matrix_mul_seq(a, b);
//         matrix_mul_par(a, b);
//     }

//     double min_seq = std::numeric_limits<double>::max();
//     std::cout << "\nBenchmarking SEQUENTIAL:\n";
//     for (int i = 0; i < 20; i++) {
//         auto start = std::chrono::high_resolution_clock::now();
//         Matrix c = matrix_mul_seq(a, b);
//         auto end = std::chrono::high_resolution_clock::now();
        
//         volatile int dummy = c[0];  // Prevent optimization
//         (void)dummy;
        
//         double duration = std::chrono::duration<double>(end - start).count();
//         if (duration < min_seq) min_seq = duration;
//         std::cout << "Run " << i+1 << ": " << duration << " s\n";
//     }

//     double min_par = std::numeric_limits<double>::max();
//     std::cout << "\nBenchmarking PARALLEL:\n";
//     for (int i = 0; i < 20; i++) {
//         auto start = std::chrono::high_resolution_clock::now();
//         Matrix c = matrix_mul_par(a, b);
//         auto end = std::chrono::high_resolution_clock::now();
        
//         double duration = std::chrono::duration<double>(end - start).count();
//         if (duration < min_par) min_par = duration;
//         std::cout << "Run " << i+1 << ": " << duration << " s\n";
//     }

//     std::cout << "\nRESULTS:\n";
//     std::cout << "Sequential min: " << min_seq << " s\n";
//     std::cout << "Parallel min:   " << min_par << " s\n";
//     std::cout << "Speedup: " << min_seq / min_par << "x\n";

//     std::cin >> std::ws;  
    
//     return 0;
// }