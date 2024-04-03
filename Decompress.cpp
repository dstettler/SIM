#include "Decompress.h"
#include "Util.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>

using std::string, std::vector, std::map;

Decompress::Decompress(string filepathDirs)
{
    this->filepathDirs = filepathDirs;
    this->codeStream = string();

    initCodeStream();

    this->outfilePath = filepathDirs + DECOMPRESSED_OUT;
    this->outfile.open(this->outfilePath);
}

Decompress::~Decompress()
{
    if (outfile.is_open())
        outfile.close();
}

void Decompress::initCodeStream()
{
    if (DEBUG_MODE)
        std::cout << "Initializing code stream..." << std::endl;

    std::ifstream f(filepathDirs + COMPRESSED_FILEPATH);
    if (f.is_open())
    {
        bool _readingCode = true;
        string line;
        while (std::getline(f, line))
        {
            // Remove Windows line ending if it is present
            if (line.back() == '\r')
                line.pop_back();
            
            if (line == "xxxx")
            {
                _readingCode = false;
                continue;
            }

            if (_readingCode)
                codeStream += line;
            else
                dictionary.push_back(line);
        }
        
        f.close();
    }
    else
    {
        exit(1);
    }
}

string Decompress::readBitsFromCodeStream(size_t start, size_t bitsToRead)
{
    string _str = string();
    for (size_t i = 0; i < bitsToRead && i + start < codeStream.size(); i++)
        _str += codeStream[start + i];

    return _str;
}

void Decompress::decompLoop()
{
    size_t nextPc;
    // 6 bits is the smallest possible instruction, so the upper possible limit will be size - 3 - 3
    // ...pc should be assigned to a value higher than this regardless
    for (size_t pc = 0; pc < codeStream.size() - 3 - 3;)
    {
        if (DEBUG_MODE)
            printf("Starting loop...\n");

        string _format = readBitsFromCodeStream(pc, 3);
        
        if (DEBUG_MODE)
            std::cout << "Got format: " << _format << std::endl;

        string _outStr = string();
        if (_format == "000")
        {
            if (pc + 32 > codeStream.size())
                break;

            nextPc = pc + 3 + 32;
            for (int i = 0; i < 32; i++)
                _outStr += codeStream.at(pc + 3 + i);
        }
        else if (_format == "001")
        {
            nextPc = pc + 3 + 3;
            _outStr = rle(pc + 3);
        }
        else if (_format == "010")
        {
            nextPc = pc + 3 + 5 + 4 + 4;
            _outStr = bitmask(pc + 3);
        }
        else if (_format == "011")
        {
            nextPc = pc + 3 + 5 + 4;
            _outStr = consecutiveBitMismatch(pc + 3, 1);
        }
        else if (_format == "100")
        {
            nextPc = pc + 3 + 5 + 4;
            _outStr = consecutiveBitMismatch(pc + 3, 2);
        }
        else if (_format == "101")
        {
            nextPc = pc + 3 + 5 + 4;
            _outStr = consecutiveBitMismatch(pc + 3, 4);
        }
        else if (_format == "110")
        {
            nextPc = pc + 3 + 5 + 5 + 4;
            _outStr = twoBitAnyMismatch(pc + 3);
        }
        else if (_format == "111")
        {
            nextPc = pc + 3 + 4;
            string _dictVal = readBitsFromCodeStream(pc + 3, 4);
            
            if (DEBUG_MODE)
                std::cout << "Got value: " << _dictVal << "/" << binStrToInt(_dictVal) << std::endl;
            
            _outStr = dictionary[binStrToInt(_dictVal)];
        }

        if (DEBUG_MODE)
            std::cout << "PC: " << pc << ", " << _outStr  << "\n----" << std::endl;

        if (_format != "001")
            prevLine = _outStr;

        outfile << _outStr << "\n";
        pc = nextPc;
    }
}

