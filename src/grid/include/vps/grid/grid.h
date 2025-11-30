#ifndef VPS_GRID_GRID_H
#define VPS_GRID_GRID_H

/// @file grid.h
/// @brief 1D uniform spatial grid for Vlasov-Poisson simulations
///
/// This module provides a uniform grid in configuration space (x-direction)
/// with support for periodic boundary conditions.

#include <cstddef>
#include <span>
#include <stdexcept>
#include <vector>

namespace vps::grid {

/// @brief Enumeration of supported boundary condition types
enum class BoundaryCondition {
    Periodic,    ///< Periodic boundaries (x wraps around)
    // Future: Dirichlet, Neumann, Open
};

/// @brief 1D uniform spatial grid
///
/// Represents a uniform discretization of a 1D spatial domain [x_min, x_max].
/// The grid stores cell-centered quantities and provides utilities for:
/// - Index/position conversion
/// - Boundary condition handling
/// - Field storage (density, electric field, potential)
///
/// Grid layout (cell-centered):
/// @code
///     x_min                                           x_max
///       |-------|-------|-------|-------|-------|-------|
///       |   0   |   1   |   2   |  ...  |  n-2  |  n-1  |
///       |-------|-------|-------|-------|-------|-------|
///            ^
///        cell center at x_min + dx/2
/// @endcode
class Grid {
public:
    // =========================================================================
    // Type Aliases
    // =========================================================================
    using value_type = double;
    using size_type = std::size_t;

    // =========================================================================
    // Constructors
    // =========================================================================
    
    /// @brief Construct a uniform grid
    /// @param n_cells Number of grid cells
    /// @param x_min Left boundary of domain
    /// @param x_max Right boundary of domain
    /// @param bc Boundary condition type
    /// @throws std::invalid_argument if n_cells == 0 or x_min >= x_max
    Grid(size_type n_cells, 
         value_type x_min, 
         value_type x_max, 
         BoundaryCondition bc = BoundaryCondition::Periodic);

    // Default copy/move
    Grid(const Grid&) = default;
    Grid(Grid&&) noexcept = default;
    Grid& operator=(const Grid&) = default;
    Grid& operator=(Grid&&) noexcept = default;
    ~Grid() = default;

    // =========================================================================
    // Grid Properties
    // =========================================================================
    
    /// @brief Returns the number of cells
    [[nodiscard]] size_type n_cells() const noexcept;
    
    /// @brief Returns the left boundary
    [[nodiscard]] value_type x_min() const noexcept;
    
    /// @brief Returns the right boundary
    [[nodiscard]] value_type x_max() const noexcept;
    
    /// @brief Returns the domain length
    [[nodiscard]] value_type length() const noexcept;
    
    /// @brief Returns the cell width (dx)
    [[nodiscard]] value_type dx() const noexcept;
    
    /// @brief Returns the boundary condition type
    [[nodiscard]] BoundaryCondition boundary_condition() const noexcept;

    // =========================================================================
    // Position/Index Conversion
    // =========================================================================
    
    /// @brief Returns the cell-center position for cell index i
    /// @param i Cell index (0 to n_cells-1)
    [[nodiscard]] value_type cell_center(size_type i) const noexcept;
    
    /// @brief Returns the left edge position for cell index i
    [[nodiscard]] value_type cell_left(size_type i) const noexcept;
    
    /// @brief Returns the right edge position for cell index i
    [[nodiscard]] value_type cell_right(size_type i) const noexcept;
    
    /// @brief Returns the cell index containing position x
    /// @param x Position in the domain
    /// @note For periodic BC, x is wrapped to [x_min, x_max) first
    [[nodiscard]] size_type cell_index(value_type x) const noexcept;
    
    /// @brief Returns interpolation weight for position x in cell
    /// @param x Position
    /// @return Pair of (left_weight, right_weight) for linear interpolation
    ///
    /// For a position x in cell i, returns weights such that:
    /// value = left_weight * field[i] + right_weight * field[i+1]
    [[nodiscard]] std::pair<value_type, value_type> 
    interpolation_weights(value_type x) const noexcept;

    // =========================================================================
    // Boundary Handling
    // =========================================================================
    
    /// @brief Wraps position x into the domain [x_min, x_max)
    /// @param x Position (possibly outside domain)
    /// @return Position wrapped to domain (for periodic BC)
    [[nodiscard]] value_type wrap_position(value_type x) const noexcept;
    
    /// @brief Wraps index i into valid range [0, n_cells)
    /// @param i Index (possibly negative or >= n_cells)
    /// @return Wrapped index (for periodic BC)
    [[nodiscard]] size_type wrap_index(std::ptrdiff_t i) const noexcept;
    
    /// @brief Checks if position x is inside the domain
    [[nodiscard]] bool contains(value_type x) const noexcept;

    // =========================================================================
    // Utility: Generate cell centers
    // =========================================================================
    
    /// @brief Returns a vector of all cell-center positions
    [[nodiscard]] std::vector<value_type> cell_centers() const;

private:
    size_type n_cells_;       ///< Number of cells
    value_type x_min_;        ///< Left boundary
    value_type x_max_;        ///< Right boundary
    value_type length_;       ///< Domain length
    value_type dx_;           ///< Cell width
    value_type inv_dx_;       ///< 1/dx (cached for performance)
    BoundaryCondition bc_;    ///< Boundary condition
};

// =============================================================================
// Field Class - Stores quantities on the grid
// =============================================================================

/// @brief Stores a scalar field on the grid
///
/// This class manages storage for cell-centered field values like
/// density, potential, electric field, etc.
class Field {
public:
    using value_type = double;
    using size_type = std::size_t;

    /// @brief Construct field on given grid
    /// @param grid The grid this field lives on
    /// @param initial_value Initial value for all cells
    explicit Field(const Grid& grid, value_type initial_value = 0.0);

    // Default copy/move
    Field(const Field&) = default;
    Field(Field&&) noexcept = default;
    Field& operator=(const Field&) = default;
    Field& operator=(Field&&) noexcept = default;
    ~Field() = default;

    // =========================================================================
    // Access
    // =========================================================================
    
    /// @brief Access element at index i
    [[nodiscard]] value_type& operator[](size_type i) noexcept;
    [[nodiscard]] const value_type& operator[](size_type i) const noexcept;
    
    /// @brief Returns the number of cells
    [[nodiscard]] size_type size() const noexcept;
    
    /// @brief Returns span over all values
    [[nodiscard]] std::span<value_type> values() noexcept;
    [[nodiscard]] std::span<const value_type> values() const noexcept;
    
    /// @brief Returns pointer to raw data
    [[nodiscard]] value_type* data() noexcept;
    [[nodiscard]] const value_type* data() const noexcept;
    
    /// @brief Returns reference to the underlying grid
    [[nodiscard]] const Grid& grid() const noexcept;

    // =========================================================================
    // Operations
    // =========================================================================
    
    /// @brief Sets all values to given value
    void fill(value_type val) noexcept;
    
    /// @brief Sets all values to zero
    void zero() noexcept;
    
    /// @brief Interpolate field value at position x (linear interpolation)
    /// @param x Position in the domain
    [[nodiscard]] value_type interpolate(value_type x) const noexcept;

private:
    const Grid* grid_;              ///< Pointer to grid (non-owning)
    std::vector<value_type> data_;  ///< Field values
};

} // namespace vps::grid

#endif // VPS_GRID_GRID_H
