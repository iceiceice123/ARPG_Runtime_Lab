#include <arpg/RuntimeVersion.hpp>

#include <gtest/gtest.h>

#include <string>

TEST(RuntimeVersion, NumbersMatchStringLiteral)
{
    const std::string expected = std::to_string(arpg::VersionMajor) + "."
                               + std::to_string(arpg::VersionMinor) + "."
                               + std::to_string(arpg::VersionPatch);

    EXPECT_EQ(expected, std::string(arpg::VersionString()));
}

TEST(RuntimeVersion, RuntimeLibraryIsLinkable)
{
    // Defined in Runtime/src/RuntimeVersion.cpp, i.e. inside the static lib.
    std::string_view fromLibrary = arpg::GetRuntimeVersionString();

    EXPECT_FALSE(fromLibrary.empty());
    EXPECT_EQ(fromLibrary, arpg::VersionString());
}
