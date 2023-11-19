#ifndef MATRIX_WORKER_IMPL_H
#define MATRIX_WORKER_IMPL_H

#include <MatrixWorker/MatrixWorkerLib.h>

constexpr int NUM_THREADS = 2; //число потоков для параллельного транспонирования

class MatrixWorker : public WorkerInterface
{
public:
    MatrixWorker() = default;
    ~MatrixWorker() = default;

    Matrix AsyncTranspose(Matrix& matrix, int numThreads = NUM_THREADS);
    void PartialTranspose(Matrix& destMatrix, const Matrix& srcMatrix, const int beginRow, const int endRow);
    std::future<Matrix> AsyncProcess(Matrix matrix) override;
};

#endif //MATRIX_WORKER_IMPL_H
