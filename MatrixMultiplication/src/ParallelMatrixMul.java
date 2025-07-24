import java.util.concurrent.ForkJoinPool;
import java.util.concurrent.RecursiveTask;

public class ParallelMatrixMul {
    private static final int SIZE = 500;
    private static final int THRESHOLD = 64; 
    
    private static long getMemoryUsage() {
        return (Runtime.getRuntime().totalMemory() - Runtime.getRuntime().freeMemory()) / 1024;
    }
    
    private static int[][] createMatrix(int value) {
        int[][] matrix = new int[SIZE][SIZE];
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                matrix[i][j] = value;
            }
        }
        return matrix;
    }
    
    // Parallel matrix multiplication using Fork-Join framework
    static class MatrixMultiplyTask extends RecursiveTask<Void> {
        private final int[][] a;
        private final int[][] b;
        private final int[][] c;
        private final int startRow;
        private final int endRow;
        
        public MatrixMultiplyTask(int[][] a, int[][] b, int[][] c, int startRow, int endRow) {
            this.a = a;
            this.b = b;
            this.c = c;
            this.startRow = startRow;
            this.endRow = endRow;
        }
        
        @Override
        protected Void compute() {
            if (endRow - startRow <= THRESHOLD) {
                for (int i = startRow; i < endRow; i++) {
                    for (int j = 0; j < SIZE; j++) {
                        int sum = 0;
                        for (int k = 0; k < SIZE; k++) {
                            sum += a[i][k] * b[k][j];
                        }
                        c[i][j] = sum;
                    }
                }
            } else {
                int mid = (startRow + endRow) / 2;
                MatrixMultiplyTask leftTask = new MatrixMultiplyTask(a, b, c, startRow, mid);
                MatrixMultiplyTask rightTask = new MatrixMultiplyTask(a, b, c, mid, endRow);
                
                leftTask.fork();
                rightTask.compute();
                leftTask.join();
            }
            return null;
        }
    }
    
    private static int[][] parallelMatrixMul(int[][] a, int[][] b) {
        int[][] c = new int[SIZE][SIZE];
        
        ForkJoinPool pool = new ForkJoinPool();
        MatrixMultiplyTask task = new MatrixMultiplyTask(a, b, c, 0, SIZE);
        pool.invoke(task);
        pool.shutdown();
        
        return c;
    }
    
    public static void main(String[] args) {
        System.gc();
        
        long initialMemory = getMemoryUsage();
        System.out.println("Initial memory usage: " + initialMemory + " KB");
        
        int[][] a = createMatrix(456);
        int[][] b = createMatrix(456);
        
        long afterMatricesMemory = getMemoryUsage();
        System.out.println("Memory after creating matrices: " + afterMatricesMemory + 
                           " KB (increase: " + (afterMatricesMemory - initialMemory) + " KB)");
        
        double minDuration = Double.POSITIVE_INFINITY;
        long peakMemory = afterMatricesMemory;
        
        System.out.println("\nParallel Matrix Multiplication Benchmark (500x500):");
        System.out.println("Available processors: " + Runtime.getRuntime().availableProcessors());
        
        for (int i = 1; i <= 50; i++) {
            System.gc();
            
            long beforeMulMemory = getMemoryUsage();
            System.out.println("Run " + i + " - Memory before multiplication: " + beforeMulMemory + " KB");
            
            long startTime = System.nanoTime();
            int[][] c = parallelMatrixMul(a, b);
            long endTime = System.nanoTime();
            
            long afterMulMemory = getMemoryUsage();
            System.out.println("Run " + i + " - Memory after multiplication: " + afterMulMemory + 
                               " KB (increase: " + (afterMulMemory - beforeMulMemory) + " KB)");
            
            double duration = (endTime - startTime) / 1_000_000_000.0;
            System.out.println("Run " + i + " - Time taken: " + duration + " seconds");
            
            minDuration = Math.min(minDuration, duration);
            peakMemory = Math.max(peakMemory, afterMulMemory);
            
            c = null;
        }
        
        System.out.println("\nBest time: " + minDuration + " seconds");
        System.out.println("Peak memory usage: " + peakMemory + " KB");
        System.out.println("Total memory increase: " + (peakMemory - initialMemory) + " KB");

        int[][] c = parallelMatrixMul(a, b);
        System.out.println("Resulting matrix first element: " + c[0][0]);
        System.out.println("Resulting matrix last element: " + c[SIZE - 1][SIZE - 1]);
    }
}