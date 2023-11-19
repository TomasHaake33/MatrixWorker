#include <MatrixWorker/MatrixWorkerLib.h>

#include <iostream>
#include <chrono>

Matrix PartialTranspose(const Matrix& matrix, const int beginRow, const int endRow)
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


int main()
{

    auto worker = get_new_worker();
    Matrix mtx
    {
       {1, 2, 3, 4, 5, 6},
       3,
       2
    };

    for (auto i = 7; i < 250001; ++i)
    {
        mtx.data.emplace_back(i);
    }
    mtx.height = 500;
    mtx.width = 500;
    auto future = worker->AsyncProcess(mtx);
    Matrix transposed;
    try
    {
        transposed = future.get();
    }
    catch (const std::exception& e)
    {
        std::cout << e.what();
        return -1;
    }

    
    std::cout << std::endl;

    
    transposed = mtx;
    auto t1 = std::chrono::high_resolution_clock::now();
    transposed = PartialTranspose(mtx, 0, mtx.height);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() << std::endl;


    return 0;
}
