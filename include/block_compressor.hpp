#ifndef BLOCK_COMPRESSOR_BLOCK_COMPRESSOR_H
#define BLOCK_COMPRESSOR_BLOCK_COMPRESSOR_H

#include <cstring>
#include <memory>
#include <vector>

#include <config.hpp>
#include <compressor.hpp>
#include <utils.hpp>
#include <stream.hpp>
#include <int_container.hpp>

namespace block_compressor
{
    class BlockCompressor
    {
    private:
        Compressor* compressor;
        IntContainer<std::uint64_t>* int_container;

        char* block;
        char* compressed_block;

        OutputStream output;

        std::size_t block_size;
        std::size_t block_current_size;
        std::size_t compressed_block_size;
        std::size_t total_compressed_size = 0;
        
        bool closed = false;

        BlockCompressor(OutputStream output, std::size_t block_size, Compressor& compressor, IntContainer<std::uint64_t>& int_container);

        void compress_and_flush_block(const char * block, std::size_t size);
    public:
        BlockCompressor(std::ostream& output, std::size_t block_size, Compressor& compressor, IntContainer<std::uint64_t>& int_container);
        BlockCompressor(const std::string& output, std::size_t block_size, Compressor& compressor, IntContainer<std::uint64_t>& int_container);
        ~BlockCompressor();

        //Append data to block (if size > remaining block size, block is filled and remaining is pushed into a new buffered block)
        void append_data(const char * data, std::size_t size);

        //Flush last block and free buffers
        void close();

        inline bool is_closed() const { return closed; }

        inline std::size_t get_block_size() const { return block_size; }
        inline const Compressor& get_compressor() const { return *compressor; }
        inline const IntContainer<std::uint64_t>& get_int_container() const { return *int_container; }

        //Writes arbitrary data directly to stream (block is not flushed)
        void write_raw_data(const char * data, std::size_t size);
    };

    BlockCompressor::BlockCompressor(OutputStream output_stream, std::size_t block_size, Compressor& compressor, IntContainer<std::uint64_t>& int_container) : output(std::move(output_stream)), int_container(&int_container)
    {
        if(!output.valid())
            throw block_compressor_error("BlockCompressor", "()", "Invalid output stream");

        //Allocate block buffer
        this->block = utils::allocate<char>(block_size);

        //Allocate compressed block buffer
        this->compressed_block_size = compressor.compression_upper_bound(block_size);
        this->compressed_block = utils::allocate<char>(compressed_block_size);

        this->int_container->push_back(0);
    }

    BlockCompressor::BlockCompressor(std::ostream& output_stream, std::size_t block_size, Compressor& compressor, IntContainer<std::uint64_t>& int_container)
        : BlockCompressor(OutputStream(output_stream), block_size, compressor, int_container){}

    BlockCompressor::BlockCompressor(const std::string& output_path, std::size_t block_size, Compressor& compressor, IntContainer<std::uint64_t>& int_container)
        : BlockCompressor(OutputStream(output_path), block_size, compressor, int_container){}

    BlockCompressor::~BlockCompressor()
    {
        close();
    }

    inline void BlockCompressor::append_data(const char* data, std::size_t size)
    {
        if(closed)
            throw block_compressor_error("BlockCompressor", "append_data", "Attempted to append data on closed block compressor");

        std::size_t offset = 0;

        //Handle buffered data that has not been flushed yet
        if(block_size != 0)
        {
            std::size_t block_remaining_size = block_size - block_current_size;

            offset = size >= block_remaining_size ? block_remaining_size : size;

            std::memcpy(block+block_current_size, data, offset);
            block_current_size += offset;

            if(block_current_size == block_size)
            {
                compress_and_flush_block(block, block_size);
                block_current_size = 0;
            }
        }

        //Directly compress full data blocks
        while(offset + block_size <= size)
        {
            compress_and_flush_block(data+offset, block_size);
            offset += block_size;
        }

        //Buffer remaining data 
        if(offset != size)
            std::memcpy(block, data+offset, size - offset);
    }

    inline void BlockCompressor::close()
    {
        if(!closed)
        {
            closed = true;

            compress_and_flush_block(block, block_current_size);

            std::free(block);
            std::free(compressed_block);
        }
    }

    inline void BlockCompressor::compress_and_flush_block(const char* data, std::size_t size)
    {
        if(size == 0)
            return;

        if(closed)
            throw block_compressor_error("BlockCompressor", "compress_and_flush_block", "Attempted to compress and flush data on closed block compressor");

        std::size_t compressed_size = compressor->compress(data, compressed_block, size, compressed_block_size);
        write_raw_data(compressed_block, compressed_size);

        total_compressed_size += compressed_size;
        int_container->push_back(total_compressed_size);
    }

    inline void BlockCompressor::write_raw_data(const char* data, std::size_t size)
    {
        if(closed)
            throw block_compressor_error("BlockCompressor", "write_data", "Attempted to write data on closed block compressor");

        output.stream().write(data, static_cast<std::streamsize>(size));
    }
}



#endif