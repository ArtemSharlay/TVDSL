#pragma once

#include "regex/RegexAST.hpp"

#include <cstddef>
#include <string>
#include <cctype>
#include <stdexcept>
#include <utility>

class RegexParser
{
public:
    explicit RegexParser(const std::string& input);

    Regex parse();

private:
    const std::string& input;
    std::size_t pos = 0;

    Regex parseExpression();

    Regex parseTerm();

    Regex parseFactor();

    Regex parseAtom();

    Regex parseCharClass();

    char parseEscape();

    bool atEnd() const;

    char peek() const;
    char get();

    bool match(char c);

    bool isAtomStart(char c) const;

    void error(const std::string& message) const;
};