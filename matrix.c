
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX 20
#define NUM_THREADS 10

int matA[MAX][MAX];
int matB[MAX][MAX];

int matSumResult[MAX][MAX];
int matDiffResult[MAX][MAX];
int matProductResult[MAX][MAX];

// Fill a matrix with random numbers from 1 to 10.
void fillMatrix(int matrix[MAX][MAX]) {
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            matrix[i][j] = rand() % 10 + 1;
        }
    }
}

// Print the matrix in rows and columns.
void printMatrix(int matrix[MAX][MAX]) {
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            printf("%5d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Find the rows assigned to each thread.
void getRows(int threadNum, int* start, int* end) {
    *start = threadNum * MAX / NUM_THREADS;
    *end = (threadNum + 1) * MAX / NUM_THREADS;
}

// Add the assigned rows of matrices A and B.
void* computeSum(void* args) {
    int threadNum = *(int*)args;
    free(args);

    int start, end;
    getRows(threadNum, &start, &end);

    for (int i = start; i < end; i++) {
        for (int j = 0; j < MAX; j++) {
            matSumResult[i][j] = matA[i][j] + matB[i][j];
        }
    }

    return NULL;
}

// Subtract matrix B from matrix A.
void* computeDiff(void* args) {
    int threadNum = *(int*)args;
    free(args);

    int start, end;
    getRows(threadNum, &start, &end);

    for (int i = start; i < end; i++) {
        for (int j = 0; j < MAX; j++) {
            matDiffResult[i][j] = matA[i][j] - matB[i][j];
        }
    }

    return NULL;
}

// Multiply the assigned rows of A by the columns of B.
void* computeProduct(void* args) {
    int threadNum = *(int*)args;
    free(args);

    int start, end;
    getRows(threadNum, &start, &end);

    for (int i = start; i < end; i++) {
        for (int j = 0; j < MAX; j++) {
            int sum = 0;

            for (int k = 0; k < MAX; k++) {
                sum += matA[i][k] * matB[k][j];
            }

            matProductResult[i][j] = sum;
        }
    }

    return NULL;
}

// Run one matrix operation using 10 threads.
int runThreads(void* (*operation)(void*)) {
    pthread_t threads[NUM_THREADS];
    int created = 0;

    // Divide the 20 rows among the 10 threads.
    for (int i = 0; i < NUM_THREADS; i++) {
        int* threadNum = malloc(sizeof(int));

        if (threadNum == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            break;
        }

        *threadNum = i;

        int result = pthread_create(
            &threads[i], NULL, operation, threadNum
        );

        if (result != 0) {
            fprintf(stderr, "Thread creation failed: %d\n", result);
            free(threadNum);
            break;
        }

        created++;
    }

    // Wait until all created threads finish.
    int success = (created == NUM_THREADS);

    for (int i = 0; i < created; i++) {
        int result = pthread_join(threads[i], NULL);

        if (result != 0) {
            fprintf(stderr, "Thread join failed: %d\n", result);
            success = 0;
        }
    }

    return success;
}

int main() {
    srand(time(0));

    // Generate the original matrices.
    fillMatrix(matA);
    fillMatrix(matB);

    printf("Matrix A:\n");
    printMatrix(matA);

    printf("Matrix B:\n");
    printMatrix(matB);

    // Perform addition, subtraction, and multiplication.
    // Each operation uses 10 threads, with 2 rows per thread.
    if (!runThreads(computeSum)) {
        return 1;
    }

    if (!runThreads(computeDiff)) {
        return 1;
    }

    if (!runThreads(computeProduct)) {
        return 1;
    }

    // Display the completed matrices.
    printf("Results:\n");

    printf("Sum:\n");
    printMatrix(matSumResult);

    printf("Difference:\n");
    printMatrix(matDiffResult);

    printf("Product:\n");
    printMatrix(matProductResult);

    return 0;
}
