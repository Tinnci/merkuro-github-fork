<!--
SPDX-License-Identifier: GPL-3.0-only
-->

# Personal Calendar

A personal calendar application built on the Merkuro codebase. This is an independent fork utilizing the original Merkuro code as a foundation for a personal calendar management project.
**Now fully migrated to Qt 6 and KDE Frameworks 6.**

**This is a personal project** - not affiliated with the original Merkuro/Kalendar project.

## Build

**Prerequisites:**
*   **Qt 6** (6.6+)
*   **KDE Frameworks 6** (6.0+)
*   **Akonadi** (Qt6 version / KPim6)
*   **Google Test**
*   **CMake** (3.16+)
*   **C++20 Compiler**

### Arch Linux Dependencies
```bash
paru -S qt6-base qt6-declarative qt6-svg qt6-location extra-cmake-modules \
        kf6-i18n kf6-kcalendarcore kf6-kcoreaddons kf6-kitemmodels \
        akonadi kdepim-runtime gtest
```

### Build Instructions

```bash
mkdir build && cd build

# Full build (Recommended)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build . --parallel 4

# Run Tests
./bin/core-unit-tests
./bin/local-backend-tests

# Run Application
./bin/personal-calendar
```

## Project Documentation

The project documentation is organized into the following categories:

### 🚀 Guides
- [**Quick Start**](docs/guides/quick-start.md): Get up and running in 30 seconds.
- [**Build Instructions**](docs/guides/build-instructions.md): Detailed compilation and requirements guide.
- [**Implementation Guide**](docs/guides/implementation-guide.md): API reference and coding patterns.

### 📅 Planning
- [**Project Status**](docs/planning/project-status.md): Current progress dashboard.

### 🔍 Analysis
- [**Architecture Analysis**](docs/analysis/architecture-analysis.md): System design and decoupling strategy.

### 📊 Progress Reports
- [Qt6 Migration Complete](docs/reports/qt6-migration-complete.md)
- [Phase 5B Complete (Multi-Calendar)](docs/reports/phase-5b-complete.md)
- [Phase 5 Complete (Local Backend)](docs/reports/phase-5-complete.md)
- [Phase 0-3 Complete (Core)](docs/reports/phase-0-3-complete.md)

## License

This project is licensed under **GPL-3.0-only**. This is a stricter license than the original project to ensure the code remains free and open source.

Based on the original Merkuro/Kalendar project code.