string Decompress::rle(size_t startingIndex)
{
    string builder = string();

    string _reps = readBitsFromCodeStream(startingIndex, 3);
    unsigned int _repsInt = binStrToInt(_reps);

    if (DEBUG_MODE)
        std::cout << "RLE reps str: " << _reps << " to int: " << _repsInt << std::endl;

    for (unsigned int i = 0; i < _repsInt + 1; i++)
    {
        builder += prevLine + "\n";
    }

    // Remove final newline
    builder.pop_back();
    
    return builder;
}

string Decompress::bitmask(size_t startingIndex)
{
    string builder = string();

    string _startingLocationBin, _bitmaskBin, _dictIndexBin;
    _startingLocationBin = readBitsFromCodeStream(startingIndex, 5);
    _bitmaskBin = readBitsFromCodeStream(startingIndex + 5, 4);
    _dictIndexBin = readBitsFromCodeStream(startingIndex + 5 + 4, 4);

    unsigned int _startingLocation = binStrToInt(_startingLocationBin);
    unsigned int _dictIndex = binStrToInt(_dictIndexBin);

    string _originalStr = dictionary[_dictIndex];

    int _maskPos = -1;
    for (size_t i = 0; i < _originalStr.size(); i++)
    {
        if (i == _startingLocation)
            _maskPos = 0;
        
        if (_maskPos >= 0 && _maskPos < 4)
        {
            if (_bitmaskBin[_maskPos] == '1')
                builder += ((_originalStr[i] == '0') ? '1' : '0' );
            else
                builder += _originalStr[i];

            _maskPos++;
        }
        else
        {
            builder += _originalStr[i];
        }
    }

    return builder;
}

string Decompress::consecutiveBitMismatch(size_t startingIndex, unsigned int bits)
{
    string builder = string();

    string _startingLocationBin, _dictIndexBin;
    _startingLocationBin = readBitsFromCodeStream(startingIndex, 5);
    _dictIndexBin = readBitsFromCodeStream(startingIndex + 5, 4);

    unsigned int _startingLocation = binStrToInt(_startingLocationBin);
    unsigned int _dictIndex = binStrToInt(_dictIndexBin);

    string _originalStr = dictionary[_dictIndex];

    int _maskPos = -1;
    for (size_t i = 0; i < _originalStr.size(); i++)
    {
        if (i == _startingLocation)
            _maskPos = 0;
        
        if (_maskPos >= 0 && _maskPos < bits)
        {
            builder += ((_originalStr[i] == '0') ? '1' : '0' );
            _maskPos++;
        }
        else
        {
            builder += _originalStr[i];
        }
    }

    return builder;
}

string Decompress::twoBitAnyMismatch(size_t startingIndex)
{
    string builder = string();

    string _firstMistmatchBin, _secondMismatchBin, _dictIndexBin;
    _firstMistmatchBin = readBitsFromCodeStream(startingIndex, 5);
    _secondMismatchBin = readBitsFromCodeStream(startingIndex + 5, 5);
    _dictIndexBin = readBitsFromCodeStream(startingIndex + 5 + 5, 4);

    unsigned int _firstMistmatch = binStrToInt(_firstMistmatchBin);
    unsigned int _secondMismatch = binStrToInt(_secondMismatchBin);
    unsigned int _dictIndex = binStrToInt(_dictIndexBin);

    string _originalStr = dictionary[_dictIndex];

    int _maskPos = -1;
    for (size_t i = 0; i < _originalStr.size(); i++)
    {
        if (i == _firstMistmatch || i == _secondMismatch)
            builder += ((_originalStr[i] == '0') ? '1' : '0' );
        else
            builder += _originalStr[i];
    }

    return builder;
}


void Decompress::run()
{
    if (DEBUG_MODE)
    {
        std::cout << codeStream << std::endl;
        std::cout << "Dict:" << std::endl;
        for (int i = 0; i < dictionary.size(); i++)
        std::cout << i << ":\t" << dictionary[i] << std::endl;
    }

    decompLoop();
}