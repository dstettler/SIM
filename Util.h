#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

using std::string;

const string INSTRUCTIONS_FILEPATH = "instr.txt";
const string COMPRESSED_FILEPATH = "compressed.txt";

const bool DEBUG_MODE = true;

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

#endif