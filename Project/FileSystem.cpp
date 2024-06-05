#include "FileSystem.h"

typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;

// Helper function to check for empty strings and handle separators
std::string HandleEmptyStringsAndSeparators(const std::string& a, const std::string& b, char separator) {
    std::string combined_path;

    // Handle empty strings
    if (a.empty()) {
        combined_path = b;
    }
    else if (b.empty()) {
        combined_path = a;
    }
    else {
        // Check if the last character of 'a' needs a separator
        if (a.back() != separator && a.back() != '\\' && a.back() != '/') {
            combined_path = a + separator;
        }
        else {
            combined_path = a;
        }

        // Append 'b'
        combined_path += b;
    }

    return combined_path;
}

// Overloaded functions for CombinePath

std::string FileSystem::CombinePath(const char* a, const char* b) {
    // Convert char* arguments to strings for consistency
    std::string str_a(a);
    std::string str_b(b);
    return HandleEmptyStringsAndSeparators(str_a, str_b, '/');  // Use forward slash as default separator
}

// std::string CombinePath(const char* a, const std::string& b) {
//     std::string str_a(a);
//     return CombinePath(str_a, b);
// }
// 
// std::string CombinePath(const std::string& a, const char* b) {
//     return CombinePath(a, std::string(b));
// }

std::string FileSystem::CombinePath(const std::string& a, const std::string& b) {
    return HandleEmptyStringsAndSeparators(a, b, '/');  // Use forward slash as default separator
}