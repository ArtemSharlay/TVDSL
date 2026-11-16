#include "parser/parser.hpp"
#include <ast/Predicate.hpp>

Parser::Parser(const std::string& input, const DFA& dfa)
    : input(std::move(input))
    , dfa(std::move(dfa))
{
    this->tokens = tokenize(dfa, input);
    int last_r = this->tokens.back().row;
    int last_c = this->tokens.back().column;
    this->tokens.push_back({ last_r, last_c, "EOF", "eof" });
    this->pos = 0;
}

TokenType Parser::peek()
{
    return tokens[std::min<std::size_t>(pos + 1, tokens.size() - 1)];
}

TokenType Parser::advance()
{

    TokenType t = tokens[pos];
    if (t.token != "EOF") {
        pos += 1;
    }
    return t;
}

TokenType Parser::tok()
{
    return this->tokens[pos];
}

TokenType Parser::expect(const std::string& expectation)
{
    if (tok().token != expectation) {

        throw std::runtime_error("WRONG AT " + std::to_string(tok().row) + " "
                                 + std::to_string(tok().column) + " expected " + expectation);
    }

    return advance();
}

bool Parser::match(const std::string& expectation)
{
    return tok().token == expectation;
}

void Parser::synchronize(const std::vector<std::string>& syncTokens)
{
    while (!match("EOF")) {
        for (const auto& token : syncTokens) {
            if (match(token)) {
                return;
            }
        }

        advance();
    }
}

std::unique_ptr<Module> Parser::parseModule()
{
    auto module = std::make_unique<Module>();
    if (match("EOF") || match("ERROR")) {
        return nullptr;
    }

    while (tok().token != "EOF") {
        size_t oldPos = pos;
        try {
            auto declaration = std::move(parseDeclaration());

            if (!declaration) {
                throw std::runtime_error("FAIL");
            }

            module->declarations.push_back(std::move(declaration));

            if (pos == oldPos) {
                throw std::runtime_error("Parser did not consume any tokens");
            }
        } catch (std::runtime_error e) {
            errors.push_back(e);
            std::cout << e.what() << "\n";
            synchronize({ "RBRACE" });

            if (match("RBRACE")) {
                advance();
            }
        }
    }

    return module;
}

std::unique_ptr<Declaration> Parser::parseDeclaration()
{
    auto name = expect("IDENT").lexeme;
    expect("LPAREN");
    std::vector<std::unique_ptr<VariableDecl>> params;

    if (!match("RPAREN")) {
        params = std::move(parseVarDefs());
        expect("RPAREN");
    }

    if (match("FAT_ARROW")) {
        advance();

        auto predicate = std::move(parsePredicate());

        return std::make_unique<FormulaDecl>(
            name, std::move(params), std::move(predicate));
    }

    auto function = std::make_unique<FunctionDecl>(name, std::move(params));

    if (match("REQUIRES")) {
        advance();
        function->requiresPredicate = std::move(parsePredicate());
    }

    expect("RETURNS");
    function->returns = std::move(parseVarDefs());

    if (match("ENSURES")) {
        advance();
        function->ensures = std::move(parsePredicate());
    }

    if (match("USES")) {
        advance();
        function->locals = std::move(parseVarDefs());
    }

    function->body = std::move(parseStatement());

    return function;
}

std::vector<std::unique_ptr<VariableDecl>> Parser::parseVarDefs()
{
   
    std::vector<std::unique_ptr<VariableDecl>> res;
    res.push_back(parseVarDef());
    while (match("COMMA")) {
        advance();
        res.push_back(parseVarDef());
    }
    return res;
}

std::unique_ptr<VariableDecl> Parser::parseVarDef()
{
    TokenType name = expect("IDENT");
    std::unique_ptr<VariableDecl> res = std::make_unique<VariableDecl>();
    res->name = name.lexeme;
    expect("COLON");

    if (!match("KW_INT")) {
        throw std::runtime_error("WRONG AT " + std::to_string(tok().row) + " "
                                 + std::to_string(tok().column));
    }

    res->type = Type::Int;
    advance();
    if (match("LBRACKET")) {
        advance();
        expect("RBRACKET");
        res->type = Type::IntArray;
    }
    return res;
}

std::unique_ptr<Statement> Parser::parseStatement()
{
    TokenType t = tok();
    if (t.token == "IF") {
        return parseIfStmt();
    }
    if (t.token == "WHILE") {
        return parseWhileStmt();
    }
    if (t.token == "LBRACE") {
        return parseBlock();
    }
    if (t.token == "ASSERT") {
        advance();
        std::unique_ptr<Predicate> cond = parsePredicate();
        expect("SEMICOLON");
        std::unique_ptr<AssertStmt> assert =
            std::make_unique<AssertStmt>(std::move(cond));
        return assert;
    }
    if (t.token == "ASSUME") {
        advance();
        std::unique_ptr<Predicate> cond = parsePredicate();
        expect("SEMICOLON");
        std::unique_ptr<AssumeStmt> assume =
            std::make_unique<AssumeStmt>(std::move(cond));
        return assume;
    }
    if (t.token == "IDENT") {
        return parseAssignment();
    }

    throw std::runtime_error(
        "ERROR parseStatement hasnt found correct token at "
        + std::to_string(tok().row) + " " + std::to_string(tok().column));
}

