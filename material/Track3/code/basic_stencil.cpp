#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 1000  // Grid size
#define ITERATIONS 100  // Number of iterations

// Function to initialize the grid
void initialize_grid(double grid[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            grid[i][j] = rand() % 100;
        }
    }
}

// Function to perform the stencil computation
void stencil_step(double grid[N][N], double new_grid[N][N]) {
    for (int i = 1; i < N-1; i++) {
        for (int j = 1; j < N-1; j++) {
            new_grid[i][j] = 0.25 * (grid[i-1][j] + grid[i+1][j] + grid[i][j-1] + grid[i][j+1]);
        }
    }
}

// Function to copy the new grid to the old grid
void copy_grid(double dest[N][N], double src[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            dest[i][j] = src[i][j];
        }
    }
}

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double grid[N][N], new_grid[N][N];
    printf("The size of this matrix is %lu \n ",sizeof(grid)/sizeof(grid[0][0]));
    initialize_grid(grid);

    for (int iter = 0; iter < ITERATIONS; iter++) {
        stencil_step(grid, new_grid);
        copy_grid(grid, new_grid);

        // Exchange boundary rows between neighboring processes
        if (rank > 0) {
            MPI_Send(grid[1], N, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD);
            MPI_Recv(grid[0], N, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        if (rank < size-1) {
            MPI_Send(grid[N-2], N, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD);
            MPI_Recv(grid[N-1], N, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    MPI_Finalize();
    return 0;
}
