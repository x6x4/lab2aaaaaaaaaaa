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

    bool match(const std::string &regex) {

        auto cur = regex.begin();
        std::size_t cur_state = 0;

        //  iterative for 
        while (cur != regex.end()) {
            auto cur_tran = m_dfa.m_Dtran.find({cur_state, *cur});
            if (cur_tran == m_dfa.m_Dtran.end()) return false;
            cur_state = cur_tran->second;
            cur++;
        }
        if (m_dfa.m_FinStates.find(cur_state) != m_dfa.m_FinStates.end()) return true;

        return false;
    }
};