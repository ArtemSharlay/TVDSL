#pragma once

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <string>
#include <optional>
#include <algorithm>

struct Regex {
    enum class Type {
        SYMBOL,
        CHAR_CLASS,
        CONCAT,
        UNION,
        STAR,
        PLUS,
    };

    Type type;

    char symbol = 0;

    std::set<char> charClass;

    std::vector<Regex> children;

    static Regex symbolNode(char c);
    static Regex charClassNode(const std::set<char>& chars);

    static Regex concat(Regex left, Regex right);
    static Regex unite(Regex left, Regex right);

    static Regex star(Regex child);
    static Regex plus(Regex child);

};