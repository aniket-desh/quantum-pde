#!/usr/bin/env python3

import json
import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path
import argparse

# Enable LaTeX rendering
plt.rcParams.update({
    "text.usetex": True,
    "font.family": "serif",
    "font.serif": ["Computer Modern Roman"],
    "text.latex.preamble": r"\usepackage{amsmath}"
})

def load_solver_data(checkpoint_dir):
    """Load solver data from JSON file."""
    data_file = Path(checkpoint_dir) / "solver_data.json"
    with open(data_file, 'r') as f:
        return json.load(f)

def plot_metrics(data, output_dir):
    """Plot various metrics from the solver data."""
    steps = [step['step'] for step in data['time_evolution']['steps']]
    times = [step['time'] for step in data['time_evolution']['steps']]
    energies = [step['total_energy'] for step in data['time_evolution']['steps']]
    norms = [step['state_norm'] for step in data['time_evolution']['steps']]

    # Create figure with subplots
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 12))
    fig.suptitle('Time Evolution Metrics', fontsize=16)

    # Plot energy
    ax1.plot(times, energies, 'b-', label='Total Energy')
    ax1.set_xlabel('Time')
    ax1.set_ylabel('Energy')
    ax1.set_title('Energy Evolution')
    ax1.grid(True)
    ax1.legend()

    # Plot state norm
    ax2.plot(times, norms, 'r-', label='State Norm')
    ax2.set_xlabel('Time')
    ax2.set_ylabel('Norm')
    ax2.set_title('State Norm Evolution')
    ax2.grid(True)
    ax2.legend()

    # Adjust layout and save
    plt.tight_layout()
    plt.savefig(Path(output_dir) / 'metrics.png')
    plt.close()

def plot_site_metrics(data, output_dir):
    """Plot metrics for each site during forward and backward sweeps."""
    steps = data['time_evolution']['steps']
    num_sites = len(steps[0]['forward_sweep'])
    
    # Create figure for forward sweep
    fig, axes = plt.subplots(num_sites, 1, figsize=(10, 4*num_sites))
    fig.suptitle('Forward Sweep Site Metrics', fontsize=16)
    
    for site in range(num_sites):
        site_energies = [step['forward_sweep'][site]['energy'] for step in steps]
        site_norms = [step['forward_sweep'][site]['state_norm'] for step in steps]
        times = [step['time'] for step in steps]
        
        ax = axes[site] if num_sites > 1 else axes
        ax.plot(times, site_energies, 'b-', label=f'Site {site} Energy')
        ax.plot(times, site_norms, 'r--', label=f'Site {site} Norm')
        ax.set_xlabel('Time')
        ax.set_ylabel('Value')
        ax.set_title(f'Site {site} Metrics')
        ax.grid(True)
        ax.legend()
    
    plt.tight_layout()
    plt.savefig(Path(output_dir) / 'forward_sweep_metrics.png')
    plt.close()
    
    # Create figure for backward sweep
    fig, axes = plt.subplots(num_sites, 1, figsize=(10, 4*num_sites))
    fig.suptitle('Backward Sweep Site Metrics', fontsize=16)
    
    for site in range(num_sites):
        site_energies = [step['backward_sweep'][site]['energy'] for step in steps]
        site_norms = [step['backward_sweep'][site]['state_norm'] for step in steps]
        times = [step['time'] for step in steps]
        
        ax = axes[site] if num_sites > 1 else axes
        ax.plot(times, site_energies, 'b-', label=f'Site {site} Energy')
        ax.plot(times, site_norms, 'r--', label=f'Site {site} Norm')
        ax.set_xlabel('Time')
        ax.set_ylabel('Value')
        ax.set_title(f'Site {site} Metrics')
        ax.grid(True)
        ax.legend()
    
    plt.tight_layout()
    plt.savefig(Path(output_dir) / 'backward_sweep_metrics.png')
    plt.close()

def main():
    parser = argparse.ArgumentParser(description='Visualize quantum solver results')
    parser.add_argument('checkpoint_dir', help='Directory containing solver output files')
    args = parser.parse_args()
    
    # Create output directory for plots
    output_dir = Path(args.checkpoint_dir) / 'plots'
    output_dir.mkdir(exist_ok=True)
    
    # Load and process data
    data = load_solver_data(args.checkpoint_dir)
    
    # Generate plots
    plot_metrics(data, output_dir)
    plot_site_metrics(data, output_dir)
    
    print(f"Plots saved to {output_dir}")

if __name__ == '__main__':
    main() 