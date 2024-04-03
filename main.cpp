#include "Compress.h"
#include "Decompress.h"

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

    // Compression
    if (args[1] == "1")
    {
        Compress c("./text/");
        c.run();
    }
    // Decompression
    else if (args[1] == "2")
    {
        Decompress decomp("./text/");
        decomp.run();
    }
    else
    {
        std::cout << "Please use a valid argument!" << std::endl;
        exit(1);
    }

    return 0;
}