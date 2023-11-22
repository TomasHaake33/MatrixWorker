#include "MatrixWorkerImpl.h"

#include <iostream>
#include <string>

namespace
{

/**
 * \brief Check matrix for correct contents
 * 
 * \param matrix
 */
void CheckMatrix(const Matrix& matrix)
{
    std::string errMsg;
    if (matrix.data.empty())
    {
        errMsg = "Matrix is empty";
    }
    else if (matrix.width <= 0 || matrix.height <= 0)
    {
        errMsg = "One of the matrix dimensions is <= 0";
    }
    else if (matrix.width * matrix.height != matrix.data.size())
    {
        errMsg = "Matrix dimensions don't match it's actual size";
    }
    
    if (!errMsg.empty())
    {
        throw(std::runtime_error(errMsg));
    }
}

} //namespace

/**
 * \brief Async transposition of matrix
 * 
 * \param matrix
 * \return future with transposed matrix
 */
std::future<Matrix> MatrixWorker::AsyncProcess(Matrix matrix)
{
    //Threads are not created on every call, but only if they are allowed to
    return std::async([=]() mutable {
        CheckMatrix(matrix);

        Matrix matrixCopy = matrix;
        std::swap(matrixCopy.height, matrixCopy.width);

        for (unsigned i = 0; i < matrix.height; ++i)
        {
            for (unsigned j = 0; j < matrix.width; ++j)
            {
                matrixCopy.data[j * matrix.height + i] = matrix.data[i * matrix.width + j];
            }
        }

        return matrixCopy;
    });
}

/**
 * \brief Creation on MatrixWorker
 * 
 * \return shared_ptr to MatrixWorker
 */
std::shared_ptr<WorkerInterface> get_new_worker()
{
    return std::make_shared<MatrixWorker>();
}
