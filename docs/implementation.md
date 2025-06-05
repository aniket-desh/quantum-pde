# Implementation Details

This document provides a detailed explanation of the implementation of the Quantum PDE Solver.

## Core Components

### Tensor Class (`include/solver/tensor.hh`)

The `Tensor` class is the fundamental building block of the tensor network implementation.

#### Data Structures
```cpp
struct Tensor {
    using DataType = Eigen::Tensor<double, 2>;  // rank-2 tensor (matrix)
    DataType data;
    std::vector<std::string> indices;  // index names
    std::vector<int> dimensions;  // tensor dimensions
};
```

#### Key Methods

1. **Constructor**
```cpp
Tensor(const std::vector<int>& dims, const std::vector<std::string>& idx)
```
- Creates a new tensor with specified dimensions and index names
- Validates that dimensions match indices
- Currently supports only rank-2 tensors

2. **Matrix Conversion**
```cpp
static Tensor from_matrix(const Eigen::MatrixXd& mat, 
                         const std::vector<std::string>& idx = {"i", "j"})
```
- Creates a tensor from an Eigen matrix
- Default indices are "i" and "j"
- Preserves matrix dimensions and values

3. **Vector Conversion**
```cpp
static Tensor from_vector(const Eigen::VectorXd& vec,
                         const std::vector<std::string>& idx = {"i", "col"})
```
- Creates a tensor from an Eigen vector
- Default indices are "i" and "col"
- Converts vector to column matrix format

4. **Conversion Methods**
```cpp
Eigen::MatrixXd to_matrix() const
Eigen::VectorXd to_vector() const
```
- Convert tensor back to Eigen matrix/vector format
- `to_vector()` requires second dimension to be 1

### TensorNetwork Class (`include/solver/tensor.hh`)

The `TensorNetwork` class manages collections of tensors and their operations.

#### Key Methods

1. **Tensor Management**
```cpp
void add_tensor(const Tensor& tensor, const std::string& name)
const Tensor& get_tensor(const std::string& name) const
```
- Add new tensors to the network with unique names
- Retrieve tensors by name
- Throws runtime_error for duplicate names or missing tensors

2. **Tensor Contraction**
```cpp
Tensor contract(const std::string& tensor1_name,
                const std::string& tensor2_name,
                const std::vector<std::string>& indices_to_contract)
```
- Contracts two tensors along specified indices
- Validates index existence and dimension matching
- Returns new tensor with remaining indices

### TimeEvolutionSolver

The solver implements time evolution of quantum states using tensor networks.

#### Key Features

1. **State Initialization**
- Supports both pure states and density matrices
- Handles single-site and multi-site systems
- Validates initial state properties

2. **Time Evolution**
- Implements tensor network-based time evolution
- Supports checkpointing for long-time evolution
- Maintains numerical stability

3. **Network Building**
- Constructs tensor network representation
- Optimizes contraction order
- Handles boundary conditions

## Implementation Details

### Tensor Operations

1. **Contraction Logic**
- Uses Eigen's tensor contraction capabilities
- Handles index matching and dimension validation
- Preserves tensor properties during operations

2. **Index Management**
- Maintains consistent index naming
- Validates index compatibility
- Handles index reordering

### Error Handling

1. **Validation**
- Dimension matching
- Index existence
- Tensor compatibility
- State properties

2. **Error Messages**
- Descriptive error messages
- Clear indication of failure points
- Suggestion for fixes

## Usage Examples

### Basic Usage

```cpp
// Create Hamiltonian
Eigen::Matrix2d H;
H << 1, 0,
     0, -1;

// Create initial state
Eigen::Vector2d psi0;
psi0 << 1, 0;

// Convert to tensors
Tensor H_tensor = Tensor::from_matrix(H, {"site_0", "site_0"});
Tensor rho0_tensor = Tensor::from_matrix(psi0 * psi0.transpose(), {"site_0", "site_0"});

// Initialize solver
TimeEvolutionSolver solver(1.0, 10, {H_tensor});
solver.initialize_state(rho0_tensor);
```

### Advanced Usage

```cpp
// Multi-site system
std::vector<Tensor> hamiltonians;
// Add multiple site Hamiltonians
// Add interaction terms
// Initialize with multi-site state
```

## Performance Considerations

1. **Memory Management**
- Efficient tensor storage
- Minimal copying
- Smart pointer usage

2. **Computation Optimization**
- Optimized contraction order
- Efficient index operations
- Minimal memory allocation

3. **Numerical Stability**
- Careful handling of small values
- Stable time evolution
- Proper normalization 