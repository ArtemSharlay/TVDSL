
#include <gtest/gtest.h>

#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "ast/Declaration.hpp"
#include "ast/Expression.hpp"

#include "ast/Predicate.hpp"
#include "ast/Statement.hpp"
#include "parser/parser.hpp"
#include "dfa/dfa.hpp"



class ParserTest : public ::testing::Test
{
protected:
    DFA dfa;
   

    std::unique_ptr<Module> parse(const std::string& source)
    {
        loadDFA(dfa, CONFIG_PATH "/resources/lexer.dfa");
        Parser parser(source, dfa);
        return parser.parseModule();
    }

    Parser parseWithErrors(const std::string& source)
    {
        loadDFA(dfa, CONFIG_PATH "/resources/lexer.dfa");
        Parser parser(source, dfa);
        parser.parseModule();
        return parser;
    }

    std::string printAst(const Module& module)
    {
        std::ostringstream out;
        module.print(out, 0);
        return out.str();
    }
};


TEST_F(ParserTest, EmptyModule)
{
    auto ast = parse("");

    ASSERT_EQ(ast, nullptr);
}


TEST_F(ParserTest, SimpleFormula)
{
    const std::string source = R"(
        positive(x: int) => x >= 0
    )";

    auto ast = parse(source);

    ASSERT_NE(ast, nullptr);
    ASSERT_EQ(ast->declarations.size(), 1);

    auto* formula = dynamic_cast<FormulaDecl*>(ast->declarations[0].get());

    ASSERT_NE(formula, nullptr);
    EXPECT_EQ(formula->name, "positive");
    ASSERT_EQ(formula->parameters.size(), 1);
    EXPECT_EQ(formula->parameters[0]->name, "x");
}


TEST_F(ParserTest, SimpleFunction)
{
    const std::string source = R"(
        increment(x: int)
        returns result: int
        {
            result = x + 1;
        }
    )";

    auto ast = parse(source);

    ASSERT_NE(ast, nullptr);
    ASSERT_EQ(ast->declarations.size(), 1);

    auto* function = dynamic_cast<FunctionDecl*>(ast->declarations[0].get());

    ASSERT_NE(function, nullptr);
    EXPECT_EQ(function->name, "increment");

    ASSERT_EQ(function->parameters.size(), 1);
    EXPECT_EQ(function->parameters[0]->name, "x");

    ASSERT_EQ(function->returns.size(), 1);
    EXPECT_EQ(function->returns[0]->name, "result");

    ASSERT_NE(function->body, nullptr);
}


TEST_F(ParserTest, FunctionWithContracts)
{
    const std::string source = R"(
        divide(a: int, b: int)
        requires b != 0
        returns result: int
        ensures result * b <= a
        {
            result = a / b;
        }
    )";

    auto ast = parse(source);

    ASSERT_NE(ast, nullptr);
    ASSERT_EQ(ast->declarations.size(), 1);

    auto* function = dynamic_cast<FunctionDecl*>(ast->declarations[0].get());

    ASSERT_NE(function, nullptr);

    EXPECT_EQ(function->name, "divide");
    ASSERT_NE(function->requiresPredicate, nullptr);
    ASSERT_NE(function->ensures, nullptr);
    ASSERT_NE(function->body, nullptr);
}

TEST_F(ParserTest, ArrayAndLocalVariables)
{
    const std::string source = R"(
        sumArray(a: int[], n: int)
        requires n >= 0
        returns result: int
        uses i: int
        {
            result = 0;
            i = 0;

            while (i < n)
            invariant i >= 0
            {
                result = result + a[i];
                i = i + 1;
            }
        }
    )";

    auto ast = parse(source);

    ASSERT_NE(ast, nullptr);
    ASSERT_EQ(ast->declarations.size(), 1);

    auto* function = dynamic_cast<FunctionDecl*>(ast->declarations[0].get());

    ASSERT_NE(function, nullptr);

    EXPECT_EQ(function->name, "sumArray");

    ASSERT_EQ(function->parameters.size(), 2);
    EXPECT_EQ(function->parameters[0]->name, "a");
    EXPECT_EQ(function->parameters[1]->name, "n");

    ASSERT_EQ(function->returns.size(), 1);
    EXPECT_EQ(function->returns[0]->name, "result");

    ASSERT_EQ(function->locals.size(), 1);
    EXPECT_EQ(function->locals[0]->name, "i");

    ASSERT_NE(function->body, nullptr);
}


