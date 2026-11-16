#pragma once
#include "ast/AST.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

struct Module : public Node
{
    std::vector<std::unique_ptr<Declaration>> declarations;
    void print(std::ostream& out, int indent = 0) const override
    {
        printIndent(out, indent);
        out << "Module" << '\n';

        for (const auto& arg : declarations)
            arg->print(out, indent + 1);
    }
};

enum class Type
{
    Int,
    IntArray
};

struct VariableDecl : public Declaration
{
    std::string name;
    Type type;

    void print(std::ostream& out, int indent = 0) const override
    {
        printIndent(out, indent);
        if (type == Type::Int) {
            out << "VariableDecl: " << name << " int" << '\n';
        }
        else {
            out << "VariableDecl: " << name << " intArray" << '\n';
        }
    }
};

struct FunctionDecl : public Declaration
{
    std::string name;

    std::vector<std::unique_ptr<VariableDecl>> parameters;
    std::vector<std::unique_ptr<VariableDecl>> returns;
    std::vector<std::unique_ptr<VariableDecl>> locals;

    std::unique_ptr<Predicate> requiresPredicate;
    std::unique_ptr<Predicate> ensures;

    std::unique_ptr<Statement> body;

    explicit FunctionDecl(std::string name,
                          std::vector<std::unique_ptr<VariableDecl>> parameters)
        : name(std::move(name))
        , parameters(std::move(parameters))
    { }

    inline void print(std::ostream& out, int indent) const
    {
        printIndent(out, indent);
        out << "FunctionDecl: " << name << '\n';

        printIndent(out, indent + 1);
        out << "Parameters:\n";

        for (const auto& param : parameters)
            param->print(out, indent + 2);

        printIndent(out, indent + 1);
        out << "Returns:\n";

        for (const auto& ret : returns)
            ret->print(out, indent + 2);

        if (!locals.empty()) {
            printIndent(out, indent + 1);
            out << "Locals:\n";

            for (const auto& local : locals)
                local->print(out, indent + 2);
        }

        if (requiresPredicate) {
            printIndent(out, indent + 1);
            out << "Requires:\n";
            requiresPredicate->print(out, indent + 2);
        }

        if (ensures) {
            printIndent(out, indent + 1);
            out << "Ensures:\n";
            ensures->print(out, indent + 2);
        }

        if (body) {
            printIndent(out, indent + 1);
            out << "Body:\n";
            body->print(out, indent + 2);
        }
    }
};

struct FormulaDecl : public Declaration
{
    std::string name;
    std::vector<std::unique_ptr<VariableDecl>> parameters;
    std::unique_ptr<Predicate> body;

    explicit FormulaDecl(std::string name,
                         std::vector<std::unique_ptr<VariableDecl>> parameters,
                         std::unique_ptr<Predicate> body)
        : name(std::move(name))
        , parameters(std::move(parameters))
        , body(std::move(body))
    { }

    inline void print(std::ostream& out, int indent) const
    {
        printIndent(out, indent);
        out << "FormulaDecl: " << name << '\n';

        printIndent(out, indent + 1);
        out << "Parameters:\n";

        for (const auto& param : parameters)
            param->print(out, indent + 2);

        if (body) {
            printIndent(out, indent + 1);
            out << "Predicate:\n";
            body->print(out, indent + 2);
        }
    }
};
