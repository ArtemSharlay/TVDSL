#include "dfa/dfa.hpp"
#include "regex/regexParser.hpp"
#include "json/tokenStruct.hpp"
#include <iostream>
#include <nfa/nfa.hpp>
#include <nlohmann/json.hpp>
#include <string>

DFA determinize(const NFA& nfa, const std::set<int>& nfaAccepting)
{
    DFA dfa;

    std::map<std::set<int>, int> ids;
    std::queue<std::set<int>> q;

    std::set<int> startSet = epsilonClosure(nfa, std::set<int> { nfa.start });

    dfa.states = 0;

    dfa.start =
        getOrCreateState(startSet, ids, dfa, q, nfaAccepting, nfa.nfaTokenType);

    while (!q.empty()) {
        std::set<int> currentSet = q.front();
        q.pop();

        int currentId = ids.at(currentSet);

        for (int ascii = 0; ascii < 128; ++ascii) {
            char c = static_cast<char>(ascii);

            std::set<int> reached = move(nfa, currentSet, c);

            std::set<int> nextSet = epsilonClosure(nfa, reached);

            int nextId = getOrCreateState(
                nextSet, ids, dfa, q, nfaAccepting, nfa.nfaTokenType);

            dfa.transition[currentId][c] = nextId;
        }
    }

    auto trapIt = ids.find(std::set<int> { });

    if (trapIt != ids.end()) {
        dfa.trap = trapIt->second;
    }

    return dfa;
}

int getOrCreateState(const std::set<int>& states,
                     std::map<std::set<int>, int>& ids,
                     DFA& dfa,
                     std::queue<std::set<int>>& q,
                     const std::set<int>& nfaAccepting,
                     const std::map<int, std::string>& nfaTokenType)
{

    auto it = ids.find(states);

    if (it != ids.end()) {
        return it->second;
    }

    int id = dfa.states++;

    dfa.transition[id] = { };

    ids[states] = id;

    q.push(states);

    for (int acceptState : nfaAccepting) {
        if (states.count(acceptState)) {
            dfa.accepting.insert(id);

            auto typeIt = nfaTokenType.find(acceptState);
            if (typeIt != nfaTokenType.end()) {
                dfa.tokenType[id] = typeIt->second;
            }

            break;
        }
    }

    return id;
}

set<int> epsilonClosure(const NFA& nfa, const set<int>& states)
{
    set<int> result = states;
    queue<int> q;

    for (int s : states)
        q.push(s);

    while (!q.empty()) {
        int current = q.front();
        q.pop();

        for (const auto& t : nfa.transitions) {

            if (t.from != current)
                continue;

            if (t.symbol.has_value())
                continue;

            if (!result.count(t.to)) {
                result.insert(t.to);
                q.push(t.to);
            }
        }
    }

    return result;
}

set<int> move(const NFA& nfa, const set<int>& states, char c)
{
    set<int> result;

    for (int s : states) {

        for (const auto& t : nfa.transitions) {

            if (t.from == s && t.symbol.has_value() && t.symbol.value() == c) {

                result.insert(t.to);
            }
        }
    }

    return result;
}

