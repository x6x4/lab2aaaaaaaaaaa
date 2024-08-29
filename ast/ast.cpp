#include "ast.h"
#include <cstddef>
#include <fstream>
#include <stdexcept>
#include <string>

#include "../regexTokenizer/regexTokenizer.h"


void AST::insertInMap(Token token) {

    if (token.isLeaf()) {
        if (leaf_map.find(token) == leaf_map.end())
            leaf_map.insert({token, {++m_leafCount}});
        else
            leaf_map[token].insert(++m_leafCount);
    }
}

//  GRAMMAR - make AST

ASTNode* AST::insert(Token token, ASTNode* left, ASTNode* right) {

    ASTNode* newNode;
    if (token.isLeaf()) 
        newNode = new ASTNode(left, right, token, ++m_nodeCount, m_leafCount + 1);
    else
        newNode = new ASTNode(left, right, token, ++m_nodeCount);

    if (left) left->m_par = newNode;
    if (right) right->m_par = newNode;

    insertInMap(token);

    return newNode;
}

//  return root
ASTNode* AST::parse_regex(tokenString &global_tokstream) {

	auto left = parse_alt(global_tokstream);

	if (!global_tokstream.eof() && global_tokstream.curToken().kind() == Token::Kind::Alter) {
		global_tokstream.movePtr();
		auto right = parse_regex(global_tokstream);

		return insert(Token::Kind::Alter, left, right);
	} else return left;
}

ASTNode* AST::parse_alt(tokenString &global_tokstream) {

    auto left = parse_unary(global_tokstream);
    
    while (!global_tokstream.eof() && global_tokstream.curToken().kind() == Token::Kind::Cat) {
        global_tokstream.movePtr();
        auto right = parse_unary(global_tokstream);
        left = insert(Token::Kind::Cat, left, right);
    }

    return left;
}

ASTNode* AST::parse_unary(tokenString &global_tokstream) {

    auto left = parse_subgroup(global_tokstream);

    if (!global_tokstream.eof() && global_tokstream.curToken().isUnary()) {
        auto tok = global_tokstream.curToken();
        global_tokstream.movePtr();
        if (tok.kind() == Token::Kind::Kline)
            return insert(tok, left);
        else if (tok.kind() == Token::Kind::ZeroOrOne)
            return insert(Token::Kind::Alter, left, insert(Token::Kind::Eps));
        else
            throw std::runtime_error("Bad unary operator");
    } else return left;
}


ASTNode* AST::parse_subgroup(tokenString &global_tokstream) {
    RegexHelper rhelper;

    Token tok = global_tokstream.curToken();
    
    if (tok.kind() == Token::Kind::Char) {
        global_tokstream.movePtr(); 
        return insert(tok.getChar());
    } 
    if (tok.kind() == Token::Kind::CaptStart 
        || tok.kind() == Token::Kind::CaptFin 
            ||tok.kind() == Token::Kind::CaptStr) {
        global_tokstream.movePtr();
        return insert(tok);
    }
    else if (tok.kind() == Token::Kind::PriorStart) {
        global_tokstream.movePtr(); 
        auto expr = parse_regex(global_tokstream);

        if (global_tokstream.eof() || global_tokstream.curToken().kind() != Token::Kind::PriorFin) {
            throw std::runtime_error("Bad priority\n");
        } else {
            global_tokstream.movePtr();
            return expr;
        }
    } 
    else {
        throw std::runtime_error("Fatal error at lowest parsing level\n");
    }
}


//  DEBUG OUTPUT

void AST::printLeafMap() {
    std::cout << leaf_map;
}

//  GRAPHVIZ

void printBT_Base(ASTNode *node, std::ofstream &file);

void AST::printAST() {

    std::ofstream file("/home/cracky/lab2aaaaaaaaaaa/tree_output/tree_output.dot");
    if (!file) {
        std::cout << "Error creating file" << std::endl;
        return;
    }

    file << "digraph Tree {" << std::endl;
    file << "node [shape=circle];" << std::endl;

    printBT_Base(m_root, file);

    file << "}" << std::endl;

    file.close();

    system("dot -Tpng /home/cracky/lab2aaaaaaaaaaa/tree_output/tree_output.dot -o /home/cracky/lab2aaaaaaaaaaa/tree_output/tree_output.png");
}

void printBT_Base(ASTNode *node, std::ofstream &file) {
    if (!node) { return;}

    file << reinterpret_cast<uintptr_t>(node) << " [label=\"";
    if (node->m_leaf_num) file << node->m_leaf_num << ' ';
    file << node->m_token << "\"];" << std::endl;

    if (node->m_left != nullptr) {
        file << reinterpret_cast<uintptr_t>(node) << " -> " << reinterpret_cast<uintptr_t>(node->m_left) << ";" << std::endl;
        printBT_Base(node->m_left, file);
    }

    if (node->m_right != nullptr) {
        file << reinterpret_cast<uintptr_t>(node) << " -> " << reinterpret_cast<uintptr_t>(node->m_right) << ";" << std::endl;
        printBT_Base(node->m_right, file);
    }
}
