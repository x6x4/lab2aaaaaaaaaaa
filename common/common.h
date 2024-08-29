#pragma once
#include <cstddef>
#include <map>
#include <set>
#include <vector>
#include <ostream>
#include <algorithm>

#include "../regexTokenizer/regexTokenizer.h"

using LeafMap = std::map<Token, std::set<std::size_t>>;


struct ASTNode {
	ASTNode* m_left = nullptr;
	ASTNode* m_right = nullptr;
    ASTNode* m_par = nullptr;
	Token m_token;
    std::size_t m_leaf_num = 0;
    std::size_t m_node_num = 0;

    ASTNode(ASTNode* left, ASTNode* right, Token token, 
        std::size_t node_num, std::size_t leaf_num = 0) :
    m_left(left), m_right(right), m_token(token), m_node_num(node_num), m_leaf_num(leaf_num) {};
};

ASTNode *get_leftmost (ASTNode *_root);
bool is_leaf(ASTNode *node);
ASTNode *get_nonnull_child (ASTNode *par);





//  USEFUL FUNCTIONS

template<typename T> 
std::set<T> operator+ 
(std::set<T> s1, std::set<T> s2) {
    s1.insert(s2.begin(), s2.end());
    return s1;
}

template<typename T> 
void operator+= 
(std::set<T> &s1, std::set<T> &s2) {
    s1 = s1+s2;
}

template<typename T> 
std::ostream &operator<<
(std::ostream &os, const std::set<T> &set) {
    os << ' ';
    for (const T &e : set) {
        os << e << ' ';
    }
    if (!set.size()) os << "NULL";
    return os << '\n';
}

template<typename T> 
std::ostream &operator<<
(std::ostream &os, const std::vector<T> &vec) {
    os << ' ';
    for (const T &e : vec) {
        os << e << ' ';
    }
    if (!vec.size()) os << "NULL";
    return os << '\n';
}


template<typename T1, typename T2>
std::ostream &operator<<
(std::ostream &os, const std::pair<T1, T2> &para) {
    return os << para.first << ' ' <<para.second;
}

template<typename T1, typename T2> 
std::ostream &operator<<
(std::ostream &os, const std::map<T1, T2> &mapa) {
    os << ' ';
    for (const auto &e : mapa) {
        os << e.first << ' ' << e.second << '\n';
    }
    return os << '\n';
}

