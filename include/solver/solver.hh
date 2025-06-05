#pragma once

#include "solver/tensor.hh"
#include <memory>
#include <vector>
#include <string>

namespace qps {

// base class for quantum problems solved using tensor networks
class Solver {
public:
    virtual ~Solver() = default;

    // initialize quantum state
    virtual void initialize_state(const Tensor& initial_state) = 0;

    // build tensor network for the problem
    virtual void build_network(const std::vector<double>& params) = 0;

    // compute quantity of interest
    virtual double compute_quantity_of_interest() = 0;

    // set checkpoint directory
    void set_checkpoint_dir(const std::string& dir) { checkpoint_dir_ = dir; }

protected:
    TensorNetwork network_;
    std::vector<Tensor> local_operators_;
    std::vector<std::string> boundary_conditions_;
    std::string checkpoint_dir_;
};

// solver for time evolution problems
class TimeEvolutionSolver : public Solver {
public:
    TimeEvolutionSolver(double time_step, int num_steps,
                       const std::vector<Tensor>& local_operators)
        : time_step_(time_step), num_steps_(num_steps),
          local_operators_(local_operators) {}

    void initialize_state(const Tensor& initial_state) override;
    void build_network(const std::vector<double>& params) override;
    double compute_quantity_of_interest() override;

private:
    void build_trotter_decomposition();
    
    double time_step_;
    int num_steps_;
    std::vector<Tensor> local_operators_;
};

// solver for thermal/statistical problems
class ThermalSolver : public Solver {
public:
    ThermalSolver(double beta, int num_steps,
                 const std::vector<Tensor>& local_operators)
        : beta_(beta), num_steps_(num_steps),
          local_operators_(local_operators) {}

    void initialize_state(const Tensor& initial_state) override;
    void build_network(const std::vector<double>& params) override;
    double compute_quantity_of_interest() override;

private:
    void build_imaginary_time_evolution();
    
    double beta_;
    int num_steps_;
    std::vector<Tensor> local_operators_;
};

// solver for expectation value problems
class ExpectationValueSolver : public Solver {
public:
    ExpectationValueSolver(const Tensor& observable)
        : observable_(observable) {}

    void initialize_state(const Tensor& initial_state) override;
    void build_network(const std::vector<double>& params) override;
    double compute_quantity_of_interest() override;

private:
    void build_observable_network();
    
    Tensor observable_;
};

} // namespace qps
