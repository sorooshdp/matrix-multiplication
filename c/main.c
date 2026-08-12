#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#endif

/// #define SIZE 500
#define SIZE 1000

size_t get_memory_usage() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    return pmc.PrivateUsage / 1024;
#else
    return 0;
#endif
}

int (*create_matrix(int value))[SIZE] {
    int (*matrix)[SIZE] = malloc(sizeof(int[SIZE][SIZE]));
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            matrix[i][j] = value;
        }
    }
    return matrix;
}

int (*matrix_mul(int (*a)[SIZE], int (*b)[SIZE]))[SIZE] {
    int (*c)[SIZE] = create_matrix(0);
    
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            int sum = 0;
            for (int k = 0; k < SIZE; k++) {
                sum += a[i][k] * b[k][j];
            }
            c[i][j] = sum;
        }
    }
    return c;
}

int main() {
    size_t initial_memory = get_memory_usage();
    printf("Initial memory usage: %zu KB\n", initial_memory);

    int (*a)[SIZE] = create_matrix(456);
    int (*b)[SIZE] = create_matrix(456);

    size_t after_matrices_memory = get_memory_usage();
    printf("Memory after creating matrices: %zu KB (increase: %zu KB)\n",
           after_matrices_memory, after_matrices_memory - initial_memory);

    double min_duration = INFINITY;
    size_t peak_memory = after_matrices_memory;

    for (int i = 0; i < 50; i++) {
        size_t before_mul_memory = get_memory_usage();
        printf("Run %d - Memory before multiplication: %zu KB\n", i + 1, before_mul_memory);

        LARGE_INTEGER frequency, start, end;
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&start);
        
        int (*c)[SIZE] = matrix_mul(a, b);
        
        QueryPerformanceCounter(&end);

        size_t after_mul_memory = get_memory_usage();
        printf("Run %d - Memory after multiplication: %zu KB (increase: %zu KB)\n",
               i + 1, after_mul_memory, after_mul_memory - before_mul_memory);

        double duration = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
        printf("Run %d - Time taken: %.6f seconds\n", i + 1, duration);

        min_duration = duration < min_duration ? duration : min_duration;
        peak_memory = peak_memory > after_mul_memory ? peak_memory : after_mul_memory;

        free(c);
    }

    printf("\nMinimum time taken: %.6f seconds\n", min_duration);
    printf("Peak memory usage: %zu KB\n", peak_memory);
    printf("Total memory increase: %zu KB\n", peak_memory - initial_memory);

    printf("First 10 elements of the result matrix:\n");
    int (*c)[SIZE] = matrix_mul(a, b);
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            printf("%d ", c[i][j]);
        }
        printf("\n");
    }

    free(a);
    free(b);
    free(c);

    printf("Press Enter to exit...");
    getchar();

    return 0;
}

// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>
// #include <math.h>
// #ifdef _WIN32
// #include <windows.h>
// #include <psapi.h>
// #endif

// #define SIZE 500
// #define NUM_THREADS 8 

// typedef struct {
//     int start_row;
//     int end_row;
//     int (*a)[SIZE];
//     int (*b)[SIZE];
//     int (*c)[SIZE];
// } ThreadData;

// size_t get_memory_usage() {
// #ifdef _WIN32
//     PROCESS_MEMORY_COUNTERS_EX pmc;
//     GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
//     return pmc.PrivateUsage / 1024;
// #else
//     return 0;
// #endif
// }

// int (*create_matrix(int value))[SIZE] {
//     int (*matrix)[SIZE] = malloc(sizeof(int[SIZE][SIZE]));
//     for (int i = 0; i < SIZE; i++) {
//         for (int j = 0; j < SIZE; j++) {
//             matrix[i][j] = value;
//         }
//     }
//     return matrix;
// }

// void free_matrix(int (*matrix)[SIZE]) {
//     free(matrix);
// }

// int (*matrix_mul(int (*a)[SIZE], int (*b)[SIZE]))[SIZE] {
//     int (*c)[SIZE] = create_matrix(0);
    
//     for (int i = 0; i < SIZE; i++) {
//         for (int j = 0; j < SIZE; j++) {
//             int sum = 0;
//             for (int k = 0; k < SIZE; k++) {
//                 sum += a[i][k] * b[k][j];
//             }
//             c[i][j] = sum;
//         }
//     }
//     return c;
// }

// DWORD WINAPI multiply_thread(LPVOID arg) {
//     ThreadData* data = (ThreadData*)arg;
//     for (int i = data->start_row; i < data->end_row; i++) {
//         for (int j = 0; j < SIZE; j++) {
//             int sum = 0;
//             for (int k = 0; k < SIZE; k++) {
//                 sum += data->a[i][k] * data->b[k][j];
//             }
//             data->c[i][j] = sum;
//         }
//     }
//     return 0;
// }

