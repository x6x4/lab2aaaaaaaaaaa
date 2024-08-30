#include "dfa_sets.h"
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

using Dstates = std::map<positionSet, std::size_t>;

std::pair<bool, std::size_t> dstates_find 
    (const Dstates& dstates, const positionSet& key) {
        for (const auto &d : dstates) {
            if (d.first == key) return {true, d.second};
        }
        return {false, 0};
}

DFA DFA_sets::makeDFA(const AST& ast) {

    positionSet s0 = firstpos.at(ast.root()->m_node_num-1);
    
    std::map<positionSet, std::size_t> unmarked = {{s0, 0}};
    std::map<positionSet, std::size_t> marked;

    trans_table Dtran;
    DFAState cur_state = 0;
    finStates FinStates;
    //std::cout << "Followpos:\n" << followpos << std::endl;
    
    while (!unmarked.empty()) {
        auto cur_state_entry = *(unmarked.begin());
        //if (cur_state_entry.first.size() > 10) break;
        marked.emplace(cur_state_entry);
        unmarked.erase(cur_state_entry.first);
        
        //std::cout << "T:" << cur_state_entry.first << std::endl;

        for (const auto &cur_leaves : ast.leafMap()) {
            positionSet U;
            //std::cout << "Char leaves:" << cur_leaves;
            for (const auto &cur_leave : cur_state_entry.first) {
                if (cur_leaves.second.find(cur_leave) != cur_leaves.second.end()) {
                    U += followpos.at(cur_leave-1);
                }
            }
            //std::cout << "U:" << U;

            if (!U.empty()) {

                auto U_entry = marked.find(U);

                if (U_entry == marked.end())
                    //  emplace
                    U_entry = unmarked.emplace(std::pair{U, ++cur_state}).first;

                if (cur_state_entry.first.find(ast.leafCount()) != cur_state_entry.first.end())
                    FinStates.emplace(cur_state_entry.second);
                if (U.find(ast.leafCount()) != U.end())
                    FinStates.emplace(cur_state);

                //std::cout << "F:" << FinStates;
                
                Dtran.emplace(std::pair{std::pair{cur_state_entry.second, cur_leaves.first}, 
                    U_entry->second});
            }
        }
    }

    //std::cout << "F:" << FinStates;

    //std::cout << "Cur: " << cur_state << std::endl;

    //std::cout << std::endl << Dtran;

    return DFA(std::move(Dtran), std::move(FinStates), cur_state);
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
    if (is_leaf(start)) nullable.at(start->m_node_num-1) = (start->m_token.kind() == Token::Kind::Eps);
    else if (start->m_token.kind() == Token::Kind::Kline) nullable.at(start->m_node_num-1) = true;
    else if (start->m_token.kind() == Token::Kind::Alter) nullable.at(start->m_node_num-1) = 
        nullable.at(start->m_left->m_node_num-1) || nullable.at(start->m_right->m_node_num-1);
    else if (start->m_token.kind() == Token::Kind::Cat) nullable.at(start->m_node_num-1) = 
        nullable.at(start->m_left->m_node_num-1) && nullable.at(start->m_right->m_node_num-1);

    if (start->m_par && start == start->m_par->m_left) 
        nullable_traversal(get_leftmost(start->m_par->m_right));

    nullable_traversal(start->m_par);
}