DFA minimizeHopcroft(const DFA& dfa)
{
    DFA result;

    const int n = dfa.states;
    if (n == 0) {
        return result;
    }

    std::map<std::string, std::set<int>> tokenGroups;
    std::set<int> nonAccepting;

    for (int s = 0; s < n; ++s) {
        auto it = dfa.tokenType.find(s);

        if (it != dfa.tokenType.end()) {
            tokenGroups[it->second].insert(s);
        }
        else {
            nonAccepting.insert(s);
        }
    }

    std::vector<std::set<int>> process;

    if (!nonAccepting.empty()) {
        process.push_back(nonAccepting);
    }

    for (const auto& [token, states] : tokenGroups) {
        if (!states.empty()) {
            process.push_back(states);
        }
    }

    if (process.empty()) {
        return result;
    }

    std::deque<std::set<int>> workList;

    for (const auto& block : process) {
        workList.push_back(block);
    }

    while (!workList.empty()) {
        std::set<int> A = workList.front();
        workList.pop_front();

        for (int c = 0; c < 128; ++c) {
            char symbol = static_cast<char>(c);

            std::set<int> X;

            for (int s = 0; s < n; ++s) {
                int target = dfa.trap;

                auto stateIt = dfa.transition.find(s);

                if (stateIt != dfa.transition.end()) {
                    auto transIt = stateIt->second.find(symbol);

                    if (transIt != stateIt->second.end()) {
                        target = transIt->second;
                    }
                }

                if (target >= 0 && target < n && A.count(target)) {
                    X.insert(s);
                }
            }

            std::vector<std::set<int>> newP;

            for (const std::set<int>& Y : process) {
                std::set<int> intersection;
                std::set<int> difference;

                for (int s : Y) {
                    if (X.count(s)) {
                        intersection.insert(s);
                    }
                    else {
                        difference.insert(s);
                    }
                }

                if (intersection.empty() || difference.empty()) {
                    newP.push_back(Y);
                    continue;
                }

                newP.push_back(intersection);
                newP.push_back(difference);

                auto it = std::find(workList.begin(), workList.end(), Y);

                if (it != workList.end()) {
                    workList.erase(it);
                    workList.push_back(intersection);
                    workList.push_back(difference);
                }
                else {
                    if (intersection.size() <= difference.size()) {
                        workList.push_back(intersection);
                    }
                    else {
                        workList.push_back(difference);
                    }
                }
            }

            process = std::move(newP);
        }
    }

    std::vector<int> stateToBlock(n, -1);

    for (int i = 0; i < static_cast<int>(process.size()); ++i) {
        int representative = *process[i].begin();

        auto it = dfa.tokenType.find(representative);

        if (it != dfa.tokenType.end()) {
            result.tokenType[i] = it->second;
        }
    }

    for (int i = 0; i < static_cast<int>(process.size()); ++i) {
        for (int s : process[i]) {
            stateToBlock[s] = i;
        }
    }

    result.states = static_cast<int>(process.size());
    result.start = stateToBlock[dfa.start];

    if (dfa.trap >= 0 && dfa.trap < n) {
        result.trap = stateToBlock[dfa.trap];
    }

    for (int i = 0; i < static_cast<int>(process.size()); ++i) {
        for (int s : process[i]) {
            if (dfa.accepting.count(s)) {
                result.accepting.insert(i);
                break;
            }
        }
    }

    for (int i = 0; i < static_cast<int>(process.size()); ++i) {

        for (int c = 0; c < 128; ++c) {
            char symbol = static_cast<char>(c);

            int targetBlock = -1;

            for (int s : process[i]) {
                auto stateIt = dfa.transition.find(s);

                if (stateIt == dfa.transition.end()) {
                    continue;
                }

                auto transIt = stateIt->second.find(symbol);

                if (transIt == stateIt->second.end()) {
                    continue;
                }

                int oldTarget = transIt->second;

                if (oldTarget < 0 || oldTarget >= n) {
                    continue;
                }

                int currentBlock = stateToBlock[oldTarget];

                if (targetBlock == -1) {
                    targetBlock = currentBlock;
                }
            }

            if (targetBlock != -1) {
                result.transition[i][symbol] = targetBlock;
            }
        }
    }

    return result;
}

vector<string> tokenize(const DFA& dfa, const string& input)
{
    vector<string> result;

    if (input == "") {
        result.push_back("ERROR");
        return result;

    }
    int pos = 0;

    while (pos < input.length()) {
        int state = dfa.start;
        int i = pos;
        int lastAcceptState = NULL;
        int lastAcceptPosition = pos;

        while (i < input.length()) {
            int nextState = dfa.next(state, input[i]);

            if (nextState == -1 || nextState == dfa.trap) {
                break;
            }

            state = nextState;
            i++;
            if (dfa.accepting.count(state)) {
                lastAcceptState = state;
                lastAcceptPosition = i;
            }
        }

        string tokenType;
        if (lastAcceptState == NULL ) {
            tokenType = "ERROR";
            lastAcceptPosition++;
        }
        else {
            auto it = dfa.tokenType.find(lastAcceptState);
            if (it != dfa.tokenType.end()) {
                tokenType = it->second;
            }
            else {
                tokenType = "ERROR";
            }
        }
        result.push_back(tokenType);

        pos = lastAcceptPosition;
        
    }
    return result;
}
