#include "dfa/dfa.hpp"
#include "dz2.h"
#include "parser/parser.hpp";
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

using namespace std;

std::string readFile(const std::string& filename)
{
    std::ifstream file(filename);

    if (!file.is_open())
        throw std::runtime_error("Cannot open file: " + filename);

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

int main()
{
    DFA dfa;

    if (!loadDFA(dfa, CONFIG_PATH "/resources/lexer.dfa")) {
        std::cout << "loading ERROR" << '\n';
    }

     try {
        std::string source = readFile("D:/tldr/TVDSL/dz2/programm.txt");
         Parser par(source,dfa);
 /*       for (auto s : par.tokens) {
             std::cout << s.token << "=token lexeme=" << s.lexeme << '\n';
        }
        std::cout << "-------------------------" << '\n';*/
      
       auto tmp = par.parseModule();
        if (tmp != nullptr) {

            tmp->print(std::cout, 0);
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
 
    return 0;
}
