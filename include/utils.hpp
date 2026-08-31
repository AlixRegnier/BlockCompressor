#ifndef BLOCK_COMPRESSOR_UTILS_H
#define BLOCK_COMPRESSOR_UTILS_H

#include <cstdint>
#include <string>

#include <error.hpp>

namespace block_compressor
{
    template <typename T>
    constexpr T bits_to_bytes(T size)
    {
        return ceil_div(size, T{8});
    }

    template <typename T>
    constexpr T ceil_div(T x, T y)
    {
        return (x + y - T{1}) / y;
    }

    template <typename T>
    constexpr T ceil_to_multiple(T x, T y)
    {
        return ceil_div(x, y) * y;
    }

    template <typename T>
    constexpr T nearest_multiple(T x, T y)
    {
        T r = x % y;
        return (r < (y + T{1}) / T{2}) ? (x - r) : (x + (y - r));
    }

    std::string error_str(const std::string& class_name, const std::string& function_name, const std::string& msg)
    {
        return "[ERROR] " + class_name + "::" + function_name + " : " + msg;
    }

    std::string warning_str(const std::string& class_name, const std::string& function_name, const std::string& msg)
    {
        return "[WARNING] " + class_name + "::" + function_name + " : " + msg;
    }
}

#endif