#include "Compress.h"
#include "Util.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>

using std::string, std::vector, std::map;

bool Compress::BinaryLine::comp(const BinaryLine &a, const BinaryLine &other)
{
    if (a.lineFreq != other.lineFreq)
        return a.lineFreq > other.lineFreq;
    else
        return a.srcPos < other.srcPos;
}

Compress::Compress(string filepathDirs)
{
    this->filepathDirs = filepathDirs;

    initSrcLines();

    outfilePath = filepathDirs + COMPRESSED_OUT;
    outfile.open(outfilePath);
}

Compress::~Compress()
{
    if (outfile.is_open())
        outfile.close();
}

void Compress::initSrcLines()
{
    if (DEBUG_MODE)
        std::cout << "Initializing source lines..." << std::endl;

    std::ifstream f(filepathDirs + INSTRUCTIONS_FILEPATH);
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

vector<Compress::BinaryLine> Compress::getMostFrequentEntries(map<string, std::pair<int, int>>* frequencyDict, int max)
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

    std::sort(_outVec.begin(), _outVec.end(), &Compress::BinaryLine::comp);

    auto _iterA = _outVec.begin();
    auto _iterB = _outVec.begin() + max;

    return vector<BinaryLine>(_iterA, _iterB);
}

int Compress::valueInVec(string val, vector<BinaryLine>* vec)
{
    for (int i = 0; i < vec->size(); i++)
        if (val == vec->at(i).lineContent)
            return i;

    return -1;
}

string Compress::bitmaskCompress(string line)
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

string Compress::mismatch1Bit(string line)
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

string Compress::mismatch2Bit(string line)
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

string Compress::mismatch4Bit(string line)
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

string Compress::mismatch2BitAnywhere(string line)
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

void Compress::mainCompLoop()
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

            if (DEBUG_MODE)
                std::cout << "Current: " << *iter << ", prev: " << _previous << std::endl;
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

unsigned int Compress::insertStringToStream(string toAdd, string* stream, unsigned int streamWidth)
{
    // Get number of characters on current line
    unsigned int _charsOnCurrentLine = 0;
    for (int i = 0; i < stream->size(); i++)
    {
        if (stream->at(i) != '\n')
            _charsOnCurrentLine++;
        else
            _charsOnCurrentLine = 0;
    }


    // Insert characters until 
    for (int i = 0; i < toAdd.size(); i++)
    {
        if (_charsOnCurrentLine < streamWidth)
        {
            stream->push_back(toAdd[i]);
        }
        else
        {
            // Reset chars since we're on a new line
            stream->push_back('\n');
            _charsOnCurrentLine = 0;

            stream->push_back(toAdd[i]);
        }
        
        _charsOnCurrentLine++;
    }

    return _charsOnCurrentLine;
}

void Compress::printFreqDict()
{
    outfile << "xxxx" << "\n";

    for (auto iter = mostFrequent.begin(); iter != mostFrequent.end(); ++iter)
    {
        outfile << iter->lineContent << "\n";
    }
}

void Compress::formatFile()
{
    outfile.close();
    std::ifstream infile(outfilePath);
    
    string codeStream;
    unsigned int _charsOnLine;

    if (infile.is_open())
    {
        string line;
        while (std::getline(infile, line))
        {
            string _toAdd = string();
            for (int i = 0; i < line.size(); i++)
                if (line.at(i) == '1' || line.at(i) == '0')
                    _toAdd += line.at(i);
                
            _charsOnLine = insertStringToStream(_toAdd, &codeStream);
        }

        for (int i = 0; i < 32 - _charsOnLine; i++)
        {
            codeStream += '0';
        }

        codeStream += "\n";

        infile.close();
        outfile.open(outfilePath);
        outfile << codeStream;
    }
    else
    {
        std::cout << "Unable to read outfile when reformatting!" << std::endl;
        exit(1);
    }
}

void Compress::run()
{
    mainCompLoop();
    formatFile();
    printFreqDict();
}