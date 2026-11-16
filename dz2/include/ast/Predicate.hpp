#pragma once
#include "ast/AST.hpp"
#include "ast/Declaration.hpp"
#include <memory>
#include <string>
#include <vector>

struct BoolPredicate : public Predicate
{
    bool value;

    explicit BoolPredicate(bool value)
        : value(std::move(value))
    { }

    inline void print(std::ostream& out, int indent) const
    {
        printIndent(out, indent);
        out << "BoolPredicate: " << (value ? "true" : "false") << '\n';
    }
};

enum class CompareOp
{
    Equal, // ==
    NotEqual, // !=
    Greater, // >
    Less, // <
    GreaterEqual, // >=
    LessEqual // <=
};

inline std::string compareOpToString(CompareOp op)
{
    switch (op) {
    case CompareOp::Equal:
        return "==";
    case CompareOp::NotEqual:
        return "!=";
    case CompareOp::Greater:
        return ">";
    case CompareOp::Less:
        return "<";
    case CompareOp::GreaterEqual:
        return ">=";
    case CompareOp::LessEqual:
        return "<=";
    }

    return "?";
}

struct ComparisonPredicate : public Predicate
{
    CompareOp op;

    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

    explicit ComparisonPredicate(CompareOp op,
                                 std::unique_ptr<Expression> left,
                                 std::unique_ptr<Expression> right)
        : op(std::move(op))
        , left(std::move(left))
        , right(std::move(right))
    { }

    inline void print(std::ostream& out, int indent) const
    {
        printIndent(out, indent);
        out << "ComparisonPredicate: " << compareOpToString(op) << '\n';

        if (left)
            left->print(out, indent + 1);

        if (right)
            right->print(out, indent + 1);
    }
};

enum class LogicalOp
{
    And,
    Or
};

inline std::string logicalOpToString(LogicalOp op)
{
    switch (op) {
    case LogicalOp::And:
        return "AND";
    case LogicalOp::Or:
        return "OR";
    }

    return "?";
}

struct LogicalPredicate : public Predicate
{
    LogicalOp op;
    std::unique_ptr<Predicate> left;
    std::unique_ptr<Predicate> right;

    explicit LogicalPredicate(LogicalOp op,
                              std::unique_ptr<Predicate> left,
                              std::unique_ptr<Predicate> right)
        : op(std::move(op))
        , left(std::move(left))
        , right(std::move(right))
    { }

    inline void print(std::ostream& out, int indent) const
    {
        printIndent(out, indent);
        out << "LogicalPredicate: " << logicalOpToString(this->op) << '\n';

        if (left)
            left->print(out, indent + 1);

        if (right)
            right->print(out, indent + 1);
    }
};

struct ImplicationPredicate : public Predicate
{
    std::unique_ptr<Predicate> left;
    std::unique_ptr<Predicate> right;

    explicit ImplicationPredicate(std::unique_ptr<Predicate> left,
                                  std::unique_ptr<Predicate> right)
        : left(std::move(left))
        , right(std::move(right))
    { }

    inline void print(std::ostream& out, int indent) const
    {
        printIndent(out, indent);
        out << "ImplicationPredicate: ->\n";

        if (left)
            left->print(out, indent + 1);

        if (right)
            right->print(out, indent + 1);
    }
};

struct NotPredicate : public Predicate
{
    std::unique_ptr<Predicate> operand;
    inline void print(std::ostream& out, int indent) const
    {
        printIndent(out, indent);
        out << "NotPredicate\n";

        if (operand)
            operand->print(out, indent + 1);
    }
};

enum class QuantifierType
{
    Forall,
    Exists
};

struct QuantifierPredicate : public Predicate
{
    QuantifierType type;

    std::unique_ptr<VariableDecl> variable;

    std::unique_ptr<Predicate> body;

    inline void print(std::ostream& out, int indent) const
    {
        printIndent(out, indent);

        out << "QuantifierPredicate: "
            << (type == QuantifierType::Forall ? "forall" : "exists") << '\n';

        if (variable) {
            printIndent(out, indent + 1);
            out << "Variable:\n";
            variable->print(out, indent + 2);
        }

        if (body) {
            printIndent(out, indent + 1);
            out << "Body:\n";
            body->print(out, indent + 2);
        }
    }
};

struct FormulaCallPredicate : public Predicate
{
    std::string name;

    std::vector<std::unique_ptr<Expression>> args;

    explicit FormulaCallPredicate(std::string name,
                                  std::vector<std::unique_ptr<Expression>> args)
        : name(std::move(name))
        , args(std::move(args))
    { }

    inline void print(std::ostream& out, int indent) const
    {
        printIndent(out, indent);
        out << "FormulaCallPredicate: " << name << '\n';

        printIndent(out, indent + 1);
        out << "Arguments:\n";

        for (const auto& arg : args) {
            if (arg)
                arg->print(out, indent + 2);
        }
    }
};
