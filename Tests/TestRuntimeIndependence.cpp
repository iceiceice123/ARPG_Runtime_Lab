// Decision 003: Runtime must be independent from Unreal.
//
// This test turns that decision into an enforceable fact. It scans every
// source file under Runtime/ and fails if any of them includes Unreal headers.
// A human forgetting the rule is not an excuse; CI must catch it.

#include <gtest/gtest.h>

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace
{
    // Substrings that identify an Unreal Engine header. Compared lower-cased.
    constexpr std::string_view kForbiddenHeaderFragments[] = {
        "coreminimal.h",
        "coretypes.h",
        "engine/",
        "uobject/",
        "gameframework/",
        "components/",
        "unreal",
        "ue_",
        "math/unrealmath",
        "templates/unreal",
        "misc/cstring",
        "hal/platform",
        "containers/unrealstring",
    };

    constexpr std::string_view kScannedExtensions[] = {
        ".h", ".hh", ".hpp", ".hxx", ".inl", ".c", ".cc", ".cpp", ".cxx",
    };

    std::string ToLower(std::string_view text)
    {
        std::string result(text);
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return result;
    }

    bool HasScannedExtension(const std::filesystem::path& path)
    {
        const std::string extension = ToLower(path.extension().string());
        return std::find(std::begin(kScannedExtensions), std::end(kScannedExtensions), extension)
            != std::end(kScannedExtensions);
    }

    // Extracts the header name of a `#include <...>` / `#include "..."` line.
    // Returns an empty string when the line is not an include directive.
    std::string ParseIncludeTarget(const std::string& line)
    {
        const std::size_t hash = line.find('#');
        if (hash == std::string::npos)
        {
            return {};
        }

        // Skip leading whitespace after '#'.
        std::size_t cursor = hash + 1;
        while (cursor < line.size() && std::isspace(static_cast<unsigned char>(line[cursor])))
        {
            ++cursor;
        }

        constexpr std::string_view kDirective = "include";
        if (line.compare(cursor, kDirective.size(), kDirective) != 0)
        {
            return {};
        }
        cursor += kDirective.size();

        while (cursor < line.size() && std::isspace(static_cast<unsigned char>(line[cursor])))
        {
            ++cursor;
        }
        if (cursor >= line.size() || (line[cursor] != '<' && line[cursor] != '"'))
        {
            return {};
        }

        const char delimiter = (line[cursor] == '<') ? '>' : '"';
        const std::size_t begin = cursor + 1;
        const std::size_t end = line.find(delimiter, begin);
        if (end == std::string::npos)
        {
            return {};
        }

        return line.substr(begin, end - begin);
    }

    std::string_view FindForbiddenFragment(const std::string& includeTarget)
    {
        const std::string lowered = ToLower(includeTarget);
        for (std::string_view fragment : kForbiddenHeaderFragments)
        {
            if (lowered.find(fragment) != std::string::npos)
            {
                return fragment;
            }
        }
        return {};
    }

    struct Violation
    {
        std::string file;
        std::size_t line = 0;
        std::string include;
        std::string fragment;
    };

    std::vector<Violation> ScanDirectory(const std::filesystem::path& root)
    {
        std::vector<Violation> violations;

        if (!std::filesystem::exists(root) || !std::filesystem::is_directory(root))
        {
            ADD_FAILURE() << "Runtime source directory not found: " << root.string();
            return violations;
        }

        for (std::filesystem::recursive_directory_iterator it(root), end; it != end; ++it)
        {
            if (!it->is_regular_file() || !HasScannedExtension(it->path()))
            {
                continue;
            }

            std::ifstream stream(it->path());
            if (!stream)
            {
                continue;
            }

            std::string line;
            std::size_t lineNumber = 0;
            while (std::getline(stream, line))
            {
                ++lineNumber;

                const std::string include = ParseIncludeTarget(line);
                if (include.empty())
                {
                    continue;
                }

                const std::string_view fragment = FindForbiddenFragment(include);
                if (!fragment.empty())
                {
                    violations.push_back({ it->path().string(), lineNumber, include, std::string(fragment) });
                }
            }
        }

        return violations;
    }

    std::string Describe(const std::vector<Violation>& violations)
    {
        std::ostringstream oss;
        oss << violations.size() << " Unreal dependency violation(s) found in Runtime/:\n";
        for (const Violation& v : violations)
        {
            oss << "  " << v.file << "(" << v.line << "): #include " << v.include
                << "   [matched: " << v.fragment << "]\n";
        }
        return oss.str();
    }
} // namespace

TEST(RuntimeIndependence, RuntimeDirectoryContainsSourceFiles)
{
    const std::filesystem::path root = ARPG_RUNTIME_ROOT_DIR;

    ASSERT_TRUE(std::filesystem::is_directory(root)) << "Missing Runtime dir: " << root.string();

    int scannedFiles = 0;
    for (std::filesystem::recursive_directory_iterator it(root), end; it != end; ++it)
    {
        if (it->is_regular_file() && HasScannedExtension(it->path()))
        {
            ++scannedFiles;
        }
    }

    EXPECT_GT(scannedFiles, 0) << "The scan found nothing to check in " << root.string();
}

TEST(RuntimeIndependence, NoUnrealHeadersIncluded)
{
    const std::vector<Violation> violations = ScanDirectory(ARPG_RUNTIME_ROOT_DIR);

    EXPECT_TRUE(violations.empty()) << Describe(violations);
}
