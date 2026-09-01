#ifndef BLOCK_COMPRESSOR_COMPRESSOR_H
#define BLOCK_COMPRESSOR_COMPRESSOR_H

#include <cstdint>

namespace block_compressor
{
    class Compressor
    {
    public:
        Compressor() = default;
        virtual ~Compressor() = default;
        virtual std::size_t compress_block(const char* input, char* output, std::size_t input_size, std::size_t output_size) = 0;
        virtual std::size_t compression_upper_bound(std::size_t size) = 0;
    };
}
#endif
