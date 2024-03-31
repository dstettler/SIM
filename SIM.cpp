#include "SIM.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

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
        while (std::getline(f, line))
            srcLines.push_back(line);
        
        f.close();
    }
    else
    {
        exit(1);
    }
}

void SIM::run()
{

}