use std::mem::MaybeUninit;
use std::thread;
use std::time::Instant;
use winapi::um::processthreadsapi::GetCurrentProcess;
use winapi::um::psapi::GetProcessMemoryInfo;
use winapi::um::psapi::PROCESS_MEMORY_COUNTERS_EX;

fn get_memory_usage_kb() -> u64 {
    unsafe {
        let process = GetCurrentProcess();
        let mut counters = MaybeUninit::<PROCESS_MEMORY_COUNTERS_EX>::uninit();
        let cb = std::mem::size_of::<PROCESS_MEMORY_COUNTERS_EX>() as u32;

        if GetProcessMemoryInfo(process, counters.as_mut_ptr() as *mut _, cb) == 0 {
            return 0;
        }

        let counters = counters.assume_init();
        (counters.PrivateUsage as u64) / 1024
    }
}

// const MATRIX_SIZE: usize = 500;
const MATRIX_SIZE: usize = 1000;
pub type Matrix = Vec<[u16; MATRIX_SIZE]>;

// #[inline(always)]
// pub fn matrix_mul_parallel(a : &Matrix, b : &Matrix) -> Matrix {

//     let mut c : Matrix = Box::new([[0;MATRIX_SIZE]; MATRIX_SIZE]);

//     let num_threads = thread::available_parallelism()
//         .map(|n| n.get())
//         .unwrap_or(4);

//     let rows_per_thread = MATRIX_SIZE / num_threads;
//     let remaining_rows = MATRIX_SIZE % num_threads;

//     let a_owned = Arc::new((**a).clone());
//     let b_owned = Arc::new((**b).clone());

//     let mut handles = Vec::with_capacity(num_threads);

//     for thread_id in 0..num_threads {
//         let a_clone = Arc::clone(&a_owned);
//         let b_clone = Arc::clone(&b_owned);

//         let start_row = thread_id * rows_per_thread;
//         let end_row = if thread_id == num_threads - 1 {
//             start_row + rows_per_thread + remaining_rows
//         } else {
//             start_row + rows_per_thread
//         };

//         let handle = thread::spawn(move || {
//             let mut local_results = Vec::with_capacity(end_row - start_row);

//             for i in start_row..end_row {
//                 let mut row = [0u16; MATRIX_SIZE];
//                 for  j in 0..MATRIX_SIZE {
//                     let mut sum = 0u16;
//                     for k in 0..MATRIX_SIZE {
//                         sum += a_clone[i][k] * b_clone[k][j];
//                     }
//                     row[j] = sum;
//                 }
//                 local_results.push((i, row));
//             }
//             local_results
//         });

//         handles.push(handle);
//     }

//     for handle in handles {
//         let thread_results = handle.join().unwrap();
//         for (row_idx, row_data) in thread_results {
//             c[row_idx] = row_data;
//         }
//     }

//     c
// }

#[inline(always)]
pub fn matrix_mul(a: &Matrix, b: &Matrix) -> Matrix {
    let mut c: Matrix = vec![[0; MATRIX_SIZE]; MATRIX_SIZE];
    for i in 0..MATRIX_SIZE {
        for j in 0..MATRIX_SIZE {
            let mut sum = 0;
            for k in 0..MATRIX_SIZE {
                sum += a[i][k] * b[k][j];
            }
            c[i][j] = sum;
        }
    }
    c
}

fn main() {
    // Allocated safely on the heap
    let a: Matrix = vec![[456; MATRIX_SIZE]; MATRIX_SIZE];
    let b: Matrix = vec![[456; MATRIX_SIZE]; MATRIX_SIZE];

    let initial_memory = get_memory_usage_kb();
    println!("Initial memory: {} KB", initial_memory);
    println!(
        "Using {} threads",
        thread::available_parallelism()
            .map(|n| n.get())
            .unwrap_or(4)
    );

    let mut min_duration = std::time::Duration::new(u64::MAX, 0);
    let mut peak_memory = initial_memory;

    for i in 0..50 {
        println!("Run {}: ", i + 1);

        let before_mem = get_memory_usage_kb();
        println!("Memory before: {} KB", before_mem);

        let start = Instant::now();
        // you can switch between matrix_mul and matrix_mul_parallel to test both implementations
        let c = matrix_mul(&a, &b);
        std::hint::black_box(c);
        let duration = start.elapsed();

        let after_mem = get_memory_usage_kb();
        peak_memory = peak_memory.max(after_mem);

        println!("Memory delta: {} KB", after_mem - before_mem);
        println!("Time: {:?}", duration);

        println!("Time taken: {:?}", duration);
        if duration < min_duration {
            min_duration = duration;
        }
    }

    let min_duration_in_seconds =
        min_duration.as_secs() as f64 + (min_duration.subsec_nanos() as f64 / 1_000_000_000.0);
    println!(
        "Minimum time taken in seconds: {:?}",
        min_duration_in_seconds
    );
    println!("Peak memory usage: {} KB", peak_memory);
    println!("Total memory increase: {} KB", peak_memory - initial_memory);

    println!("Press Enter to exit...");
    let mut input = String::new();
    std::io::stdin().read_line(&mut input).unwrap();
    println!("Exiting...");
    std::process::exit(0);
}
