
#include "regex/regex.h"
#include "regexTokenizer/regexTokenizer.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

int main(int argc, char** argv) {
    std::string regex_str, str_to_match;
    std::ifstream file (argv[1]);
    try {
        if (file.is_open()) ;
        else 
            throw std::runtime_error("No such file!");
    }
    catch(std::runtime_error &e) {
        std::cout << e.what() << std::endl;
    }

    file >> regex_str;

    auto regex = Regex(regex_str);
    do {
        str_to_match = {};
        file >> str_to_match; 
        //if (!str_to_match.empty()) {
            std::cout << regex_str << ": " << str_to_match
            << ": " << regex.match(str_to_match) << std::endl;
        //}
    } while (!str_to_match.empty());
    std::cout << std::endl; 
}