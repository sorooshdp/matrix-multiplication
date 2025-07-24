#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <omp.h>
#include <windows.h>
#include <psapi.h>
#include <float.h>

#define MATRIX_SIZE 500
#define MATRIX_VALUE 456

typedef uint32_t matrix_t;

size_t get_memory_usage_kb() {
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        return pmc.PrivateUsage / 1024;
    }
    return 0;
}

matrix_t* create_matrix() {
    matrix_t* matrix = (matrix_t*)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(matrix_t));
    for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++) {
        matrix[i] = MATRIX_VALUE;
    }
    return matrix;
}

matrix_t* transpose_matrix(const matrix_t* matrix) {
    matrix_t* transposed = (matrix_t*)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(matrix_t));
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            transposed[j * MATRIX_SIZE + i] = matrix[i * MATRIX_SIZE + j];
        }
    }
    return transposed;
}

void matrix_mul(const matrix_t* a, const matrix_t* b, matrix_t* c) {
    matrix_t* b_transposed = transpose_matrix(b);
    
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            uint32_t sum = 0;
            for (int k = 0; k < MATRIX_SIZE; k++) {
                sum += (uint32_t)a[i * MATRIX_SIZE + k] * 
                       (uint32_t)b_transposed[j * MATRIX_SIZE + k];
            }
            c[i * MATRIX_SIZE + j] = (matrix_t)sum;
        }
    }
    free(b_transposed);
}

void matrix_mul_par(const matrix_t* a, const matrix_t* b, matrix_t* c) {
    matrix_t* b_transposed = transpose_matrix(b);
    
    #pragma omp parallel for collapse(2) schedule(static)
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            uint32_t sum = 0;
            for (int k = 0; k < MATRIX_SIZE; k++) {
                sum += (uint32_t)a[i * MATRIX_SIZE + k] * 
                       (uint32_t)b_transposed[j * MATRIX_SIZE + k];
            }
            c[i * MATRIX_SIZE + j] = (matrix_t)sum;
        }
    }
    free(b_transposed);
}

int main() {
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
    
    matrix_t* a = create_matrix();
    matrix_t* b = create_matrix();
    matrix_t* c_seq = (matrix_t*)calloc(MATRIX_SIZE * MATRIX_SIZE, sizeof(matrix_t));
    matrix_t* c_par = (matrix_t*)calloc(MATRIX_SIZE * MATRIX_SIZE, sizeof(matrix_t));
    
    int num_threads = omp_get_max_threads();
    omp_set_num_threads(num_threads);
    printf("Using %d threads\n", num_threads);
    printf("Matrix size: %dx%d\n", MATRIX_SIZE, MATRIX_SIZE);
    printf("Element size: %zu bytes\n", sizeof(matrix_t));
    printf("Total matrix memory: %.2f MB\n", 
        (MATRIX_SIZE * MATRIX_SIZE * sizeof(matrix_t) * 3) / (1024.0 * 1024.0));
    
    double min_seq = DBL_MAX;
    printf("\nBenchmarking SEQUENTIAL:\n");
    for (int i = 0; i < 50; i++) {
        size_t before_mem = get_memory_usage_kb();
        double start = omp_get_wtime();
        matrix_mul(a, b, c_seq);
        double duration = omp_get_wtime() - start;
        size_t after_mem = get_memory_usage_kb();
        
        if (duration < min_seq) min_seq = duration;
        printf("Run %d: %.6f s, Memory: %zu KB\n", i+1, duration, after_mem - before_mem);
    }
    
    double min_par = DBL_MAX;
    printf("\nBenchmarking PARALLEL:\n");
    for (int i = 0; i < 50; i++) {
        size_t before_mem = get_memory_usage_kb();
        double start = omp_get_wtime();
        matrix_mul_par(a, b, c_par);
        double duration = omp_get_wtime() - start;
        size_t after_mem = get_memory_usage_kb();
        
        if (duration < min_par) min_par = duration;
        printf("Run %d: %.6f s, Memory: %zu KB\n", i+1, duration, after_mem - before_mem);
    }
    
    printf("\nRESULTS:\n");
    printf("Sequential min: %.6f s\n", min_seq);
    printf("Parallel min:   %.6f s\n", min_par);
    printf("Speedup: %.2fx\n", min_seq / min_par);
    printf("Parallel efficiency: %.1f%%\n", (min_seq / min_par) / num_threads * 100.0);
    
    printf("\nFirst 10 elements of SEQUENTIAL result:\n");
    matrix_mul_par(a, b, c_par);
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            printf("%d ", c_seq[i * MATRIX_SIZE + j]);
        }
        printf("\n");
    }

    free(a);
    free(b);
    free(c_seq);
    free(c_par);

    getchar();
    
    return 0;
}

