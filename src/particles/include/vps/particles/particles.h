#ifndef VPS_PARTICLES_PARTICLES_H
#define VPS_PARTICLES_PARTICLES_H

/// @file particles.h
/// @brief Struct-of-Arrays container for phase-space points
///
/// This module provides an efficient SoA (Struct of Arrays) data structure
/// for storing phase-space points (x, v, f) used in Vlasov simulations.
/// The SoA layout enables:
/// - Efficient SIMD vectorization
/// - Cache-friendly access patterns
/// - Easy parallelization with OpenMP

#include <cstddef>
#include <span>
#include <vector>

namespace vps::particles {

/// @brief Struct-of-Arrays container for phase-space points
///
/// Each phase-space point has three properties:
/// - x: position in configuration space
/// - v: velocity
/// - f: distribution function value (weight)
///
/// Instead of storing an array of structs {x, v, f}, we store three
/// separate arrays for better memory access patterns.
///
/// @code
/// Particles p(1000);  // Reserve space for 1000 points
/// p.push_back(0.5, 1.0, 0.1);  // Add a point
///
/// // Access all positions for vectorized operations
/// auto positions = p.x();
/// @endcode
class Particles {
public:
    // =========================================================================
    // Type Aliases
    // =========================================================================
    using value_type = double;
    using size_type = std::size_t;
    using container_type = std::vector<value_type>;

    // =========================================================================
    // Constructors
    // =========================================================================
    
    /// @brief Default constructor - creates empty container
    Particles() = default;

    /// @brief Construct with reserved capacity
    /// @param capacity Initial capacity to reserve
    explicit Particles(size_type capacity);

    /// @brief Construct with initial size and default values
    /// @param size Number of points to create
    /// @param x_val Default x value
    /// @param v_val Default v value  
    /// @param f_val Default f value
    Particles(size_type size, value_type x_val, value_type v_val, value_type f_val);

    // Default copy/move operations
    Particles(const Particles&) = default;
    Particles(Particles&&) noexcept = default;
    Particles& operator=(const Particles&) = default;
    Particles& operator=(Particles&&) noexcept = default;
    ~Particles() = default;

    // =========================================================================
    // Capacity
    // =========================================================================
    
    /// @brief Returns the number of phase-space points
    [[nodiscard]] size_type size() const noexcept;
    
    /// @brief Returns the current capacity
    [[nodiscard]] size_type capacity() const noexcept;
    
    /// @brief Checks if the container is empty
    [[nodiscard]] bool empty() const noexcept;
    
    /// @brief Reserves memory for at least n points
    /// @param n Minimum capacity to reserve
    void reserve(size_type n);
    
    /// @brief Resizes the container to contain n points
    /// @param n New size
    void resize(size_type n);
    
    /// @brief Resizes with specified default values
    void resize(size_type n, value_type x_val, value_type v_val, value_type f_val);
    
    /// @brief Clears all points
    void clear() noexcept;

    // =========================================================================
    // Element Access - Span-based (preferred for algorithms)
    // =========================================================================
    
    /// @brief Returns a span over all x (position) values
    [[nodiscard]] std::span<value_type> x() noexcept;
    [[nodiscard]] std::span<const value_type> x() const noexcept;
    
    /// @brief Returns a span over all v (velocity) values
    [[nodiscard]] std::span<value_type> v() noexcept;
    [[nodiscard]] std::span<const value_type> v() const noexcept;
    
    /// @brief Returns a span over all f (distribution function) values
    [[nodiscard]] std::span<value_type> f() noexcept;
    [[nodiscard]] std::span<const value_type> f() const noexcept;

    // =========================================================================
    // Element Access - Index-based
    // =========================================================================
    
    /// @brief Access x value at index i
    [[nodiscard]] value_type& x(size_type i) noexcept;
    [[nodiscard]] const value_type& x(size_type i) const noexcept;
    
    /// @brief Access v value at index i
    [[nodiscard]] value_type& v(size_type i) noexcept;
    [[nodiscard]] const value_type& v(size_type i) const noexcept;
    
    /// @brief Access f value at index i
    [[nodiscard]] value_type& f(size_type i) noexcept;
    [[nodiscard]] const value_type& f(size_type i) const noexcept;

    // =========================================================================
    // Modifiers
    // =========================================================================
    
    /// @brief Adds a new phase-space point
    /// @param x_val Position value
    /// @param v_val Velocity value
    /// @param f_val Distribution function value
    void push_back(value_type x_val, value_type v_val, value_type f_val);
    
    /// @brief Removes the last point
    void pop_back();

    // =========================================================================
    // Raw Data Access (for interop with C APIs, MPI, etc.)
    // =========================================================================
    
    /// @brief Returns pointer to x data
    [[nodiscard]] value_type* x_data() noexcept;
    [[nodiscard]] const value_type* x_data() const noexcept;
    
    /// @brief Returns pointer to v data
    [[nodiscard]] value_type* v_data() noexcept;
    [[nodiscard]] const value_type* v_data() const noexcept;
    
    /// @brief Returns pointer to f data
    [[nodiscard]] value_type* f_data() noexcept;
    [[nodiscard]] const value_type* f_data() const noexcept;

private:
    container_type x_;  ///< Position values
    container_type v_;  ///< Velocity values
    container_type f_;  ///< Distribution function values
};

// =============================================================================
// Free Functions
// =============================================================================

/// @brief Advances particle positions by velocity * dt (free streaming)
/// @param particles The particles to advance
/// @param dt Time step
///
/// Implements: x_new = x_old + v * dt
/// This is parallelized with OpenMP when enabled.
void advance_positions(Particles& particles, double dt);

/// @brief Advances particle velocities by acceleration * dt
/// @param particles The particles to advance  
/// @param acceleration Acceleration value (same for all particles)
/// @param dt Time step
///
/// Implements: v_new = v_old + a * dt
void advance_velocities(Particles& particles, double acceleration, double dt);

} // namespace vps::particles

#endif // VPS_PARTICLES_PARTICLES_H
