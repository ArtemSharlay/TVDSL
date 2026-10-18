#include "regex/regexAST.hpp"

Regex Regex::symbolNode(char c)
{
    Regex r;
    r.type = Type::SYMBOL;
    r.symbol = c;
    return r;
}

Regex Regex::charClassNode(const std::set<char>& chars)
{
    Regex r;
    r.type = Type::CHAR_CLASS;
    r.charClass = chars;
    return r;
}

Regex Regex::concat(Regex left, Regex right)
{
    Regex r;
    r.type = Type::CONCAT;
    r.children.push_back(std::move(left));
    r.children.push_back(std::move(right));
    return r;
}

Regex Regex::unite(Regex left, Regex right)
{
    Regex r;
    r.type = Type::UNION;
    r.children.push_back(std::move(left));
    r.children.push_back(std::move(right));
    return r;
}

Regex Regex::star(Regex child)
{
    Regex r;
    r.type = Type::STAR;
    r.children.push_back(std::move(child));
    return r;
}

Regex Regex::plus(Regex child)
{
    Regex r;
    r.type = Type::PLUS;
    r.children.push_back(std::move(child));
    return r;
}
