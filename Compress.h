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
    string getBinStrFromInt(int i, int numChars);
    // Returns index of val in *vec, -1 if not found
    int valueInVec(string val, vector<BinaryLine>* vec);

    // Returns index of first character mismatch in str, -1 if not found
    // `offset` can be used to skip ahead
    int getFirstMismatch(std::string str1, std::string str2, int offset = 0);
    /* 
        Returns if there is another mismatch between the two given strings.
        `firstMismatch` - First known mismatch position
        `skip` - How many indices to skip after the `firstMismatch` before checking for new mismatches
        Returns:
            true if another mismatch is found after `firstMismatch` + `skip`
    */
    bool isAnotherMistmatch(std::string str1, std::string str2, int firstMismatch, int skip);
    /* 
        Returns if there are consecutive mismatches.
        `firstMismatch` - First known mismatch position
        `length` - How many indices to check for mismatches
        Returns:
            true if every index in range `length` after `firstMismatch`
    */
    bool consecutiveMismatches(std::string str1, std::string str2, int firstMismatch, int length);

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