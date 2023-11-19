#include <MatrixWorker/MatrixWorkerLib.h>

#include <iostream>

int main()
{
    auto worker = get_new_worker();
    Matrix mtx
    {
       {1, 2, 3, 4, 5, 6},
       3,
       2
    };

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

    for (const auto i : transposed.data)
    {
        std::cout << i << " ";
    }

    return 0;
}
