#include <gtest/gtest.h>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <unsupported/Eigen/CXX11/Tensor>
#include <unsupported/Eigen/CXX11/src/Tensor/Tensor.h>
#include "solver/solver.hh"

using namespace qps;

TEST(TimeEvolution, Basic) {
    // 2x2 hamiltonian (qubit in magnetic field)
    Eigen::Matrix2d H;
    H << 1, 2,
         2, -1;

    // initial state |0⟩
    Eigen::Vector2d psi0;
    psi0 << 1, 0;

    // create solver
    TimeEvolutionSolver solver(1.0, 10, {Tensor::from_matrix(H)});
    solver.initialize_state(Tensor::from_vector(psi0));
    solver.build_network({});

    // compute final state norm
    double norm = solver.compute_quantity_of_interest();
    EXPECT_NEAR(norm, 1.0, 1e-10);
}

TEST(ExpectationValue, Basic) {
    // 2x2 observable (e.g., Pauli Z)
    Eigen::Matrix2d O;
    O << 1, 0,
         0, -1;

    // state |+⟩ = (|0⟩ + |1⟩)/√2
    Eigen::Vector2d psi;
    psi << 1, 1;
    psi.normalize();

    // create solver
    ExpectationValueSolver solver(Tensor::from_matrix(O));
    solver.initialize_state(Tensor::from_vector(psi));
    solver.build_network({});

    // compute expectation value
    double value = solver.compute_quantity_of_interest();
    EXPECT_NEAR(value, 0.0, 1e-10);  // <+|Z|+> = 0
}

TEST(ThermalState, Basic) {
    // 2x2 hamiltonian
    Eigen::Matrix2d H;
    H << 1, 0,
         0, -1;

    // initial state (identity matrix)
    Eigen::Matrix2d rho0 = Eigen::Matrix2d::Identity();

    // create solver
    ThermalSolver solver(1.0, 10, {Tensor::from_matrix(H)});
    solver.initialize_state(Tensor::from_matrix(rho0));
    solver.build_network({});

    // compute final state trace
    double trace = solver.compute_quantity_of_interest();
    EXPECT_NEAR(trace, 1.0, 1e-10);  // trace should be preserved
}
