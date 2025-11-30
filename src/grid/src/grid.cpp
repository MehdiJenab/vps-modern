#include "vps/grid/grid.h"

#include <cassert>
#include <cmath>
#include <stdexcept>

namespace vps::grid {

// =============================================================================
// Grid Implementation
// =============================================================================

Grid::Grid(size_type n_cells, value_type x_min, value_type x_max, BoundaryCondition bc)
    : n_cells_(n_cells)
    , x_min_(x_min)
    , x_max_(x_max)
    , length_(x_max - x_min)
    , dx_(length_ / static_cast<value_type>(n_cells))
    , inv_dx_(1.0 / dx_)
    , bc_(bc)
{
    if (n_cells == 0) {
        throw std::invalid_argument("Grid must have at least one cell");
    }
    if (x_min >= x_max) {
        throw std::invalid_argument("x_min must be less than x_max");
    }
}

Grid::size_type Grid::n_cells() const noexcept {
    return n_cells_;
}

Grid::value_type Grid::x_min() const noexcept {
    return x_min_;
}

Grid::value_type Grid::x_max() const noexcept {
    return x_max_;
}

Grid::value_type Grid::length() const noexcept {
    return length_;
}

Grid::value_type Grid::dx() const noexcept {
    return dx_;
}

BoundaryCondition Grid::boundary_condition() const noexcept {
    return bc_;
}

Grid::value_type Grid::cell_center(size_type i) const noexcept {
    assert(i < n_cells_ && "Cell index out of bounds");
    return x_min_ + (static_cast<value_type>(i) + 0.5) * dx_;
}

Grid::value_type Grid::cell_left(size_type i) const noexcept {
    assert(i < n_cells_ && "Cell index out of bounds");
    return x_min_ + static_cast<value_type>(i) * dx_;
}

Grid::value_type Grid::cell_right(size_type i) const noexcept {
    assert(i < n_cells_ && "Cell index out of bounds");
    return x_min_ + static_cast<value_type>(i + 1) * dx_;
}

Grid::size_type Grid::cell_index(value_type x) const noexcept {
    // Wrap position first for periodic BC
    value_type x_wrapped = wrap_position(x);
    
    // Compute index
    auto idx = static_cast<std::ptrdiff_t>((x_wrapped - x_min_) * inv_dx_);
    
    // Clamp to valid range (handles edge cases due to floating point)
    if (idx < 0) idx = 0;
    if (static_cast<size_type>(idx) >= n_cells_) idx = static_cast<std::ptrdiff_t>(n_cells_ - 1);
    
    return static_cast<size_type>(idx);
}

std::pair<Grid::value_type, Grid::value_type> 
Grid::interpolation_weights(value_type x) const noexcept {
    value_type x_wrapped = wrap_position(x);
    
    // Position relative to left edge of containing cell
    size_type idx = cell_index(x_wrapped);
    value_type x_left = cell_left(idx);
    
    // Weight is distance from left edge, normalized by dx
    value_type right_weight = (x_wrapped - x_left) * inv_dx_;
    value_type left_weight = 1.0 - right_weight;
    
    return {left_weight, right_weight};
}

Grid::value_type Grid::wrap_position(value_type x) const noexcept {
    if (bc_ == BoundaryCondition::Periodic) {
        // Use fmod for wrapping
        value_type x_rel = x - x_min_;
        x_rel = std::fmod(x_rel, length_);
        if (x_rel < 0.0) {
            x_rel += length_;
        }
        return x_min_ + x_rel;
    }
    // For non-periodic, just return x (or clamp in future)
    return x;
}

Grid::size_type Grid::wrap_index(std::ptrdiff_t i) const noexcept {
    if (bc_ == BoundaryCondition::Periodic) {
        std::ptrdiff_t n = static_cast<std::ptrdiff_t>(n_cells_);
        i = i % n;
        if (i < 0) {
            i += n;
        }
        return static_cast<size_type>(i);
    }
    // For non-periodic, clamp
    if (i < 0) return 0;
    if (static_cast<size_type>(i) >= n_cells_) return n_cells_ - 1;
    return static_cast<size_type>(i);
}

bool Grid::contains(value_type x) const noexcept {
    return x >= x_min_ && x < x_max_;
}

std::vector<Grid::value_type> Grid::cell_centers() const {
    std::vector<value_type> centers(n_cells_);
    for (size_type i = 0; i < n_cells_; ++i) {
        centers[i] = cell_center(i);
    }
    return centers;
}

// =============================================================================
// Field Implementation
// =============================================================================

Field::Field(const Grid& grid, value_type initial_value)
    : grid_(&grid)
    , data_(grid.n_cells(), initial_value)
{}

Field::value_type& Field::operator[](size_type i) noexcept {
    assert(i < data_.size() && "Index out of bounds");
    return data_[i];
}

const Field::value_type& Field::operator[](size_type i) const noexcept {
    assert(i < data_.size() && "Index out of bounds");
    return data_[i];
}

Field::size_type Field::size() const noexcept {
    return data_.size();
}

std::span<Field::value_type> Field::values() noexcept {
    return data_;
}

std::span<const Field::value_type> Field::values() const noexcept {
    return data_;
}

Field::value_type* Field::data() noexcept {
    return data_.data();
}

const Field::value_type* Field::data() const noexcept {
    return data_.data();
}

const Grid& Field::grid() const noexcept {
    return *grid_;
}

void Field::fill(value_type val) noexcept {
    std::fill(data_.begin(), data_.end(), val);
}

void Field::zero() noexcept {
    fill(0.0);
}

Field::value_type Field::interpolate(value_type x) const noexcept {
    // Get cell index and weights
    size_type idx = grid_->cell_index(x);
    auto [w_left, w_right] = grid_->interpolation_weights(x);
    
    // Get neighbor index (handles periodic wrapping)
    size_type idx_next = grid_->wrap_index(static_cast<std::ptrdiff_t>(idx) + 1);
    
    // Linear interpolation
    return w_left * data_[idx] + w_right * data_[idx_next];
}

} // namespace vps::grid
