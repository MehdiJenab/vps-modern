#include <gtest/gtest.h>
#include <vps/grid/grid.h>

#include <cmath>
#include <numbers>

namespace vps::grid::test {

// =============================================================================
// Grid Construction Tests
// =============================================================================

TEST(GridTest, BasicConstruction) {
    Grid g(100, 0.0, 10.0);
    
    EXPECT_EQ(g.n_cells(), 100);
    EXPECT_DOUBLE_EQ(g.x_min(), 0.0);
    EXPECT_DOUBLE_EQ(g.x_max(), 10.0);
    EXPECT_DOUBLE_EQ(g.length(), 10.0);
    EXPECT_DOUBLE_EQ(g.dx(), 0.1);
    EXPECT_EQ(g.boundary_condition(), BoundaryCondition::Periodic);
}

TEST(GridTest, NonZeroOrigin) {
    Grid g(50, -5.0, 5.0);
    
    EXPECT_DOUBLE_EQ(g.x_min(), -5.0);
    EXPECT_DOUBLE_EQ(g.x_max(), 5.0);
    EXPECT_DOUBLE_EQ(g.length(), 10.0);
    EXPECT_DOUBLE_EQ(g.dx(), 0.2);
}

TEST(GridTest, InvalidConstruction_ZeroCells) {
    EXPECT_THROW(Grid(0, 0.0, 1.0), std::invalid_argument);
}

TEST(GridTest, InvalidConstruction_InvertedDomain) {
    EXPECT_THROW(Grid(10, 10.0, 0.0), std::invalid_argument);
}

TEST(GridTest, InvalidConstruction_ZeroLength) {
    EXPECT_THROW(Grid(10, 5.0, 5.0), std::invalid_argument);
}

// =============================================================================
// Cell Position Tests
// =============================================================================

TEST(GridTest, CellCenters) {
    Grid g(4, 0.0, 4.0);  // dx = 1.0
    
    // Cell centers should be at 0.5, 1.5, 2.5, 3.5
    EXPECT_DOUBLE_EQ(g.cell_center(0), 0.5);
    EXPECT_DOUBLE_EQ(g.cell_center(1), 1.5);
    EXPECT_DOUBLE_EQ(g.cell_center(2), 2.5);
    EXPECT_DOUBLE_EQ(g.cell_center(3), 3.5);
}

TEST(GridTest, CellEdges) {
    Grid g(4, 0.0, 4.0);  // dx = 1.0
    
    // Cell 0 edges
    EXPECT_DOUBLE_EQ(g.cell_left(0), 0.0);
    EXPECT_DOUBLE_EQ(g.cell_right(0), 1.0);
    
    // Cell 2 edges
    EXPECT_DOUBLE_EQ(g.cell_left(2), 2.0);
    EXPECT_DOUBLE_EQ(g.cell_right(2), 3.0);
}

TEST(GridTest, CellCentersVector) {
    Grid g(3, 0.0, 3.0);
    auto centers = g.cell_centers();
    
    EXPECT_EQ(centers.size(), 3);
    EXPECT_DOUBLE_EQ(centers[0], 0.5);
    EXPECT_DOUBLE_EQ(centers[1], 1.5);
    EXPECT_DOUBLE_EQ(centers[2], 2.5);
}

// =============================================================================
// Cell Index Tests
// =============================================================================

TEST(GridTest, CellIndex_Interior) {
    Grid g(10, 0.0, 10.0);  // dx = 1.0
    
    EXPECT_EQ(g.cell_index(0.5), 0);
    EXPECT_EQ(g.cell_index(1.5), 1);
    EXPECT_EQ(g.cell_index(9.5), 9);
}

TEST(GridTest, CellIndex_Edges) {
    Grid g(10, 0.0, 10.0);
    
    // Left edge belongs to cell 0
    EXPECT_EQ(g.cell_index(0.0), 0);
    
    // Just before right boundary
    EXPECT_EQ(g.cell_index(9.99), 9);
}

TEST(GridTest, CellIndex_Periodic) {
    Grid g(10, 0.0, 10.0, BoundaryCondition::Periodic);
    
    // Position beyond x_max wraps to beginning
    EXPECT_EQ(g.cell_index(10.5), 0);
    EXPECT_EQ(g.cell_index(11.5), 1);
    
    // Negative position wraps from end
    EXPECT_EQ(g.cell_index(-0.5), 9);
    EXPECT_EQ(g.cell_index(-1.5), 8);
}

// =============================================================================
// Position Wrapping Tests
// =============================================================================

TEST(GridTest, WrapPosition_InsideDomain) {
    Grid g(10, 0.0, 10.0, BoundaryCondition::Periodic);
    
    EXPECT_DOUBLE_EQ(g.wrap_position(5.0), 5.0);
    EXPECT_DOUBLE_EQ(g.wrap_position(0.0), 0.0);
}

TEST(GridTest, WrapPosition_BeyondMax) {
    Grid g(10, 0.0, 10.0, BoundaryCondition::Periodic);
    
    EXPECT_DOUBLE_EQ(g.wrap_position(10.0), 0.0);
    EXPECT_DOUBLE_EQ(g.wrap_position(12.5), 2.5);
    EXPECT_DOUBLE_EQ(g.wrap_position(25.0), 5.0);  // Two full periods
}

TEST(GridTest, WrapPosition_BelowMin) {
    Grid g(10, 0.0, 10.0, BoundaryCondition::Periodic);
    
    EXPECT_DOUBLE_EQ(g.wrap_position(-2.5), 7.5);
    EXPECT_DOUBLE_EQ(g.wrap_position(-10.0), 0.0);
    EXPECT_DOUBLE_EQ(g.wrap_position(-12.5), 7.5);
}

TEST(GridTest, WrapPosition_NonZeroOrigin) {
    Grid g(10, -5.0, 5.0, BoundaryCondition::Periodic);
    
    EXPECT_DOUBLE_EQ(g.wrap_position(0.0), 0.0);
    EXPECT_DOUBLE_EQ(g.wrap_position(7.0), -3.0);
    EXPECT_DOUBLE_EQ(g.wrap_position(-7.0), 3.0);
}

// =============================================================================
// Index Wrapping Tests
// =============================================================================

TEST(GridTest, WrapIndex) {
    Grid g(10, 0.0, 10.0, BoundaryCondition::Periodic);
    
    EXPECT_EQ(g.wrap_index(0), 0);
    EXPECT_EQ(g.wrap_index(9), 9);
    EXPECT_EQ(g.wrap_index(10), 0);
    EXPECT_EQ(g.wrap_index(11), 1);
    EXPECT_EQ(g.wrap_index(-1), 9);
    EXPECT_EQ(g.wrap_index(-10), 0);
}

// =============================================================================
// Contains Tests
// =============================================================================

TEST(GridTest, Contains) {
    Grid g(10, 0.0, 10.0);
    
    EXPECT_TRUE(g.contains(0.0));
    EXPECT_TRUE(g.contains(5.0));
    EXPECT_TRUE(g.contains(9.99));
    EXPECT_FALSE(g.contains(-0.1));
    EXPECT_FALSE(g.contains(10.0));  // x_max is exclusive
    EXPECT_FALSE(g.contains(10.1));
}

// =============================================================================
// Interpolation Weight Tests
// =============================================================================

TEST(GridTest, InterpolationWeights_CellCenter) {
    Grid g(4, 0.0, 4.0);  // dx = 1.0, centers at 0.5, 1.5, 2.5, 3.5
    
    // At cell center, weights should be (0.5, 0.5) for edge-based interp
    // But we use cell-centered, so at center weights are (0.5, 0.5)
    auto [w_left, w_right] = g.interpolation_weights(0.5);
    EXPECT_DOUBLE_EQ(w_left + w_right, 1.0);
}

TEST(GridTest, InterpolationWeights_LeftEdge) {
    Grid g(4, 0.0, 4.0);  // dx = 1.0
    
    // At left edge of cell 1 (x=1.0), full weight on left
    auto [w_left, w_right] = g.interpolation_weights(1.0);
    EXPECT_DOUBLE_EQ(w_left, 1.0);
    EXPECT_DOUBLE_EQ(w_right, 0.0);
}

TEST(GridTest, InterpolationWeights_RightEdge) {
    Grid g(4, 0.0, 4.0);  // dx = 1.0
    
    // Just before right edge of cell 1
    auto [w_left, w_right] = g.interpolation_weights(1.9);
    EXPECT_NEAR(w_left, 0.1, 1e-10);
    EXPECT_NEAR(w_right, 0.9, 1e-10);
}

// =============================================================================
// Field Tests
// =============================================================================

TEST(FieldTest, Construction) {
    Grid g(10, 0.0, 10.0);
    Field f(g);
    
    EXPECT_EQ(f.size(), 10);
    
    // Default value is 0
    for (std::size_t i = 0; i < f.size(); ++i) {
        EXPECT_DOUBLE_EQ(f[i], 0.0);
    }
}

TEST(FieldTest, ConstructionWithValue) {
    Grid g(10, 0.0, 10.0);
    Field f(g, 5.0);
    
    for (std::size_t i = 0; i < f.size(); ++i) {
        EXPECT_DOUBLE_EQ(f[i], 5.0);
    }
}

TEST(FieldTest, ElementAccess) {
    Grid g(10, 0.0, 10.0);
    Field f(g);
    
    f[5] = 42.0;
    EXPECT_DOUBLE_EQ(f[5], 42.0);
}

TEST(FieldTest, Fill) {
    Grid g(10, 0.0, 10.0);
    Field f(g);
    
    f.fill(3.14);
    for (std::size_t i = 0; i < f.size(); ++i) {
        EXPECT_DOUBLE_EQ(f[i], 3.14);
    }
}

TEST(FieldTest, Zero) {
    Grid g(10, 0.0, 10.0);
    Field f(g, 5.0);
    
    f.zero();
    for (std::size_t i = 0; i < f.size(); ++i) {
        EXPECT_DOUBLE_EQ(f[i], 0.0);
    }
}

TEST(FieldTest, SpanAccess) {
    Grid g(10, 0.0, 10.0);
    Field f(g, 1.0);
    
    auto vals = f.values();
    EXPECT_EQ(vals.size(), 10);
    
    // Modify through span
    vals[0] = 999.0;
    EXPECT_DOUBLE_EQ(f[0], 999.0);
}

TEST(FieldTest, GridReference) {
    Grid g(10, 0.0, 10.0);
    Field f(g);
    
    EXPECT_DOUBLE_EQ(f.grid().dx(), 1.0);
    EXPECT_EQ(&f.grid(), &g);
}

TEST(FieldTest, InterpolateConstant) {
    Grid g(10, 0.0, 10.0);
    Field f(g, 5.0);  // Constant field
    
    // Interpolation of constant should give constant
    EXPECT_DOUBLE_EQ(f.interpolate(0.0), 5.0);
    EXPECT_DOUBLE_EQ(f.interpolate(5.5), 5.0);
    EXPECT_DOUBLE_EQ(f.interpolate(9.9), 5.0);
}

TEST(FieldTest, InterpolateLinear) {
    Grid g(4, 0.0, 4.0);  // dx = 1.0
    Field f(g);
    
    // Set up linear field: f[i] = i
    for (std::size_t i = 0; i < f.size(); ++i) {
        f[i] = static_cast<double>(i);
    }
    
    // At cell edges (start of each cell)
    // x=0: cell 0, left edge
    EXPECT_DOUBLE_EQ(f.interpolate(0.0), 0.0);
    
    // x=1.5: midpoint between cells 1 and 2
    // f[1]=1, f[2]=2, should interpolate to 1.5
    EXPECT_DOUBLE_EQ(f.interpolate(1.5), 1.5);
}

TEST(FieldTest, InterpolatePeriodic) {
    Grid g(4, 0.0, 4.0, BoundaryCondition::Periodic);
    Field f(g);
    
    f[0] = 0.0;
    f[1] = 1.0;
    f[2] = 2.0;
    f[3] = 1.0;
    
    // Position beyond domain should wrap and interpolate correctly
    double val = f.interpolate(4.5);  // Wraps to 0.5
    double expected = f.interpolate(0.5);
    EXPECT_DOUBLE_EQ(val, expected);
}

// =============================================================================
// Copy/Move Tests
// =============================================================================

TEST(FieldTest, CopyConstruction) {
    Grid g(10, 0.0, 10.0);
    Field f1(g, 1.0);
    f1[5] = 42.0;
    
    Field f2(f1);
    
    EXPECT_DOUBLE_EQ(f2[5], 42.0);
    
    // Modify original, copy unchanged
    f1[5] = 0.0;
    EXPECT_DOUBLE_EQ(f2[5], 42.0);
}

TEST(FieldTest, MoveConstruction) {
    Grid g(10, 0.0, 10.0);
    Field f1(g, 1.0);
    f1[5] = 42.0;
    
    Field f2(std::move(f1));
    
    EXPECT_DOUBLE_EQ(f2[5], 42.0);
}

} // namespace vps::grid::test
