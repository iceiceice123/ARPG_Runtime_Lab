#include <arpg/RuntimeVersion.hpp>

// The translation unit exists so that the Runtime target is a real, linkable
// library from day one instead of an interface-only target.
namespace arpg
{
    std::string_view GetRuntimeVersionString()
    {
        return VersionString();
    }
} // namespace arpg
