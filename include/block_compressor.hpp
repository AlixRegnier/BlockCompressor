#ifndef BLOCK_COMPRESSOR_BLOCK_COMPRESSOR_H
#define BLOCK_COMPRESSOR_BLOCK_COMPRESSOR_H

#include <config.hpp>
#include <cstring>
#include <compressor.hpp>
#include <utils.hpp>

namespace block_compressor
{
    class block_compressor
    {
    private:
        Compressor* compressor;

        char* block;
        char* compressed_block;

        std::size_t block_size;
        std::size_t block_current_size;
        std::size_t compressed_block_size;

        bool closed = false;

        //Buffers and IO variables
        std::size_t total_size = 0;

        std::shared_ptr<std::istream> input_stream_ptr;
        std::shared_ptr<std::ostream> output_stream_ptr;

        std::vector<std::uint64_t> block_pos;

        void compress_and_flush_block(const char * block, std::size_t size);
    public:
        block_compressor(std::shared_ptr<std::istream> input_stream_ptr, std::shared_ptr<std::ostream> output_stream_ptr, std::size_t block_size, Compressor& compressor);
        block_compressor(std::istream& input, std::ostream& output, std::size_t block_size, Compressor& compressor);
        block_compressor(const std::string& input, const std::string& output, std::size_t block_size, Compressor& compressor);
        ~block_compressor();

        //Append data to block (if size > remaining block size, block is filled and remaining is pushed into a new buffered block)
        void append_data(const char * data, std::size_t size);

        //Close file descriptors, flush last block
        void close();

        bool is_closed() const;

        Compressor& get_compressor() const;
        void set_compressor(Compressor& compressor);

        //Writes arbitrary data directly to stream (block is not flushed)
        void write_data(const char * data, std::size_t size);

    };

    block_compressor::block_compressor(std::shared_ptr<std::istream> input_stream_ptr, std::shared_ptr<std::ostream> output_stream_ptr, std::size_t block_size, Compressor& compressor)
    {
        set_block_size(block_size);
        set_compressor(compressor);

        block_pos.push_back(0);
    }

    block_compressor::block_compressor(std::istream& input_stream, std::ostream& output_stream, std::size_t block_size, Compressor& compressor)
        : block_compressor( std::make_shared<std::ifstream>(input_stream),
                            std::make_shared<std::ofstream>(output_stream),
                            block_size,
                            compressor
                        ) {}

    block_compressor::block_compressor(const std::string& input_path, const std::string& output_path, std::size_t block_size, Compressor& compressor)
        : block_compressor( std::make_shared<std::ifstream>(input_path),
                            std::make_shared<std::ofstream>(output_path),
                            block_size,
                            compressor
                        ) {}

    block_compressor::~block_compressor()
    {
        //TODO: close + write block positions
    }

    inline void block_compressor::append_data(const char* data, std::size_t size)
    {
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

    inline void block_compressor::close()
    {
        closed = true;

        compress_and_flush_block(block, block_current_size);

        //TODO: write ouf block positions
    }

    inline void block_compressor::compress_and_flush_block(const char* data, std::size_t size)
    {
        if(size == 0)
            return;

        std::size_t compressed_size = compressor->compress_block(data, compressed_block, size, compressed_block_size);
        write_data(compressed_block, compressed_size);

        block_pos.push_back(compressed_size + block_pos.back());
    }
    
    inline bool block_compressor::is_closed() const
    {
        return closed;
    }

    inline Compressor& block_compressor::get_compressor() const 
    {
        return *compressor;
    }

    inline void block_compressor::set_block_size(std::size_t new_block_size)
    {
        block = reallocate<char>(block, block_size, new_block_size);
        block_size = new_block_size;
    }

    inline void block_compressor::set_compressor(Compressor& compressor)
    {
        std::size_t old_size = compressed_block_size; 
        compressed_block_size = compressor.compression_upper_bound(block_size);

        compressed_block = reallocate<char>(compressed_block, old_size, compressed_block_size);
    }

    inline void block_compressor::write_data(const char* data, std::size_t size)
    {
        output_stream_ptr->write(data, static_cast<std::streamsize>(size));
    }
}



#endif