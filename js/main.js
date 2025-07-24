const { Worker, isMainThread, parentPort, workerData } = require('worker_threads');
const os = require('os');
const SIZE = 500;
const NUM_THREADS = os.cpus().length;

function getMemoryUsage() {
    return Math.round(process.memoryUsage().heapUsed / 1024);
}

function matrixMul(a, b) {
    const c = new Int32Array(SIZE * SIZE);
    
    for (let i = 0; i < SIZE; i++) {
        for (let j = 0; j < SIZE; j++) {
            let sum = 0;
            for (let k = 0; k < SIZE; k++) {
                sum += a[i * SIZE + k] * b[k * SIZE + j];
            }
            c[i * SIZE + j] = sum;
        }
    }
    
    return c;
}

// Worker Code
if (!isMainThread) {
    parentPort.on('message', (msg) => {
        if (msg.type === 'calculate') {
            const { a, b, startRow, endRow } = msg.data;
            const partial = new Int32Array(SIZE * SIZE);
            
            for (let i = startRow; i < endRow; i++) {
                for (let k = 0; k < SIZE; k++) {
                    const temp = a[i * SIZE + k];
                    for (let j = 0; j < SIZE; j++) {
                        partial[i * SIZE + j] += temp * b[k * SIZE + j];
                    }
                }
            }
            
            parentPort.postMessage({ 
                type: 'result', 
                partial: partial.buffer 
            }, [partial.buffer]);
        }
    });
}

// Create worker pool for reuse
function createWorkerPool() {
    const workers = [];
    for (let i = 0; i < NUM_THREADS; i++) {
        const worker = new Worker(__filename);
        worker.on('error', console.error);
        workers.push(worker);
    }
    return workers;
}

// Parallel Multiplication with worker reuse
async function matrixMulParallel(workers, a, b) {
    return new Promise((resolve, reject) => {
        const result = new Int32Array(SIZE * SIZE);
        const rowsPerThread = Math.ceil(SIZE / workers.length);
        let completed = 0;

        for (let i = 0; i < workers.length; i++) {
            const startRow = i * rowsPerThread;
            const endRow = Math.min(startRow + rowsPerThread, SIZE);
            if (startRow >= SIZE) continue;
            
            const worker = workers[i];
            const handleMessage = (msg) => {
                if (msg.type === 'result') {
                    worker.off('message', handleMessage);
                    const partial = new Int32Array(msg.partial);
                    for (let idx = 0; idx < partial.length; idx++) {
                        result[idx] += partial[idx];
                    }
                    completed++;
                    if (completed === workers.length) resolve(result);
                }
            };
            
            worker.on('message', handleMessage);
            worker.postMessage({
                type: 'calculate',
                data: { a, b, startRow, endRow }
            });
        }
    });
}

async function main() {
    const a = new Int32Array(new SharedArrayBuffer(SIZE * SIZE * Int32Array.BYTES_PER_ELEMENT));
    const b = new Int32Array(new SharedArrayBuffer(SIZE * SIZE * Int32Array.BYTES_PER_ELEMENT));
    a.fill(456);
    b.fill(456);

    const initial = getMemoryUsage();
    console.log(`Initial memory: ${initial} KB`);
    console.log(`Using ${NUM_THREADS} threads`);

    console.log("Warming up...");
    matrixMul(a, b);
    
    const workerPool = createWorkerPool();
    await matrixMulParallel(workerPool, a, b);

    let minTimeSeq = Infinity;
    console.log("\nBenchmarking SEQUENTIAL:");
    for (let i = 0; i < 50; i++) {
        if (global.gc) global.gc();
        const before = getMemoryUsage();
        const start = process.hrtime.bigint();
        
        matrixMul(a, b);
        
        const end = process.hrtime.bigint();
        const after = getMemoryUsage();
        
        const duration = Number(end - start) / 1e9;
        minTimeSeq = Math.min(minTimeSeq, duration);
        console.log(`Run ${i+1}: ${duration.toFixed(6)} s (Mem delta: ${after - before} KB)`);
    }

    let minTimePar = Infinity;
    console.log("\nBenchmarking PARALLEL:");
    for (let i = 0; i < 50; i++) {
        const before = getMemoryUsage();
        const start = process.hrtime.bigint();
        await matrixMulParallel(workerPool, a, b);
        const end = process.hrtime.bigint();
        const after = getMemoryUsage();
        
        const duration = Number(end - start) / 1e9;
        minTimePar = Math.min(minTimePar, duration);
        console.log(`Run ${i+1}: ${duration.toFixed(6)} s (Mem delta: ${after - before} KB)`);
    }

    const resultSeq = matrixMul(a, b);
    const resultPar = await matrixMulParallel(workerPool, a, b);
    console.log("\nValidation:");
    for (let i = 0; i < 10; i++) {
        if (resultSeq[i] !== resultPar[i]) {
            console.error(`Mismatch at index ${i}: Seq=${resultSeq[i]}, Par=${resultPar[i]}`);
        } else {
            console.log(`Index ${i}: ${resultPar[i]}`);
        }
    }

    workerPool.forEach(worker => worker.terminate());

    console.log("\nRESULTS:");
    console.log(`Sequential min: ${minTimeSeq.toFixed(6)} s`);
    console.log(`Parallel min:   ${minTimePar.toFixed(6)} s`);
    console.log(`Speedup: ${(minTimeSeq / minTimePar).toFixed(2)}x`);
}

if (isMainThread) {
    main().catch(console.error);
}