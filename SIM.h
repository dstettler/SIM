#ifndef SIM_H
#define SIM_H

#include <map>
#include <string>
#include <vector>
#include <memory>
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
    struct BinaryLine
    {
        string lineContent;
        int lineFreq;
        unsigned int srcPos;

        bool operator<(const BinaryLine &other) const;
    };

    vector<string> srcLines;
    vector<string> mostFrequent;

    SIMMode mode;
    
    std::ofstream outfile;

    // For testing purposes allow additional paths to files. Will be ./ in final
    string filepathDirs;

    void initSrcLines();
    vector<string> getMostFrequentEntries(map<string, std::pair<int, int>>* frequencyDict, int max = 0);
    string getBinStrFromInt(int i, int numChars);
    // Returns index of val in *vec, -1 if not found
    int valueInVec(string val, vector<string>* vec);

    void mainLoop();
    void printFreqDict();

    public:
    ~SIM();
    void init(string filepathDirs, SIMMode mode);
    void run();
};

#endif // SIM_H