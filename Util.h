#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

using std::string;

// comp in
const string INSTRUCTIONS_FILEPATH = "original.txt";
// decomp in
const string COMPRESSED_FILEPATH = "compressed.txt";
// comp out
const string COMPRESSED_OUT = "cout.txt";
// decomp out
const string DECOMPRESSED_OUT = "dout.txt";

const bool DEBUG_MODE = false;

string getBinStrFromInt(int i, int numChars);

// Returns index of first character mismatch in str, -1 if not found
// `offset` can be used to skip ahead
int getFirstMismatch(std::string str1, std::string str2, int offset = 0);
/* 
    Returns if there is another mismatch between the two given strings.
    `firstMismatch` - First known mismatch position
    `skip` - How many indices to skip after the `firstMismatch` before checking for new mismatches
    Returns:
        true if another mismatch is found after `firstMismatch` + `skip`
*/
bool isAnotherMistmatch(std::string str1, std::string str2, int firstMismatch, int skip);
/* 
    Returns if there are consecutive mismatches.
    `firstMismatch` - First known mismatch position
    `length` - How many indices to check for mismatches
    Returns:
        true if every index in range `length` after `firstMismatch`
*/
bool consecutiveMismatches(std::string str1, std::string str2, int firstMismatch, int length);

unsigned int binStrToInt(string str);

#endif