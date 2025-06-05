#include "solver/solver.hh"
#include <cmath>
#include <stdexcept>
#include <unsupported/Eigen/MatrixFunctions>
#include <fstream>
#include <iomanip>
#include <iostream>

namespace qps {

// helper for debugging
static void print_tensor_indices(const std::string& name, const Tensor& t) {
    std::cout << "[DEBUG] Tensor '" << name << "' indices: ";
    for (const auto& idx : t.indices) std::cout << idx << " ";
    std::cout << std::endl;
}

void TimeEvolutionSolver::initialize_state(const Tensor& initial_state) {
    network_.add_tensor(initial_state, "psi_0");
}

void TimeEvolutionSolver::build_network(const std::vector<double>& params) {
    build_trotter_decomposition();
}

double TimeEvolutionSolver::compute_quantity_of_interest() {
    auto final_state = network_.get_tensor("psi_final");
    return final_state.to_matrix().norm();
}

void TimeEvolutionSolver::build_trotter_decomposition() {
    double dt = time_step_ / num_steps_;
    size_t n_sites = local_operators_.size();
    
    // Setup debug log file
    std::ofstream debug_log;
    if (!checkpoint_dir_.empty()) {
        debug_log.open(checkpoint_dir_ + "/debug.log");
        debug_log << "# Debug log for time evolution\n";
        debug_log << "# Format: [step] [tensor_name] [indices]\n\n";
    }
    
    // Setup solver data file
    std::ofstream solver_data;
    if (!checkpoint_dir_.empty()) {
        solver_data.open(checkpoint_dir_ + "/solver_data.json");
        solver_data << "{\n";
        solver_data << "  \"time_evolution\": {\n";
        solver_data << "    \"parameters\": {\n";
        solver_data << "      \"time_step\": " << time_step_ << ",\n";
        solver_data << "      \"num_steps\": " << num_steps_ << ",\n";
        solver_data << "      \"dt\": " << dt << "\n";
        solver_data << "    },\n";
        solver_data << "    \"steps\": [\n";
    }
    
    // precompute and add exp_op tensors once
    for (size_t i = 0; i < n_sites; ++i) {
        Eigen::MatrixXd H = local_operators_[i].to_matrix();
        Eigen::MatrixXcd exp_op = (-std::complex<double>(0,1) * (dt/2.0) * H).exp();
        Tensor exp_tensor = Tensor::from_matrix(exp_op.real(), {"site_" + std::to_string(i), "site_" + std::to_string((i+1)%n_sites)});
        network_.add_tensor(exp_tensor, "exp_op_" + std::to_string(i));
        if (debug_log.is_open()) {
            debug_log << "[init] exp_op_" << i << " indices: ";
            for (const auto& idx : exp_tensor.indices) debug_log << idx << " ";
            debug_log << "\n";
        }
    }
    
    // main Trotter loop
    std::string prev_state = "psi_0";
    for (int step = 0; step < num_steps_; ++step) {
        double current_time = step * dt;
        double state_norm = 0.0;
        double energy = 0.0;
        std::string current_state = prev_state;
        
        // Start JSON step entry
        if (solver_data.is_open()) {
            solver_data << "      {\n";
            solver_data << "        \"step\": " << step << ",\n";
            solver_data << "        \"time\": " << current_time << ",\n";
            solver_data << "        \"forward_sweep\": [\n";
        }
        
        // forward sweep
        for (size_t i = 0; i < n_sites; ++i) {
            std::string next_state = "psi_" + std::to_string(step) + "_" + std::to_string(i+1);
            std::string idx_contract = "site_" + std::to_string(i);
            if (debug_log.is_open()) {
                debug_log << "[" << step << "] Before contraction: " << current_state << " indices: ";
                for (const auto& idx : network_.get_tensor(current_state).indices) debug_log << idx << " ";
                debug_log << "\n";
            }
            auto result = network_.contract(current_state, "exp_op_" + std::to_string(i), {idx_contract});
            if (debug_log.is_open()) {
                debug_log << "[" << step << "] After contraction: " << next_state << " indices: ";
                for (const auto& idx : result.indices) debug_log << idx << " ";
                debug_log << "\n";
            }
            network_.add_tensor(result, next_state);
            Eigen::MatrixXd state_mat = result.to_matrix();
            Eigen::MatrixXd H = local_operators_[i].to_matrix();
            energy += (state_mat.transpose() * H * state_mat).trace();
            // Log forward sweep data
            if (solver_data.is_open()) {
                solver_data << "          {\n";
                solver_data << "            \"site\": " << i << ",\n";
                solver_data << "            \"energy\": " << energy << ",\n";
                solver_data << "            \"state_norm\": " << state_mat.norm() << "\n";
                solver_data << "          }" << (i < n_sites - 1 ? "," : "") << "\n";
            }
            current_state = next_state;
        }
        if (solver_data.is_open()) {
            solver_data << "        ],\n";
            solver_data << "        \"backward_sweep\": [\n";
        }
        // backward sweep
        for (int i = n_sites - 1; i >= 0; --i) {
            std::string next_state = (i == 0) ? "psi_" + std::to_string(step + 1) : "psi_" + std::to_string(step) + "_" + std::to_string(i);
            std::string idx_contract = "site_" + std::to_string(i);
            if (debug_log.is_open()) {
                debug_log << "[" << step << "] Before backward contraction: " << current_state << " indices: ";
                for (const auto& idx : network_.get_tensor(current_state).indices) debug_log << idx << " ";
                debug_log << "\n";
            }
            auto result = network_.contract(current_state, "exp_op_" + std::to_string(i), {idx_contract});
            if (debug_log.is_open()) {
                debug_log << "[" << step << "] After backward contraction: " << next_state << " indices: ";
                for (const auto& idx : result.indices) debug_log << idx << " ";
                debug_log << "\n";
            }
            network_.add_tensor(result, next_state);
            Eigen::MatrixXd state_mat = result.to_matrix();
            Eigen::MatrixXd H = local_operators_[i].to_matrix();
            energy += (state_mat.transpose() * H * state_mat).trace();
            // Log backward sweep data
            if (solver_data.is_open()) {
                solver_data << "          {\n";
                solver_data << "            \"site\": " << i << ",\n";
                solver_data << "            \"energy\": " << energy << ",\n";
                solver_data << "            \"state_norm\": " << state_mat.norm() << "\n";
                solver_data << "          }" << (i > 0 ? "," : "") << "\n";
            }
            current_state = next_state;
        }
        prev_state = current_state;
        auto state = network_.get_tensor(prev_state);
        state_norm = state.to_matrix().norm();
        // Save state to file
        if (!checkpoint_dir_.empty()) {
            std::string state_file = checkpoint_dir_ + "/state_" + std::to_string(step + 1) + ".txt";
            std::ofstream state_out(state_file);
            if (state_out.is_open()) {
                state_out << state.to_matrix();
                state_out.close();
            }
        }
        // Complete JSON step entry
        if (solver_data.is_open()) {
            solver_data << "        ],\n";
            solver_data << "        \"total_energy\": " << energy << ",\n";
            solver_data << "        \"state_norm\": " << state_norm << ",\n";
            solver_data << "        \"state_file\": \"state_" << step + 1 << ".txt\"\n";
            solver_data << "      }" << (step < num_steps_ - 1 ? "," : "") << "\n";
        }
    }
    // rename the last state to psi_final
    network_.add_tensor(network_.get_tensor(prev_state), "psi_final");
    // Close JSON file
    if (solver_data.is_open()) {
        solver_data << "    ]\n";
        solver_data << "  }\n";
        solver_data << "}\n";
        solver_data.close();
    }
    if (debug_log.is_open()) {
        debug_log.close();
    }
}

void ThermalSolver::initialize_state(const Tensor& initial_state) {
    network_.add_tensor(initial_state, "rho_0");
}

void ThermalSolver::build_network(const std::vector<double>& params) {
    build_imaginary_time_evolution();
}

double ThermalSolver::compute_quantity_of_interest() {
    auto final_state = network_.get_tensor("rho_final");
    return final_state.to_matrix().trace();
}

void ThermalSolver::build_imaginary_time_evolution() {
    double dbeta = beta_ / num_steps_;
    
    // initialize logging
    std::ofstream log_file;
    if (!checkpoint_dir_.empty()) {
        log_file.open(checkpoint_dir_ + "/thermal_log.txt");
        log_file << "# step beta trace energy state_file\n";
    }
    
    for (int step = 0; step < num_steps_; ++step) {
        double current_beta = step * dbeta;
        double trace = 0.0;
        double energy = 0.0;
        
        for (size_t i = 0; i < local_operators_.size(); ++i) {
            // get matrix from tensor
            Eigen::MatrixXd H = local_operators_[i].to_matrix();
            
            // compute e^{-betaH}
            Eigen::MatrixXd exp_op = (-dbeta * H).exp();
            
            // convert back to tensor and add to network
            Tensor exp_tensor = Tensor::from_matrix(exp_op,
                {"site_" + std::to_string(i), "site_" + std::to_string(i+1)});
            network_.add_tensor(exp_tensor, "exp_op_" + std::to_string(i));
            
            std::string current_state = (step == 0) ? "rho_0" : 
                                      "rho_" + std::to_string(step);
            std::string next_state = "rho_" + std::to_string(step + 1);
            
            auto result = network_.contract(current_state, "exp_op_" + std::to_string(i),
                                         {"site_" + std::to_string(i)});
            network_.add_tensor(result, next_state);
            
            // compute local energy contribution
            Eigen::MatrixXd state_mat = result.to_matrix();
            energy += (state_mat.transpose() * H * state_mat).trace();
        }
        
        // compute state trace
        auto state = network_.get_tensor("rho_" + std::to_string(step + 1));
        trace = state.to_matrix().trace();
        
        // log data
        if (!checkpoint_dir_.empty()) {
            // save state
            std::string state_file = checkpoint_dir_ + "/state_" + std::to_string(step + 1) + ".txt";
            std::ofstream state_out(state_file);
            if (state_out.is_open()) {
                state_out << state.to_matrix();
                state_out.close();
            }
            
            // log metrics
            log_file << std::fixed << std::setprecision(6)
                    << step + 1 << " "
                    << current_beta << " "
                    << trace << " "
                    << energy << " "
                    << "state_" << step + 1 << ".txt\n";
        }
    }
    
    if (!checkpoint_dir_.empty()) {
        log_file.close();
    }
}

void ExpectationValueSolver::initialize_state(const Tensor& initial_state) {
    network_.add_tensor(initial_state, "psi");
}

void ExpectationValueSolver::build_network(const std::vector<double>& params) {
    build_observable_network();
}

double ExpectationValueSolver::compute_quantity_of_interest() {
    auto result = network_.get_tensor("expectation");
    return result.data(0, 0);
}

void ExpectationValueSolver::build_observable_network() {
    // build network for <psi|O|psi>
    network_.add_tensor(observable_, "observable");
    auto result1 = network_.contract("psi", "observable", {"site"});
    network_.add_tensor(result1, "O_psi");
    
    auto result2 = network_.contract("psi", "O_psi", {"site"});
    network_.add_tensor(result2, "expectation");
}

} // namespace qps
