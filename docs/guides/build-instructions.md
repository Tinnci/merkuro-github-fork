# Build Instructions

**Merkuro Calendar - Phase 0-3 Core Library**

This guide covers the build process for the independent core library and its backends.

## 🛠 System Requirements

*   **Compiler**: GCC/Clang with C++17 support
*   **Qt**: Qt5 development libraries (minimum 5.15.2)
*   **CMake**: Version 3.16 or later
*   **Testing**: Google Test (`libgtest-dev`) for unit tests
*   **Optional**: KDE development packages (only for full Akonadi integration)

---

## 🚀 Quick Build (Recommended)

This builds the core library and local backends without requiring any KDE dependencies.

```bash
# Navigate to project root
cd /path/to/merkuro-github-fork

# Create build directory
mkdir build && cd build

# Configure (Core Only mode)
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_CORE_ONLY=ON ..

# Build
make -j4

# Run Verification Tests
./tests/core-unit-tests
```

**Expected Result:**
*   Time: ~9 seconds
*   Output: `[PASSED] 71 tests.` (Core + Local Backends)

---

## 📦 Build Configurations

You can configure the build using CMake flags:

### 1. Core-Only Build (Default)
**Ideal for**: Development, CI, Non-KDE environments.
*   **Flag**: `-DBUILD_CORE_ONLY=ON`
*   **Dependencies**: Qt5::Core only.
*   **Output**: `libpersonalcalendar-core.a` (Static Library)

```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_CORE_ONLY=ON ..
```

### 2. Full Build
**Ideal for**: Production KDE integration.
*   **Flag**: `-DBUILD_CORE_ONLY=OFF` (or omit the flag)
*   **Dependencies**: Full KDE Frameworks stack (Akonadi, KCalendarCore, etc.).
*   **Note**: Requires `extra-cmake-modules` and KDE dev packages installed.

```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

### Build Types (`CMAKE_BUILD_TYPE`)
*   `Debug`: Includes debug symbols, no optimization. (Best for dev)
*   `Release`: Full optimization, stripped symbols.
*   `RelWithDebInfo`: Optimized with debug symbols.

---

## 🧪 Testing

The project uses Google Test for unit testing.

### Running Tests
```bash
# Run all tests
./tests/core-unit-tests

# Run specific backend tests (if built)
./tests/local-backend-tests
```

### Advanced Test Options
```bash
# Run specific test suite
./tests/core-unit-tests --gtest_filter=CalendarEventTest.*

# Run with execution time
./tests/core-unit-tests --gtest_print_time=true
```

---

## 🔧 Troubleshooting

### Common Issues

1.  **"Could not find ECM" error**
    *   *Cause*: Missing KDE Extra CMake Modules.
    *   *Solution*: Use the core-only flag:
        ```bash
        cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_CORE_ONLY=ON ..
        ```

2.  **"Qt5 not found" error**
    *   *Solution*: Install Qt5 development packages.
        *   Ubuntu: `sudo apt install qt5-qmake qt5-default`
        *   Fedora: `sudo dnf install qt5-qtbase-devel`

3.  **"GTest not found" error**
    *   *Solution*: Install Google Test.
        *   Ubuntu: `sudo apt install libgtest-dev`
        *   Fedora: `sudo dnf install gtest-devel`

4.  **Tests won't run**
    *   *Check*: Ensure `libpersonalcalendar-core.a` compiled successfully. Look for "Built target personalcalendar-core" in the make output.

---

## ⚡ Performance Notes

**Compilation Times (Clean Build)**:
*   Core library: ~5 seconds
*   Tests: ~3 seconds
*   Total: ~9 seconds

**Incremental Build**: < 1 second

---

## 📝 Integration

### Linking against the library

If you want to use this core library in another application:

**CMakeLists.txt**:
```cmake
find_package(personalcalendar-core REQUIRED)
target_link_libraries(your_app personalcalendar-core)
```

**C++ Code**:
```cpp
#include <core/models/CalendarEvent.h>
#include <core/operations/EventOperations.h>
#include <core/ServiceContainer.h>

using namespace PersonalCalendar::Core;
```