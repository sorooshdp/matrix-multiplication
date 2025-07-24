const SIZE = 500;
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
function main() {
    const a = new Int32Array(SIZE * SIZE).fill(456);
    const b = new Int32Array(SIZE * SIZE).fill(456);

    let initial = getMemoryUsage();
    console.log(`Initial memory: ${initial} KB`);

    let minTime = Infinity;
    let peakMem = initial;

    for (let i = 0; i < 50; i++) {
        const before = getMemoryUsage();
        console.log(`Run ${i+1} - Before: ${before} KB`);

        const start = process.hrtime.bigint();
        const c = matrixMul(a, b);
        const end = process.hrtime.bigint();

        const after = getMemoryUsage();
        peakMem = Math.max(peakMem, after);

        const duration = Number(end - start) / 1e9;
        console.log(`Delta: ${after - before} KB`);
        console.log(`Time: ${duration.toFixed(6)} s`);

        minTime = Math.min(minTime, duration);
    }
    
    console.log("\nResult (first 10 elements):");   
    const c = matrixMul(a, b);
    for (let i = 0; i < 10; i++) {
        console.log(c[i]);
    }

    console.log(`\nPeak: ${peakMem} KB`);
    console.log(`Total increase: ${peakMem - initial} KB`);
    console.log(`Min time: ${minTime.toFixed(6)} s`);
}
main();