TEST_F(ParserTest, IfElseStatement)
{
    const std::string source = R"(
        max(a: int, b: int)
        returns result: int
        {
            if (a > b) {
                result = a;
            } else {
                result = b;
            }
        }
    )";

    auto ast = parse(source);

    ASSERT_NE(ast, nullptr);
    ASSERT_EQ(ast->declarations.size(), 1);

    auto* function = dynamic_cast<FunctionDecl*>(ast->declarations[0].get());

    ASSERT_NE(function, nullptr);
    ASSERT_NE(function->body, nullptr);

    auto* block = dynamic_cast<BlockStmt*>(function->body.get());

    ASSERT_NE(block, nullptr);
    ASSERT_EQ(block->body.size(), 1);

    auto* ifStmt = dynamic_cast<IfStmt*>(block->body[0].get());

    ASSERT_NE(ifStmt, nullptr);
    ASSERT_NE(ifStmt->condition, nullptr);
    ASSERT_NE(ifStmt->thenBranch, nullptr);
    ASSERT_NE(ifStmt->elseBranch, nullptr);
}


TEST_F(ParserTest, AssertAndAssume)
{
    const std::string source = R"(
        check(x: int)
        returns result: int
        {
            assume x >= 0;
            assert x >= 0;
            result = x;
        }
    )";

    auto ast = parse(source);

    ASSERT_NE(ast, nullptr);
    ASSERT_EQ(ast->declarations.size(), 1);

    auto* function = dynamic_cast<FunctionDecl*>(ast->declarations[0].get());

    ASSERT_NE(function, nullptr);

    auto* block = dynamic_cast<BlockStmt*>(function->body.get());

    ASSERT_NE(block, nullptr);
    ASSERT_EQ(block->body.size(), 3);

    EXPECT_NE(dynamic_cast<AssumeStmt*>(block->body[0].get()), nullptr);

    EXPECT_NE(dynamic_cast<AssertStmt*>(block->body[1].get()), nullptr);

    EXPECT_NE(dynamic_cast<AssignmentStmt*>(block->body[2].get()), nullptr);
}


TEST_F(ParserTest, QuantifiersAndLogic)
{
    const std::string source = R"(
        allPositive(a: int[])
        => forall(x: int | x >= 0) and true
    )";

    auto ast = parse(source);

    ASSERT_NE(ast, nullptr);
    ASSERT_EQ(ast->declarations.size(), 1);

    auto* formula = dynamic_cast<FormulaDecl*>(ast->declarations[0].get());

    ASSERT_NE(formula, nullptr);
    EXPECT_EQ(formula->name, "allPositive");
    ASSERT_NE(formula->body, nullptr);
}


TEST_F(ParserTest, FunctionCallAssignment)
{
    const std::string source = R"(
        calculate(x: int)
        returns result: int
        {
            result = foo(x);
        }
    )";

    auto ast = parse(source);

    ASSERT_NE(ast, nullptr);
    ASSERT_EQ(ast->declarations.size(), 1);

    auto* function = dynamic_cast<FunctionDecl*>(ast->declarations[0].get());

    ASSERT_NE(function, nullptr);
    ASSERT_NE(function->body, nullptr);
}

TEST_F(ParserTest, ArrayAssignment)
{
    const std::string source = R"(
        update(a: int[], i: int)
        returns result: int
        {
            a[i] = 10;
            result = a[i];
        }
    )";

    auto ast = parse(source);

    ASSERT_NE(ast, nullptr);
    ASSERT_EQ(ast->declarations.size(), 1);
}


TEST_F(ParserTest, TupleAssignment)
{
    const std::string source = R"(
        swapValues(a: int, b: int)
        returns x: int, y: int
        {
            x, y = swap(a, b);
        }
    )";

    auto ast = parse(source);

    ASSERT_NE(ast, nullptr);
    ASSERT_EQ(ast->declarations.size(), 1);
}


TEST_F(ParserTest, NestedBlocks)
{
    const std::string source = R"(
        nested(x: int)
        returns result: int
        {
            {
                result = x + 1;
            }
        }
    )";

    auto ast = parse(source);

    ASSERT_NE(ast, nullptr);
    ASSERT_EQ(ast->declarations.size(), 1);
}


