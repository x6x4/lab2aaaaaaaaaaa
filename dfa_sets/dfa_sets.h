#pragma once
#include "../common/common.h"
#include "../ast/ast.h"
#include <cstddef>
#include <set>


typedef std::size_t DFAState;

using positionSet = std::set<std::size_t>;
using trans_table = std::map<std::pair<DFAState, Token>, DFAState>;
using finStates = std::set<DFAState>;

class Regex;

class DFA {
friend Regex;
public:
    DFA(trans_table Dtran, finStates FinStates, std::size_t states_num) 
        : m_Dtran(std::move(Dtran)), m_FinStates(std::move(FinStates)),
        m_states_num(states_num) {};

    void printDFA();
    std::size_t size() const {return m_states_num;}
    DFA inverse() const;
    DFA operator*(const DFA& rhs) const;
    DFA diff(const DFA& rhs) const {
        return *this * rhs.inverse();
    }

private:    
    trans_table m_Dtran;
    finStates m_FinStates;
    std::size_t m_states_num;

    void printDFA_Base(std::ofstream &file);
    
    friend std::ostream &operator<< (std::ostream &os, const DFA &dfa) {
        os << dfa.m_Dtran << '\n'; 
        return os;     
    };
};


using node_list = std::set<std::size_t>;

class DFA_sets {

public:
    DFA makeDFA(const AST& ast);

    DFA_sets(const AST& ast) {
        nullable = std::vector<bool>(ast.nodeCount());
        firstpos = std::vector<node_list>(ast.nodeCount());
        lastpos = std::vector<node_list>(ast.nodeCount());
        followpos = std::vector<node_list>(ast.leafCount());

        calculate(ast.root());
    }

private:

    void calculate(ASTNode *root);

    std::vector<bool> nullable;
    void nullable_traversal (ASTNode *start);
    std::vector<node_list> firstpos;
    void firstpos_traversal (ASTNode *start);
    std::vector<node_list> lastpos;
    void lastpos_traversal (ASTNode *start);
    std::vector<node_list> followpos;
    void followpos_traversal (ASTNode *start);

    friend std::ostream &operator<< (std::ostream &os, const DFA_sets &sets);
};

