#include "vps/particles/particles.h"

#include <algorithm>
#include <cassert>

#ifdef VPS_ENABLE_OPENMP
#include <omp.h>
#endif

namespace vps::particles {

// =============================================================================
// Constructors
// =============================================================================

Particles::Particles(size_type capacity) {
    reserve(capacity);
}

Particles::Particles(size_type size, value_type x_val, value_type v_val, value_type f_val)
    : x_(size, x_val)
    , v_(size, v_val)
    , f_(size, f_val) 
{}

// =============================================================================
// Capacity
// =============================================================================

Particles::size_type Particles::size() const noexcept {
    return x_.size();
}

Particles::size_type Particles::capacity() const noexcept {
    return x_.capacity();
}

bool Particles::empty() const noexcept {
    return x_.empty();
}

void Particles::reserve(size_type n) {
    x_.reserve(n);
    v_.reserve(n);
    f_.reserve(n);
}

void Particles::resize(size_type n) {
    x_.resize(n);
    v_.resize(n);
    f_.resize(n);
}

void Particles::resize(size_type n, value_type x_val, value_type v_val, value_type f_val) {
    x_.resize(n, x_val);
    v_.resize(n, v_val);
    f_.resize(n, f_val);
}

void Particles::clear() noexcept {
    x_.clear();
    v_.clear();
    f_.clear();
}

// =============================================================================
// Element Access - Span-based
// =============================================================================

std::span<Particles::value_type> Particles::x() noexcept {
    return x_;
}

std::span<const Particles::value_type> Particles::x() const noexcept {
    return x_;
}

std::span<Particles::value_type> Particles::v() noexcept {
    return v_;
}

std::span<const Particles::value_type> Particles::v() const noexcept {
    return v_;
}

std::span<Particles::value_type> Particles::f() noexcept {
    return f_;
}

std::span<const Particles::value_type> Particles::f() const noexcept {
    return f_;
}

// =============================================================================
// Element Access - Index-based
// =============================================================================

Particles::value_type& Particles::x(size_type i) noexcept {
    assert(i < size() && "Index out of bounds");
    return x_[i];
}

const Particles::value_type& Particles::x(size_type i) const noexcept {
    assert(i < size() && "Index out of bounds");
    return x_[i];
}

Particles::value_type& Particles::v(size_type i) noexcept {
    assert(i < size() && "Index out of bounds");
    return v_[i];
}

const Particles::value_type& Particles::v(size_type i) const noexcept {
    assert(i < size() && "Index out of bounds");
    return v_[i];
}

Particles::value_type& Particles::f(size_type i) noexcept {
    assert(i < size() && "Index out of bounds");
    return f_[i];
}

const Particles::value_type& Particles::f(size_type i) const noexcept {
    assert(i < size() && "Index out of bounds");
    return f_[i];
}

// =============================================================================
// Modifiers
// =============================================================================

void Particles::push_back(value_type x_val, value_type v_val, value_type f_val) {
    x_.push_back(x_val);
    v_.push_back(v_val);
    f_.push_back(f_val);
}

void Particles::pop_back() {
    assert(!empty() && "Cannot pop from empty container");
    x_.pop_back();
    v_.pop_back();
    f_.pop_back();
}

// =============================================================================
// Raw Data Access
// =============================================================================

Particles::value_type* Particles::x_data() noexcept {
    return x_.data();
}

const Particles::value_type* Particles::x_data() const noexcept {
    return x_.data();
}

Particles::value_type* Particles::v_data() noexcept {
    return v_.data();
}

const Particles::value_type* Particles::v_data() const noexcept {
    return v_.data();
}

Particles::value_type* Particles::f_data() noexcept {
    return f_.data();
}

const Particles::value_type* Particles::f_data() const noexcept {
    return f_.data();
}

// =============================================================================
// Free Functions
// =============================================================================

void advance_positions(Particles& particles, double dt) {
    auto x = particles.x();
    auto v = particles.v();
    const auto n = particles.size();

#ifdef VPS_ENABLE_OPENMP
    #pragma omp parallel for simd
#endif
    for (std::size_t i = 0; i < n; ++i) {
        x[i] += v[i] * dt;
    }
}

void advance_velocities(Particles& particles, double acceleration, double dt) {
    auto v = particles.v();
    const auto n = particles.size();
    const double dv = acceleration * dt;

#ifdef VPS_ENABLE_OPENMP
    #pragma omp parallel for simd
#endif
    for (std::size_t i = 0; i < n; ++i) {
        v[i] += dv;
    }
}

} // namespace vps::particles
