#pragma once 

#include <cstddef>
#include <string>
#include <cctype>
#include <stdexcept>
#include <utility>

struct Token
{
    std::string name;
    std::string regex;
};

struct LexerConfig
{
    std::string alphabet;
    std::vector<Token> tokens;
};