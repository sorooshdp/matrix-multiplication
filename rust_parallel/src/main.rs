// use std::time::Instant;
// use std::mem::MaybeUninit;
// use winapi::um::psapi::GetProcessMemoryInfo;
// use winapi::um::processthreadsapi::GetCurrentProcess;
// use winapi::um::psapi::PROCESS_MEMORY_COUNTERS_EX;
// use faer::prelude::*;

// fn get_memory_usage_kb() -> u64 {
//     unsafe {
//         let process = GetCurrentProcess();
//         let mut counters = MaybeUninit::<PROCESS_MEMORY_COUNTERS_EX>::uninit();
//         let cb = std::mem::size_of::<PROCESS_MEMORY_COUNTERS_EX>() as u32;

//         if GetProcessMemoryInfo(
//             process,
//             counters.as_mut_ptr() as *mut _,
//             cb
//         ) == 0 {
//             return 0;
//         }

//         let counters = counters.assume_init();
//         (counters.PrivateUsage as u64) / 1024  
//     }
// }

// const MATRIX_SIZE: usize = 500;

// fn main() {
//     let a = Mat::<f64>::from_fn(MATRIX_SIZE, MATRIX_SIZE, |_, _| 456.0);
//     let b = Mat::<f64>::from_fn(MATRIX_SIZE, MATRIX_SIZE, |_, _| 456.0);

//     let initial_memory = get_memory_usage_kb();
//     println!("Initial memory: {} KB", initial_memory);
    
//     let mut min_duration = std::time::Duration::new(u64::MAX, 0);
//     let mut peak_memory = initial_memory;
    
//     for i in 0..50 {
//         println!("Run {}: ", i+1);
        
//         let before_mem = get_memory_usage_kb();
//         println!("Memory before: {} KB", before_mem);

//         let start = Instant::now();
//         let c = &a * &b;
//         std::hint::black_box(c);
//         let duration = start.elapsed();
        
//         let after_mem = get_memory_usage_kb();
//         peak_memory = peak_memory.max(after_mem);

//         println!("Memory delta: {} KB", after_mem - before_mem);
//         println!("Time: {:?}", duration);
        
//         if duration < min_duration {
//             min_duration = duration;
//         }
//     }
    
//     let min_duration_in_seconds = min_duration.as_secs() as f64 + (min_duration.subsec_nanos() as f64 / 1_000_000_000.0);
//     println!("Minimum time taken in seconds: {:?}", min_duration_in_seconds);
//     println!("Peak memory usage: {} KB", peak_memory);
//     println!("Total memory increase: {} KB", peak_memory - initial_memory);
    
//     println!("Press Enter to exit...");
//     let mut input = String::new();
//     std::io::stdin().read_line(&mut input).unwrap();
//     println!("Exiting...");
// }

use std::time::Instant;
use rayon::prelude::*;
use winapi::um::psapi::GetProcessMemoryInfo;
use winapi::um::processthreadsapi::GetCurrentProcess;
use winapi::um::psapi::PROCESS_MEMORY_COUNTERS_EX;
use std::mem::MaybeUninit;
use std::hint::black_box;

fn get_memory_usage_kb() -> u64 {
    unsafe {
        let process = GetCurrentProcess();
        let mut counters = MaybeUninit::<PROCESS_MEMORY_COUNTERS_EX>::uninit();
        let cb = std::mem::size_of::<PROCESS_MEMORY_COUNTERS_EX>() as u32;

        if GetProcessMemoryInfo(
            process,
            counters.as_mut_ptr() as *mut _,
            cb
        ) == 0 {
            return 0;
        }

        let counters = counters.assume_init();
        (counters.PrivateUsage as u64) / 1024  
    }
}

const MATRIX_SIZE: usize = 500;
const MATRIX_VALUE: u32 = 456;  

type Matrix = Vec<u32>;  

fn transpose(matrix: &Matrix) -> Matrix {
    let mut transposed = vec![0; MATRIX_SIZE * MATRIX_SIZE];
    for i in 0..MATRIX_SIZE {
        for j in 0..MATRIX_SIZE {
            transposed[j * MATRIX_SIZE + i] = matrix[i * MATRIX_SIZE + j];
        }
    }
    transposed
}

