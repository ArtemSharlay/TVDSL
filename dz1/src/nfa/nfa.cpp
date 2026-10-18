#include "nfa/nfa.hpp"
#include "json/tokenStruct.hpp"
#include "regex/regexParser.hpp"


int NFA::newState()
{
    return states++;
}

void NFA::addSymbol(int from, int to, char c)
{
    transitions.push_back({ from, to, c });
}

void NFA::addEpsilon(int from, int to)
{
    transitions.push_back({ from, to, std::nullopt });
}

Fragment buildNFA(const Regex& r, NFA& nfa)
{
    switch (r.type) {
    case Regex::Type::SYMBOL: {
        int s = nfa.newState();
        int e = nfa.newState();

        nfa.addSymbol(s, e, r.symbol);

        return { s, e };
    }

    case Regex::Type::CHAR_CLASS: {
        int s = nfa.newState();
        int e = nfa.newState();
        for (char c : r.charClass) {
            nfa.addSymbol(s, e, c);
        }
        return { s, e };
    }

    case Regex::Type::CONCAT: {
        Fragment a = buildNFA(r.children[0], nfa);
        Fragment b = buildNFA(r.children[1], nfa);

        nfa.addEpsilon(a.end, b.start);

        return { a.start, b.end };
    }

    case Regex::Type::UNION: {
        int s = nfa.newState();
        int e = nfa.newState();

        Fragment a = buildNFA(r.children[0], nfa);
        Fragment b = buildNFA(r.children[1], nfa);

        nfa.addEpsilon(s, a.start);
        nfa.addEpsilon(s, b.start);

        nfa.addEpsilon(a.end, e);
        nfa.addEpsilon(b.end, e);

        return { s, e };
    }

    case Regex::Type::STAR: {
        int s = nfa.newState();
        int e = nfa.newState();

        Fragment a = buildNFA(r.children[0], nfa);

        nfa.addEpsilon(s, a.start);
        nfa.addEpsilon(s, e);

        nfa.addEpsilon(a.end, a.start);
        nfa.addEpsilon(a.end, e);

        return { s, e };
    }

    case Regex::Type::PLUS: {
        Fragment a = buildNFA(r.children[0], nfa);

        int s = nfa.newState();
        int e = nfa.newState();

        nfa.addEpsilon(s, a.start);
        nfa.addEpsilon(a.end, a.start);
        nfa.addEpsilon(a.end, e);

        return { s, e };
    }
    }

    throw std::runtime_error("Unknown regex node");
}

void createGeneralNFA(NFA& nfa,std::vector<Token> tokens)
{

    int globalStart = nfa.newState();
    nfa.start = globalStart;

    for (const auto& token : tokens) {
        RegexParser parser(token.regex);
        Regex ast = parser.parse();

        Fragment fragment = buildNFA(ast, nfa);

        nfa.addEpsilon(globalStart, fragment.start);

        nfa.accept.insert(fragment.end);
        nfa.nfaTokenType[fragment.end] = token.name;
        
    }
}