//
// Created by Xaine on 17/04/2023.
//

#ifndef NEA_SEARCH_H
#define NEA_SEARCH_H

#include <algorithm>
#include <string>
#include <cctype>

class Search {
public:
    static bool findString(const std::string & strHaystack, const std::string & strNeedle)
    {

        // check if needle is the same as the haystack
        if (strNeedle == strHaystack) {
            // needle is the same as the haystack
            // return true
            return true;
        }

        // check if needle is bigger than haystack
        if (strNeedle.size() > strHaystack.size()) {
            // needle is bigger than haystack
            // return false
            return false;
        }
        // loop through all characters in the haystack
        for (int index = 0; index < strHaystack.size(); index++) {
            // loop through until you find first character of needle
            // check if needle is bigger than the remainder of haystack
            if (strNeedle.size() > strHaystack.size() - index) {
                // needle is bigger than the remainder of haystack
                // return false
                return false;
            }
            if (tolower(strHaystack[index]) == tolower(strNeedle[0])) {
                // found first character of needle
                // check if we are only looking for one character
                if (strNeedle.size() == 1) {
                    // we are only looking for one character
                    // return true
                    return true;
                }

                // we are looking for more than one character
                // check if the size of the needle is bigger than the remainder of haystack
                if (strNeedle.size() > strHaystack.size() - index) {
                    // needle is bigger than the remainder of haystack
                    // return false
                    return false;
                }

                // needle is smaller than the remainder of haystack
                // loop through the remainder of haystack
                for (int i = 1; i < strNeedle.size(); i++) {
                    // check if the next character in haystack is the same as the next character in needle
                    if (tolower(strHaystack[index + i]) != tolower(strNeedle[i])) {
                        // next character in haystack is not the same as the next character in needle
                        // break out of loop
                        return false;
                    }

                    // next character in haystack is the same as the next character in needle
                    // check if we are at the end of the needle
                    if (i == strNeedle.size() - 1) {
                        // we are at the end of the needle
                        // return true
                        return true;
                    }
                }
            }
        }
        // never found first character of needle
        return false;
    }
};


#endif //NEA_SEARCH_H
