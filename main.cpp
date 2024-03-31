#include "SIM.h"

#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[])
{
    std::vector<std::string> args;
    for (int i = 0; i < argc; i++)
        args.push_back(std::string(argv[i]));

    if (args.size() != 2)
    {
        std::cout << "Please use the correct number of arguments!" << std::endl;
        exit(1);
    }

    SIM::SIMMode _selectedMode;
    if (args[1] == "1")
        _selectedMode = SIM::SIMMode::Compression;
    else if (args[1] == "2")
        _selectedMode = SIM::SIMMode::Decompression;
    else
    {
        std::cout << "Please use a valid argument!" << std::endl;
        exit(1);
    }

    SIM s;
    s.init("./text/", _selectedMode);
    s.run();
    return 0;
}