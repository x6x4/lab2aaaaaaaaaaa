#include "dfa_sets.h"
#include <cstddef>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

using Dstates = std::map<state, std::size_t>;

std::pair<bool, std::size_t> dstates_find 
    (const Dstates& dstates, const state& key) {
        for (const auto &d : dstates) {
            if (d.first == key) return {true, d.second};
        }
        return {false, 0};
}

DFA DFA_sets::makeDFA(const AST& ast) {

    state s0 = firstpos.at(ast.root()->m_node_num-1);
    
    std::map<state, std::size_t> unmarked = {{s0, 0}};
    std::map<state, std::size_t> marked;

    trans_table Dtran;
    std::size_t cur_state = 0;
    finStates FinStates;
    std::cout << "Followpos:\n" << followpos << std::endl;
    
    while (!unmarked.empty()) {
        auto cur_state_entry = *(unmarked.begin());
        //if (cur_state_entry.first.size() > 10) break;
        marked.insert(cur_state_entry);
        unmarked.erase(cur_state_entry.first);
        
        std::cout << "T:" << cur_state_entry.first << std::endl;

        for (const auto &cur_char_leaves : ast.charMap()) {
            state U;
            std::cout << "Char leaves:" << cur_char_leaves;
            for (const auto &cur_leave : cur_state_entry.first) {
                if (cur_char_leaves.second.find(cur_leave) != cur_char_leaves.second.end()) {
                    U += followpos.at(cur_leave-1);
                }
            }
            std::cout << "U:" << U;

            if (!U.empty()) {

                auto U_entry = marked.find(U);

                if (U_entry == marked.end())
                    //  emplace
                    U_entry = unmarked.insert({U, ++cur_state}).first;

                if (cur_state_entry.first.find(ast.leafCount()) != cur_state_entry.first.end())
                    FinStates.insert(cur_state_entry.second);
                if (U.find(ast.leafCount()) != U.end())
                    FinStates.insert(cur_state);

                std::cout << "F:" << FinStates;
                
                Dtran.insert({{cur_state_entry.second, cur_char_leaves.first}, 
                    U_entry->second});
            }
        }
    }

    std::cout << std::endl << Dtran;

    return DFA(std::move(Dtran), std::move(FinStates));
}


void DFA_sets::calculate(ASTNode *root) {
    ASTNode *leftmost = get_leftmost(root);
    
    nullable_traversal(leftmost);
    firstpos_traversal(leftmost); 
    lastpos_traversal(leftmost); 
    followpos_traversal(leftmost); 
}

void DFA_sets::nullable_traversal (ASTNode *start) {

    if (!start) return; 
    if (is_leaf(start)) nullable.at(start->m_node_num-1) = false;
    else if (start->m_op == '*') nullable.at(start->m_node_num-1) = true;
    else if (start->m_op == '|') nullable.at(start->m_node_num-1) = 
        nullable.at(start->m_left->m_node_num-1) || nullable.at(start->m_right->m_node_num-1);
    else if (start->m_op == '_') nullable.at(start->m_node_num-1) = 
        nullable.at(start->m_left->m_node_num-1) && nullable.at(start->m_right->m_node_num-1);

    if (start->m_par && start == start->m_par->m_left) 
        nullable_traversal(get_leftmost(start->m_par->m_right));

    nullable_traversal(start->m_par);
}

void DFA_sets::firstpos_traversal (ASTNode *start) {

    if (!start) return; 

    if (is_leaf(start)) {
        firstpos.at(start->m_node_num-1).insert(start->m_leaf_num);
    }

    else if (start->m_op == '*') {
        ASTNode* child = get_nonnull_child(start);
        firstpos.at(start->m_node_num-1) = firstpos.at(child->m_node_num - 1);
    }

    else if (start->m_op == '|') {
        firstpos.at(start->m_node_num-1) = 
            firstpos.at(start->m_left->m_node_num-1) + firstpos.at(start->m_right->m_node_num-1);
    }

    else if (start->m_op == '_') {

        if (nullable.at(start->m_left->m_node_num-1))
            firstpos.at(start->m_node_num-1) = 
                firstpos.at(start->m_left->m_node_num-1) + firstpos.at(start->m_right->m_node_num-1);
        else 
            firstpos.at(start->m_node_num-1) = firstpos.at(start->m_left->m_node_num-1);
    }

    if (start->m_par && start == start->m_par->m_left) 
        firstpos_traversal(get_leftmost(start->m_par->m_right));

    firstpos_traversal(start->m_par);
}

void DFA_sets::lastpos_traversal (ASTNode *start) {

    if (!start) return; 

    if (is_leaf(start)) {
        lastpos.at(start->m_node_num-1).insert(start->m_leaf_num);
    }

    else if (start->m_op == '*') {
        ASTNode* child = get_nonnull_child(start);
        lastpos.at(start->m_node_num-1) = lastpos.at(child->m_node_num - 1);
    }

    else if (start->m_op == '|') {
        lastpos.at(start->m_node_num-1) = 
            lastpos.at(start->m_left->m_node_num-1) + lastpos.at(start->m_right->m_node_num-1);
    }

    else if (start->m_op == '_') {

        if (nullable.at(start->m_right->m_node_num-1))
            lastpos.at(start->m_node_num-1) = 
                lastpos.at(start->m_left->m_node_num-1) + lastpos.at(start->m_right->m_node_num-1);
        else 
            lastpos.at(start->m_node_num-1) = lastpos.at(start->m_right->m_node_num-1);
    }

    if (start->m_par && start == start->m_par->m_left) 
        lastpos_traversal(get_leftmost(start->m_par->m_right));

    lastpos_traversal(start->m_par);
}

void DFA_sets::followpos_traversal (ASTNode *start) {

    if (!start) return; 

    else if (start->m_op == '_') {
        for (auto &&entry : lastpos.at(start->m_left->m_node_num-1))
            followpos.at(entry-1) += firstpos.at(start->m_right->m_node_num-1);
    }

    else if (start->m_op == '*') {
        for (auto &&entry : lastpos.at(start->m_node_num-1))
            followpos.at(entry-1) += firstpos.at(start->m_node_num-1);
    }

    if (start->m_par && start == start->m_par->m_left) 
        followpos_traversal(get_leftmost(start->m_par->m_right));

    followpos_traversal(start->m_par);
}


std::ostream &operator<< (std::ostream &os, const DFA_sets &sets) {
    os << "Nullable:\n";
    os << sets.nullable << std::endl;

    os << "Firstpos:\n";
    os << sets.firstpos;

    os << "Lastpos:\n";
    os << sets.lastpos;

    os << "Followpos:\n";
    os << sets.followpos;

    return os;
}



void DFA::printDFA() {
    std::ofstream file("dfa_output.dot");
    if (!file) {
        std::cout << "Error creating file" << std::endl;
        return;
    }

    file << "digraph DFA {" << std::endl;
    file << "rankdir=\"LR\"" << std::endl;
    file << "node [shape=circle];" << std::endl;

    printDFA_Base(file);

    file << "}" << std::endl;

    file.close();

    system("dot -Tpng dfa_output.dot -o dfa_output.png");
}

void DFA::printDFA_Base(std::ofstream &file) {

    for (const auto &entry : m_Dtran) {
        file << entry.first.first << " -> " << entry.second;
        file << " [label=\"" << entry.first.second 
            << "\"]" << ";" << std::endl;
    }

    for (const auto &entry : m_FinStates) 
        file << entry << "[label=\"" << entry << "\"" 
        << "shape=doublecircle" << "]" << ";" << std::endl;

}
