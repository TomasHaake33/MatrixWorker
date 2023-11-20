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
 * \brief Transpose a part of matrix bounded by rows (naive)
 * 
 * \param destMatrix destination matrix
 * \param srcMatrix source matrix
 * \param beginRow row from thich the part starts
 * \param endRow row at which the part ends
 * \returntransposed part of the matrix
 */
void MatrixWorker::PartialTranspose(Matrix& destMatrix, const Matrix& srcMatrix, const unsigned beginRow, const unsigned endRow)
{
    for (unsigned i = beginRow; i < endRow; ++i)
    {
        for (unsigned j = 0; j < srcMatrix.width; ++j)
        {
            destMatrix.data[j * destMatrix.width + i] = srcMatrix.data[i * srcMatrix.width + j];
        }
    }
}

/**
 * \brief Parallel matrix transposition
 * 
 * \param matrix
 * \param numThreads number of threads
 * \return transposed matrix
 */
Matrix MatrixWorker::AsyncTranspose(Matrix& matrix, int numThreads)
{
    if (numThreads == 0)
    {
        throw(std::runtime_error("numThreads is zero"));
    }

    CheckMatrix(matrix);
    Matrix matrixCopy(matrix); //matrix copy. Write the result in here, read from original
    
    auto rowStep = matrix.height / numThreads; //step in rows
    if (rowStep == 0)
    {
        std::cout << "numThreads is too large for the current matrix. Attempting to run in a single thread" << std::endl;
        numThreads = 1;
        rowStep = matrix.height;
    }

    std::swap(matrixCopy.height, matrixCopy.width);

    std::vector<std::thread> threads; //threads which transpose matrix parts 
    unsigned beginRow = 0;
    //numThreads - 1 threads will work on equal parts, the remainder is for the last thread
    for (auto i = 0; i < numThreads - 1; ++i)
    {
        const auto endRow = beginRow + rowStep;
        threads.emplace_back(std::thread(
            &MatrixWorker::PartialTranspose, 
            this,
            std::ref(matrixCopy),
            std::ref(matrix),
            beginRow,
            endRow));
        beginRow += rowStep;
    }
    threads.emplace_back(std::thread(
        &MatrixWorker::PartialTranspose,
        this,
        std::ref(matrixCopy),
        std::ref(matrix), 
        beginRow, 
        matrix.height));

    //waiting for threads to end
    for (auto& thread : threads)
    {
        thread.join();
    }

    return matrixCopy;
}

/**
 * \brief Delayed AsyncTranspose execution
 * 
 * \param matrix
 * \return future with transposed matrix
 */
std::future<Matrix> MatrixWorker::AsyncProcess(Matrix matrix)
{
    return std::async(std::launch::deferred, [=]() mutable {
        return AsyncTranspose(matrix);
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
