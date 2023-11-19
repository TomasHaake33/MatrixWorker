#define BOOST_TEST_MODULE WorkerInterfaceTests

#include <MatrixWorker/MatrixWorkerLib.h>

#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE(EmptyMatrix)
{
    std::string errMsg;
    Matrix emptyMatrix
    {
        std::vector<int>(0),
        10,
        10
    };

    auto worker = get_new_worker();
    try
    {
        worker->AsyncProcess(emptyMatrix).get();
    }
    catch (const std::exception& e)
    {
        errMsg = e.what();
    }
    BOOST_REQUIRE(errMsg == "Matrix is empty");
}

BOOST_AUTO_TEST_CASE(ZeroSizedMatrix)
{
    std::string errMsg;
    Matrix firstMatrix
    {
        std::vector<int>(10, 0),
        0,
        10
    };
    Matrix secondMatrix
    {
        std::vector<int>(10, 0),
        10,
        0
    };
    
    auto worker = get_new_worker();
    try
    {
        worker->AsyncProcess(firstMatrix).get();
    }
    catch (const std::exception& e)
    {
        errMsg = e.what();
    }
    try
    {
        worker->AsyncProcess(secondMatrix).get();
    }
    catch (const std::exception& e)
    {
        errMsg = e.what();
    }

    BOOST_REQUIRE(errMsg == "One of the matrix dimensions is <= 0");
}

BOOST_AUTO_TEST_CASE(IncorrectDims)
{
    std::string errMsg;
    Matrix matrix
    {
        std::vector<int>(10, 0),
        10,
        10
    };

    auto worker = get_new_worker();
    try
    {
        worker->AsyncProcess(matrix).get();
    }
    catch (const std::exception& e)
    {
        errMsg = e.what();
    }
    BOOST_REQUIRE(errMsg == "Matrix dimensions don't match it's actual size");
}

BOOST_AUTO_TEST_CASE(SmallCoorectMatrix)
{
    Matrix matrix
    {
        std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12},
        4,
        3
    };

    auto worker = get_new_worker();
    Matrix res;
    try
    {
        res = worker->AsyncProcess(matrix).get();
    }
    catch (const std::exception&) {}

    Matrix compare
    {
        {1, 5, 9, 2, 6, 10, 3, 7, 11, 4, 8, 12},
        3,
        4
    };

    BOOST_REQUIRE(res.data == compare.data);
    BOOST_REQUIRE(matrix.height == res.width);
    BOOST_REQUIRE(matrix.width == res.height);
}
