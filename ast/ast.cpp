#include "ast.h"
#include <cstddef>
#include <fstream>
#include <stdexcept>
#include <string>

#include "../regexTokenizer/regexTokenizer.h"


void AST::insertInMap(char oper) {
    RegexHelper rhelper;
    ++m_nodeCount;

    if (rhelper.non_special(oper)) {
        char_map[oper].insert(++m_leafCount);
    }
}

//  GRAMMAR - make AST

ASTNode* AST::insert(char oper, ASTNode* left, ASTNode* right) {

    ASTNode* newNode = new ASTNode(left, right, oper, m_nodeCount++);

    if (left) left->m_par = newNode;
    if (right) right->m_par = newNode;

    insertInMap(oper);

    return newNode;
}

//  return root
std::pair<ASTNode*, tokenString&> AST::parse_regex(tokenString &global_tokstream) {
	auto [left, stream] = parse_alt(global_tokstream);
	// Check
	if (stream.curToken().kind() == Token::Kind::Alter) {
		stream.movePtr();
		auto [right, s2] = parse_regex(stream);

		return {insert('|', left, right), s2};
	} else return {left, stream};
}

std::pair<ASTNode*, tokenString&> AST::parse_alt(tokenString &global_tokstream) {
    auto [left, changed_stream] = parse_concat(global_tokstream);
    
    while (changed_stream.curToken().kind() == Token::Kind::Cat) {
        changed_stream.movePtr();
        auto [right, changed_stream2] = parse_concat(changed_stream);
        left = insert('_', left, right);
        changed_stream = changed_stream2;
    }

    return {left, changed_stream};
}

std::pair<ASTNode*, tokenString&> AST::parse_concat(tokenString &global_tokstream) {
    auto [left, changed_stream] = parse_char(global_tokstream);
    if (changed_stream.curToken().isUnary()) {
        char op = changed_stream.curToken().getChar();
        changed_stream.movePtr();
        return {insert(op, left), changed_stream};
    } else return {left, changed_stream};
}


std::pair<ASTNode*, tokenString&> AST::parse_char(tokenString &global_tokstream) {
    RegexHelper rhelper;

    Token tok = global_tokstream.curToken();
    
    if (tok.kind() == Token::Kind::Char) {
        global_tokstream.movePtr(); 
        return {insert(tok.getChar()), global_tokstream};
    } 
    else if (tok.kind() == Token::Kind::PriorStart) {
        global_tokstream.movePtr(); 
        auto [expr, internal_stream] = parse_regex(global_tokstream);
        if (internal_stream.curToken().kind() != Token::Kind::PriorFin) {
            throw std::runtime_error("Bad priority\n");
        } else {
            internal_stream.movePtr();
            global_tokstream.setPtr(internal_stream.getPtr()); 
            return {expr, global_tokstream};
        }
    } else {
        throw std::runtime_error("Fatal error at lowest parsing level\n");
    }
}


//  DEBUG OUTPUT

void AST::printLeafMap() {
    std::cout << char_map;
}

//  GRAPHVIZ

void printBT_Base(ASTNode *node, std::ofstream &file);

void AST::printAST() {

    std::ofstream file("tree_output/tree_output.dot");
    if (!file) {
        std::cout << "Error creating file" << std::endl;
        return;
    }

    file << "digraph Tree {" << std::endl;
    file << "node [shape=circle];" << std::endl;

    printBT_Base(m_root, file);

    file << "}" << std::endl;

    file.close();

    system("dot -Tpng tree_output/tree_output.dot -o tree_output/tree_output.png");
}

void printBT_Base(ASTNode *node, std::ofstream &file) {
    if (!node) { return;}

    file << reinterpret_cast<uintptr_t>(node) << " [label=\"";
    if (node->m_leaf_num) file << node->m_leaf_num << ' ';
    file << node->m_op << "\"];" << std::endl;

    if (node->m_left != nullptr) {
        file << reinterpret_cast<uintptr_t>(node) << " -> " << reinterpret_cast<uintptr_t>(node->m_left) << ";" << std::endl;
        printBT_Base(node->m_left, file);
    }

    if (node->m_right != nullptr) {
        file << reinterpret_cast<uintptr_t>(node) << " -> " << reinterpret_cast<uintptr_t>(node->m_right) << ";" << std::endl;
        printBT_Base(node->m_right, file);
    }
}