TEST_F(ParserTest, MultipleDeclarations)
{
    const std::string source = R"(
        positive(x: int) => x >= 0

        negative(x: int) => x < 0

        identity(x: int)
        returns result: int
        {
            result = x;
        }
    )";

    auto ast = parse(source);

    ASSERT_NE(ast, nullptr);
    ASSERT_EQ(ast->declarations.size(), 3);

    EXPECT_NE(dynamic_cast<FormulaDecl*>(ast->declarations[0].get()), nullptr);

    EXPECT_NE(dynamic_cast<FormulaDecl*>(ast->declarations[1].get()), nullptr);

    EXPECT_NE(dynamic_cast<FunctionDecl*>(ast->declarations[2].get()), nullptr);
}


TEST_F(ParserTest, MissingClosingParenthesis)
{
    const std::string source = R"(
        foo(x: int
        returns result: int
        {
            result = x;
        }
    )";

     auto ast = parseWithErrors(source);

   ASSERT_STREQ(ast.errors[0].what(), "WRONG AT 3 9 expected RPAREN");
}

TEST_F(ParserTest, MissingClosingBrace)
{
    const std::string source = R"(
        foo(x: int)
        returns result: int
        {
            result = x;
    )";

    auto ast = parseWithErrors(source);
    ASSERT_STREQ(ast.errors[0].what(), "EXPECTED RBRACE AT 5 23");
}

TEST_F(ParserTest, UnexpectedCharacters)
{
    const std::string source = R"(
        foo(x: int)
        returns result: int
        {
            @@@
            result = x;
        }
    )";

        auto ast = parseWithErrors(source);
    ASSERT_STREQ(ast.errors[0].what(), "ERROR parseStatement hasnt found correct token at 5 13");
}


TEST_F(ParserTest, InvalidOperator)
{
    const std::string source = R"(
        foo(x: int)
        returns result: int
        {
            result = x ** 2;
        }
    )";

        auto ast = parseWithErrors(source);
    ASSERT_STREQ(ast.errors[0].what(), "WRONG TOKEN 5 25");
}


TEST_F(ParserTest, MissingExpression)
{
    const std::string source = R"(
        foo(x: int)
        returns result: int
        {
            result = x + ;
        }
    )";

        auto ast = parseWithErrors(source);
    ASSERT_STREQ(ast.errors[0].what(), "WRONG TOKEN 5 26");
}


TEST_F(ParserTest, MissingVariableName)
{
    const std::string source = R"(
        foo(: int)
        returns result: int
        {
            result = 1;
        }
    )";

        auto ast = parseWithErrors(source);
    ASSERT_STREQ(ast.errors[0].what(), "WRONG AT 2 13 expected IDENT");
}


TEST_F(ParserTest, MissingColon)
{
    const std::string source = R"(
        foo(x int)
        returns result: int
        {
            result = x;
        }
    )";

        auto ast = parseWithErrors(source);
    ASSERT_STREQ(ast.errors[0].what(), "WRONG AT 2 15 expected COLON");
}


TEST_F(ParserTest, MissingSemicolon)
{
    const std::string source = R"(
        foo(x: int)
        returns result: int
        {
            result = x
        }
    )";

       auto ast = parseWithErrors(source);
    ASSERT_STREQ(ast.errors[0].what(), "WRONG AT 6 9 expected SEMICOLON");
}


TEST_F(ParserTest, InvalidComparison)
{
    const std::string source = R"(
        foo(x: int) => x >< 0
    )";

        auto ast = parseWithErrors(source);
    ASSERT_STREQ(ast.errors[0].what(), "WRONG TOKEN 2 27");
}


TEST_F(ParserTest, ExtraTokensAfterModule)
{
    const std::string source = R"(
        foo(x: int) => x >= 0
        )
    )";

        auto ast = parseWithErrors(source);
    ASSERT_STREQ(ast.errors[0].what(), "WRONG AT 3 9 expected IDENT");
}


TEST_F(ParserTest, MissingCallClosingParenthesis)
{
    const std::string source = R"(
        foo(x: int)
        returns result: int
        {
            result = bar(x;
        }
    )";

        auto ast = parseWithErrors(source);
    ASSERT_STREQ(ast.errors[0].what(), "WRONG AT 5 27 expected RPAREN");
}


TEST_F(ParserTest, InvalidQuantifier)
{
    const std::string source = R"(
        foo(x: int)
        => forall(x int | x >= 0)
    )";

        auto ast = parseWithErrors(source);
    ASSERT_STREQ(ast.errors[0].what(), "WRONG AT 3 21 expected COLON");
}

