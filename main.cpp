
#include "regex/regex.h"
#include <iostream>

int main() {
    std::string regex_str, str_to_match;
    std::cin >> regex_str;

    auto regex = Regex(regex_str);
    while (1) {
        std::cin >> str_to_match;
        std::cout << regex.match(str_to_match) << std::endl;
    }
}