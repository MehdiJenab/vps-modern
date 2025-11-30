# Vlasov-Poisson Kinetic Plasma Solver

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.20+-green.svg)](https://cmake.org/)

A modern C++23 implementation of a fully kinetic Vlasov-Poisson solver for collisionless plasma simulations. This code employs the **method of characteristics** (kinetic phase point trajectories) to solve the Vlasov equation, coupled with Poisson's equation for self-consistent electric field computation.

## Scientific Background

The Vlasov-Poisson system describes the evolution of a collisionless plasma:

```
∂fₛ/∂t + v ∂fₛ/∂x + (qₛE/mₛ) ∂fₛ/∂v = 0

∂²φ/∂x² = -(e/ε₀) Σₛ qₛ ∫ fₛ dv
```

where `fₛ(x, v, t)` is the distribution function for species `s`, and `E = -∂φ/∂x` is the self-consistent electric field.

This solver uses **mobile phase points** that carry constant distribution function values (Liouville's theorem), interpolated onto a fixed spatial grid for field computation. This approach avoids the recurrence effect that plagues traditional Eulerian Vlasov codes.

## Key Features

- **Modern C++23**: Leverages concepts, ranges, `std::span`, and other modern features
- **Struct-of-Arrays (SoA) Layout**: Cache-efficient memory layout for vectorization
- **Multi-Species Support**: Electrons, ions, and dust particles (planned)
- **Modular Architecture**: Clean separation of concerns with independent modules
- **Comprehensive Testing**: Google Test-based unit tests for all components
- **Parallel Ready**: OpenMP support with MPI and Kokkos/GPU planned

## Development Roadmap

| Phase | Description | Status |
|-------|-------------|--------|
| 1 | Free-streaming with periodic boundaries | ✅ Complete |
| 2 | Poisson solver + Linear Landau damping | 🔄 In Progress |
| 3 | Nonlinear Landau damping → BGK modes | ⏳ Planned |
| 4 | Multi-species (ions, dust particles) | ⏳ Planned |
| 5 | External distribution function input (Schamel/ELIN) | ⏳ Planned |
| 6 | MPI parallelization | ⏳ Planned |
| 7 | Kokkos GPU portability | ⏳ Planned |

## Physics Applications

This code is designed to study:

- **Landau Damping**: Linear and nonlinear regimes of collisionless wave damping
- **BGK Modes**: Bernstein-Greene-Kruskal equilibria and their stability
- **Electron/Ion Holes**: Phase-space vortices and their dynamics
- **Soliton Collisions**: Head-on and overtaking collisions of electrostatic solitons
- **Dust-Ion Acoustic Waves**: Wave dynamics in dusty plasmas
- **Trapped Particle Effects**: Schamel distribution functions and electron trapping

## Quick Start

### Prerequisites

- C++23 compatible compiler (GCC 13+, Clang 17+)
- CMake ≥ 3.20
- (Optional) OpenMP for parallelization

### Building

```bash
# Clone the repository
git clone https://github.com/smhosseini/vlasov-poisson-solver.git
cd vlasov-poisson-solver

# Build (uses out-of-source build automatically)
make

# Run tests
make test

# Run the solver
make run

# See all available commands
make help
```

### Alternative: CMake Presets

```bash
cmake --preset debug          # Configure debug build
cmake --build build/debug     # Build
ctest --preset debug          # Run tests
```

### Build Options

| Option | Default | Description |
|--------|---------|-------------|
| `VPS_BUILD_TESTS` | ON | Build unit tests |
| `VPS_BUILD_BENCHMARKS` | OFF | Build performance benchmarks |
| `VPS_ENABLE_OPENMP` | ON | Enable OpenMP parallelization |
| `VPS_ENABLE_MPI` | OFF | Enable MPI parallelization |

## Project Structure

```
vlasov-poisson-solver/
├── CMakeLists.txt          # Root build configuration
├── CMakePresets.json       # Standardized build presets
├── GNUmakefile             # Convenience wrapper for common tasks
├── src/
│   ├── particles/          # Phase point (particle) container
│   │   ├── include/vps/particles/
│   │   ├── src/
│   │   └── test/
│   ├── grid/               # Spatial grid and field storage
│   │   ├── include/vps/grid/
│   │   ├── src/
│   │   └── test/
│   └── app/                # Main application
└── docs/                   # Documentation (planned)
```

## Architecture

### Struct-of-Arrays (SoA) Design

Traditional particle codes use Array-of-Structs (AoS):
```cpp
struct Particle { double x, v, f; };
std::vector<Particle> particles;  // Poor cache utilization
```

This solver uses SoA for better vectorization and cache efficiency:
```cpp
class Particles {
    std::vector<double> x_;  // All positions contiguous
    std::vector<double> v_;  // All velocities contiguous  
    std::vector<double> f_;  // All distribution values contiguous
};
```

### Method of Characteristics

Phase points follow the characteristic equations:

```
dx/dt = v
dv/dt = (q/m) E(x, t)
```

The distribution function `f` remains constant along characteristics (Liouville's theorem), eliminating numerical diffusion in velocity space.

## Related Publications

This code builds upon methods developed in the following peer-reviewed publications:

### Electron Holes and BGK Modes
- S.M. Hosseini Jenab, G. Brodin, J. Juno, I. Kourakis, "Ultrafast electron holes in plasma phase space dynamics," *Scientific Reports* **11**, 16358 (2021). [DOI: 10.1038/s41598-021-95652-w](https://doi.org/10.1038/s41598-021-95652-w)
- S.M. Hosseini Jenab, G. Brodin, "Head-on collision of nonlinear solitary solutions to Vlasov–Poisson equations," *Phys. Plasmas* **26**, 022303 (2019). [DOI: 10.1063/1.5078865](https://doi.org/10.1063/1.5078865)
- S.M. Hosseini Jenab, F. Spanier, G. Brodin, "Scattering of electron holes in the context of ion-acoustic regime," *Phys. Plasmas* **26**, 022305 (2019). [DOI: 10.1063/1.5055945](https://doi.org/10.1063/1.5055945)

### Ion-Acoustic Solitons and Trapped Particles
- S.M. Hosseini Jenab, F. Spanier, G. Brodin, "A study of the stability properties of Sagdeev solutions in the ion-acoustic regime using kinetic simulations," *Phys. Plasmas* **25**, 072304 (2018). [DOI: 10.1063/1.5036764](https://doi.org/10.1063/1.5036764)
- S.M. Hosseini Jenab, F. Spanier, "Electron holes dynamics during collisions of ion-acoustic solitons," *IEEE Trans. Plasma Sci.* **45**(8), 2022 (2017). [DOI: 10.1109/TPS.2017.2715558](https://doi.org/10.1109/TPS.2017.2715558)
- S.M. Hosseini Jenab, F. Spanier, "Ion-acoustic solitons with trapped electrons: Fully kinetic approach," *Phys. Rev. E* **95**, 053201 (2017). [DOI: 10.1103/PhysRevE.95.053201](https://doi.org/10.1103/PhysRevE.95.053201)
- S.M. Hosseini Jenab, F. Spanier, "Overtaking of ion-acoustic solitons in the fully kinetic regime," *Phys. Plasmas* **24**, 032309 (2017). [DOI: 10.1063/1.4978488](https://doi.org/10.1063/1.4978488)
- S.M. Hosseini Jenab, F. Spanier, "Trapping effect on ion-acoustic solitary waves: Fully kinetic approach," *Phys. Plasmas* **23**, 082104 (2016). [DOI: 10.1063/1.4964909](https://doi.org/10.1063/1.4964909)

### Dusty Plasmas
- S.M. Hosseini Jenab, I. Kourakis, "Multicomponent kinetic simulation of Bernstein–Greene–Kruskal modes associated with ion acoustic and dust-ion acoustic excitations," *Phys. Plasmas* **21**, 043701 (2014). [DOI: 10.1063/1.4869730](https://doi.org/10.1063/1.4869730)
- S.M. Hosseini Jenab, I. Kourakis, "Vlasov-kinetic simulations of electrostatic waves in dusty plasmas," *Eur. Phys. J. D* **68**, 77 (2014). [DOI: 10.1140/epjd/e2014-50177-4](https://doi.org/10.1140/epjd/e2014-50177-4)
- S.M. Hosseini Jenab, F. Spanier, "Langmuir-like ionic waves in dusty plasma: Kinetic simulation," *IEEE Trans. Plasma Sci.* **45**(3), 413 (2017). [DOI: 10.1109/TPS.2016.2642998](https://doi.org/10.1109/TPS.2016.2642998)
- S.M. Hosseini Jenab, I. Kourakis, H. Abbasi, "Fully kinetic simulation of ion-acoustic and dust-ion acoustic waves," *Phys. Plasmas* **18**, 073703 (2011). [DOI: 10.1063/1.3609814](https://doi.org/10.1063/1.3609814)

### Numerical Methods
- H. Abbasi, M. Ghadimi, M. Jenab, N. Javaheri, "Vlasov model using kinetic phase point trajectories for the study of BGK modes," *Comput. Phys. Commun.* **177**, 124 (2007). [DOI: 10.1016/j.cpc.2007.02.009](https://doi.org/10.1016/j.cpc.2007.02.009)
- H. Abbasi, S.M. Hosseini Jenab, H. Hakimi Pajouh, "Preventing recurrence effect in the time-marching solution of the Vlasov–Poisson system," *Phys. Rev. E* **84**, 036702 (2011). [DOI: 10.1103/PhysRevE.84.036702](https://doi.org/10.1103/PhysRevE.84.036702)

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

This work builds on over a decade of research in kinetic plasma simulations. The author acknowledges collaborations with F. Spanier (University of Graz), G. Brodin (Umeå University), I. Kourakis (Khalifa University), J. Juno (University of Iowa), and H. Abbasi (Amirkabir University of Technology).

---

*For questions or collaboration inquiries, please open an issue or contact the maintainer.*
