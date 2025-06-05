#include <gtest/gtest.h>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <unsupported/Eigen/CXX11/Tensor>
#include <unsupported/Eigen/CXX11/src/Tensor/Tensor.h>
#include "solver/tensor.hh"

using namespace qps;  // add this line to use Tensor and TensorNetwork

TEST(EigenTest, BasicOperations) {
    // create a 2x2 matrix
    Eigen::Matrix2d mat;
    mat << 1, 2,
           3, 4;

    // create a 2d vector
    Eigen::Vector2d vec(1, 1);

    // test basic operations
    EXPECT_DOUBLE_EQ(mat(0,0), 1.0);
    EXPECT_DOUBLE_EQ(mat(0,1), 2.0);
    EXPECT_DOUBLE_EQ(mat(1,0), 3.0);
    EXPECT_DOUBLE_EQ(mat(1,1), 4.0);

    // test matrix-vector multiplication
    Eigen::Vector2d result = mat * vec;
    EXPECT_DOUBLE_EQ(result(0), 3.0);  // 1*1 + 2*1 = 3
    EXPECT_DOUBLE_EQ(result(1), 7.0);  // 3*1 + 4*1 = 7
}

TEST(EigenTest, SVD) {
    // create a 3x3 matrix
    Eigen::Matrix3d mat;
    mat << 1, 2, 3,
           4, 5, 6,
           7, 8, 9;

    // perform svd
    Eigen::JacobiSVD<Eigen::Matrix3d> svd(mat, Eigen::ComputeFullU | Eigen::ComputeFullV);
    
    // test orthogonality of u and v
    EXPECT_NEAR((svd.matrixU() * svd.matrixU().transpose()).norm(), 3.0, 1e-10);
    EXPECT_NEAR((svd.matrixV() * svd.matrixV().transpose()).norm(), 3.0, 1e-10);

    // test reconstruction
    Eigen::Matrix3d reconstructed = svd.matrixU() * svd.singularValues().asDiagonal() * svd.matrixV().transpose();
    EXPECT_NEAR((mat - reconstructed).norm(), 0.0, 1e-10);
}

TEST(TensorNetworkTest, FourTensorContraction) {
    // create 4 rank-2 tensors (matrices)
    std::vector<int> dims = {2, 2};  // 2x2 matrices
    std::vector<std::string> indices1 = {"i", "j"};
    std::vector<std::string> indices2 = {"j", "k"};
    std::vector<std::string> indices3 = {"k", "l"};
    std::vector<std::string> indices4 = {"l", "i"};

    // create tensors with specific values
    Tensor A(dims, indices1);
    Tensor B(dims, indices2);
    Tensor C(dims, indices3);
    Tensor D(dims, indices4);

    // fill with test values
    A.data(0,0) = 1; A.data(0,1) = 2; A.data(1,0) = 3; A.data(1,1) = 4;
    B.data(0,0) = 5; B.data(0,1) = 6; B.data(1,0) = 7; B.data(1,1) = 8;
    C.data(0,0) = 9; C.data(0,1) = 10; C.data(1,0) = 11; C.data(1,1) = 12;
    D.data(0,0) = 13; D.data(0,1) = 14; D.data(1,0) = 15; D.data(1,1) = 16;

    // create network and add tensors
    TensorNetwork network;
    network.add_tensor(A, "A");
    network.add_tensor(B, "B");
    network.add_tensor(C, "C");
    network.add_tensor(D, "D");

    // contract all tensors (trace of ABCD)
    auto result1 = network.contract("A", "B", {"j"});
    network.add_tensor(result1, "AB");
    auto result2 = network.contract("C", "D", {"l"});
    network.add_tensor(result2, "CD");
    auto result3 = network.contract("AB", "CD", {"k"});
    network.add_tensor(result3, "ABCD");
    auto final_result = network.contract("ABCD", "i", {"i"});

    // compute expected result directly using matrix multiplication
    Eigen::Matrix2d matA, matB, matC, matD;
    matA << 1, 2, 3, 4;
    matB << 5, 6, 7, 8;
    matC << 9, 10, 11, 12;
    matD << 13, 14, 15, 16;

    double expected = (matA * matB * matC * matD).trace();

    // compare results
    EXPECT_NEAR(final_result.data(0), expected, 1e-10);
} 