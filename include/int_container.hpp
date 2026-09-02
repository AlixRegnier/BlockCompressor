#ifndef BLOCK_COMPRESSOR_INT_CONTAINER_H
#define BLOCK_COMPRESSOR_INT_CONTAINER_H

#include <utils.hpp>
#include <vector>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

namespace block_compressor
{
    template <typename T>
    class IntContainer
    {
    private:
        std::size_t count;
    protected:
        inline void increment_count() { ++count; };
    public:
        IntContainer() = default;
        virtual ~IntContainer() = default;

        virtual void reserve(std::size_t capacity) = 0;
        virtual void push_back(T x) = 0; 
        
        inline std::size_t size() const { return count; }
        virtual std::size_t upper_bound_size() const = 0;

        void deserialize(const std::string& path) const;
        virtual void deserialize(const char* data, std::size_t size) const = 0;
    
        std::size_t serialize(const std::string& path, int mode) const;
        virtual std::size_t serialize(char* data) const = 0;    

        virtual T operator[](std::size_t idx) const = 0;
    };

    template <typename T>
    void IntContainer<T>::deserialize(const std::string& path) const
    {
        int fd = open(path.c_str(), O_RDONLY);
        std::size_t file_size = lseek(fd, 0, SEEK_END);
        const char* map = (const char*)mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, fd, 0);

        deserialize(map, file_size);

        munmap(map, file_size);
        close(fd);
    }

    template <typename T>
    std::size_t IntContainer<T>::serialize(const std::string& path, int mode) const
    {
        int fd = open(path.c_str(), O_TRUNC | O_CREAT | O_RDWR, mode);
        std::size_t file_size = upper_bound_size();
        char* map = (char*)mmap(nullptr, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

        std::size_t written_bytes = serialize(map);

        munmap(map, file_size);

        if(written_bytes > file_size)
            throw block_compressor_error("IntContainer", "serialize", "Serialization written more bytes than declared");

        if (written_bytes < file_size && ftruncate(fd, written_bytes) == -1)
            throw block_compressor_error("IntContainer", "serialize", "Could not truncate serialized file");

        close(fd);
    }

    template <typename T>
    class IntContainerIdentity : public IntContainer<T>
    {
    private:
        std::vector<T> integers;
    public:
        IntContainerIdentity() = default;
        virtual ~IntContainerIdentity() = default;

        virtual inline void reserve(std::size_t capacity) override { integers.reserve(capacity); }
        
        virtual inline void push_back(T x) override 
        { 
            integers.push_back(x); 
            IntContainer<T>::increment_count(); 
        }

        virtual inline std::size_t upper_bound_size() const override { return sizeof(T) * integers.size(); }
        
        virtual inline void deserialize(const char* data, std::size_t size) const override 
        { 
            integers.resize(size/sizeof(T));
            std::memcpy(reinterpret_cast<char*>(integers.data()), data, size);
        }

        virtual std::size_t serialize(char* data) const override
        {
            std::memcpy(data, reinterpret_cast<const char*>(integers.data()), upper_bound_size());
            return upper_bound_size();
        }

        virtual inline T operator[](std::size_t idx) const override { return integers[idx]; }
    };
}

#endif