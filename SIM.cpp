#include "SIM.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>

using std::string, std::vector, std::map, std::shared_ptr;

bool SIM::BinaryLine::comp(const BinaryLine &a, const BinaryLine &other)
{
    if (a.lineFreq != other.lineFreq)
        return a.lineFreq > other.lineFreq;
    else
        return a.srcPos < other.srcPos;
}

void SIM::init(string filepathDirs, SIM::SIMMode mode)
{
    this->filepathDirs = filepathDirs;
    this->mode = mode;

    initSrcLines();

    switch (mode)
    {
        case SIMMode::Compression:
            outfile.open(filepathDirs + COMPRESSED_FILEPATH);
            break;
        case SIMMode::Decompression:
            outfile.open(filepathDirs + INSTRUCTIONS_FILEPATH);
            break;
    }
}

SIM::~SIM()
{
    outfile.close();
}

void SIM::initSrcLines()
{
    if (DEBUG_MODE)
        std::cout << "Initializing source lines..." << std::endl;

    std::ifstream f(filepathDirs + ((mode == SIMMode::Compression) ? INSTRUCTIONS_FILEPATH : COMPRESSED_FILEPATH));
    if (f.is_open())
    {
        string line;
        //  line -> [frequency, original pos]
        map<string, std::pair<int, int>> _lineFreqsMap;
        int i = 0;
        while (std::getline(f, line))
        {
            srcLines.push_back(line);
            auto _foundIter = _lineFreqsMap.find(line); 
            if (_foundIter != _lineFreqsMap.end())
                _foundIter->second.first++;
            else
                _lineFreqsMap.emplace(line, std::pair<int, int>(1, i));
            
            i++;
        }
        
        f.close();

        mostFrequent = getMostFrequentEntries(&_lineFreqsMap, 16);
    }
    else
    {
        exit(1);
    }
}

vector<SIM::BinaryLine> SIM::getMostFrequentEntries(map<string, std::pair<int, int>>* frequencyDict, int max)
{
    vector<BinaryLine> _outVec;

    for (auto iter = frequencyDict->begin(); iter != frequencyDict->end(); ++iter)
    {
        BinaryLine _line;
        _line.lineContent = iter->first;
        _line.lineFreq = iter->second.first;
        _line.srcPos = iter->second.second;
        _outVec.push_back(_line);
    }

    std::sort(_outVec.begin(), _outVec.end(), &SIM::BinaryLine::comp);

    auto _iterA = _outVec.begin();
    auto _iterB = _outVec.begin() + max;

    return vector<BinaryLine>(_iterA, _iterB);
}

string SIM::getBinStrFromInt(int i, int numChars)
{
    string builder = string();
    int _i = i;
    while (_i != 0)
    {
        builder = ((_i % 2 == 0) ? "0" : "1") + builder;
        _i /= 2;
    }

    if (builder.size() < numChars)
        for (int i = 0; i < numChars - builder.size(); i++)
            builder = "0" + builder;

    if (DEBUG_MODE)
        printf("Got %s from %d\n", builder.c_str(), i);
        
    return builder;
}

int SIM::valueInVec(string val, vector<BinaryLine>* vec)
{
    for (int i = 0; i < vec->size(); i++)
        if (val == vec->at(i).lineContent)
            return i;

    return -1;
}

void SIM::mainLoop()
{
    int _reps = 0;
    for (auto iter = srcLines.begin(); iter  != srcLines.end(); ++iter)
    {
        std::string _previous = string();
        if (iter != srcLines.begin())
        {
            --iter;
            _previous = *iter;
            ++iter;
        }

        // RLE logic
        if (*iter == _previous && _reps < 7)
        {
            _reps++;
            continue;
        }
        else if (*iter == _previous && _reps >= 7 || (*iter != _previous && _reps > 0))
        {
            outfile << "001 " << getBinStrFromInt(_reps, 3) << "\n";
        }
        _reps = 0;

        // Dict index
        int _dictIndex = valueInVec(*iter, &mostFrequent);
        if (_dictIndex != -1)
        {
            outfile << "111 " << getBinStrFromInt(_dictIndex, 4) << "\n";
            continue;
        }

        outfile << "000 "  << *iter << "\n";
    }
}

void SIM::printFreqDict()
{
    outfile << "xxxx" << "\n";
    for (auto iter = mostFrequent.begin(); iter != mostFrequent.end(); ++iter)
    {
        if (DEBUG_MODE)
            std::cout << iter->lineContent << std::endl;

        outfile << iter->lineContent << "\n";
    }
}

void SIM::run()
{
    mainLoop();
    printFreqDict();
}