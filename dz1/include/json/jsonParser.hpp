#pragma once
#include <iostream>
#include <map>
#include <optional>
#include <queue>
#include <set>
#include <string>
#include <vector>
#include <dfa/dfa.hpp>
#include "nfa/nfa.hpp"
#include "json/tokenStruct.hpp"
#include <nlohmann/json.hpp>
#include <fstream>

void exportDFAtoJSON(const DFA& dfa, const std::string& filename);

bool exportJSONtoLexerConfig(LexerConfig& config);