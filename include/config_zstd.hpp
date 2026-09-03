#ifndef BLOCK_COMPRESSOR_CONFIG_ZSTD_H
#define BLOCK_COMPRESSOR_CONFIG_ZSTD_H

#include <config.hpp>

namespace block_compressor
{
    class ConfigZstd : public Config
    {
    private:
        std::uint64_t preset = default_preset;
        std::uint64_t wlog = default_wlog;

    public:
        using Config::import_config;
        using Config::export_config;
        
        static constexpr std::uint64_t default_preset = 3;
        static constexpr std::uint64_t default_wlog = 0; //use Zstd default wlog value

        ConfigZstd() = default;


        explicit ConfigZstd(const std::string& config_path) : Config() { import_config(config_path); }

        explicit ConfigZstd(const ConfigIO& config_io) : Config() { import_config(config_io); }

        virtual ~ConfigZstd() = default;

        virtual void import_config(const ConfigIO& config_io) override
        {
            set_preset(config_io.get<std::uint64_t>("preset", default_preset));
            set_wlog(config_io.get<std::uint64_t>("wlog", default_wlog));

            Config::import_config(config_io); 
        }

        virtual void export_config(const std::string& config_path, ConfigIO& config_io) const override
        {
            config_io.set<std::uint64_t>("preset", preset);
            config_io.set<std::uint64_t>("wlog", wlog);

            Config::export_config(config_path, config_io);
        }

        virtual std::uint64_t get_preset() const { return preset; }
        virtual std::uint64_t get_wlog() const { return wlog; }

        virtual void set_preset(std::uint64_t preset) { this->preset = preset; }
        virtual void set_wlog(std::uint64_t wlog) { this->wlog = wlog; }
    };
}

#endif