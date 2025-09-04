#include <BlockCompressorZSTD.h>

//Return the number of written bytes
std::size_t BlockCompressorZSTD::compress_buffer(const std::uint8_t * const input, std::uint8_t * const output, std::size_t in_size, std::size_t out_size)
{
    return ZSTD_compress2(context, output, out_size, input, in_size);
}

//Init ZSTD filters and resize output buffer according to estimated compressed block size
BlockCompressorZSTD::BlockCompressorZSTD(const std::string& out_prefix, const std::string& config_path) : BlockCompressor(out_prefix, config_path)
{
    //Configure options and filters (compression level) 
    context = ZSTD_createCCtx();

    ZSTD_CCtx_setParameter(context, ZSTD_c_compressionLevel, ZSTD_defaultCLevel());

    //Compression is not inplace, so we need to allocate out_buffer once for storing data
    //Get maximum estimated (upper bound) encoded size
    out_buffer.resize(ZSTD_compressBound(in_buffer.size()));
}

BlockCompressorZSTD::~BlockCompressorZSTD()
{
    //Free memory
    ZSTD_freeCCtx(context);
}