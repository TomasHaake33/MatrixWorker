#include "MatrixWorkerImpl.h"

#include <iostream>
#include <string>

namespace
{

/**
 * \brief Проверка матрицы на корректность
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
 * \brief Транспонирование части матрицы в пределах строк (наивное)
 * 
 * \param matrix
 * \param beginRow строка, с которой начинается часть
 * \param endRow последняя строка части
 * \return транспонированная часть матрицы
 */
Matrix MatrixWorker::PartialTranspose(const Matrix& matrix, const int beginRow, const int endRow)
{
    const auto retWidth = static_cast<unsigned>(endRow - beginRow);
    std::vector<int> ret(matrix.width * (endRow - beginRow), 0);
    for (auto i = 0; i < retWidth; ++i)
    {
        for (auto j = 0; j < matrix.width; ++j)
        {
            ret[j * retWidth + i] = matrix.data[(i + beginRow) * matrix.width + j];
        }
    }
    return Matrix{ ret, retWidth, matrix.width };
}

/**
 * \brief Асинхронное транспонирование матрицы
 * 
 * \param matrix
 * \param numThreads число потоков
 */
void MatrixWorker::AsyncTranspose(Matrix& matrix, int numThreads)
{
    CheckMatrix(matrix);

    auto t1 = std::chrono::high_resolution_clock::now();
    //Матрицу делим на части по строкам. Futures содержит их транспонированные варианты
    //Эти результаты потребуется конкатенировать по горизонтали
    std::vector<std::future<Matrix>> futures;
    auto rowStep = matrix.height / numThreads; //шаг в строках
    if (rowStep == 0)
    {
        std::cout << "numThreads is too large for the current matrix. Attempting to run in a single thread" << std::endl;
        numThreads = 1;
        rowStep = matrix.height;
    }

    //Запускаем асинхронно транспонирование каждой из частей
    auto beginRow = 0;
    for (auto i = 0; i < numThreads - 1; ++i)
    {
        const auto endRow = beginRow + rowStep;
        futures.emplace_back(std::async(
            &MatrixWorker::PartialTranspose, 
            this,
            matrix,
            beginRow,
            endRow));
        beginRow += rowStep;
    }
    futures.emplace_back(std::async(
        &MatrixWorker::PartialTranspose,
        this, matrix, 
        beginRow, 
        matrix.height));

    //Конкатенируем полученные результаты, записывая сразу в matrix
    std::swap(matrix.height, matrix.width);
    for (auto f = 0; f < futures.size(); ++f)
    {
        auto part = futures[f].get();
        const auto matrixRowBegin = f * rowStep; //горизонитальное смещение строк данной part относительно начал строк matrix
        for (auto i = 0; i < part.data.size(); ++i)
        {
            const auto partRow = i / part.width; //номер текущей строки в part
            const auto rowIdx = i % part.width; //индекс элемента в текущей строке part
            //заполняем кусок matrix, которому соответствует part
            matrix.data[matrix.width * partRow + rowIdx + matrixRowBegin] = part.data[part.width * partRow + rowIdx];
        }
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() << std::endl;
}

/**
 * \brief Отложенный запуск AsyncTranspose, ожидающий запроса результата
 * 
 * \param matrix
 * \return future с транспонированной матрицей
 */
std::future<Matrix> MatrixWorker::AsyncProcess(Matrix matrix)
{
    return std::async(std::launch::deferred, [=]() mutable {
        AsyncTranspose(matrix);
        return matrix;
    });
}

/**
 * \brief Создание MatrixWorker
 * 
 * \return shared_ptr на MatrixWorker
 */
std::shared_ptr<WorkerInterface> get_new_worker()
{
    return std::make_shared<MatrixWorker>();
}
