#include "regex/regexParser.hpp"
#include "json/jsonParser.hpp"
#include "json/tokenStruct.hpp"
#include <fstream>
#include <iostream>
#include <nfa/nfa.hpp>
#include <dfa/dfa.hpp>
#include <nlohmann/json.hpp>
#include <string>


int main()
{
    LexerConfig config;

    if (!exportJSONtoLexerConfig(config)) {
        return 0;
    }

    NFA nfa;
    createGeneralNFA(nfa, config.tokens);


    DFA dfa = determinize(nfa, nfa.accept);

    DFA dfamin = minimizeHopcroft(dfa);

    exportDFAtoJSON(dfamin, "dfa.json");

    return 0;
}
