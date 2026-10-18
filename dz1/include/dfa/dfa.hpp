#pragma once

#include "regex/RegexAST.hpp"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <map>
#include <optional>
#include <queue>
#include <set>
#include <string>
#include <vector>

#include "nfa/nfa.hpp"
#include "json/tokenStruct.hpp"

using namespace std;

struct DFA
{
    int start = 0;
    int states = 0;
    int trap = -1;

    map<int, map<char, int>> transition;

    set<int> accepting;

    std::map<int, std::string> tokenType;

    int next(int state, char c) const
    {

        auto it = transition.find(state);

        if (it == transition.end())
            return -1;

        auto jt = it->second.find(c);

        if (jt == it->second.end())
            return -1;

        return jt->second;
    }
};

DFA determinize(const NFA& nfa, const std::set<int>& nfaAccepting);

int getOrCreateState(const std::set<int>& states,
                     std::map<std::set<int>, int>& ids,
                     DFA& dfa,
                     std::queue<std::set<int>>& q,
                     const std::set<int>& nfaAccepting,
                     const std::map<int, std::string>& nfaTokenType);

set<int> epsilonClosure(const NFA& nfa, const set<int>& states);

set<int> move(const NFA& nfa, const set<int>& states, char c);

DFA minimizeHopcroft(const DFA& dfa);

std::vector<std::string> tokenize(const DFA& dfa, const std::string& input);
