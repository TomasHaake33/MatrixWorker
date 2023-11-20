#ifndef MATRIX_WORKER_IMPL_H
#define MATRIX_WORKER_IMPL_H

#include <MatrixWorker/MatrixWorkerLib.h>

constexpr int NUM_THREADS = 2; //number of threads for transposition. Must be <= matrix.height

class MatrixWorker : public WorkerInterface
{
public:
    MatrixWorker() = default;
    ~MatrixWorker() = default;

    Matrix AsyncTranspose(Matrix& matrix, int numThreads = NUM_THREADS);
    void PartialTranspose(Matrix& destMatrix, const Matrix& srcMatrix, const unsigned beginRow, const unsigned endRow);
    std::future<Matrix> AsyncProcess(Matrix matrix) override;
};

#endif //MATRIX_WORKER_IMPL_H