std::unique_ptr<IfStmt> Parser::parseIfStmt()
{
  
    std::unique_ptr<IfStmt> ifStmt = std::make_unique<IfStmt>();
    advance();
    expect("LPAREN");
    ifStmt->condition = parseCondition();
    expect("RPAREN");
    ifStmt->thenBranch = parseStatement();

    if (match("ELSE")) {
        advance();
        ifStmt->elseBranch = parseStatement();
    }
    return ifStmt;
}

std::unique_ptr<WhileStmt> Parser::parseWhileStmt()
{
   
    std::unique_ptr<WhileStmt> whileStmt = std::make_unique<WhileStmt>();
    advance();
    expect("LPAREN");
    whileStmt->condition = parseCondition();
    expect("RPAREN");
    if (match("INVARIANT")) {
        advance();
        whileStmt->invariant = parsePredicate();
    }
    whileStmt->body = parseStatement();

    return whileStmt;
}

std::unique_ptr<BlockStmt> Parser::parseBlock()
{
    
    std::unique_ptr<BlockStmt> block = std::make_unique<BlockStmt>();
    expect("LBRACE");
    while (!match("RBRACE") && !match("EOF")) {
        int start = this->pos;
        try {
            block->body.push_back(parseStatement());
        } catch (std::runtime_error e) {
            errors.push_back(e);
            std::cout << e.what() << "\n ";
            synchronize({ "SEMICOLON", "RBRACE" });
            if (match("SEMICOLON")) {

                advance();
            }
        }
    }

    if (match("EOF")) {
        throw std::runtime_error("EXPECTED RBRACE AT " + std::to_string(tok().row)
                                 + " " + std::to_string(tok().column));
        return nullptr;
    }

    advance();
    return block;
}

std::unique_ptr<Statement> Parser::parseAssignment()
{
    
    std::string name = tok().lexeme;
    advance();
    if (!match("ASSIGN") && !match("LBRACKET") && !match("COMMA")) {
        std::cout << "ERROR parseAssignment\n";
        return nullptr;
    }
    if (match("LBRACKET")) {
        std::unique_ptr<ArrayAssignmentStmt> arrayAssign =
            std::make_unique<ArrayAssignmentStmt>();
        arrayAssign->arrayName = name;
        advance();
        arrayAssign->index = parseExpr();
        expect("RBRACKET");
        expect("ASSIGN");
        arrayAssign->value = parseExpr();
        expect("SEMICOLON");
        return arrayAssign;
    }
    if (match("COMMA")) {
        std::unique_ptr<TupleAssignmentStmt> tupleAssign =
            std::make_unique<TupleAssignmentStmt>();
        tupleAssign->targets.push_back(name);
        while (match("COMMA")) {
            advance();
            tupleAssign->targets.push_back(expect("IDENT").lexeme);
        }
        expect("ASSIGN");
        if (!(match("IDENT") && peek().token == "LPAREN")) {
            throw std::runtime_error("WRONG TOKEN " + std::to_string(tok().row)
                                     + " " + std::to_string(tok().column));
        }
        tupleAssign->call = parseCall();
        expect("SEMICOLON");
        return tupleAssign;
    }
    std::unique_ptr<AssignmentStmt> assign = std::make_unique<AssignmentStmt>();
    expect("ASSIGN");
    assign->target = name;
    assign->value = parseExpr();
    expect("SEMICOLON");
    return assign;
}

std::unique_ptr<Predicate> Parser::parseCondition()
{
   
    auto left = parseOrExpr();
    if (match("ARROW")) {
        advance();
        auto right = parseCondition();
        return std::make_unique<ImplicationPredicate>(std::move(left),
                                                      std::move(right));
    }

    return left;
}

std::unique_ptr<Predicate> Parser::parseOrExpr()
{
   
    auto left = parseAndExpr();

    while (match("OR")) {
        advance();

        auto right = parseAndExpr();

        left = std::make_unique<LogicalPredicate>(
            LogicalOp::Or, std::move(left), std::move(right));
    }

    return left;
}

std::unique_ptr<Predicate> Parser::parseAndExpr()
{
  
    auto left = parseNotExpr();

    while (match("AND")) {
        advance();

        auto right = parseNotExpr();

        left = std::make_unique<LogicalPredicate>(
            LogicalOp::And, std::move(left), std::move(right));
    }

    return left;
}

std::unique_ptr<Predicate> Parser::parseNotExpr()
{
    
    if (match("NOT")) {
        advance();
        auto notExpr = std::make_unique<NotPredicate>();
        notExpr->operand = parseNotExpr();

        return notExpr;
    }
    return parseAtom();
}

