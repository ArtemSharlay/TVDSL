#pragma once
#include "ast/AST.hpp"
#include <memory>
#include <string>
#include <vector>

struct NumberExpr : public Expression
{
    int value;

    explicit NumberExpr(int value)
        : value(std::move(value))
    { }

    void print(std::ostream& out, int indent = 0) const override
    {
        printIndent(out, indent);
        out << "Number: " << value << '\n';
    }
};

struct BoolLiteralExpr : public Expression
{
    bool value;
    void print(std::ostream& out, int indent = 0) const override
    {
        printIndent(out, indent);
        out << "BoolLiteral: " << value << '\n';
    }
};

struct VariableExpr : public Expression
{
    std::string name;

    explicit VariableExpr(std::string name)
        : name(std::move(name))
    { }

    void print(std::ostream& out, int indent = 0) const override
    {
        printIndent(out, indent);
        out << "Variable: " << name << '\n';
    }
};

struct BinaryExpr : public Expression
{
    std::string op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

    explicit BinaryExpr(std::string op,
                        std::unique_ptr<Expression> left,
                        std::unique_ptr<Expression> right)
        : op(std::move(op))
        , left(std::move(left))
        , right(std::move(right))
    { }

    void print(std::ostream& out, int indent = 0) const override
    {
        printIndent(out, indent);

        if (op == "+") {
            out << "Add\n";
        }
        if (op == "-") {
            out << "Sub\n";
        }
        if (op == "*") {
            out << "Mul\n";
        }
        if (op == "/") {
            out << "Div\n";
        }

        left->print(out, indent + 1);
        right->print(out, indent + 1);
    }
};

enum class UnaryOp
{
    Negate
};

struct UnaryExpr : public Expression
{
    UnaryOp op;
    std::unique_ptr<Expression> operand;

    explicit UnaryExpr(UnaryOp op, std::unique_ptr<Expression> operand)
        : op(std::move(op))
        , operand(std::move(operand))
    { }

    void print(std::ostream& out, int indent = 0) const override
    {
        printIndent(out, indent);

        if (op == UnaryOp::Negate) {
            out << "Neg\n";
        }

        operand->print(out, indent + 1);
    }
};

struct FunctionCallExpr : public Expression
{
    std::string name;
    std::vector<std::unique_ptr<Expression>> args;

    void print(std::ostream& out, int indent = 0) const override
    {
        printIndent(out, indent);
        out << "FunctionCall: " << name << '\n';

        for (const auto& arg : args)
            arg->print(out, indent + 1);
    }
};

struct ArrayAccessExpr : public Expression
{
    std::string name;
    std::unique_ptr<Expression> index;

    explicit ArrayAccessExpr(std::string name,
                             std::unique_ptr<Expression> index)
        : name(std::move(name))
        , index(std::move(index))
    { }

    void print(std::ostream& out, int indent = 0) const override
    {
        printIndent(out, indent);
        out << "ArrayAccess: " << name << '\n';

        index->print(out, indent + 1);
    }
};
