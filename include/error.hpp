#ifndef BLOCK_COMPRESSOR_ERROR_H
#define BLOCK_COMPRESSOR_ERROR_H

#include <stdexcept>
#include <string>

#include <utils.hpp>

namespace block_compressor
{
    class block_compressor_error : public std::runtime_error
    {
    public:
        explicit block_compressor_error(const std::string& class_name, const std::string& function_name, const std::string& msg)
            : std::runtime_error(error_str(class_name, function_name, msg)) {}
    };
}

#endif