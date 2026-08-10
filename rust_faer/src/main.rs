use faer::{Mat, prelude::*};
use std::hint::black_box;
use std::mem::MaybeUninit;
use std::time::Instant;
use winapi::um::psapi::{GetProcessMemoryInfo, PROCESS_MEMORY_COUNTERS_EX};
use winapi::um::processthreadsapi::GetCurrentProcess;

fn get_memory_usage_kb() -> u64 {
    unsafe {
        let process = GetCurrentProcess();
        let mut counters = MaybeUninit::<PROCESS_MEMORY_COUNTERS_EX>::uninit();
        let cb = std::mem::size_of::<PROCESS_MEMORY_COUNTERS_EX>() as u32;
        if GetProcessMemoryInfo(process, counters.as_mut_ptr() as *mut _, cb) == 0 {
            return 0;
        }
        (counters.assume_init().WorkingSetSize as u64) / 1024
    }
}

const MATRIX_SIZE: usize = 1000;

fn main() {
    let a = Mat::<f64>::from_fn(MATRIX_SIZE, MATRIX_SIZE, |_, _| 456.0);
    let b = Mat::<f64>::from_fn(MATRIX_SIZE, MATRIX_SIZE, |_, _| 456.0);

    let initial_memory = get_memory_usage_kb();
    println!("Initial memory: {} KB", initial_memory);
    println!("Matrix size: {}x{}", MATRIX_SIZE, MATRIX_SIZE);
    println!("Library: faer (pure-Rust, auto-parallelized internally)");

    // Warmup
    { let _ = black_box(&a * &b); }

    let mut min_duration = std::time::Duration::MAX;
    let mut peak_memory = initial_memory;

    for i in 0..50 {
        let before_mem = get_memory_usage_kb();
        let start = Instant::now();
        let c = &a * &b;
        black_box(c);
        let duration = start.elapsed();
        let after_mem = get_memory_usage_kb();
        peak_memory = peak_memory.max(after_mem);

        println!("Run {:2}: {:9.6} s, Memory delta: {} KB",
                 i + 1,
                 duration.as_secs_f64(),
                 after_mem as i64 - before_mem as i64);

        if duration < min_duration {
            min_duration = duration;
        }
    }

    println!("\nMinimum time: {:.6} s", min_duration.as_secs_f64());
    println!("Peak memory: {} KB", peak_memory);
    println!("Total memory increase: {} KB", peak_memory - initial_memory);
}