
#include "regex/regex.h"
#include "regexTokenizer/regexTokenizer.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

int main(int argc, char** argv) {
    std::string regex_str, str_to_match;
    std::ifstream file ("/home/cracky/lab2aaaaaaaaaaa/tests/shit.txt");
    try {
        if (file.is_open()) ;
        else 
            throw std::runtime_error("No such file!");
    }
    catch(std::runtime_error &e) {
        std::cout << e.what() << std::endl;
    }
    std::cin >> regex_str;

    auto regex = Regex(regex_str);
    std::cout << regex.match("agdg") << std::endl;
    /*while (!file.eof()) {
        file >> str_to_match;
        std::cout << regex.match(str_to_match) << std::endl;
    }*/
}