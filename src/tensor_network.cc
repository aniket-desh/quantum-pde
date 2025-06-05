#include "solver/tensor.hh"
#include <stdexcept>
#include <algorithm>

namespace qps {

void TensorNetwork::add_tensor(const Tensor& tensor, const std::string& name) {
    if (tensors_.find(name) != tensors_.end()) {
        throw std::runtime_error("tensor with name '" + name + "' already exists");
    }
    tensors_[name] = tensor;
}

const Tensor& TensorNetwork::get_tensor(const std::string& name) const {
    auto it = tensors_.find(name);
    if (it == tensors_.end()) {
        throw std::runtime_error("tensor with name '" + name + "' not found");
    }
    return it->second;
}

Tensor TensorNetwork::contract(const std::string& tensor1_name,
                             const std::string& tensor2_name,
                             const std::vector<std::string>& indices_to_contract) {
    const Tensor& t1 = get_tensor(tensor1_name);
    const Tensor& t2 = get_tensor(tensor2_name);

    // find positions of indices to contract
    std::vector<int> t1_indices, t2_indices;
    for (const auto& idx : indices_to_contract) {
        auto it1 = std::find(t1.indices.begin(), t1.indices.end(), idx);
        auto it2 = std::find(t2.indices.begin(), t2.indices.end(), idx);
        
        if (it1 == t1.indices.end() || it2 == t2.indices.end()) {
            throw std::runtime_error("index '" + idx + "' not found in one or both tensors");
        }
        
        t1_indices.push_back(std::distance(t1.indices.begin(), it1));
        t2_indices.push_back(std::distance(t2.indices.begin(), it2));
    }

    // verify dimensions match for contracted indices
    for (size_t i = 0; i < indices_to_contract.size(); ++i) {
        if (t1.dimensions[t1_indices[i]] != t2.dimensions[t2_indices[i]]) {
            throw std::runtime_error("dimension mismatch in contraction for index '" + 
                                   indices_to_contract[i] + "'");
        }
    }

    // fixed-size array for rank-2 tensors
    Eigen::array<Eigen::IndexPair<int>, 1> dims;
    dims[0] = Eigen::IndexPair<int>(t1_indices[0], t2_indices[0]);

    auto result_data = t1.data.contract(t2.data, dims);

    // create new tensor with remaining indices
    std::vector<std::string> new_indices;
    std::vector<int> new_dims;
    
    // add indices and dimensions from first tensor
    for (size_t i = 0; i < t1.indices.size(); ++i) {
        if (std::find(t1_indices.begin(), t1_indices.end(), i) == t1_indices.end()) {
            new_indices.push_back(t1.indices[i]);
            new_dims.push_back(t1.dimensions[i]);
        }
    }
    
    // add indices and dimensions from second tensor
    for (size_t i = 0; i < t2.indices.size(); ++i) {
        if (std::find(t2_indices.begin(), t2_indices.end(), i) == t2_indices.end()) {
            new_indices.push_back(t2.indices[i]);
            new_dims.push_back(t2.dimensions[i]);
        }
    }

    Tensor result(new_dims, new_indices);
    result.data = result_data;
    return result;
}

} // namespace qps
