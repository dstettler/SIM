#ifndef COMPRESS_H
#define COMPRESS_H

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <fstream>

using std::string, std::vector, std::map, std::shared_ptr;

class Compress
{
    struct BinaryLine
    {
        string lineContent;
        int lineFreq;
        unsigned int srcPos;

        static bool comp(const BinaryLine &a, const BinaryLine &other);
    };

    vector<string> srcLines;
    vector<BinaryLine> mostFrequent;

    // For testing purposes allow additional paths to files. Will be ./ in final
    string filepathDirs;
    string outfilePath;
    std::ofstream outfile;

    void initSrcLines();
    vector<BinaryLine> getMostFrequentEntries(map<string, std::pair<int, int>>* frequencyDict, int max = 0);
    // Returns index of val in *vec, -1 if not found
    int valueInVec(string val, vector<BinaryLine>* vec);

    unsigned int insertStringToStream(string toAdd, string* stream, unsigned int streamWidth = 32);

    string bitmaskCompress(string line);
    string mismatch1Bit(string line);
    string mismatch2Bit(string line);
    string mismatch4Bit(string line);
    string mismatch2BitAnywhere(string line);

    void mainCompLoop();
    void printFreqDict();
    void formatFile();

    public:
    Compress(string filepathDirs);
    ~Compress();
    void run();
};

#endif // COMPRESS_H