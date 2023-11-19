#include "MatrixWorkerImpl.h"

#include <iostream>
#include <string>

namespace
{

/**
 * \brief �������� ������� �� ������������
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
 * \brief ���������������� ����� ������� � �������� ����� (�������)
 * 
 * \param destMatrix �������, ���� ������������ ���������
 * \param srcMatrix �������, ������ ����������� �������� ��������
 * \param beginRow ������, � ������� ���������� �����
 * \param endRow ��������� ������ �����
 * \return ����������������� ����� �������
 */
void MatrixWorker::PartialTranspose(Matrix& destMatrix, const Matrix& srcMatrix, const int beginRow, const int endRow)
{
    for (auto i = beginRow; i < endRow; ++i)
    {
        for (auto j = 0; j < srcMatrix.width; ++j)
        {
            destMatrix.data[j * destMatrix.width + i] = srcMatrix.data[i * srcMatrix.width + j];
        }
    }
}

/**
 * \brief ������������� ���������������� �������
 * 
 * \param matrix
 * \param numThreads ����� �������
 * \return ����������������� �������
 */
Matrix MatrixWorker::AsyncTranspose(Matrix& matrix, int numThreads)
{
    CheckMatrix(matrix);
    
    Matrix matrixCopy(matrix); //����� �������, ���������� ���������������� ����� ����, � ��������� �� ���������

    auto rowStep = matrix.height / numThreads; //��� � �������
    if (rowStep == 0)
    {
        std::cout << "numThreads is too large for the current matrix. Attempting to run in a single thread" << std::endl;
        numThreads = 1;
        rowStep = matrix.height;
    }

    std::swap(matrixCopy.height, matrixCopy.width);

    std::vector<std::thread> threads; //������, ��� ��������������� ������� �� ������
    auto beginRow = 0;
    //numThreads - 1 ������� ����� �������� ��� ������� ����������� �������, ��� �������� �������� ��������� �����
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

    //���� ���������� ������ �������
    for (auto& thread : threads)
    {
        thread.join();
    }

    return matrixCopy;
}

/**
 * \brief ���������� ������ AsyncTranspose, ��������� ������� ����������
 * 
 * \param matrix
 * \return future � ����������������� ��������
 */
std::future<Matrix> MatrixWorker::AsyncProcess(Matrix matrix)
{
    return std::async(std::launch::deferred, [=]() mutable {
        return AsyncTranspose(matrix);
    });
}

/**
 * \brief �������� MatrixWorker
 * 
 * \return shared_ptr �� MatrixWorker
 */
std::shared_ptr<WorkerInterface> get_new_worker()
{
    return std::make_shared<MatrixWorker>();
}
