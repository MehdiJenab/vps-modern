/// @file main.cpp
/// @brief Vlasov-Poisson Solver - Main Entry Point
///
/// This is a minimal working example demonstrating free-streaming
/// of particles in a periodic domain.

#include <vps/particles/particles.h>
#include <vps/grid/grid.h>

#include <cmath>
#include <iomanip>
#include <iostream>
#include <numbers>

/// @brief Initialize particles with a sinusoidal density perturbation
///
/// Creates particles distributed in phase space to represent:
/// f(x,v) = f0(v) * (1 + epsilon * cos(k*x))
///
/// where f0(v) is a Maxwellian velocity distribution.
///
/// @param grid The spatial grid
/// @param n_particles_per_cell Number of particles per cell
/// @param v_thermal Thermal velocity
/// @param epsilon Perturbation amplitude
/// @param k Wavenumber of perturbation
/// @return Initialized Particles container
vps::particles::Particles initialize_particles(
    const vps::grid::Grid& grid,
    std::size_t n_particles_per_cell,
    double v_thermal,
    double epsilon,
    double k)
{
    const std::size_t n_cells = grid.n_cells();
    const std::size_t n_v = n_particles_per_cell;  // Velocity points per cell
    const std::size_t total_particles = n_cells * n_v;
    
    vps::particles::Particles particles(total_particles);
    
    // Velocity range: -4*v_th to +4*v_th
    const double v_min = -4.0 * v_thermal;
    const double v_max = 4.0 * v_thermal;
    const double dv = (v_max - v_min) / static_cast<double>(n_v);
    
    // Generate particles
    for (std::size_t i = 0; i < n_cells; ++i) {
        double x = grid.cell_center(i);
        
        // Density perturbation factor
        double density_factor = 1.0 + epsilon * std::cos(k * x);
        
        for (std::size_t j = 0; j < n_v; ++j) {
            double v = v_min + (static_cast<double>(j) + 0.5) * dv;
            
            // Maxwellian distribution
            double f_maxwell = std::exp(-v * v / (2.0 * v_thermal * v_thermal));
            f_maxwell /= std::sqrt(2.0 * std::numbers::pi) * v_thermal;
            
            // Apply density perturbation
            double f = f_maxwell * density_factor;
            
            particles.push_back(x, v, f);
        }
    }
    
    return particles;
}

/// @brief Compute density on grid from particles
///
/// Uses first-order (NGP - Nearest Grid Point) deposition
void compute_density(
    const vps::particles::Particles& particles,
    const vps::grid::Grid& grid,
    vps::grid::Field& density)
{
    density.zero();
    
    const double inv_dx = 1.0 / grid.dx();
    
    for (std::size_t p = 0; p < particles.size(); ++p) {
        double x = particles.x(p);
        double f = particles.f(p);
        
        // NGP: deposit to nearest cell
        std::size_t i = grid.cell_index(x);
        density[i] += f * inv_dx;
    }
}

/// @brief Apply periodic boundary conditions to particles
void apply_periodic_bc(
    vps::particles::Particles& particles,
    const vps::grid::Grid& grid)
{
    auto x_span = particles.x();
    
    for (std::size_t p = 0; p < particles.size(); ++p) {
        x_span[p] = grid.wrap_position(x_span[p]);
    }
}

/// @brief Print simulation status
void print_status(
    int step,
    double time,
    const vps::particles::Particles& particles,
    const vps::grid::Field& density)
{
    // Find min/max density
    double rho_min = density[0];
    double rho_max = density[0];
    for (std::size_t i = 0; i < density.size(); ++i) {
        rho_min = std::min(rho_min, density[i]);
        rho_max = std::max(rho_max, density[i]);
    }
    
    std::cout << std::fixed << std::setprecision(4)
              << "Step " << std::setw(5) << step
              << " | t = " << std::setw(8) << time
              << " | particles = " << particles.size()
              << " | rho: [" << std::setw(8) << rho_min 
              << ", " << std::setw(8) << rho_max << "]"
              << std::endl;
}

int main() {
    std::cout << "====================================================\n";
    std::cout << "       Vlasov-Poisson Solver - Free Streaming       \n";
    std::cout << "====================================================\n\n";

    // =========================================================================
    // Simulation Parameters
    // =========================================================================
    
    // Grid parameters
    const std::size_t n_cells = 64;
    const double x_min = 0.0;
    const double x_max = 2.0 * std::numbers::pi;
    
    // Time parameters
    const double dt = 0.1;
    const int n_steps = 100;
    const int print_interval = 10;
    
    // Physics parameters
    const double v_thermal = 1.0;
    const double epsilon = 0.1;      // Perturbation amplitude
    const double k = 1.0;            // Wavenumber (one wavelength in domain)
    const std::size_t n_particles_per_cell = 32;

    // =========================================================================
    // Initialization
    // =========================================================================
    
    std::cout << "Initializing simulation...\n";
    std::cout << "  Grid cells:    " << n_cells << "\n";
    std::cout << "  Domain:        [" << x_min << ", " << x_max << "]\n";
    std::cout << "  Time step:     " << dt << "\n";
    std::cout << "  Total steps:   " << n_steps << "\n";
    std::cout << "  Particles/cell:" << n_particles_per_cell << "\n";
    std::cout << "\n";
    
    // Create grid
    vps::grid::Grid grid(n_cells, x_min, x_max, vps::grid::BoundaryCondition::Periodic);
    
    // Create density field
    vps::grid::Field density(grid);
    
    // Initialize particles
    auto particles = initialize_particles(grid, n_particles_per_cell, v_thermal, epsilon, k);
    
    std::cout << "Total particles: " << particles.size() << "\n\n";
    
    // Compute initial density
    compute_density(particles, grid, density);
    
    // =========================================================================
    // Main Time Loop
    // =========================================================================
    
    std::cout << "Starting simulation...\n";
    std::cout << "----------------------------------------------------\n";
    
    print_status(0, 0.0, particles, density);
    
    for (int step = 1; step <= n_steps; ++step) {
        // Free streaming: advance positions
        // x_new = x_old + v * dt
        vps::particles::advance_positions(particles, dt);
        
        // Apply periodic boundary conditions
        apply_periodic_bc(particles, grid);
        
        // Compute density (for diagnostics)
        compute_density(particles, grid, density);
        
        // Print status
        if (step % print_interval == 0) {
            print_status(step, static_cast<double>(step) * dt, particles, density);
        }
    }
    
    std::cout << "----------------------------------------------------\n";
    std::cout << "Simulation complete!\n";
    
    return 0;
}
