#pragma once
#include "ast/AST.hpp"
#include <memory>
#include <string>
#include <vector>
#include "Expression.hpp"

struct AssignmentStmt : public Statement
{
    std::string target;
    std::unique_ptr<Expression> value;

    void print(std::ostream& out, int indent) const
    {
        printIndent(out, indent);
        out << "AssignmentStmt: " << target << '\n';

        if (value)
            value->print(out, indent + 1);
    }
};

struct ArrayAssignmentStmt : public Statement
{
    std::string arrayName;
    std::unique_ptr<Expression> index;
    std::unique_ptr<Expression> value;

    void print(std::ostream& out, int indent) const
    {
        printIndent(out, indent);
        out << "ArrayAssignmentStmt: " << arrayName << '\n';

        printIndent(out, indent + 1);
        out << "Index:\n";

        if (index)
            index->print(out, indent + 2);

        printIndent(out, indent + 1);
        out << "Value:\n";

        if (value)
            value->print(out, indent + 2);
    }
};

struct TupleAssignmentStmt : public Statement
{
    std::vector<std::string> targets;
    std::unique_ptr<FunctionCallExpr> call;

    void print(std::ostream& out, int indent) const
    {
        printIndent(out, indent);
        out << "TupleAssignmentStmt\n";

        printIndent(out, indent + 1);
        out << "Targets:\n";

        for (const auto& target : targets) {
            printIndent(out, indent + 2);
            out << target << '\n';
        }

        printIndent(out, indent + 1);
        out << "FunctionCall:\n";

        if (call)
            call->print(out, indent + 2);
    }
};

struct IfStmt : public Statement
{
    std::unique_ptr<Predicate> condition;
    std::unique_ptr<Statement> thenBranch;
    std::unique_ptr<Statement> elseBranch;

    void print(std::ostream& out, int indent) const
    {
        printIndent(out, indent);
        out << "IfStmt\n";

        printIndent(out, indent + 1);
        out << "Condition:\n";

        if (condition)
            condition->print(out, indent + 2);

        printIndent(out, indent + 1);
        out << "Then:\n";

        if (thenBranch)
            thenBranch->print(out, indent + 2);

        if (elseBranch) {
            printIndent(out, indent + 1);
            out << "Else:\n";

            elseBranch->print(out, indent + 2);
        }
    }
};

struct WhileStmt : public Statement
{
    std::unique_ptr<Predicate> condition;
    std::unique_ptr<Statement> body;
    std::unique_ptr<Predicate> invariant;

    void print(std::ostream& out, int indent) const
    {
        printIndent(out, indent);
        out << "WhileStmt\n";

        printIndent(out, indent + 1);
        out << "Condition:\n";

        if (condition)
            condition->print(out, indent + 2);

        if (invariant) {
            printIndent(out, indent + 1);
            out << "Invariant:\n";

            invariant->print(out, indent + 2);
        }

        printIndent(out, indent + 1);
        out << "Body:\n";

        if (body)
            body->print(out, indent + 2);
    }
};

struct BlockStmt : public Statement
{
    std::vector<std::unique_ptr<Statement>> body;

    void print(std::ostream& out, int indent) const
    {
        printIndent(out, indent);
        out << "BlockStmt\n";

        for (const auto& stmt : body) {
            if (stmt)
                stmt->print(out, indent + 1);
        }
    }
};

struct AssertStmt : public Statement
{
    std::unique_ptr<Predicate> predicate;

    explicit AssertStmt(std::unique_ptr<Predicate> predicate)
        : predicate(std::move(predicate))
    { }


    void print(std::ostream& out, int indent) const
    {
        printIndent(out, indent);
        out << "AssertStmt\n";

        if (predicate)
            predicate->print(out, indent + 1);
    }
};

struct AssumeStmt : public Statement
{
    std::unique_ptr<Predicate> predicate;

        explicit AssumeStmt(std::unique_ptr<Predicate> predicate)
        : predicate(std::move(predicate))
    { }

    void print(std::ostream& out, int indent) const
    {
        printIndent(out, indent);
        out << "AssumeStmt\n";

        if (predicate)
            predicate->print(out, indent + 1);
    }
};