void DFA_sets::firstpos_traversal (ASTNode *start) {

    if (!start) return; 

    if (is_leaf(start)) {
        if (start->m_token.kind() == Token::Kind::Eps) {
            firstpos.at(start->m_node_num-1).clear();
        } else {
            firstpos.at(start->m_node_num-1).emplace(start->m_leaf_num);
        }
    }

    else if (start->m_token.kind() == Token::Kind::Kline) {
        ASTNode* child = get_nonnull_child(start);
        firstpos.at(start->m_node_num-1) = firstpos.at(child->m_node_num - 1);
    }

    else if (start->m_token.kind() == Token::Kind::Alter) {
        firstpos.at(start->m_node_num-1) = 
            firstpos.at(start->m_left->m_node_num-1) + firstpos.at(start->m_right->m_node_num-1);
    }

    else if (start->m_token.kind() == Token::Kind::Cat) {

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
        if (start->m_token.kind() == Token::Kind::Eps) {
            lastpos.at(start->m_node_num-1).clear();
        } else {
            lastpos.at(start->m_node_num-1).emplace(start->m_leaf_num);
        }
    }

    else if (start->m_token.kind() == Token::Kind::Kline) {
        ASTNode* child = get_nonnull_child(start);
        lastpos.at(start->m_node_num-1) = lastpos.at(child->m_node_num - 1);
    }

    else if (start->m_token.kind() == Token::Kind::Alter) {
        lastpos.at(start->m_node_num-1) = 
            lastpos.at(start->m_left->m_node_num-1) + lastpos.at(start->m_right->m_node_num-1);
    }

    else if (start->m_token.kind() == Token::Kind::Cat) {

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

    else if (start->m_token.kind() == Token::Kind::Cat) {
        for (auto &&entry : lastpos.at(start->m_left->m_node_num-1))
            followpos.at(entry-1) += firstpos.at(start->m_right->m_node_num-1);
    }

    else if (start->m_token.kind() == Token::Kind::Kline) {
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
    std::ofstream file("/home/cracky/lab2aaaaaaaaaaa/dfa_output.dot");
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

    system("dot -Tpng /home/cracky/lab2aaaaaaaaaaa/dfa_output.dot -o /home/cracky/lab2aaaaaaaaaaa/dfa_output.png");
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

#include <array>

DFA DFA::inverse() const {
    // Create a new DFA with the same states, but reversed accept and non-accept states.
    trans_table new_Dtran = m_Dtran;
    finStates new_FinStates;
    for (DFAState state = 0; state < size() + 1; state++) {
        if (m_FinStates.count(state) == 0) {
            new_FinStates.insert(state);
        } 
    }

    // Create the dead state
    DFAState deadState = -1; // New state index
    new_FinStates.insert(deadState);

    //std::cout << "F:" << new_FinStates;
    
    //new_Dtran[std::make_pair(deadState, Token{})] = deadState; // Transition to self
    for (DFAState state = 0; state < size() + 1; state++) {
        for (int i = 0; i < 256; i++) {
            char ch = i;
            if (new_Dtran.find({state, ch}) == new_Dtran.end() || new_FinStates.count(state) == 0) {
                if (isprint(ch) && (ch != '"') && (ch != '\\')) {
                    new_Dtran.insert({{state, Token(char(i))}, deadState});
                    new_Dtran.insert({{deadState, Token(char(i))}, deadState});
                }
            }
        }
    }

    //std::cout << std::endl << new_Dtran;

    return DFA(new_Dtran, new_FinStates, size() + 1);
}


DFA DFA::operator*(const DFA& rhs) const {
    // Create a new DFA with a new transition table and final states
    trans_table new_Dtran;
    finStates new_FinStates;

    // Combine the final states of both DFAs
    for (auto const& state : m_FinStates) {
        new_FinStates.insert(state);
    }
    for (auto const& state : rhs.m_FinStates) {
        new_FinStates.insert(state);
    }

    std::size_t new_second_state;

// Iterate through all possible combinations of states from both DFAs
for (const auto& trans1_token1 : m_Dtran) {
    for (const auto& trans2_token2 : rhs.m_Dtran) {
        // Check if transitions for the same token exist in both DFAs
        if (trans1_token1.first.second.getChar() == trans2_token2.first.second.getChar()) {
            // Multiply the states and create a new transition
            std::size_t new_first_state = trans1_token1.first.first * 100 + trans2_token2.first.first;
            std::size_t new_second_state = trans1_token1.second * 100 + trans2_token2.second;
            if (m_FinStates.count(trans1_token1.second) && rhs.m_FinStates.count(trans2_token2.second))
                new_FinStates.insert(new_second_state);
            new_Dtran.insert({{new_first_state, trans1_token1.first.second}, new_second_state});
        }
    }
}


    // Return the new DFA
    return DFA(new_Dtran, new_FinStates, new_second_state+1);
}

/*
DFA DFA::operator*(const DFA& rhs)  {
  size_t nstatesL = std::max(m_Dtran.rbegin()->first.first, *m_FinStates.rbegin());

  auto getI = [=](size_t i, size_t j) -> size_t { return nstatesL * j + i; };

  trans_table ntt;
  for(const auto& it1 : m_Dtran) {
    for(const auto& it2 : rhs.m_Dtran) {
      if(it1.first.second == it2.first.second) {
        ntt[{getI(it1.first.first, it2.first.first), it1.first.second}] = getI(it1.second, it2.second);
      }
    }
  }

  Dstates nfs;
  for(const auto& it1 : m_FinStates) {
    for(const auto& it2 : rhs.m_FinStates) {
      nfs.insert(getI(it1, it2));
    }
  }
  return DFA(std::move(ntt), std::move(nfs));
}*/