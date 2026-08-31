#ifndef BLOCK_COMPRESSOR_CONFIG_H
#define BLOCK_COMPRESSOR_CONFIG_H

#include <config_io.hpp>
#include <utils.hpp>

namespace block_compressor
{
    class Config
    {
    private:
        std::uint64_t bits_per_element;
        std::uint64_t elements_per_row;
        std::uint64_t rows_per_block;
        std::uint64_t row_size;
        std::uint64_t block_size;

        std::uint64_t dirty_rows_per_block;
        std::uint64_t dirty_block_size;

    public:
        const std::uint64_t default_block_size = 1 << 16; //64 kB
        const std::uint64_t default_bits_per_element = 1;

        explicit Config(const ConfigIO& config_io);
        explicit Config();
        virtual ~Config() = default;

        virtual void sync_parameters();

        virtual inline std::size_t get_bits_per_element() const { return bits_per_element; }
        virtual inline std::size_t get_elements_per_row() const { return elements_per_row; }
        virtual inline std::size_t get_rows_per_block() const { return rows_per_block; }
        virtual inline std::size_t get_row_size() const { return row_size; }
        virtual inline std::size_t get_block_size() const { return block_size; }

        virtual void set_bits_per_element(std::size_t v, bool sync = true);
        virtual void set_elements_per_row(std::size_t v, bool sync = true);
        virtual void set_rows_per_block(std::size_t v, bool sync = true);
        virtual void target_block_size(std::size_t v, bool sync = true);
    };

    Config::Config(const ConfigIO& config_io)
    {
        set_elements_per_row(config_io.get<std::uint64_t>("elements_per_row"), false);
        set_bits_per_element(config_io.get<std::uint64_t>("bits_per_element", default_bits_per_element), false);
        target_block_size(config_io.get<std::uint64_t>("target_block_size"), false);
        set_rows_per_block(config_io.get<std::uint64_t>("rows_per_block"), false);

        sync_parameters();
    }

    Config::Config(){}

    inline void Config::sync_parameters()
    {
        row_size = bits_to_bytes(elements_per_row * bits_per_element);

        if(dirty_block_size != 0 && dirty_rows_per_block != 0)
            throw block_compressor_error("Config", "sync_parameters", "Cannot tune both the number of rows per block and the block size");

        if(dirty_rows_per_block != 0)
        {
            rows_per_block = dirty_rows_per_block;
            block_size = row_size * rows_per_block;
        }
        else //if(dirty_block_size != 0)
        {
            block_size = std::max(row_size, nearest_multiple(dirty_block_size, row_size));
            rows_per_block = block_size / row_size;
        }

        //Reset dirty values
        dirty_rows_per_block = dirty_block_size = 0;
    }

    inline void Config::set_bits_per_element(std::size_t bits_per_element, bool sync)
    { 
        if(bits_per_element == 0)
            throw block_compressor_error("Config", "set_bits_per_element", "Attempted to set the number of bits per element to 0");

        this->bits_per_element = static_cast<std::uint64_t>(bits_per_element);

        if(sync)
            sync_parameters();
    }

    inline void Config::set_elements_per_row(std::size_t elements_per_row, bool sync)
    { 
        if(elements_per_row == 0)
            throw block_compressor_error("Config", "set_elements_per_row", "Attempted to set the number of elements per row to 0");

        this->elements_per_row = static_cast<std::uint64_t>(elements_per_row);

        if(sync)
            sync_parameters();
    }

    inline void Config::set_rows_per_block(std::size_t rows_per_block, bool sync)
    { 
        if(rows_per_block == 0)
            throw block_compressor_error("Config", "set_rows_per_block", "Attempted to set the number of rows per block to 0");

        dirty_rows_per_block = static_cast<std::uint64_t>(rows_per_block);

        if(sync)
            sync_parameters();
    }

    inline void Config::target_block_size(std::size_t target_block_size, bool sync)
    { 
        if(target_block_size == 0)
            throw block_compressor_error("Config", "target_block_size", "Attempted to set the block size to 0");

        dirty_block_size = std::max(row_size, nearest_multiple(target_block_size, row_size));
        dirty_rows_per_block = block_size / row_size;

        if(sync)
            sync_parameters();
    }
}

#endif