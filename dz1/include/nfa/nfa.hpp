#pragma once

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <map>
#include <optional>
#include <queue>
#include <set>
#include <string>
#include <vector>

#include "regex/RegexAST.hpp"
#include "nfa/nfa.hpp"
#include "json/tokenStruct.hpp"

struct NFATransition
{
    int from;
    int to;
    std::optional<char> symbol; // nullopt = epsilon
};

struct AcceptInfo
{
    std::string tokenName;
    int priority;
};

struct NFA
{
    int start;
    std::set<int> accept;
    std::vector<NFATransition> transitions;
    std::map<int, std::string> nfaTokenType;
    int states = 0;

    int newState();

    void addSymbol(int from, int to, char c);
    void addEpsilon(int from, int to);
};

struct Fragment
{
    int start;
    int end;
};

Fragment buildNFA(const Regex& r, NFA& nfa);

void createGeneralNFA(NFA& nfa, std::vector<Token> tokens);
