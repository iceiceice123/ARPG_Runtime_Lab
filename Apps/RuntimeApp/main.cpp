#include <arpg/RuntimeVersion.hpp>
#include <arpg/Assert.hpp>

#include <cstdio>
#include <string>

int main()
{
    std::printf("ARPG Runtime Lab %s\n", std::string(arpg::VersionString()).c_str());

    return 0;
}