TEST_F(ParserTest, FunctionAstStructure)
{
    const std::string source = R"(
        increment(x: int)
        returns result: int
        {
            result = x + 1;
        }
    )";

    auto ast = parse(source);

    ASSERT_NE(ast, nullptr);
    ASSERT_EQ(ast->declarations.size(), 1);

    auto* function = dynamic_cast<FunctionDecl*>(ast->declarations[0].get());

    ASSERT_NE(function, nullptr);

    EXPECT_EQ(function->name, "increment");

    ASSERT_EQ(function->parameters.size(), 1);
    EXPECT_EQ(function->parameters[0]->name, "x");

    ASSERT_EQ(function->returns.size(), 1);
    EXPECT_EQ(function->returns[0]->name, "result");

    auto* body = dynamic_cast<BlockStmt*>(function->body.get());

    ASSERT_NE(body, nullptr);
    ASSERT_EQ(body->body.size(), 1);

    auto* assignment = dynamic_cast<AssignmentStmt*>(body->body[0].get());

    ASSERT_NE(assignment, nullptr);
    EXPECT_EQ(assignment->target, "result");
    ASSERT_NE(assignment->value, nullptr);
}


TEST_F(ParserTest, FormulaAstStructure)
{
    const std::string source = R"(
        positive(x: int) => x >= 0
    )";

    auto ast = parse(source);

    ASSERT_NE(ast, nullptr);
    ASSERT_EQ(ast->declarations.size(), 1);

    auto* formula = dynamic_cast<FormulaDecl*>(ast->declarations[0].get());

    ASSERT_NE(formula, nullptr);

    EXPECT_EQ(formula->name, "positive");
    ASSERT_EQ(formula->parameters.size(), 1);
    EXPECT_EQ(formula->parameters[0]->name, "x");

    ASSERT_NE(formula->body, nullptr);
}

TEST_F(ParserTest, IfAstStructure)
{
    const std::string source = R"(
        test(x: int)
        returns result: int
        {
            if (x > 0) {
                result = 1;
            } else {
                result = 0;
            }
        }
    )";

    auto ast = parse(source);

    ASSERT_NE(ast, nullptr);

    auto* function = dynamic_cast<FunctionDecl*>(ast->declarations[0].get());

    ASSERT_NE(function, nullptr);

    auto* block = dynamic_cast<BlockStmt*>(function->body.get());

    ASSERT_NE(block, nullptr);
    ASSERT_EQ(block->body.size(), 1);

    auto* ifStmt = dynamic_cast<IfStmt*>(block->body[0].get());

    ASSERT_NE(ifStmt, nullptr);
    EXPECT_NE(ifStmt->thenBranch, nullptr);
    EXPECT_NE(ifStmt->elseBranch, nullptr);
}

TEST_F(ParserTest, WhileAstStructure)
{
    const std::string source = R"(
        loop(n: int)
        returns result: int
        uses i: int
        {
            i = 0;

            while (i < n)
            invariant i >= 0
            {
                i = i + 1;
            }

            result = i;
        }
    )";

    auto ast = parse(source);

    ASSERT_NE(ast, nullptr);

    auto* function = dynamic_cast<FunctionDecl*>(ast->declarations[0].get());

    ASSERT_NE(function, nullptr);

    auto* block = dynamic_cast<BlockStmt*>(function->body.get());

    ASSERT_NE(block, nullptr);
    ASSERT_EQ(block->body.size(), 3);

    auto* whileStmt = dynamic_cast<WhileStmt*>(block->body[1].get());

    ASSERT_NE(whileStmt, nullptr);
    ASSERT_NE(whileStmt->condition, nullptr);
    ASSERT_NE(whileStmt->invariant, nullptr);
    ASSERT_NE(whileStmt->body, nullptr);
}


TEST_F(ParserTest, AstSnapshot)
{
    const std::string source = R"(
        foo(x: int)
        returns result: int
        {
            result = x + 1;
        }
    )";

    auto ast = parse(source);

    ASSERT_NE(ast, nullptr);

    const std::string actual = printAst(*ast);


    EXPECT_NE(actual.find("foo"), std::string::npos);
    EXPECT_NE(actual.find("result"), std::string::npos);
    EXPECT_NE(actual.find("AssignmentStmt"), std::string::npos);
}