std::unique_ptr<Predicate> Parser::parseAtom()
{
  
    auto t = tok();
    if (match("TRUE")) {
        advance();
        auto boolLit = std::make_unique<BoolPredicate>(true);

        return boolLit;
    }
    if (match("FALSE")) {
        advance();
        auto boolLit = std::make_unique<BoolPredicate>(false);

        return boolLit;
    }
    if (match("FORALL") || match("EXISTS")) {
        return parseQuantifier();
    }
    if (match("LPAREN")) {
        advance();
        auto condition = parseCondition();
        expect("RPAREN");
        return condition;
    }
    if (match("IDENT") && peek().token == "LPAREN") {
        return parseFormulaRef();
    }

    return parseComparison();
}

std::unique_ptr<Predicate> Parser::parseFormulaRef()
{
   
    auto nameToken = advance();
    std::string name = nameToken.lexeme;

    expect("LPAREN");

    std::vector<std::unique_ptr<Expression>> arguments;

    if (!match("RPAREN")) {
        arguments = parseArguments();
    }

    expect("RPAREN");

    return std::make_unique<FormulaCallPredicate>(name, std::move(arguments));
}

std::vector<std::unique_ptr<Expression>> Parser::parseArguments()
{
   
    std::vector<std::unique_ptr<Expression>> arguments;

    arguments.push_back(parseExpr());

    while (match("COMMA")) {
        advance();
        arguments.push_back(parseExpr());
    }

    return arguments;
}

std::unique_ptr<Predicate> Parser::parseComparison()
{
   
    auto left = parseExpr();

    CompareOp op = parseCompareOp();

    auto right = parseExpr();

    return std::make_unique<ComparisonPredicate>(
        op, std::move(left), std::move(right));
}

CompareOp Parser::parseCompareOp()
{
   
    if (match("EQ")) {
        advance();
        return CompareOp::Equal;
    }

    if (match("NEQ")) {
        advance();
        return CompareOp::NotEqual;
    }

    if (match("GE")) {
        advance();
        return CompareOp::GreaterEqual;
    }

    if (match("LE")) {
        advance();
        return CompareOp::LessEqual;
    }

    if (match("GT")) {
        advance();
        return CompareOp::Greater;
    }

    if (match("LT")) {
        advance();
        return CompareOp::Less;
    }

    throw std::runtime_error(
        "Expected comparison operator at " + std::to_string(tok().row) + " "
        + std::to_string(tok().column));
}

std::unique_ptr<Predicate> Parser::parseQuantifier()
{
   
    auto quant = std::make_unique<QuantifierPredicate>();
    quant->type = QuantifierType::Exists;
    if (match("FORALL")) {
        quant->type = QuantifierType::Forall;
    }
    advance();
    expect("LPAREN");
    quant->variable = parseVarDef();
    expect("PIPE");
    quant->body = parsePredicate();
    expect("RPAREN");
    return quant;
}

std::unique_ptr<Expression> Parser::parseExpr()
{
   
    auto left = parseTerm();

    while (match("PLUS") || match("MINUS")) {
        std::string op;

        if (match("PLUS")) {
            op = "+";
        }
        else {
            op = "-";
        }

        advance();

        auto right = parseTerm();

        left =
            std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }

    return left;
}

std::unique_ptr<Expression> Parser::parseTerm()
{
  
    auto left = parseUnary();

    while (match("MUL") || match("DIV")) {
        std::string op = match("MUL") ? "*" : "/";

        advance();

        auto right = parseUnary();

        left =
            std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }

    return left;
}

std::unique_ptr<Expression> Parser::parseUnary()
{
  
    if (match("MINUS")) {
        advance();

        auto operand = parseUnary();

        return std::make_unique<UnaryExpr>(UnaryOp::Negate, std::move(operand));
    }

    return parsePrimary();
}

std::unique_ptr<Expression> Parser::parsePrimary()
{
   
    if (match("INT")) {
        TokenType token = advance();

        return std::make_unique<NumberExpr>(std::stoi(token.lexeme));
    }
    if (match("LENGTH")) {
        return parseCall();
    }

    if (match("LPAREN")) {
        advance();

        auto expr = parseExpr();

        expect("RPAREN");

        return expr;
    }

    if (match("IDENT")) {
        auto next = peek().token;
        if (next == ("LPAREN")) {

            return parseCall();
        }

        if (next == ("LBRACKET")) {
            auto name = tok().lexeme;
            advance();
            advance();
            auto index = parseExpr();

            expect("RBRACKET");

            return std::make_unique<ArrayAccessExpr>(name, std::move(index));
        }

        auto name = tok().lexeme;
        advance();
        return std::make_unique<VariableExpr>(name);
    }

    throw std::runtime_error("WRONG TOKEN " + std::to_string(tok().row) + " "
                             + std::to_string(tok().column));
}

std::unique_ptr<FunctionCallExpr> Parser::parseCall()
{
  
    auto call = std::make_unique<FunctionCallExpr>();
    call->name = advance().lexeme;
    expect("LPAREN");

    if (!match("RPAREN")) {
        call->args.push_back(parseExpr());
        while (match("COMMA")) {
            advance();
            call->args.push_back(parseExpr());
        }
        expect("RPAREN");
    }
    return call;
}

std::unique_ptr<Predicate> Parser::parsePredicate()
{
   
    return parseCondition();
}
