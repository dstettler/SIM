#include "Decompress.h"
#include "Constants.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>

using std::string, std::vector, std::map;

Decompress::Decompress(string filepathDirs)
{
    this->filepathDirs = filepathDirs;

    outfilePath = filepathDirs + INSTRUCTIONS_FILEPATH;
    outfile.open(outfilePath);
}

Decompress::~Decompress()
{
    if (outfile.is_open())
        outfile.close();
}

void Decompress::run()
{
    
}