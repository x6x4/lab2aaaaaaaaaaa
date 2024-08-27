
#include "regexTokenizer.h"

#include <iostream>
#include <stdexcept>


tokenString RgxTokenizer::tokenize(const std::string &init_regex) {

    RegexHelper rhelper;

    tokenString new_regex;
    bool is_metaname = 0;
    std::string metaname;
    bool is_repeat = 0;
    std::string n_repeats;

    for (size_t i = 0; i < init_regex.length(); i++) {

        if (init_regex[i] == '>') {
            is_metaname = 0;
            new_regex.push_back(metaname);
            metaname = {};
            continue;
        }

        if (is_metaname) {
            metaname.push_back(init_regex[i]);
            continue;
        }

        if (init_regex[i] == '<') {
            metaname = 1;
            continue;
        }

        if (init_regex[i] == '}') {
            is_repeat = 0;
            new_regex.push_back(atoi(n_repeats.c_str()));
            n_repeats = {};
            continue;
        }

        if (is_repeat) {
            n_repeats.push_back(init_regex[i]);
            continue;
        }

        if (init_regex[i] == '{') {
            is_repeat = 1;
            continue;
        }


        if (init_regex[i] == '%') {
            if (i + 1 < init_regex.length()) {
                new_regex.push_back(init_regex[i+1]);
            }
            //  skip %c%
            i += 3; 
            if (i + 1 < init_regex.length()) {  // ???? #
                new_regex.push_back(Token::Kind::Cat);
            }
            continue;
        }

        if (rhelper.non_special(init_regex[i]))
            new_regex.push_back(init_regex[i]);
        else {
            switch (init_regex[i]) {
                case '\\':
                    new_regex.push_back(Token::Kind::Alter);
                    break;
                case '*':
                    new_regex.push_back(Token::Kind::Kline);
                    break;   
                case '<':
                    new_regex.push_back(Token::Kind::CaptStart);
                    break; 
                case '>':
                    new_regex.push_back(Token::Kind::CaptFin);
                    break; 
                case '?':
                    new_regex.push_back(Token::Kind::ZeroOrOne);
                    break; 
                case '(':
                    new_regex.push_back(Token::Kind::PriorStart);
                    break;
                case ')':
                    new_regex.push_back(Token::Kind::PriorFin);
                    break; 
                default:
                    std::string s = "Incorrect char :";
                    s.push_back(init_regex[i]);
                    throw std::runtime_error(s);
            }
        }
        
        if (i + 1 < init_regex.length() && 
            rhelper.can_paste_concat(init_regex[i], init_regex[i + 1])) {
                new_regex.push_back(Token::Kind::Cat);
        }
    }

    return new_regex;
}