#ifndef DECOMPRESS_H
#define DECOMPRESS_H

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <fstream>

using std::string, std::vector, std::map, std::shared_ptr;

class Decompress
{
    string codeStream;
    vector<string> dictionary;

    string prevLine;

    string filepathDirs;
    string outfilePath;
    std::ofstream outfile;

    string readBitsFromCodeStream(size_t start, size_t bitsToRead);

    string rle(size_t startingIndex);
    string bitmask(size_t startingIndex);
    string consecutiveBitMismatch(size_t startingIndex, unsigned int bits);
    string twoBitAnyMismatch(size_t startingIndex);

    void initCodeStream();
    void decompLoop();

    public:
    Decompress(string filepathDirs);
    ~Decompress();
    void run();
};

#endif // DECOMPRESS_H