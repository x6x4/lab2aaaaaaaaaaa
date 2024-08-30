#pragma once
#include "../common/common.h"
#include "../regexTokenizer/regexTokenizer.h"
#include <map>
#include <vector>
#include <string>
#include <iostream>

typedef std::map<std::string, ASTNode*> captTable;

class AST {
    captTable table;

    std::size_t m_nodeCount = 0;
    std::size_t m_leafCount = 0;
    LeafMap leaf_map;
    void insertInMap(Token token);

    ASTNode *m_root = nullptr;

public:

    AST(tokenString &regex_ptr) {
        m_root = parse_regex(regex_ptr);
        
        ASTNode *augment = insert(Token::Kind::Augment);
        m_root = insert(Token::Kind::Cat, m_root, augment);
    }

    auto root() const {return m_root; }
    auto nodeCount() const {return m_nodeCount; }
    auto leafCount() const {return m_leafCount; }
    const LeafMap &leafMap() const { return leaf_map; }

    ASTNode* insert(Token token, ASTNode* left = nullptr, ASTNode* right = nullptr);

    void printAST();
    void printLeafMap();

    void clear(ASTNode** node) {
        static int n = 0;
        if (!(*node)) return;

        if ((*node)->m_left == nullptr && (*node)->m_right == nullptr) {
            delete *node;
            n++;
            *node = nullptr;
            return;
        }

        clear(&(*node)->m_left);
        clear(&(*node)->m_right);
        delete *node;
        *node = nullptr;
    }

    ~AST() {
        clear(&m_root);
    }

private:

    ASTNode* parse_regex(tokenString &global_tokstream);
    ASTNode* parse_alt(tokenString &global_tokstream);
    ASTNode* parse_unary(tokenString &global_tokstream);
    ASTNode* parse_subgroup(tokenString &global_tokstream);
};

