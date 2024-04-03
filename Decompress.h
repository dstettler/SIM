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
    string filepathDirs;
    string outfilePath;
    std::ofstream outfile;

    public:
    Decompress(string filepathDirs);
    ~Decompress();
    void run();
};

#endif // DECOMPRESS_H