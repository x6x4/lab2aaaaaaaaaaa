#pragma once
#include <cstddef>
#include <cstdlib>
#include <string>
#include "../ast/ast.h"
#include "../dfa_sets/dfa_sets.h"
#include "../regexTokenizer/regexTokenizer.h"


inline DFA compile(const std::string &regex) {
    tokenString regex_str = RgxTokenizer().tokenize(regex);

    AST ast(regex_str);

    ast.printAST();
    //ast.printLeafMap();

    DFA_sets sets(ast);
    
    DFA dfa = sets.makeDFA(ast);
    //dfa.printDFA();
    //auto invDFA = dfa.inverse();
    //invDFA.printDFA();
    return dfa;
}

class Regex {

DFA m_dfa;

public:

    Regex(const std::string &regex) : m_dfa (compile(regex)) {}

    Regex(DFA dfa) : m_dfa(dfa) {}
    //  method dfa - transition by symbol, return cur_state
    //  check for end state

    DFA dfa() {return m_dfa; }

    bool match(std::string sample) {

        std::size_t curIdx = 0;
        DFAState cur_state = 0;
        bool capture_on = false;
        std::string captureStr;

        //  iterative for 
        while (curIdx < sample.size()) {
            auto cur_tran = m_dfa.m_Dtran.find({cur_state, sample[curIdx]});
                if (cur_tran == m_dfa.m_Dtran.end()) {
                    auto next_open_tran = m_dfa.m_Dtran.find({cur_state, Token::Kind::CaptStart});
                    auto next_close_tran = m_dfa.m_Dtran.find({cur_state, Token::Kind::CaptFin});
                    auto next_capt_tran = m_dfa.m_Dtran.find({cur_state, Token::Kind::CaptStr});

                    if (next_open_tran != m_dfa.m_Dtran.end()) {
                        capture_on = true;
                        cur_state = next_open_tran->second;
                        continue;
                    }
                    if (next_close_tran != m_dfa.m_Dtran.end()) {
                        capture_on = false;
                        cur_state = next_close_tran->second;
                        continue;
                    }
                    if (next_capt_tran != m_dfa.m_Dtran.end()) {
                        auto leftover = sample.substr(curIdx);
                        if (leftover.compare(0, captureStr.length(), captureStr))
                            return false;
                        
                        curIdx += leftover.length();
                        cur_state = next_capt_tran->second;
                        continue;
                    }

                    return false;
                }

            auto curTok = cur_tran->first.second;

            if (capture_on) {
                if (curTok.kind() == Token::Kind::Char) {
                    captureStr.push_back(curTok.getChar());
                }
            }

            cur_state = cur_tran->second;
            curIdx++;
        }
        if (m_dfa.m_FinStates.find(cur_state) != m_dfa.m_FinStates.end()) return true;

        return false;
    }


    bool match_notcapt(std::string sample) {

        std::size_t curIdx = 0;
        DFAState cur_state = 0;
        bool capture_on = false;
        std::string captureStr;

        //  iterative for 
        while (curIdx < sample.size()) {
            auto cur_tran = m_dfa.m_Dtran.find({cur_state, sample[curIdx]});
            
            if (cur_tran == m_dfa.m_Dtran.end()) { return false;}

            cur_state = cur_tran->second;
            curIdx++;
        }
        if (m_dfa.m_FinStates.find(cur_state) != m_dfa.m_FinStates.end()) return true;

        return false;
    }
};