// int (*matrix_mul_parallel(int (*a)[SIZE], int (*b)[SIZE]))[SIZE] {
//     int (*c)[SIZE] = create_matrix(0);
//     HANDLE threads[NUM_THREADS];
//     ThreadData thread_data[NUM_THREADS];
//     int rows_per_thread = SIZE / NUM_THREADS;

//     for (int i = 0; i < NUM_THREADS; i++) {
//         thread_data[i].start_row = i * rows_per_thread;
//         thread_data[i].end_row = (i == NUM_THREADS - 1) ? SIZE : (i + 1) * rows_per_thread;
//         thread_data[i].a = a;
//         thread_data[i].b = b;
//         thread_data[i].c = c;
        
//         threads[i] = CreateThread(NULL, 0, multiply_thread, &thread_data[i], 0, NULL);
//         if (threads[i] == NULL) {
//             fprintf(stderr, "Error creating thread %d\n", i);
//             // Cleanup existing threads if any fail
//             for (int j = 0; j < i; j++) CloseHandle(threads[j]);
//             free_matrix(c);
//             return NULL;
//         }
//     }

//     WaitForMultipleObjects(NUM_THREADS, threads, TRUE, INFINITE);
    
//     for (int i = 0; i < NUM_THREADS; i++) {
//         CloseHandle(threads[i]);
//     }

//     return c;
// }

// int main() {
//     size_t initial_memory = get_memory_usage();
//     printf("Initial memory usage: %zu KB\n", initial_memory);

//     int (*a)[SIZE] = create_matrix(456);
//     int (*b)[SIZE] = create_matrix(456);

//     size_t after_matrices_memory = get_memory_usage();
//     printf("Memory after creating matrices: %zu KB (increase: %zu KB)\n",
//            after_matrices_memory, after_matrices_memory - initial_memory);

//     double min_duration_seq = INFINITY;
//     size_t peak_memory_seq = after_matrices_memory;
//     printf("\nTesting SEQUENTIAL multiplication:\n");
//     for (int i = 0; i < 50; i++) {
//         size_t before_mul_memory = get_memory_usage();
        
//         LARGE_INTEGER frequency, start, end;
//         QueryPerformanceFrequency(&frequency);
//         QueryPerformanceCounter(&start);
        
//         int (*c)[SIZE] = matrix_mul(a, b);
        
//         QueryPerformanceCounter(&end);
//         size_t after_mul_memory = get_memory_usage();

//         double duration = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
//         min_duration_seq = duration < min_duration_seq ? duration : min_duration_seq;
//         peak_memory_seq = (after_mul_memory > peak_memory_seq) ? after_mul_memory : peak_memory_seq;
        
//         free_matrix(c);
//     }
//     printf(">> SEQUENTIAL min time: %.6f seconds\n", min_duration_seq);

//     double min_duration_par = INFINITY;
//     size_t peak_memory_par = after_matrices_memory;
//     printf("\nTesting PARALLEL multiplication:\n");
//     for (int i = 0; i < 50; i++) {
//         size_t before_mul_memory = get_memory_usage();
        
//         LARGE_INTEGER frequency, start, end;
//         QueryPerformanceFrequency(&frequency);
//         QueryPerformanceCounter(&start);
        
//         int (*c)[SIZE] = matrix_mul_parallel(a, b);
        
//         QueryPerformanceCounter(&end);
//         size_t after_mul_memory = get_memory_usage();

//         double duration = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
//         min_duration_par = duration < min_duration_par ? duration : min_duration_par;
//         peak_memory_par = (after_mul_memory > peak_memory_par) ? after_mul_memory : peak_memory_par;
        
//         free_matrix(c);
//     }
//     printf(">> PARALLEL min time: %.6f seconds\n", min_duration_par);

//     printf("\nRESULTS SUMMARY:\n");
//     printf("Sequential - Min time: %.6fs, Peak memory: %zu KB\n", 
//            min_duration_seq, peak_memory_seq);
//     printf("Parallel   - Min time: %.6fs, Peak memory: %zu KB\n", 
//            min_duration_par, peak_memory_par);
//     printf("Speedup: %.2fx\n", min_duration_seq / min_duration_par);
//     printf("Memory overhead: %d KB\n", (int)(peak_memory_par - peak_memory_seq));

//     free_matrix(a);
//     free_matrix(b);

//     printf("Press Enter to exit...");
//     getchar();

//     return 0;
// }
