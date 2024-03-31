#include "SIM.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>

using std::string, std::vector, std::map, std::shared_ptr;

bool SIM::BinaryLine::operator< (const BinaryLine &other) const
{
    if (this->lineFreq != other.lineFreq)
        return this->lineFreq < other.lineFreq;
    else
        return this->srcPos < other.srcPos;
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

vector<string> SIM::getMostFrequentEntries(map<string, std::pair<int, int>>* frequencyDict, int max)
{
    vector<string> _outVec;
    std::priority_queue<BinaryLine> _priorityQueue;

    for (auto iter = frequencyDict->begin(); iter != frequencyDict->end(); ++iter)
    {
        BinaryLine _line;
        _line.lineContent = iter->first;
        _line.lineFreq = iter->second.first;
        _line.srcPos = iter->second.second;
        _priorityQueue.emplace(_line);
    }

    int i = 0;
    while ((!_priorityQueue.empty() && i < max) || (!_priorityQueue.empty() && max == 0))
    {
        _outVec.push_back(_priorityQueue.top().lineContent);
        _priorityQueue.pop();
        i++;
    }

    return _outVec;
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

    printf("Got %s from %d\n", builder.c_str(), i);
    return builder;
}

int SIM::valueInVec(string val, vector<string>* vec)
{
    for (int i = 0; i < vec->size(); i++)
        if (val == vec->at(i))
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
            std::cout << *iter << std::endl;

        outfile << *iter << "\n";
    }
}

void SIM::run()
{
    mainLoop();
    printFreqDict();
}