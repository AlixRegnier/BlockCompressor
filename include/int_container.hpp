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

        inline virtual void push_back(T x) { ++count; };
        
        inline std::size_t size() const { return count; }
        
        void deserialize(const std::string& path);
        virtual void deserialize(const char* data) = 0;
    
        void serialize(const std::string& path);
        virtual void serialize(const char* data) const = 0;    

        virtual T operator[](std::size_t idx) const = 0;
    };
}

#endif