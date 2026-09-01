#ifndef BLOCK_COMPRESSOR_UTILS_H
#define BLOCK_COMPRESSOR_UTILS_H

#include <cstdint>
#include <string>
#include <cstdlib>

#include <error.hpp>

namespace block_compressor
{

    template<typename T>
    inline T* reallocate(T* ptr, std::size_t old_size, std::size_t new_size)
    {
        if(old_size == new_size)
            return ptr;

        void* tmp;

        if(ptr == nullptr)
            tmp = std::malloc(sizeof(T)*new_size);
        else
            tmp = std::realloc(ptr, sizeof(T)*new_size);

        if(!tmp)
            throw block_compressor_error("utils", "realloc", "Realloc returned a null pointer, OOM ?");

        return static_cast<T*>(tmp);
    }

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