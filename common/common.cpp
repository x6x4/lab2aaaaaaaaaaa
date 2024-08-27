#include "common.h"


ASTNode *get_leftmost (ASTNode *_root) {
    ASTNode *node = _root;
    ASTNode *prev = node;
    
    while (node) 
        {prev = node; node = node->m_left;}

    return prev;
}

bool is_leaf(ASTNode *node) { return !(node->m_left) && !(node->m_right); }

ASTNode *get_nonnull_child (ASTNode *par) {
    if (par->m_left) return par->m_left;
    if (par->m_right) return par->m_right;
    return nullptr;
}