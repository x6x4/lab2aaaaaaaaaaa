
#include "dfa_sets/dfa_sets.h"
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
    DFA newdfa = regex.dfa();
    DFA invdfa = newdfa.inverse();
    //invdfa.printDFA();
    Regex inv(invdfa);
    //DFA newnewdfa = compile("ab");
    //newnewdfa.printDFA();
    //DFA newnewnewdfa = newdfa * newnewdfa;
    //newnewnewdfa.printDFA();

    do {
        str_to_match = {};
        file >> str_to_match; 
        if (!str_to_match.empty()) {
            std::cout << regex_str << ": " << str_to_match
            << ": " << regex.match(str_to_match) << std::endl;
            std::cout << "Inv " << regex_str << ": " << str_to_match
            << ": " << inv.match(str_to_match) << std::endl;
        }
    } while (!str_to_match.empty());
    std::cout << std::endl; 
}