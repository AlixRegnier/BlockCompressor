#include <BlockDecompressorZSTD.h>
#include <cstdlib>
#include <iostream>

int main(int argc, char ** argv)
{
    if(argc != 6)
    {
        std::cout << "Usage: ./mainBlockDecompressorZSTD <config_file> <matrix> <ef_path> <header> <output>\n\n";
        return 1;
    }   

    char* ptrnull;
    
    std::size_t header_size = (std::size_t)std::strtoull(argv[4], &ptrnull, 10);

    //Initialize decompressor and decompressor each blocks to <output>
    BlockDecompressorZSTD(argv[1], argv[2], argv[3], header_size).decompress_all(argv[5]);
}