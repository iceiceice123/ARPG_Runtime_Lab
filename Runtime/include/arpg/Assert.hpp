#include <source_location>   // std::source_location
#include <iostream>          // std::cout
#include <cstdlib>           // std::abort
#include <intrin.h>          // __debugbreak()（MSVC 专用）

//这里用宏的目的就是为了两件事
//
#ifdef NDEBUG
    #define ARPG_ASSERT(expr)  ((void)0)        
#else
    #define ARPG_ASSERT(expr)                    \
        do {                                     \
            if (!(expr)) {                       \
                std::source_location loc = std::source_location::current(); \
                std::cout << "Assert failed: " << #expr << " at " << loc.file_name() << ":" << loc.line() << '\n';                    \
                __debugbreak();                   \
                std::abort();                     \
            }                                    \
        } while (0)
#endif