#ifndef MATRIX_WORKER_IMPL_H
#define MATRIX_WORKER_IMPL_H

#include <MatrixWorker/MatrixWorkerLib.h>

constexpr int NUM_THREADS = 2;

class MatrixWorker : public WorkerInterface
{
public:
    MatrixWorker() = default;
    ~MatrixWorker() = default;

    void AsyncTranspose(Matrix& matrix, int numThreads = NUM_THREADS);
    Matrix PartialTranspose(const Matrix& matrix, const int beginRow, const int endRow);
    std::future<Matrix> AsyncProcess(Matrix matrix) override;
};

#endif //MATRIX_WORKER_IMPL_H
