use criterion::{black_box, criterion_group, criterion_main, Criterion};

pub type Matrix = Vec<Vec<i32>>;

pub fn matrix_mul(a: Matrix, b: Matrix) -> Matrix {
   let mut c : Matrix = vec![vec![0;350];350];

   for i in 0..350 {
    for j in 0..350 {
        let mut sum = 0;
        for k in 0..350 {
            sum += a[i][k] * b[k][j];
        }
        c[i][j] = sum;
    }
   } 

   c
}

fn matrix_mul_benchmark(c: &mut Criterion) {
    // Prepare the input matrices.
    let a: Matrix = vec![vec![456; 350]; 350];
    let b: Matrix = vec![vec![456; 350]; 350];

    // This will create a benchmark for the "matrix_mul" function.
    // Using `black_box` ensures that the compiler doesn’t optimize away the inputs.
    c.bench_function("matrix_mul", |bencher| {
        bencher.iter(|| {
            let _result = matrix_mul(black_box(a.clone()), black_box(b.clone()));
        });
    });
}

criterion_group!(benches, matrix_mul_benchmark);
criterion_main!(benches);
