#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "json/jsonParser.hpp"

std::string toUpper(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return std::toupper(c);
    });
    return s;
}

void testEmptyString(const DFA& dfa)
{
    std::vector<std::string> expectation = { "ERROR" };
    assert(tokenize(dfa, "") == expectation);
}

void testWhitespace(const DFA& dfa)
{
    std::vector<std::string> expectation = { "WS" };
    assert(tokenize(dfa, " ") == expectation);
    assert(tokenize(dfa, "     ") == expectation);
    assert(tokenize(dfa, "\t") == expectation);
    assert(tokenize(dfa, "\t \t") == expectation);
    assert(tokenize(dfa, "\r\n") == expectation);
    assert(tokenize(dfa, " \t\r\n  ") == expectation);
}

void testIntegerLiterals(const DFA& dfa)
{
    std::vector<std::string> expectation1 = { "INT" };
    std::vector<std::string> expectation2 = { "INT","INT" };
    assert(tokenize(dfa, "0") == expectation1);
    assert(tokenize(dfa, "64") == expectation1);
    std::vector<std::string> res = tokenize(dfa, "00"); 
    for (auto it : res) {
        std::cout << it << '\n';
    }
    assert(tokenize(dfa, "00") == expectation2);
    assert(tokenize(dfa, "01") == expectation2);
}

void testIdentifiersWithUnderscore(const DFA& dfa)
{
    std::vector<std::string> expectation = { "IDENT" };
    assert(tokenize(dfa, "_name") == expectation);
    assert(tokenize(dfa, "my_name") == expectation);
    assert(tokenize(dfa, "name_123") == expectation);
}

void testKeywords(const DFA& dfa)
{
    const std::vector<std::string> keywords = {
        "function", "returns", "while",     "if",    "else",
        "assert",   "assume",  "invariant", "length"
    };

    for (const auto& keyword : keywords) {
        std::vector<std::string> expectation;
        expectation.push_back(toUpper(keyword));
        assert(tokenize(dfa, keyword) == expectation);
        expectation.pop_back();
    }
}

void testDelimiters(const DFA& dfa)
{

    std::vector<std::string> expectation1 = { "LPAREN" };
    std::vector<std::string> expectation2 = { "RPAREN" };
    std::vector<std::string> expectation3 = { "LBRACKET" };
    std::vector<std::string> expectation4 = { "RBRACKET" };
    std::vector<std::string> expectation5 = { "LBRACE" };
    std::vector<std::string> expectation6 = { "RBRACE" };
    assert(tokenize(dfa, "(") == expectation1);
    assert(tokenize(dfa, ")") == expectation2);
    assert(tokenize(dfa, "[") == expectation3);
    assert(tokenize(dfa, "]") == expectation4);
    assert(tokenize(dfa, "{") == expectation5);
    assert(tokenize(dfa, "}") == expectation6);
}

void testOperators(const DFA& dfa)
{
    std::vector<std::string> expectation1 = { "PLUS" };
    std::vector<std::string> expectation2 = { "MINUS" };
    std::vector<std::string> expectation3 = { "MUL" };
    std::vector<std::string> expectation4 = { "SUB" };
    std::vector<std::string> expectation5 = { "EQ" };
    std::vector<std::string> expectation6 = { "NEQ" };
    std::vector<std::string> expectation7 = { "LE" };
    std::vector<std::string> expectation8 = { "GE" };
    std::vector<std::string> expectation9 = { "LT" };
    std::vector<std::string> expectation10 = { "GT" };
    std::vector<std::string> expectation11 = { "ASSIGN" };
    assert(tokenize(dfa, "+") == expectation1);
    assert(tokenize(dfa, "-") == expectation2);
    assert(tokenize(dfa, "*") == expectation3);
    assert(tokenize(dfa, "/") == expectation4);
    assert(tokenize(dfa, "==") == expectation5);
    assert(tokenize(dfa, "!=") == expectation6);
    assert(tokenize(dfa, "<=") == expectation7);
    assert(tokenize(dfa, ">=") == expectation8);
    assert(tokenize(dfa, "<") == expectation9);
    assert(tokenize(dfa, ">") == expectation10);
    assert(tokenize(dfa, "=") == expectation11);
}

void testComments(const DFA& dfa)
{
    std::vector<std::string> expectation = { "COMMENT" };
    assert(tokenize(dfa, "//comment") == expectation);
}

void testNonAsciiTrap(const DFA& dfa)
{
    const std::string input = "\xC3\xA9";
    std::vector<std::string> expectation = { "ERROR","ERROR" };
    assert(tokenize(dfa, input) == expectation);
}

void runTests(const DFA& dfa)
{
    testEmptyString(dfa);
    testWhitespace(dfa);
    testIntegerLiterals(dfa);
    testIdentifiersWithUnderscore(dfa);
    testKeywords(dfa);
    testDelimiters(dfa);
    testOperators(dfa);
    testComments(dfa);
    testNonAsciiTrap(dfa);

    std::cout << "All required lexer tests passed\n";
}

int main()
{
    LexerConfig config;

    if (!exportJSONtoLexerConfig(config)) {
        std::cout << "ERROR";
        return 0;
    }

    NFA nfa;
    createGeneralNFA(nfa, config.tokens);

    DFA dfa = determinize(nfa, nfa.accept);
    DFA dfamin = minimizeHopcroft(dfa);
    std::cout << dfa.states << "----" <<dfamin.states << '\n';

    runTests(dfamin);
    std::vector res =
        tokenize(dfamin, "//comment\r1\n2\n3 function returns if ifx");
    for (auto it : res) {
        std::cout << it << '\n';
    }

    return 0;
}
