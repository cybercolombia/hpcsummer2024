#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <future>

// Function to generate a random matrix
std::vector<std::vector<double>> generateMatrix(size_t rows, size_t cols) {
    std::vector<std::vector<double>> matrix(rows, std::vector<double>(cols));
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 10.0);

    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            matrix[i][j] = dis(gen);
        }
    }
    return matrix;
}

// Function to multiply two matrices
std::vector<std::vector<double>> multiplyMatrices(
    const std::vector<std::vector<double>>& A,
    const std::vector<std::vector<double>>& B) {
    size_t rowsA = A.size();
    size_t colsA = A[0].size();
    size_t colsB = B[0].size();

    std::vector<std::vector<double>> result(rowsA, std::vector<double>(colsB, 0.0));

    auto worker = [&](size_t startRow, size_t endRow) {
        for (size_t i = startRow; i < endRow; ++i) {
            for (size_t j = 0; j < colsB; ++j) {
                for (size_t k = 0; k < colsA; ++k) {
                    result[i][j] += A[i][k] * B[k][j];
                }
            }
        }
    };

    unsigned int numThreads = std::thread::hardware_concurrency();
    std::vector<std::future<void>> futures;
    size_t rowsPerThread = rowsA / numThreads;

    for (unsigned int t = 0; t < numThreads; ++t) {
        size_t startRow = t * rowsPerThread;
        size_t endRow = (t == numThreads - 1) ? rowsA : startRow + rowsPerThread;
        futures.push_back(std::async(std::launch::async, worker, startRow, endRow));
    }

    for (auto& f : futures) {
        f.get();
    }

    return result;
}

int main(int argc, char *args[]) {
    if (argc != 4) {
        std::cerr << "Error: Incorrect number of arguments.\n";
        std::cerr << "Usage: " << args[0] << " <rowsA> <colsA> <colsB>\n";
        return 1;
    }
    size_t rowsA = 500, colsA = 500, colsB = 500;
    rowsA = std::stoul(args[1]);
    colsA = std::stoul(args[2]);
    colsB = std::stoul(args[3]);
    if (rowsA == 0 || colsA == 0 || colsB == 0) {
        std::cerr << "Error: Matrix dimensions must be greater than zero.\n";
        return 1;
    }
    if (colsA != colsB) {
        std::cerr << "Error: Number of columns in A must equal number of rows in B.\n";
        return 1;
    }
    std::cout << "Matrix A: " << rowsA << "x" << colsA << "\n";
    std::cout << "Matrix B: " << colsA << "x" << colsB << "\n"; 
    auto A = generateMatrix(rowsA, colsA);
    auto B = generateMatrix(colsA, colsB);

    auto start = std::chrono::high_resolution_clock::now();
    auto result = multiplyMatrices(A, B);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Matrix multiplication completed in " << elapsed.count() << " seconds.\n";

    return 0;
}
