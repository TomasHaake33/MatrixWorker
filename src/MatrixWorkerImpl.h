#ifndef MATRIX_WORKER_IMPL_H
#define MATRIX_WORKER_IMPL_H

#include <MatrixWorker/MatrixWorkerLib.h>

class MatrixWorker : public WorkerInterface
{
public:
    MatrixWorker() = default;
    ~MatrixWorker() = default;

    std::future<Matrix> AsyncProcess(Matrix matrix) override;
};

#endif //MATRIX_WORKER_IMPL_H
