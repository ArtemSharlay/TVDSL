#include "regex/regexParser.hpp"



RegexParser::RegexParser(const std::string& input)
    : input(input)
{ }

Regex RegexParser::parse()
{
    if (input.empty()) {
        error("Empty regex");
    }

    Regex result = parseExpression();

    if (!atEnd()) {
        error("Unexpected character '" + std::string(1, peek()) + "'");
    }

    return result;
}

Regex RegexParser::parseExpression()
{
    Regex result = parseTerm();

    while (!atEnd() && peek() == '|') {
        get(); 

        Regex right = parseTerm();

        result = Regex::unite(std::move(result), std::move(right));
    }

    return result;
}

Regex RegexParser::parseTerm()
{
    if (atEnd() || !isAtomStart(peek())) {
        error("Expected expression");
    }

    Regex result = parseFactor();

    while (!atEnd() && isAtomStart(peek())) {
        Regex right = parseFactor();

        result = Regex::concat(std::move(result), std::move(right));
    }

    return result;
}

Regex RegexParser::parseFactor()
{
    Regex result = parseAtom();

    while (!atEnd()) {
        char c = peek();

        if (c == '*') {
            get();
            result = Regex::star(std::move(result));
        }
        else if (c == '+') {
            get();
            result = Regex::plus(std::move(result));
        }
        else {
            break;
        }
    }

    return result;
}

Regex RegexParser::parseAtom()
{
    if (atEnd()) {
        error("Unexpected end of regex");
    }

    char c = peek();

    if (c == '(') {
        get(); 

        Regex result = parseExpression();

        if (!match(')')) {
            error("Expected ')'");
        }

        return result;
    }

    if (c == '[') {
        return parseCharClass();
    }

    if (c == '\\') {
        char escaped = parseEscape();
        return Regex::symbolNode(escaped);
    }

    if (c == ')' || c == '|' || c == '*' || c == '+' ) {
        error("Unexpected character '" + std::string(1, c) + "'");
    }

    get();

    return Regex::symbolNode(c);
}

Regex RegexParser::parseCharClass()
{
    get();

    if (atEnd()) {
        error("Unterminated character class");
    }

    bool negated = false;

    if (peek() == '^') {
        negated = true;
        get();
    }

    std::set<char> chars;

    bool hasCharacter = false;

    while (!atEnd() && peek() != ']') {
        char first;

        if (peek() == '\\') {
            first = parseEscape();
        }
        else {
            first = get();
        }

        hasCharacter = true;


        if (!atEnd() && peek() == '-' && pos + 1 < input.size()
            && input[pos + 1] != ']')
        {

            get();

            char last;

            if (atEnd()) {
                error("Unterminated character range");
            }

            if (peek() == '\\') {
                last = parseEscape();
            }
            else {
                last = get();
            }

            if (first > last) {
                error("Invalid character range");
            }

            for (unsigned int c = static_cast<unsigned char>(first);
                 c <= static_cast<unsigned char>(last);
                 ++c)
            {

                chars.insert(static_cast<char>(c));
            }
        }
        else {
            chars.insert(first);
        }
    }

    if (atEnd()) {
        error("Unterminated character class");
    }

    get(); 

    if (!hasCharacter) {
        error("Empty character class");
    }

     if (negated) {
        std::set<char> all;

        for (int i = 0; i < 128; ++i) {
            all.insert(static_cast<char>(i));
        }

        for (char c : chars) {
            all.erase(c);
        }

        chars = std::move(all);
    }


    return Regex::charClassNode(chars);
}

char RegexParser::parseEscape()
{
    if (!match('\\')) {
        error("Expected '\\'");
    }

    if (atEnd()) {
        error("Backslash at end of regex");
    }

    char c = get();

    switch (c) {
    case 'n':
        return '\n';

    case 'r':
        return '\r';

    case 't':
        return '\t';

    case '\\':
        return '\\';

    default:

        return c;
    }
}

bool RegexParser::atEnd() const
{
    return pos >= input.size();
}

char RegexParser::peek() const
{
    if (atEnd()) {
        error("Unexpected end of regex");
    }

    return input[pos];
}

char RegexParser::get()
{
    if (atEnd()) {
        error("Unexpected end of regex");
    }

    return input[pos++];
}

bool RegexParser::match(char c)
{
    if (!atEnd() && input[pos] == c) {
        ++pos;
        return true;
    }

    return false;
}

bool RegexParser::isAtomStart(char c) const
{
    return c != '|' && c != ')' && c != '*' && c != '+' ;
}

void RegexParser::error(const std::string& message) const
{
    throw std::runtime_error("Regex parse error at position "
                             + std::to_string(pos) + ": " + message);
}