#pragma once

#include <Eigen/Dense>
#include <Eigen/Core>
#include <unsupported/Eigen/CXX11/Tensor>
#include <unsupported/Eigen/CXX11/src/Tensor/Tensor.h>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

namespace qps {

// tensor in the network with dimensions and data
struct Tensor {
    using DataType = Eigen::Tensor<double, 2>;  // rank-2 tensor (matrix)
    DataType data;
    std::vector<std::string> indices;  // index names
    std::vector<int> dimensions;  // tensor dimensions

    // default constructor for unordered_map
    Tensor() : data(0, 0) {}

    Tensor(const std::vector<int>& dims, const std::vector<std::string>& idx)
        : indices(idx), dimensions(dims) {
        if (dims.size() != idx.size()) {
            throw std::runtime_error("number of dimensions must match number of indices");
        }
        if (dims.size() != 2) {
            throw std::runtime_error("only rank-2 tensors are supported");
        }
        
        data = DataType(dims[0], dims[1]);
    }

    // helper function to create tensor from matrix
    static Tensor from_matrix(const Eigen::MatrixXd& mat, 
                            const std::vector<std::string>& idx = {"i", "j"}) {
        Tensor tensor({static_cast<int>(mat.rows()), static_cast<int>(mat.cols())}, idx);
        for (int i = 0; i < mat.rows(); ++i) {
            for (int j = 0; j < mat.cols(); ++j) {
                tensor.data(i, j) = mat(i, j);
            }
        }
        return tensor;
    }

    // helper function to create tensor from vector
    static Tensor from_vector(const Eigen::VectorXd& vec,
                            const std::vector<std::string>& idx = {"i", "col"}) {
        Tensor tensor({static_cast<int>(vec.size()), 1}, idx);
        for (int i = 0; i < vec.size(); ++i) {
            tensor.data(i, 0) = vec(i);
        }
        return tensor;
    }

    // helper function to convert tensor to matrix
    Eigen::MatrixXd to_matrix() const {
        Eigen::MatrixXd mat(data.dimension(0), data.dimension(1));
        for (int i = 0; i < data.dimension(0); ++i) {
            for (int j = 0; j < data.dimension(1); ++j) {
                mat(i, j) = data(i, j);
            }
        }
        return mat;
    }

    // helper function to convert tensor to vector
    Eigen::VectorXd to_vector() const {
        if (data.dimension(1) != 1) {
            throw std::runtime_error("tensor must have second dimension of 1 to convert to vector");
        }
        Eigen::VectorXd vec(data.dimension(0));
        for (int i = 0; i < data.dimension(0); ++i) {
            vec(i) = data(i, 0);
        }
        return vec;
    }

    int rank() const { return indices.size(); }
};

// tensor network with tensors and operations
class TensorNetwork {
public:
    TensorNetwork() = default;
    ~TensorNetwork() = default;

    void add_tensor(const Tensor& tensor, const std::string& name);
    Tensor contract(const std::string& tensor1_name,
                   const std::string& tensor2_name,
                   const std::vector<std::string>& indices_to_contract);
    const Tensor& get_tensor(const std::string& name) const;

private:
    std::unordered_map<std::string, Tensor> tensors_;
};

} // namespace qps

