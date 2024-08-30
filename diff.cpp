#include "dfa_sets/dfa_sets.h"
#include "regex/regex.h"
#include "regexTokenizer/regexTokenizer.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

int main(int argc, char** argv) {

    auto regex_str_d1 = "ab*";
    auto regex1 = Regex(regex_str_d1);
    DFA dfa1 = regex1.dfa();   
    auto regex_str_d2 = "a";
    auto regex2 = Regex(regex_str_d2);
    DFA dfa2 = regex2.dfa();
    DFA diffdfa = dfa1.diff(dfa2);

    auto regexdiff = Regex(diffdfa);

    std::vector<std::string> test_strs = {
        "a", "ab", "ab", "abbbb", ""
    };
    
    //diffdfa.printDFA();

    for (const auto &str_to_match : test_strs) {
        std::cout << "Diff " << regex_str_d1 << '-' 
        << regex_str_d2 << ": " << str_to_match
        << ": " << regexdiff.match_notcapt(str_to_match) << std::endl;
    }
    std::cout << std::endl; 

    std::cout << "Language - Language : ab*\n";

    auto regex11 = Regex("ab*");
    DFA dfa11 = regex11.dfa();    
    auto regex22 = Regex("ab*");
    DFA dfa22 = regex22.dfa();

    DFA diffdfa2 = dfa11.diff(dfa22);

    auto regexdiff2 = Regex(diffdfa2);

    std::vector<std::string> test_strs3 = {
        "a", "ab", "c", "abbbb", ""
    };
    
    diffdfa2.printDFA();

    for (const auto &str_to_match : test_strs3) {
        std::cout << "Diff" << ": " << str_to_match
        << ": " << regexdiff2.match_notcapt(str_to_match) << std::endl;
    }
    std::cout << std::endl; 

    std::cout << "Multiplication\n";

    DFA mult_dfa1 = Regex("acb*").dfa();
    DFA mult_dfa2 = Regex("ac*").dfa();
    DFA mult = mult_dfa1 * mult_dfa2;
    //mult.printDFA();
    auto regexMult = Regex(mult);

    std::vector<std::string> test_strs2 = {
        "a", "c", "b", "accc", "ac", ""
    };

    for (const auto &str_to_match : test_strs2) {
        std::cout << "Mult" << ": " << str_to_match
        << ": " << regexMult.match_notcapt(str_to_match) << std::endl;
    }
    std::cout << std::endl; 
}