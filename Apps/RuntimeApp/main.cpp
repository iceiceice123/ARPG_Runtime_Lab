#include <arpg/RuntimeVersion.hpp>

#include <cstdio>
#include <string>

int main()
{
    std::printf("ARPG Runtime Lab %s\n", std::string(arpg::VersionString()).c_str());
    std::printf("Runtime skeleton online (Task 1).\n");
    std::printf("This process links arpg_runtime and depends on nothing but the C++ standard library.\n");
    return 0;
}