// #include <stdio.h>
// #include <stdlib.h>
// #include <omp.h>
// #include <windows.h>
// #include <psapi.h>

// #define SIZE 500

// size_t get_memory_usage() {
//     PROCESS_MEMORY_COUNTERS_EX pmc;
//     GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
//     return pmc.PrivateUsage / 1024;
// }

// int* create_matrix(int value) {
//     int* matrix = (int*)malloc(SIZE * SIZE * sizeof(int));
//     for (int i = 0; i < SIZE * SIZE; i++) {
//         matrix[i] = value;
//     }
//     return matrix;
// }

// void free_matrix(int* matrix) {
//     free(matrix);
// }

// void matrix_mul_seq(const int* a, const int* b, int* c) {
//     for (int i = 0; i < SIZE * SIZE; i++) c[i] = 0;
    
//     for (int i = 0; i < SIZE; i++) {
//         for (int k = 0; k < SIZE; k++) {
//             int temp = a[i * SIZE + k];
//             for (int j = 0; j < SIZE; j++) {
//                 c[i * SIZE + j] += temp * b[k * SIZE + j];
//             }
//         }
//     }
// }

// void matrix_mul_par(const int* a, const int* b, int* c) {
//     for (int i = 0; i < SIZE * SIZE; i++) c[i] = 0;
    
//     #pragma omp parallel for
//     for (int i = 0; i < SIZE; i++) {
//         for (int k = 0; k < SIZE; k++) {
//             int temp = a[i * SIZE + k];
//             for (int j = 0; j < SIZE; j++) {
//                 // No atomic needed since each thread writes to separate i
//                 c[i * SIZE + j] += temp * b[k * SIZE + j];
//             }
//         }
//     }
// }

// int main() {
//     SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
    
//     int* a = create_matrix(456);
//     int* b = create_matrix(456);
//     int* c_seq = create_matrix(0);
//     int* c_par = create_matrix(0);
    
//     int num_threads = omp_get_max_threads();
//     omp_set_num_threads(num_threads);
//     printf("Using %d threads\n", num_threads);
    
//     printf("Warming up...\n");
//     for (int i = 0; i < 3; i++) {
//         matrix_mul_seq(a, b, c_seq);
//         matrix_mul_par(a, b, c_par);
//     }
    
//     LARGE_INTEGER frequency;
//     QueryPerformanceFrequency(&frequency);
    
//     double min_seq = 1e10;
//     printf("\nBenchmarking SEQUENTIAL:\n");
//     for (int i = 0; i < 20; i++) {
//         LARGE_INTEGER start, end;
//         QueryPerformanceCounter(&start);
//         matrix_mul_seq(a, b, c_seq);
//         QueryPerformanceCounter(&end);
        
//         double duration = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
//         min_seq = duration < min_seq ? duration : min_seq;
//         printf("Run %d: %.6f s\n", i+1, duration);
        
//         volatile int dummy = c_seq[0];
//         (void)dummy;
//     }
    
//     double min_par = 1e10;
//     printf("\nBenchmarking PARALLEL:\n");
//     for (int i = 0; i < 20; i++) {
//         LARGE_INTEGER start, end;
//         QueryPerformanceCounter(&start);
//         matrix_mul_par(a, b, c_par);
//         QueryPerformanceCounter(&end);
        
//         double duration = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
//         min_par = duration < min_par ? duration : min_par;
//         printf("Run %d: %.6f s\n", i+1, duration);
        
//         // Prevent optimization
//         volatile int dummy = c_par[0];
//         (void)dummy;
//     }
    
//     printf("\nRESULTS:\n");
//     printf("Sequential min: %.6f s\n", min_seq);
//     printf("Parallel min:   %.6f s\n", min_par);
//     printf("Speedup: %.2fx\n", min_seq / min_par);
    
//     free_matrix(a);
//     free_matrix(b);
//     free_matrix(c_seq);
//     free_matrix(c_par);

//     getchar();
    
//     return 0;
// }