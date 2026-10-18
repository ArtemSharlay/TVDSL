#include "json/jsonParser.hpp"

void exportDFAtoJSON(const DFA& dfa, const std::string& filename)
{
    std::ofstream out(filename);

    if (!out.is_open()) {
        std::cerr << "Cannot open file: " << filename << '\n';
        return;
    }

    out << "{\n";
    out << "  \"start\": " << dfa.start << ",\n";
    out << "  \"trap\": " << dfa.trap << ",\n";
    out << "  \"states\": " << dfa.states << ",\n";
    out << "  \"accepting\": [";

    bool first = true;
    for (int state : dfa.accepting) {
        if (!first) {
            out << ", ";
        }

        out << state;
        first = false;
    }

    out << "],\n";


    out << "  \"transitions\": [\n";

    for (int state = 0; state < dfa.states; ++state) {
        out << "    {\n";
        out << "      \"state\": " << state << ",\n";
        out << "      \"transitions\": {";

        auto it = dfa.transition.find(state);

        if (it != dfa.transition.end() && !it->second.empty()) {
            out << '\n';

            bool firstTransition = true;

            for (const auto& transition : it->second) {
                unsigned char c = static_cast<unsigned char>(transition.first);

                if (!firstTransition) {
                    out << ",\n";
                }

                out << "        \"";


                switch (c) {
                case '"':
                    out << "\\\"";
                    break;
                case '\\':
                    out << "\\\\";
                    break;
                case '\n':
                    out << "\\n";
                    break;
                case '\r':
                    out << "\\r";
                    break;
                case '\t':
                    out << "\\t";
                    break;
                default:
                    if (c < 32) {
                        out << "ASCII NUMBER:" << static_cast<int>(c); 
                    }
                    else {
                        out << static_cast<char>(c);
                    }
                }

                out << "\": " << transition.second;

                firstTransition = false;
            }

            out << '\n';
        }

        out << "      }\n";
        out << "    }";

        if (state + 1 < dfa.states) {
            out << ',';
        }

        out << '\n';
    }

    out << "  ]\n";
    out << "}\n";

    if (!out) {
        std::cerr << "Error writing JSON file\n";
        return;
    }

    std::cout << "DFA exported to " << filename << '\n';
}

bool exportJSONtoLexerConfig(LexerConfig &config)
{
    std::ifstream file(CONFIG_PATH "/config/tokens.json");

    if (!file.is_open()) {
        std::cerr << "Cannot open tokens.json\n";
        return 0;
    }

    nlohmann::json j;
    file >> j;

    config.alphabet = j.at("alphabet").get<std::string>();

    for (const auto& item : j.at("tokens")) {
        Token token;

        token.name = item.at("name").get<std::string>();
        token.regex = item.at("regex").get<std::string>();
        config.tokens.push_back(token);
    }

    return 1;
}
