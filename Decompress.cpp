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

void Decompress::run()
{
    if (DEBUG_MODE)
    {
        std::cout << codeStream << std::endl;
        std::cout << "Dict:" << std::endl;
        for (int i = 0; i < dictionary.size(); i++)
        std::cout << i << ":\t" << dictionary[i] << std::endl;
    }

    
}