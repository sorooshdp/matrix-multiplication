public class MatrixMul {
    private static final int SIZE = 500;
    
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
    
    private static int[][] matrixMul(int[][] a, int[][] b) {
        int[][] c = new int[SIZE][SIZE];
        
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
        
        System.out.println("\nMatrix Multiplication Benchmark (500x500):");
        
        for (int i = 1; i <= 50; i++) {
            System.gc();  
            
            long beforeMulMemory = getMemoryUsage();
            System.out.println("Run " + i + " - Memory before multiplication: " + beforeMulMemory + " KB");
            
            long startTime = System.nanoTime();
            int[][] c = matrixMul(a, b);
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
        //show ten elements of the resulting matrix
        int[][] c = matrixMul(a, b);
        System.out.println("Resulting matrix first element: " + c[0][0]);
        System.out.println("Resulting matrix last element: " + c[SIZE - 1][SIZE - 1]);

    }
}