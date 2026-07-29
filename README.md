# Matrix Multiplication — multi-language implementations

This repository contains several implementations and experiments for matrix multiplication (sequential and parallel) across different languages and approaches. It includes code, small benchmarks, memory/profile artifacts and example runs.

Quick overview of top-level folders

- `rust_parallel/` — Rust binary demonstrating sequential vs parallel matrix multiplication using Rayon; includes a `Cargo.toml` and `src/main.rs` with a built-in micro-benchmark and memory measurements.
- `matrix_multiplication/` — Rust workspace/crate with sources and benches (see `Cargo.toml` and `benches/`).
- `MatrixMultiplication/` — Java project (src/lib/bin layout) with a VS Code config and example sources.
- `python/` — Python reference implementation (`main.py`) and `result.txt`.
- `c/` — C implementations (`main.c`, `parallel.c`) and pre-built executables plus `result.txt`.
- `cpp/` — C++ implementations (`main.cpp`, `parallel.cpp`) and pre-built executables plus `result.txt`.
- `js/` — JavaScript implementations (`main.js`, `matrixMul.js`) and `result.txt`.
- Misc: various benchmarking images and PDFs (root) and helper scripts (`parallel.py`, `times.py`, `memory.py`).

Quick start

1) Install prerequisites for the language you want to run (Rust, Python, Node.js, C/C++ toolchain, Java).

2) Examples to build/run (PowerShell):

Rust (rust_parallel):

```powershell
cd rust_parallel
cargo run --release
```

Rust (workspace with benches):

```powershell
cd matrix_multiplication
cargo bench
```

Python:

```powershell
cd python
python .\main.py
```

C (if you want to recompile):

```powershell
cd c
gcc -O3 -o matrix.exe main.c
.\matrix.exe
```

C++ (recompile or run provided exe):

```powershell
cd cpp
g++ -O3 -o matrix.exe main.cpp
.\matrix.exe
```

Java (MatrixMultiplication): open the folder in VS Code or use `javac`/`java` on `src/` sources.

JavaScript (Node):

```powershell
cd js
node .\main.js
```

Notes

- Use release/optimized builds for performance comparisons (Rust: `--release`, C/C++: `-O3`).
- The `rust_parallel` binary measures memory on Windows via `winapi` and prints per-run timings — the code uses Rayon and a tile/block parallel strategy.
- Check `result.txt` files in language folders for sample outputs produced during development.
- If you want a consolidated benchmark harness, tell me which language(s) to prioritize and I can add a small runner and CI-friendly scripts.

