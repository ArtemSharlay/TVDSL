#pragma once

#include "ast/AST.hpp"
#include "dfa/dfa.hpp"
#include <algorithm>
#include <ast/Declaration.hpp>
#include <ast/Statement.hpp>
#include <string>
#include <vector>
#include <ast/Predicate.hpp>
#include <iostream>

class Parser
{
    public:
    const std::string& input;
    const DFA& dfa;
    std::vector<TokenType> tokens;
    std::vector<std::runtime_error> errors;
    int pos;


    Parser(const std::string& input, const DFA& dfa);

    TokenType peek();

    TokenType advance();

    TokenType tok();

    TokenType expect(const std::string& expectation);

    bool match(const std::string& expectation);

    void synchronize(const std::vector<std::string>& syncTokens);

    std::unique_ptr<Module> parseModule();

    std::unique_ptr<Declaration> parseDeclaration();

    std::vector<std::unique_ptr<VariableDecl>> parseVarDefs();
    std::unique_ptr<VariableDecl> parseVarDef();

    std::unique_ptr<Predicate> parsePredicate();

    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<IfStmt> parseIfStmt();
    std::unique_ptr<WhileStmt> parseWhileStmt();
    std::unique_ptr<BlockStmt> parseBlock();
    std::unique_ptr<Statement> parseAssignment();

    std::unique_ptr<Predicate> parseCondition();
    std::unique_ptr<Predicate> parseOrExpr();
    std::unique_ptr<Predicate> parseAndExpr();
    std::unique_ptr<Predicate> parseNotExpr();
    std::unique_ptr<Predicate> parseAtom();
    std::unique_ptr<Predicate> parseQuantifier();
    std::unique_ptr<Predicate> parseFormulaRef();
    std::vector<std::unique_ptr<Expression>> parseArguments();
    std::unique_ptr<Predicate> parseComparison();
    CompareOp parseCompareOp();

    std::unique_ptr<Expression> parseExpr();
    std::unique_ptr<Expression> parseTerm();
    std::unique_ptr<Expression> parseUnary();
    std::unique_ptr<Expression> parsePrimary();
    std::unique_ptr<FunctionCallExpr> parseCall();
};
