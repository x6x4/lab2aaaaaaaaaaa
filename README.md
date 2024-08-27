
Parser regularochek.

Makes AST from regex and then converts it to minDFA.

Operator precedence (high to low): * ?, concat, |

(a|bc)*abc

нерабочее:
a(b|c)(c|b)cc*
a(bc)|(bb)|(cb)|(cc) c*c

[algorithm](https://ebooks.inflibnet.ac.in/csp10/chapter/lexical-phase-re-to-dfa/)
