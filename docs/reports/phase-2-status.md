# Phase 2 Status: Akonadi Integration

## Executive Summary

**Status**: 🚧 **STUBBED / BLOCKED**

Phase 2 implementation has started but is blocked by missing system dependencies (`KF5Akonadi`, `KF5CalendarCore`). The infrastructure is in place to enable it once dependencies are installed.

- **Backend Code**: `AkonadiCalendarBackend` (Skeleton)
- **Data Converter**: `AkonadiDataConverter` (Implemented with Logic)
- **Build System**: Updated to check for Akonadi and degrade gracefully if missing.

---

## Implementation Details

### Files Created/Updated
- `src/backends/akonadi/AkonadiDataConverter.h/cpp`:
    - Implemented logic to convert `Core::CalendarEvent` ↔ `KCalendarCore::Incidence`
    - Handles Recurrence, Basic fields (UID, Title, Time)
    - Protected by `#if defined(KCALENDARCORE_FOUND)` guards.

- `CMakeLists.txt` (Root):
    - Added checks for `KF5Akonadi` and `KF5CalendarCore`.
    - Only adds `src/backends/akonadi` subdirectory if found.

### Dependencies Required

To enable this backend, the following development packages must be installed:
- `extra-cmake-modules`
- `libkf5akonadi-dev` (or `akonadi`)
- `libkf5calendarcore-dev` (or `kcalendarcore`)
- `libkf5mime-dev`
- `kdepim-runtime` (Runtime requirement)

### Next Steps (When Environment is Ready)

1.  Install dependencies.
2.  Run `cmake -DBUILD_CORE_ONLY=OFF ..`
3.  Verify `Akonadi found: Enabling Akonadi Backend` message.
4.  Compile and run integration tests.
