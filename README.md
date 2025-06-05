# Quantum PDE Solver

A C++ implementation of a quantum partial differential equation solver using tensor networks and matrix product states.

## Theory

This project implements a numerical solver for quantum partial differential equations (PDEs) using tensor network methods. The key theoretical components are:

### Tensor Networks
Tensor networks provide an efficient representation of quantum many-body states by decomposing high-dimensional tensors into networks of lower-dimensional tensors. This approach significantly reduces the computational complexity of quantum simulations.

### Matrix Product States (MPS)
The solver uses Matrix Product States, a specific form of tensor network that represents quantum states as a product of matrices. MPS are particularly effective for one-dimensional quantum systems and provide a natural way to represent quantum states with limited entanglement.

### Time Evolution
The time evolution of quantum states is implemented using tensor network methods, allowing for efficient simulation of quantum dynamics while maintaining accuracy and numerical stability.

## Features

- Efficient tensor network operations for quantum state representation
- Time evolution of quantum states using tensor network methods
- Support for both single-site and multi-site quantum systems
- Checkpointing system for long-time evolution
- Integration with Eigen for efficient linear algebra operations

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Running

The project includes a `run_all.sh` script that automates the complete workflow:

1. Builds the project using CMake
2. Runs the quantum solver
3. Generates visualization of the results
4. Cleans up temporary files

To use the script:
```bash
./run_all.sh
```

The script will:
- Create and enter a build directory
- Configure and build the project
- Execute the solver with default parameters
- Generate visualization plots in the `plots/` directory
- Clean up build artifacts

## Usage

The solver can be used to simulate quantum systems by specifying:
- Hamiltonian operators
- Initial quantum states
- Time evolution parameters
- System geometry and boundary conditions

Example usage is provided in the `src/main.cc` file.

## Dependencies

- C++17 or later
- Eigen3
- CMake 3.10 or later
- GTest (for testing)

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
