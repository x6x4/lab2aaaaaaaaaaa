#pragma once
#include <cstddef>
#include <string>
#include "../ast/ast.h"
#include "../dfa_sets/dfa_sets.h"
#include "../regexTokenizer/regexTokenizer.h"


class Regex {

DFA m_dfa;


public:

    Regex(const std::string &regex) : m_dfa (compile(regex)) {}

    DFA compile(const std::string &regex) {
        tokenString regex_str = RgxTokenizer().tokenize(regex);

        AST ast(regex_str);
        ast.printAST();
        ast.printLeafMap();

        DFA_sets sets(ast);
        
        DFA dfa = sets.makeDFA(ast);
        dfa.printDFA();
        return dfa;
    }

    //  method dfa - transition by symbol, return cur_state
    //  check for end state

    bool match(const std::string &sample) {

        auto curIdx = 0;
        DFAState cur_state = 0;

        //  iterative for 
        while (curIdx != sample.size()) {
            auto cur_tran = m_dfa.m_Dtran.find({cur_state, sample[curIdx]});
            
            if (cur_tran == m_dfa.m_Dtran.end()) return false;

            auto curTok = cur_tran->first.second;

            if (curTok.kind() == Token::Kind::CaptureString) {
                auto leftover = sample.substr(curIdx+1);
                if (leftover.compare(0, curTok.Str().length(), curTok.Str()))
                    return false;
                
                curIdx += leftover.length();
                cur_state = cur_tran->second;
                continue;
            }

            cur_state = cur_tran->second;
            curIdx++;
        }
        if (m_dfa.m_FinStates.find(cur_state) != m_dfa.m_FinStates.end()) return true;

        return false;
    }
};