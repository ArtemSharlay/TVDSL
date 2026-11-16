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
#include <fstream>

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


struct TokenType
{ 
    int row;
    int column;
    string token;
    string lexeme;
};

std::vector<TokenType> tokenize(const DFA& dfa, const std::string& input);

bool saveDFA(const DFA& dfa, const string& filename);

bool loadDFA(DFA& dfa, const string& filename);
