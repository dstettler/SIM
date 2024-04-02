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
            outfilePath = filepathDirs + COMPRESSED_FILEPATH; 
            break;
        case SIMMode::Decompression:
            outfilePath = filepathDirs + INSTRUCTIONS_FILEPATH;
            break;
    }

    outfile.open(outfilePath);
}

SIM::~SIM()
{
    if (outfile.is_open())
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
            // Remove Windows line ending if it is present
            if (line.back() == '\r')
                line.pop_back();

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

    int _charsDiff = numChars - builder.size();
    if (_charsDiff > 0)
        for (int i = 0; i < _charsDiff; i++)
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

int SIM::getFirstMismatch(string str1, string str2, int offset)
{
    for (int i = 0 + offset; i < str1.size() && i < str2.size(); i++)
        if (str1.at(i) != str2.at(i))
            return i;

    return -1;
}

bool SIM::isAnotherMistmatch(std::string str1, std::string str2, int firstMismatch, int skip)
{
    for (int i = firstMismatch + skip; i < str1.size() && i < str2.size(); i++)
        if (str1.at(i) != str2.at(i))
            return true;

    return false;
}

bool SIM::consecutiveMismatches(std::string str1, std::string str2, int firstMismatch, int length)
{
    for (int i = firstMismatch; (i < str1.size() && i < str2.size()) || (i < firstMismatch + length); i++)
        if (str1.at(i) != str2.at(i))
            return false;

    return true;
}

string SIM::bitmaskCompress(string line)
{
    for (auto dictEntry = mostFrequent.begin(); dictEntry != mostFrequent.end(); ++dictEntry)
    {
        int _firstMismatch = getFirstMismatch(line, dictEntry->lineContent);
        
        if (DEBUG_MODE)
            std::cout << _firstMismatch << " : " << std::distance(mostFrequent.begin(), dictEntry) << std::endl;

        if (_firstMismatch == -1 || isAnotherMistmatch(line, dictEntry->lineContent, _firstMismatch, 4))
            continue;

        string _generatedString = getBinStrFromInt(_firstMismatch, 5);
        if (DEBUG_MODE)
            _generatedString += " ";
        for (int i = 0; i < 4; i++)
        {
            if (i + _firstMismatch > line.size())
            {
                for (int j = 0; j < 4 - i; j++)
                    _generatedString += '0';
                break;
            }

            if (line.at(i + _firstMismatch) != dictEntry->lineContent.at(i + _firstMismatch))
                _generatedString += '1';
            else
                _generatedString += '0';
        }

        if (DEBUG_MODE)
            _generatedString += " ";
        _generatedString += getBinStrFromInt(std::distance(mostFrequent.begin(), dictEntry), 4);

        return _generatedString;
    }

    return "INVALID";
}

string SIM::mismatch1Bit(string line)
{
    for (auto dictEntry = mostFrequent.begin(); dictEntry != mostFrequent.end(); ++dictEntry)
    {
        int _firstMismatch = getFirstMismatch(line, dictEntry->lineContent);
        if (_firstMismatch == -1 || isAnotherMistmatch(line, dictEntry->lineContent, _firstMismatch, 1))
            continue;

        string _generatedString = getBinStrFromInt(_firstMismatch, 5);
        if (DEBUG_MODE)
            _generatedString += " ";

        if (DEBUG_MODE)
            _generatedString += " ";
        _generatedString += getBinStrFromInt(std::distance(mostFrequent.begin(), dictEntry), 4);

        return _generatedString;
    }

    return "INVALID";
}

string SIM::mismatch2Bit(string line)
{
    for (auto dictEntry = mostFrequent.begin(); dictEntry != mostFrequent.end(); ++dictEntry)
    {
        int _firstMismatch = getFirstMismatch(line, dictEntry->lineContent);
        if (
            _firstMismatch == -1 || 
            isAnotherMistmatch(line, dictEntry->lineContent, _firstMismatch, 2) || 
            !consecutiveMismatches(line, dictEntry->lineContent, _firstMismatch, 2))
            continue;

        string _generatedString = getBinStrFromInt(_firstMismatch, 5);
        if (DEBUG_MODE)
            _generatedString += " ";

        if (DEBUG_MODE)
            _generatedString += " ";
        _generatedString += getBinStrFromInt(std::distance(mostFrequent.begin(), dictEntry), 4);

        return _generatedString;
    }

    return "INVALID";
}

string SIM::mismatch4Bit(string line)
{
    for (auto dictEntry = mostFrequent.begin(); dictEntry != mostFrequent.end(); ++dictEntry)
    {
        int _firstMismatch = getFirstMismatch(line, dictEntry->lineContent);
        if (
            _firstMismatch == -1 || 
            isAnotherMistmatch(line, dictEntry->lineContent, _firstMismatch, 4) || 
            !consecutiveMismatches(line, dictEntry->lineContent, _firstMismatch, 4))
            continue;

        string _generatedString = getBinStrFromInt(_firstMismatch, 5);
        if (DEBUG_MODE)
            _generatedString += " ";

        if (DEBUG_MODE)
            _generatedString += " ";
        _generatedString += getBinStrFromInt(std::distance(mostFrequent.begin(), dictEntry), 4);

        return _generatedString;
    }

    return "INVALID";
}

string SIM::mismatch2BitAnywhere(string line)
{
    for (auto dictEntry = mostFrequent.begin(); dictEntry != mostFrequent.end(); ++dictEntry)
    {
        int _firstMismatch = getFirstMismatch(line, dictEntry->lineContent);
        if (_firstMismatch == -1)
            continue;

        int _secondMismatch = getFirstMismatch(line, dictEntry->lineContent, _firstMismatch + 1);
        if (_secondMismatch == -1 || isAnotherMistmatch(line, dictEntry->lineContent, _secondMismatch, 1))
            continue;

        string _generatedString = getBinStrFromInt(_firstMismatch, 5);
        if (DEBUG_MODE)
            _generatedString += " ";

        _generatedString += getBinStrFromInt(_secondMismatch, 5);

        if (DEBUG_MODE)
            _generatedString += " ";
        _generatedString += getBinStrFromInt(std::distance(mostFrequent.begin(), dictEntry), 4);

        return _generatedString;
    }

    return "INVALID";
}

void SIM::mainCompLoop()
{
    string _debugSpace = string();
    if (DEBUG_MODE)
        _debugSpace = " ";

    int _reps = -1;
    for (auto iter = srcLines.begin(); iter != srcLines.end(); ++iter)
    {
        if (DEBUG_MODE)
        {
            std::cout << "-----\nLine: " << std::distance(srcLines.begin(), iter) << std::endl;
        }

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
            if (DEBUG_MODE)
                std::cout << *iter << ", continuing..." << std::endl;
            _reps++;
            continue;
        }
        else if (*iter == _previous && _reps >= 7 || (*iter != _previous && _reps > 0))
        {
            if (DEBUG_MODE)
                std::cout << "Terminating RLE with " << _reps << " reps" << std::endl;
            outfile << "001" << _debugSpace << getBinStrFromInt(_reps, 3) << "\n";
            int _tempReps = _reps;
            _reps = -1;

            if (*iter == _previous && _tempReps >= 7)
                continue;
        }

        // Dict index
        int _dictIndex = valueInVec(*iter, &mostFrequent);
        if (_dictIndex != -1)
        {
            if (DEBUG_MODE)
                std::cout << "Dict" << std::endl;

            outfile << "111" << _debugSpace << getBinStrFromInt(_dictIndex, 4) << "\n";
            continue;
        }

        // Begin checking differing bitmask/mismatch methods
        map<string, string> _masks;
        _masks.emplace("010", bitmaskCompress(*iter));
        _masks.emplace("011", mismatch1Bit(*iter));
        _masks.emplace("100", mismatch2Bit(*iter));
        _masks.emplace("101", mismatch4Bit(*iter));
        _masks.emplace("110", mismatch2BitAnywhere(*iter));
        
        if (DEBUG_MODE)
            std::cout << "Iterating masks..." << std::endl;

        auto _shortest = _masks.begin();
        for (auto maskIter = _masks.begin(); maskIter != _masks.end(); ++maskIter)
        {
            if (DEBUG_MODE)
            {
                std::cout << maskIter->first << " " << maskIter->second << std::endl;
                std::cout << maskIter->second.size() << " < " << _shortest->second.size() << std::endl;
            }

            if ((maskIter->second.size() < _shortest->second.size() && maskIter->second != "INVALID") || _shortest->second == "INVALID")
                _shortest = maskIter;
        }

        if (_shortest->second != "INVALID")
        {
            outfile << _shortest->first << _debugSpace << _shortest->second << "\n";
            continue;
        }

        if (DEBUG_MODE)
            std::cout << "Writing original code..." << std::endl;

        outfile << "000" << _debugSpace << *iter << "\n";
    }
}

void SIM::printFreqDict()
{
    outfile << "xxxx" << "\n";

    for (auto iter = mostFrequent.begin(); iter != mostFrequent.end(); ++iter)
    {
        outfile << iter->lineContent << "\n";
    }
}

void SIM::formatFile()
{
    outfile.close();
    std::ifstream infile(outfilePath);
}

void SIM::run()
{
    if (mode == SIMMode::Compression)
    {
        mainCompLoop();
        // formatFile();
        printFreqDict();
    }
}