#[inline(always)]
fn matrix_mul(a: &Matrix, b: &Matrix) -> Matrix {
    let b_transposed = transpose(b);
    let mut c = vec![0; MATRIX_SIZE * MATRIX_SIZE];
    
    for i in 0..MATRIX_SIZE {
        for j in 0..MATRIX_SIZE {
            let mut sum = 0;  
            for k in 0..MATRIX_SIZE {
                sum += a[i * MATRIX_SIZE + k] * b_transposed[j * MATRIX_SIZE + k];  
            }
            c[i * MATRIX_SIZE + j] = sum;  
        }
    }
    c
}

fn matrix_mul_par(a: &Matrix, b: &Matrix) -> Matrix {
    let b_transposed = transpose(b);
    let mut c = vec![0; MATRIX_SIZE * MATRIX_SIZE];
    let block_size = 50;  

    c.par_chunks_exact_mut(MATRIX_SIZE * block_size)
        .enumerate()
        .for_each(|(block_i, c_block)| {
            let start_i = block_i * block_size;
            let end_i = (start_i + block_size).min(MATRIX_SIZE);
            
            for i in start_i..end_i {
                for j in 0..MATRIX_SIZE {
                    let mut sum = 0;
                    let a_row = &a[i * MATRIX_SIZE..(i+1)*MATRIX_SIZE];
                    let b_row = &b_transposed[j * MATRIX_SIZE..(j+1)*MATRIX_SIZE];
                    
                    for k in 0..MATRIX_SIZE {
                        sum += a_row[k] * b_row[k];
                    }
                    c_block[(i - start_i) * MATRIX_SIZE + j] = sum;
                }
            }
        });
    c
}

fn main() {
    let a = vec![MATRIX_VALUE; MATRIX_SIZE * MATRIX_SIZE];
    let b = vec![MATRIX_VALUE; MATRIX_SIZE * MATRIX_SIZE];

    let initial_memory = get_memory_usage_kb();
    println!("Initial memory: {} KB", initial_memory);
    println!("Using {} threads", rayon::current_num_threads());
    println!("Matrix size: {}x{}", MATRIX_SIZE, MATRIX_SIZE);
    println!("Element size: {} bytes", std::mem::size_of::<u32>());
    println!("Total matrix memory: {:.2} MB", 
        (MATRIX_SIZE * MATRIX_SIZE * std::mem::size_of::<u32>() * 3) as f64 / (1024.0 * 1024.0));

    let mut min_seq = std::time::Duration::MAX;
    println!("\nBenchmarking SEQUENTIAL:");
    for i in 0..50 {
        let before_mem = get_memory_usage_kb();
        let start = Instant::now();
        let result = matrix_mul(&a, &b);
        black_box(result);
        let duration = start.elapsed();
        let after_mem = get_memory_usage_kb();
        
        println!("Run {}: {:9.6} s, Memory: {:6} KB", i+1, duration.as_secs_f64(), after_mem - before_mem);

        if duration < min_seq {
            min_seq = duration;
        }
    }

    let mut min_par = std::time::Duration::MAX;
    println!("\nBenchmarking PARALLEL:");
    for i in 0..50 {
        let before_mem = get_memory_usage_kb();
        let start = Instant::now();
        let result = matrix_mul_par(&a, &b);
        black_box(result);
        let duration = start.elapsed();
        let after_mem = get_memory_usage_kb();
        
        println!("Run {}: {:9.6} s, Memory: {:6} KB", i+1, duration.as_secs_f64(), after_mem - before_mem);
        if duration < min_par {
            min_par = duration;
        }
    }

    println!("\nRESULTS:");
    println!("Sequential min: {:.6} s", min_seq.as_secs_f64());
    println!("Parallel min:   {:.6} s", min_par.as_secs_f64());
    println!("Speedup:        {:.2}x", min_seq.as_secs_f64() / min_par.as_secs_f64());
    println!("Parallel efficiency: {:.1}%", 
        (min_seq.as_secs_f64() / min_par.as_secs_f64()) / rayon::current_num_threads() as f64 * 100.0);
    
    let c_seq = matrix_mul(&a, &b);
    let c_par = matrix_mul_par(&a, &b);
    
    let expected = MATRIX_VALUE * MATRIX_VALUE * MATRIX_SIZE as u32;
    println!("\nValidation:");
    println!("Expected value: {}", expected);
    println!("Sequential result[0]: {}", c_seq[0]);
    println!("Parallel result[0]:   {}", c_par[0]);
    
    assert_eq!(c_seq, c_par, "Sequential and parallel results differ!");
    assert_eq!(c_seq[0], expected, "Result value is incorrect!");
    println!("Results match!");
}