// Task 1 skeleton: the only thing the Runtime publishes right now is its own
// version. Everything else arrives in the following tasks.
#pragma once

#include <string_view>

#define ARPG_RUNTIME_VERSION_MAJOR 0
#define ARPG_RUNTIME_VERSION_MINOR 1
#define ARPG_RUNTIME_VERSION_PATCH 0

#define ARPG_RUNTIME_STRINGIFY_IMPL(x) #x
#define ARPG_RUNTIME_STRINGIFY(x)      ARPG_RUNTIME_STRINGIFY_IMPL(x)

#define ARPG_RUNTIME_VERSION_STRING        \
    ARPG_RUNTIME_STRINGIFY(ARPG_RUNTIME_VERSION_MAJOR) "." \
    ARPG_RUNTIME_STRINGIFY(ARPG_RUNTIME_VERSION_MINOR) "." \
    ARPG_RUNTIME_STRINGIFY(ARPG_RUNTIME_VERSION_PATCH)

namespace arpg
{
    constexpr int VersionMajor = ARPG_RUNTIME_VERSION_MAJOR;
    constexpr int VersionMinor = ARPG_RUNTIME_VERSION_MINOR;
    constexpr int VersionPatch = ARPG_RUNTIME_VERSION_PATCH;

    constexpr std::string_view VersionString()
    {
        return ARPG_RUNTIME_VERSION_STRING;
    }

    // Defined in Runtime/src/RuntimeVersion.cpp.
    std::string_view GetRuntimeVersionString();
} // namespace arpg
