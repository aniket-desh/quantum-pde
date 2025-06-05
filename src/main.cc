#include "solver/solver.hh"
#include <Eigen/Dense>
#include <iostream>

int main() {
    // Simple 2x2 hamiltonian (single qubit)
    Eigen::Matrix2d H;
    H << 1, 0,
         0, -1;

    // initial state |0⟩
    Eigen::Vector2d psi0;
    psi0 << 1, 0;

    // use density matrix as initial state
    Eigen::Matrix2d rho0 = psi0 * psi0.transpose();
    qps::Tensor H_tensor = qps::Tensor::from_matrix(H, {"site_0", "site_0"});
    qps::Tensor rho0_tensor = qps::Tensor::from_matrix(rho0, {"site_0", "site_0"});

    qps::TimeEvolutionSolver solver(1.0, 10, {H_tensor});
    solver.set_checkpoint_dir("checkpoints");
    solver.initialize_state(rho0_tensor);
    solver.build_network({});

    double norm = solver.compute_quantity_of_interest();
    std::cout << "Final state norm: " << norm << std::endl;

    return 0;
}