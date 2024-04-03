#include "Util.h"

#include <string>

using std::string;

string getBinStrFromInt(int i, int numChars)
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

int getFirstMismatch(string str1, string str2, int offset)
{
    for (int i = 0 + offset; i < str1.size() && i < str2.size(); i++)
        if (str1.at(i) != str2.at(i))
            return i;

    return -1;
}

bool isAnotherMistmatch(std::string str1, std::string str2, int firstMismatch, int skip)
{
    for (int i = firstMismatch + skip; i < str1.size() && i < str2.size(); i++)
        if (str1.at(i) != str2.at(i))
            return true;

    return false;
}

bool consecutiveMismatches(std::string str1, std::string str2, int firstMismatch, int length)
{
    for (int i = firstMismatch; (i < str1.size() && i < str2.size()) || (i < firstMismatch + length); i++)
        if (str1.at(i) != str2.at(i))
            return false;

    return true;
}