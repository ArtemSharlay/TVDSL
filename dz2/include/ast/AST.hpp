#pragma once
#include <ostream>

struct Node
{
    virtual ~Node() = default;
    virtual void print(std::ostream& out, int indent = 0) const = 0;
};

struct Declaration : public Node
{
    virtual ~Declaration() = default;
};

struct Statement : public Node
{
    virtual ~Statement() = default;
};

struct Expression : public Node
{
    virtual ~Expression() = default;
};

struct Predicate : public Node
{
    virtual ~Predicate() = default;
};

inline void printIndent(std::ostream& out, int indent)
{
    for (int i = 0; i < indent; ++i)
        out << "  ";
}