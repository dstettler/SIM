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

    this->outfilePath = filepathDirs + INSTRUCTIONS_FILEPATH;
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
    for (int i = 0; i < bitsToRead && i + start < codeStream.size(); i++)
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
        string _format = readBitsFromCodeStream(pc, 3);
        
        string _outStr = string();
        if (_format == "000")
        {
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
            _outStr = oneBitMismatch(pc + 3);
        }
        else if (_format == "100")
        {
            nextPc = pc + 3 + 5 + 4;
            _outStr = twoBitConMismatch(pc + 3);
        }
        else if (_format == "101")
        {
            nextPc = pc + 3 + 5 + 4;
            _outStr = fourBitConMismatch(pc + 3);
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
            
            _outStr = dictionary[binStrToInt(_dictVal)];
        }

        if (DEBUG_MODE)
            std::cout << "PC: " << pc << ", " << _outStr  << "\n----" << std::endl;

        prevLine = _outStr;

        outfile << _outStr << "\n";
        pc = nextPc;
    }
}

string Decompress::rle(size_t startingIndex)
{
    return "UNDEFINED";
}

string Decompress::bitmask(size_t startingIndex)
{
    return "UNDEFINED";
}

string Decompress::oneBitMismatch(size_t startingIndex)
{
    return "UNDEFINED";
}

string Decompress::twoBitConMismatch(size_t startingIndex)
{
    return "UNDEFINED";
}

string Decompress::fourBitConMismatch(size_t startingIndex)
{
    return "UNDEFINED";
}

string Decompress::twoBitAnyMismatch(size_t startingIndex)
{
    return "UNDEFINED";
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