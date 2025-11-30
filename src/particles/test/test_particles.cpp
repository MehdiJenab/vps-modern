#include <gtest/gtest.h>
#include <vps/particles/particles.h>

#include <cmath>
#include <numeric>

namespace vps::particles::test {

// =============================================================================
// Construction Tests
// =============================================================================

TEST(ParticlesTest, DefaultConstruction) {
    Particles p;
    EXPECT_EQ(p.size(), 0);
    EXPECT_TRUE(p.empty());
}

TEST(ParticlesTest, ConstructWithCapacity) {
    Particles p(100);
    EXPECT_EQ(p.size(), 0);
    EXPECT_GE(p.capacity(), 100);
}

TEST(ParticlesTest, ConstructWithSizeAndValues) {
    Particles p(10, 1.0, 2.0, 3.0);
    EXPECT_EQ(p.size(), 10);
    
    for (std::size_t i = 0; i < p.size(); ++i) {
        EXPECT_DOUBLE_EQ(p.x(i), 1.0);
        EXPECT_DOUBLE_EQ(p.v(i), 2.0);
        EXPECT_DOUBLE_EQ(p.f(i), 3.0);
    }
}

// =============================================================================
// Copy/Move Tests
// =============================================================================

TEST(ParticlesTest, CopyConstruction) {
    Particles p1(5, 1.0, 2.0, 3.0);
    Particles p2(p1);
    
    EXPECT_EQ(p2.size(), 5);
    EXPECT_DOUBLE_EQ(p2.x(0), 1.0);
    
    // Modify original, copy should be unchanged
    p1.x(0) = 999.0;
    EXPECT_DOUBLE_EQ(p2.x(0), 1.0);
}

TEST(ParticlesTest, MoveConstruction) {
    Particles p1(5, 1.0, 2.0, 3.0);
    Particles p2(std::move(p1));
    
    EXPECT_EQ(p2.size(), 5);
    EXPECT_DOUBLE_EQ(p2.x(0), 1.0);
}

// =============================================================================
// Capacity Tests
// =============================================================================

TEST(ParticlesTest, Reserve) {
    Particles p;
    p.reserve(1000);
    
    EXPECT_GE(p.capacity(), 1000);
    EXPECT_EQ(p.size(), 0);  // Reserve doesn't change size
}

TEST(ParticlesTest, Resize) {
    Particles p;
    p.resize(10);
    
    EXPECT_EQ(p.size(), 10);
}

TEST(ParticlesTest, ResizeWithValues) {
    Particles p;
    p.resize(5, 1.0, 2.0, 3.0);
    
    EXPECT_EQ(p.size(), 5);
    EXPECT_DOUBLE_EQ(p.x(4), 1.0);
    EXPECT_DOUBLE_EQ(p.v(4), 2.0);
    EXPECT_DOUBLE_EQ(p.f(4), 3.0);
}

TEST(ParticlesTest, Clear) {
    Particles p(10, 1.0, 2.0, 3.0);
    p.clear();
    
    EXPECT_EQ(p.size(), 0);
    EXPECT_TRUE(p.empty());
}

// =============================================================================
// Element Access Tests
// =============================================================================

TEST(ParticlesTest, IndexAccess) {
    Particles p(3, 0.0, 0.0, 0.0);
    
    p.x(0) = 1.0;
    p.v(1) = 2.0;
    p.f(2) = 3.0;
    
    EXPECT_DOUBLE_EQ(p.x(0), 1.0);
    EXPECT_DOUBLE_EQ(p.v(1), 2.0);
    EXPECT_DOUBLE_EQ(p.f(2), 3.0);
}

TEST(ParticlesTest, SpanAccess) {
    Particles p(5, 1.0, 2.0, 3.0);
    
    auto x_span = p.x();
    auto v_span = p.v();
    auto f_span = p.f();
    
    EXPECT_EQ(x_span.size(), 5);
    EXPECT_EQ(v_span.size(), 5);
    EXPECT_EQ(f_span.size(), 5);
    
    // Modify through span
    x_span[0] = 100.0;
    EXPECT_DOUBLE_EQ(p.x(0), 100.0);
}

TEST(ParticlesTest, ConstSpanAccess) {
    const Particles p(5, 1.0, 2.0, 3.0);
    
    auto x_span = p.x();
    EXPECT_EQ(x_span.size(), 5);
    EXPECT_DOUBLE_EQ(x_span[0], 1.0);
}

TEST(ParticlesTest, RawDataAccess) {
    Particles p(3, 1.0, 2.0, 3.0);
    
    double* x_ptr = p.x_data();
    EXPECT_NE(x_ptr, nullptr);
    EXPECT_DOUBLE_EQ(x_ptr[0], 1.0);
    
    // Modify through pointer
    x_ptr[0] = 999.0;
    EXPECT_DOUBLE_EQ(p.x(0), 999.0);
}

// =============================================================================
// Modifier Tests
// =============================================================================

TEST(ParticlesTest, PushBack) {
    Particles p;
    
    p.push_back(1.0, 2.0, 3.0);
    p.push_back(4.0, 5.0, 6.0);
    
    EXPECT_EQ(p.size(), 2);
    EXPECT_DOUBLE_EQ(p.x(0), 1.0);
    EXPECT_DOUBLE_EQ(p.v(0), 2.0);
    EXPECT_DOUBLE_EQ(p.f(0), 3.0);
    EXPECT_DOUBLE_EQ(p.x(1), 4.0);
    EXPECT_DOUBLE_EQ(p.v(1), 5.0);
    EXPECT_DOUBLE_EQ(p.f(1), 6.0);
}

TEST(ParticlesTest, PopBack) {
    Particles p;
    p.push_back(1.0, 2.0, 3.0);
    p.push_back(4.0, 5.0, 6.0);
    
    p.pop_back();
    
    EXPECT_EQ(p.size(), 1);
    EXPECT_DOUBLE_EQ(p.x(0), 1.0);
}

// =============================================================================
// Free Function Tests
// =============================================================================

TEST(ParticlesTest, AdvancePositions) {
    Particles p;
    p.push_back(0.0, 1.0, 1.0);  // x=0, v=1
    p.push_back(0.0, 2.0, 1.0);  // x=0, v=2
    p.push_back(0.0, -1.0, 1.0); // x=0, v=-1
    
    const double dt = 0.5;
    advance_positions(p, dt);
    
    // x_new = x_old + v * dt
    EXPECT_DOUBLE_EQ(p.x(0), 0.5);   // 0 + 1 * 0.5
    EXPECT_DOUBLE_EQ(p.x(1), 1.0);   // 0 + 2 * 0.5
    EXPECT_DOUBLE_EQ(p.x(2), -0.5);  // 0 + (-1) * 0.5
    
    // Velocities unchanged
    EXPECT_DOUBLE_EQ(p.v(0), 1.0);
    EXPECT_DOUBLE_EQ(p.v(1), 2.0);
    EXPECT_DOUBLE_EQ(p.v(2), -1.0);
}

TEST(ParticlesTest, AdvanceVelocities) {
    Particles p;
    p.push_back(0.0, 1.0, 1.0);
    p.push_back(0.0, 2.0, 1.0);
    
    const double a = 2.0;   // acceleration
    const double dt = 0.5;
    advance_velocities(p, a, dt);
    
    // v_new = v_old + a * dt
    EXPECT_DOUBLE_EQ(p.v(0), 2.0);  // 1 + 2 * 0.5
    EXPECT_DOUBLE_EQ(p.v(1), 3.0);  // 2 + 2 * 0.5
}

TEST(ParticlesTest, FreeStreamingMultipleSteps) {
    // Test free streaming: particle should move linearly
    Particles p;
    p.push_back(0.0, 1.0, 1.0);  // Starting at x=0, v=1
    
    const double dt = 0.1;
    const int steps = 10;
    
    for (int i = 0; i < steps; ++i) {
        advance_positions(p, dt);
    }
    
    // After 10 steps of dt=0.1 with v=1, x should be 1.0
    EXPECT_NEAR(p.x(0), 1.0, 1e-10);
}

// =============================================================================
// Performance/Stress Tests
// =============================================================================

TEST(ParticlesTest, LargeAllocation) {
    const std::size_t n = 1'000'000;
    Particles p(n, 0.0, 1.0, 1.0);
    
    EXPECT_EQ(p.size(), n);
    
    // Do a simple operation to ensure data is valid
    advance_positions(p, 0.1);
    
    // All particles should have moved
    EXPECT_DOUBLE_EQ(p.x(0), 0.1);
    EXPECT_DOUBLE_EQ(p.x(n - 1), 0.1);
}

} // namespace vps::particles::test
