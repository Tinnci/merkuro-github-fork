# Build Instructions

**Merkuro Calendar - Qt6/KF6 Core Library**

This guide covers the build process for the independent core library and its backends.

## 🛠 System Requirements

*   **Compiler**: GCC/Clang with C++20 support
*   **Qt**: Qt6 development libraries (minimum 6.6.0)
*   **CMake**: Version 3.16 or later
*   **Testing**: Google Test (`gtest`) for unit tests
*   **Akonadi**: KPim6 (Qt6 version) for Akonadi backend

---

## 🚀 Quick Build (Recommended)

This builds the core library and local backends.

```bash
# Navigate to project root
cd /path/to/merkuro-github-fork

# Create build directory
mkdir build && cd build

# Configure
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Build
cmake --build . --parallel 4

# Run Verification Tests
./bin/core-unit-tests
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
*   **Dependencies**: Qt6::Core only.
*   **Output**: `libpersonalcalendar-core.a` (Static Library)

```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_CORE_ONLY=ON ..
```

### 2. Full Build
**Ideal for**: Production KDE integration (Akonadi).
*   **Flag**: `-DBUILD_CORE_ONLY=OFF` (or omit the flag)
*   **Dependencies**: Full KDE Frameworks 6 stack (Akonadi, KCalendarCore, etc.).
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
./bin/core-unit-tests

# Run specific backend tests (if built)
./bin/local-backend-tests
```

### Advanced Test Options
```bash
# Run specific test suite
./bin/core-unit-tests --gtest_filter=CalendarEventTest.*

# Run with execution time
./bin/core-unit-tests --gtest_print_time=true
```

---

## 🔧 Troubleshooting

### Common Issues

1.  **"Could not find ECM" error**
    *   *Cause*: Missing KDE Extra CMake Modules.
    *   *Solution*: Install `extra-cmake-modules` or use the core-only flag:
        ```bash
        cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_CORE_ONLY=ON ..
        ```

2.  **"Qt6 not found" error**
    *   *Solution*: Install Qt6 development packages (`qt6-base`, `qt6-declarative`).

3.  **"GTest not found" error**
    *   *Solution*: Install Google Test.

4.  **Akonadi Backend Missing**
    *   *Cause*: Missing `KPim6Akonadi` or `KPim6AkonadiCalendar` packages.
    *   *Solution*: Install `libkf6akonadi-dev` or equivalent.

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