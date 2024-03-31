#ifndef SIM_H
#define SIM_H

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <queue>
#include <fstream>

using std::string, std::vector, std::map, std::shared_ptr;

const string INSTRUCTIONS_FILEPATH = "original.txt";
const string COMPRESSED_FILEPATH = "compressed.txt";

const bool DEBUG_MODE = true;

class SIM
{
    public:
    enum class SIMMode 
    {
        Compression,
        Decompression
    };

    private:
    vector<string> srcLines;
    map<string, int> lineFreqs;

    SIMMode mode;
    
    std::ofstream outfile;

    // For testing purposes allow additional paths to files. Will be ./ in final
    string filepathDirs;

    void initSrcLines();

    public:
    ~SIM();
    void init(string filepathDirs, SIMMode mode);
    void run();
};

#endif // SIM_H