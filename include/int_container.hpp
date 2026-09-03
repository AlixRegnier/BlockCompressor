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
        std::uint64_t count;
    protected:
        inline void increment_count() { ++count; };
    public:
        IntContainer() = default;
        virtual ~IntContainer() = default;

        virtual void reserve(std::size_t capacity) = 0;
        virtual void push_back(T x) = 0; 
        
        inline std::size_t size() const { return count; }
        virtual std::size_t upper_bound_size() const = 0;

        void deserialize(const std::string& path);
        virtual void deserialize(const char* data, std::size_t size) = 0;
    
        std::size_t serialize(const std::string& path, int mode) const;
        virtual std::size_t serialize(char* data) const = 0;    

        virtual T get(std::size_t idx) const = 0;
        inline T operator[](std::size_t idx) const { return get(idx); };
    };

    template <typename T>
    void IntContainer<T>::deserialize(const std::string& path)
    {
        constexpr std::size_t count_bytes = sizeof(count);

        int fd = open(path.c_str(), O_RDONLY);

        if(fd < 0)
            throw block_compressor_error("IntContainer", "deserialize", "Open syscall failed on: '" + path + "'");

        std::size_t file_size = lseek(fd, 0, SEEK_END);
        
        if(file_size == 0)
        {
            close(fd);
            throw block_compressor_error("IntContainer", "deserialize", "File is empty: '" + path + "'");
        }

        const char* map = (const char*)mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, fd, 0);

        if(map == MAP_FAILED)
        {
            close(fd);
            throw block_compressor_error("IntContainer", "deserialize", "mmap failed, OOM ?");
        }

        count = *reinterpret_cast<std::uint64_t*>(*map); //TODO: endianess
        deserialize(map+count_bytes, file_size);

        munmap(map, file_size);
        close(fd);
    }

    template <typename T>
    std::size_t IntContainer<T>::serialize(const std::string& path, int mode) const
    {
        constexpr std::size_t count_bytes = sizeof(std::uint64_t);

        int fd = open(path.c_str(), O_TRUNC | O_CREAT | O_RDWR, mode);

        if(fd < 0)
            throw block_compressor_error("IntContainer", "serialize", "Open syscall failed on: '" + path + "'");

        std::size_t file_size = count_bytes + upper_bound_size();

        if (ftruncate(fd, file_size) < 0)
        {
            close(fd);
            throw block_compressor_error("IntContainer", "serialize", "ftruncate failed");
        }

        char* map = (char*)mmap(nullptr, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

        if(map == MAP_FAILED)
        {
            close(fd);
            throw block_compressor_error("IntContainer", "serialize", "mmap failed, OOM ?");
        }

        std::memcpy(map, reinterpret_cast<const char*>(&count), count_bytes); //TODO: endianess
        std::size_t written_bytes = serialize(map + count_bytes);

        munmap(map, file_size);

        if(written_bytes > file_size)
        {
            close(fd);
            throw block_compressor_error("IntContainer", "serialize", "Serialization written more bytes than declared");
        }

        if (written_bytes < file_size && ftruncate(fd, written_bytes) < 0)
        {
            close(fd);
            throw block_compressor_error("IntContainer", "serialize", "Could not truncate serialized file: '" + path + "'");
        }

        close(fd);
        return written_bytes;
    }

    template <typename T>
    class IntContainerRaw : public IntContainer<T>
    {
    private:
        std::vector<T> integers;
    public:
        IntContainerRaw() = default;
        virtual ~IntContainerRaw() = default;
        using IntContainer<T>::deserialize;
        using IntContainer<T>::serialize;

        virtual inline void reserve(std::size_t capacity) override { integers.reserve(capacity); }

        virtual inline void push_back(T x) override 
        { 
            integers.push_back(x); 
            IntContainer<T>::increment_count(); 
        }

        virtual inline std::size_t upper_bound_size() const override { return sizeof(T) * integers.size(); }

        virtual inline void deserialize(const char* data, std::size_t size) override 
        { 
            integers.resize(size/sizeof(T));
            std::memcpy(reinterpret_cast<char*>(integers.data()), data, size); //TODO: endianess
        }

        virtual std::size_t serialize(char* data) const override
        {
            std::memcpy(data, reinterpret_cast<const char*>(integers.data()), upper_bound_size()); //TODO: endianess
            return upper_bound_size();
        }

        virtual inline T get(std::size_t idx) const override { return integers[idx]; }
    };
}

#endif