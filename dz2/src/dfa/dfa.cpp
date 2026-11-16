#include "dfa/dfa.hpp"



vector<TokenType> tokenize(const DFA& dfa, const string& input)
{
    vector<TokenType> result;

    if (input == "") {
        result.push_back({ 0, 0, "ERROR", "ERROR" });
        return result;
    }

    int pos = 0;
    int row = 1;
    int column = 1;
    while (pos < input.length()) {
        TokenType tokenT;

        int startRow = row;
        int startColumn = column;
        
        string lexeme = "";

        int state = dfa.start;
        int i = pos;
        int lastAcceptState = NULL;
        int lastAcceptPosition = pos;

        int lastAcceptRow = row;
        int lastAcceptColumn = column;

        int currentRow = row;
        int currentColumn = column;

        while (i < input.length()) {
            int nextState = dfa.next(state, input[i]);

            if (nextState == -1 || nextState == dfa.trap) {
                break;
            }

            state = nextState;
            lexeme += input[i];

            if (input[i] == '\n') {
                currentRow++;
                currentColumn = 1;
            }
            else {
                currentColumn++;
            }

            i++;
            if (dfa.accepting.count(state)) {
                lastAcceptState = state;
                lastAcceptPosition = i;

                lastAcceptRow = currentRow;
                lastAcceptColumn = currentColumn;
            }
        }

       
        if (lastAcceptState == NULL) {
            tokenT.token = "ERROR";
            tokenT.lexeme = string(1, input[pos]);
            tokenT.row = startRow;
            tokenT.column = startColumn;

            if (input[pos] == '\n') {
                row++;
                column = 1;
            }
            else {
                column++;
            }

            pos++;
            result.push_back(tokenT);
            continue;
        }
        else {
            lexeme = input.substr(pos, lastAcceptPosition - pos);

            auto it = dfa.tokenType.find(lastAcceptState);
            if (it != dfa.tokenType.end()) {
                tokenT.token = it->second;
                tokenT.lexeme = lexeme;
              
            }
            else {
                tokenT.token = "ERROR";
                tokenT.lexeme = lexeme;
            }

            tokenT.lexeme = lexeme;
            tokenT.row = startRow;
            tokenT.column = startColumn;
        }
        if (tokenT.token != "WS" && tokenT.token != "COMMENT") {
            result.push_back(tokenT);
        }
        pos = lastAcceptPosition;
        row = lastAcceptRow;
        column = lastAcceptColumn;
    }
    return result;
}

bool saveDFA(const DFA& dfa, const string& filename)
{
    std::cout << "SAVE STARTED" <<'\n';
    ofstream out(filename);

    if (!out.is_open()) {
        return false;
    }

    out << dfa.states << '\n';
    out << dfa.start << '\n';
    out << dfa.trap << '\n';

    out << dfa.accepting.size() << '\n';

    for (int state : dfa.accepting) {
        out << state << '\n';
    }

    out << dfa.tokenType.size() << '\n';

    for (const auto& [state, type] : dfa.tokenType) {
        out << state << ' ' << type << '\n';
    }

    int transitionCount = 0;

    for (const auto& [state, transitions] : dfa.transition) {
        transitionCount += transitions.size();
    }

    out << transitionCount << '\n';

    for (const auto& [state, transitions] : dfa.transition) {
        for (const auto& [symbol, target] : transitions) {
            out << state << ' '
                << static_cast<int>(static_cast<unsigned char>(symbol)) << ' '
                << target << '\n';
        }
    }
    std::cout << "SAVEd" << '\n';
    return true;
}

bool loadDFA(DFA& dfa, const string& filename)
{
    ifstream in(filename);

    if (!in.is_open()) {
        return false;
    }

    DFA loaded;

    in >> loaded.states;
    in >> loaded.start;
    in >> loaded.trap;

    if (!in || loaded.states <= 0 || loaded.start < 0
        || loaded.start >= loaded.states || loaded.trap < 0
        || loaded.trap >= loaded.states)
    {
        return false;
    }

    int acceptingCount;
    in >> acceptingCount;

    for (int i = 0; i < acceptingCount; i++) {
        int state;
        in >> state;

        if (!in || state < 0 || state >= loaded.states) {
            return false;
        }

        loaded.accepting.insert(state);
    }


    int tokenCount;
    in >> tokenCount;

    for (int i = 0; i < tokenCount; i++) {
        int state;
        string type;

        in >> state >> type;

        if (!in || state < 0 || state >= loaded.states
            || !loaded.tokenType.emplace(state, type).second)
        {
            return false;
        }
    }

    int transitionCount;
    in >> transitionCount;

    for (int i = 0; i < transitionCount; i++) {
        int state;
        int symbol;
        int target;

        in >> state >> symbol >> target;

        if (!in || state < 0 || state >= loaded.states || symbol < 0
            || symbol > 127 || target < 0 || target >= loaded.states)
        {
            return false;
        }

        char ch = static_cast<char>(symbol);

        if (!loaded.transition[state].emplace(ch, target).second) {
            return false;
        }
    }

    if (!in) {
        return false;
    }

    dfa = std::move(loaded);
    return true;
}