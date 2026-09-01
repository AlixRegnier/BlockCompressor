#ifndef BLOCK_COMPRESSOR_INT_CONTAINER_H
#define BLOCK_COMPRESSOR_INT_CONTAINER_H

#include <utils.hpp>

namespace block_compressor
{
    template <typename T>
    class IntContainer
    {
    private:
        std::size_t count;
    public:
        IntContainer() = default;
        virtual ~IntContainer() = default;

        inline virtual void add_integer(T x) { ++count; };
        
        inline std::size_t size() const { return count; }
        
        virtual void deserialize(std::string& path) = 0; 
        virtual void serialize(std::string& path) = 0;
        
        virtual T operator[](std::size_t idx) const = 0;
    };
}

#endif