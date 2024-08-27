#pragma once
#include "../common/common.h"
#include "../regexTokenizer/regexTokenizer.h"
#include <map>
#include <vector>
#include <string>
#include <iostream>


class AST {
    std::size_t m_nodeCount = 0;
    std::size_t m_leafCount = 0;
    charNodeMap char_map;
    void insertInMap(char oper);

    ASTNode *m_root = nullptr;

public:

    AST(tokenString &regex_ptr) {
        m_root = parse_regex(regex_ptr).first;
        
        ASTNode *augment = insert('#');
        m_root = insert('_', m_root, augment);
    }

    auto root() const {return m_root; }
    auto nodeCount() const {return m_nodeCount; }
    auto leafCount() const {return m_leafCount; }
    const charNodeMap &charMap() const { return char_map; }

    ASTNode* insert(char oper, ASTNode* left = nullptr, ASTNode* right = nullptr);

    void printAST();
    void printLeafMap();

private:

    std::pair<ASTNode*, tokenString &> parse_regex(tokenString &global_tokstream);
    std::pair<ASTNode*, tokenString &> parse_alt(tokenString &global_tokstream);
    std::pair<ASTNode*, tokenString &> parse_concat(tokenString &global_tokstream);
    std::pair<ASTNode*, tokenString &> parse_char(tokenString &global_tokstream);
};

