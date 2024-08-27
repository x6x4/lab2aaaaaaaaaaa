#pragma once

#include <cctype>
#include <stdexcept>
#include <string>
#include <vector>


class RegexHelper {

    std::string unary = "*?";
    std::string binary = "|_";
    std::string prior = "()";
    std::string repeat = "{}";
    std::string capture = "<>";

    std::string open_paren = "(<";
    std::string close_paren = ")>}";

    bool is_unary (char ch) {return unary.find(ch) != std::string::npos;}
    bool is_binary (char ch) {return binary.find(ch) != std::string::npos;}
    bool is_paren (char ch) {return prior.find(ch) != std::string::npos;};
    bool is_repeat (char ch) {return repeat.find(ch) != std::string::npos;};
    bool is_capture (char ch) {return capture.find(ch) != std::string::npos;};


    bool is_open_paren(char ch) {
        return open_paren.find(ch) != std::string::npos;
    }
    bool is_close_paren(char ch) {
        return close_paren.find(ch) != std::string::npos;
    }


public:

    bool non_special(char ch) {
        return !is_unary(ch) && !is_binary(ch) && !is_paren(ch) 
            && !is_repeat(ch) && !is_capture(ch) && isprint(ch);
    }


    //  r1_r2
    //  r1?_r2  
    //  r1{n}_r2
    //  r1_(r2)
    //  (r1)<r2>

    bool can_paste_concat(char cur, char next) {
        return (non_special(cur) && non_special(next)) 
        || (is_unary(cur) && non_special(next))
        || (is_close_paren(cur) && non_special(next)) 
        || (non_special(cur) && is_open_paren(next))
        || (is_close_paren(cur) && is_open_paren(next));
    }

};

class Token {
public:
    enum class Kind {
        Char, 
        Alter, 
        Cat, 
        Kline, 
        CaptStart, 
        CaptFin, 
        Augment, 
        Eps,
        ZeroOrOne, 
        MetaName, 
        Repeat, 
        PriorStart, 
        PriorFin
    };

    Token(Kind kind) : m_kind(kind) {}
    Token(char ch) : m_kind(Kind::Char), m_ch(ch) {}
    Token(std::string str) : m_kind(Kind::MetaName), m_metaname(str) {}
    Token(int n) : m_kind(Kind::Repeat), repeats(n) {}

    Kind kind() const {return m_kind;}
    char getChar() const {if (m_kind == Token::Kind::Char) return m_ch; else
     throw std::runtime_error("not char");}
    bool isUnary() const {return (m_kind == Token::Kind::Kline) 
                                        || (m_kind == Token::Kind::ZeroOrOne);}

private:
    Kind m_kind;
    char m_ch;
    std::string m_metaname;
    size_t repeats;
};


class tokenString {
    std::vector<Token> data;
    int idx = 0;

public:
    auto curToken() {
        return data.at(idx);
    }
    void movePtr() {
        idx++;
    }
    void push_back(Token tok) {
        data.push_back(tok);
    }
    void setPtr(int new_idx) {
        if (new_idx < data.size())
            idx = new_idx;
        else
            throw std::runtime_error("Bad token stream index");
    }
    int getPtr() const {
        return idx;
    }
};

class RgxTokenizer {

public: 
    tokenString tokenize(const std::string &my_regex);
};