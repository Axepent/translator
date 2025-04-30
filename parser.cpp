#include "parser.h"
#include <iostream>
#include <cstring>

Parser::Parser(vector<Lexem> lexems) : tokens(lexems), currentToken(0) 
{
    root = (ParserTreeItem_t*)calloc(1, sizeof(ParserTreeItem_t));
    root->name = (char*)"<signal-program>";
    root->isTerminal = 0;
    root->childrenCount = 0;
    Cursor = root;
}

void Parser::parse() 
{
    parseSignalProgram();
}

Lexem Parser::getCurrentToken() 
{
    if (currentToken < tokens.size()) 
    {
        return tokens[currentToken];
    }
    return Lexem{0, 0, -1, ""};
}

void Parser::advance() 
{
    if (currentToken < tokens.size()) currentToken++;
}

bool Parser::match(int expectedToken) 
{
    Lexem tok = getCurrentToken();
    if (tok.id == expectedToken) 
    {
        AddChild(Cursor, (char*)tok.lexem.c_str(), 1);
        advance();
        return true;
    } 
    else 
    {
        std::cerr << "Syntax error at line " << tok.line << ", column " << tok.column
                  << ": expected token id " << expectedToken << ", got " << tok.id << std::endl;
        return false;
    }
}

void Parser::parseSignalProgram() 
{
    AddChild(Cursor, (char*)"<program>", 0);
    parseProgram();
    ReturnCursor();
}

void Parser::parseProgram() 
{
    match(401); // PROGRAM
    AddChild(Cursor, (char*)"<procedure-identifier>", 0);
    parseProcedureIdentifier();
    ReturnCursor();
    match(59); // ;
    AddChild(Cursor, (char*)"<block>", 0);
    parseBlock();
    ReturnCursor();
    ReturnCursor();
    match(46); // .
    ReturnCursor();
}

void Parser::parseProcedureIdentifier() 
{
    AddChild(Cursor, (char*)"<identifier>", 0);
    parseIdentifier();
    ReturnCursor();
    ReturnCursor();
}

void Parser::parseBlock() 
{
    AddChild(Cursor, (char*)"<declarations>", 0);
    parseDeclarations();
    ReturnCursor();
    ReturnCursor();
    match(402); // BEGIN
    AddChild(Cursor, (char*)"<statements-list>", 0);
    ReturnCursor();
    ReturnCursor();
    match(403); // END
    ReturnCursor();
}

void Parser::parseDeclarations() 
{
    if (getCurrentToken().id == 404) // CONST
    {
        AddChild(Cursor, (char*)"<constant-declarations>", 0);
        parseConstantDeclarations();
        ReturnCursor();
    } 
    else 
    {
        AddChild(Cursor, (char*)"<empty>", 0);
    }
    ReturnCursor();
}

void Parser::parseConstantDeclarations() 
{
    match(404); // CONST
    AddChild(Cursor, (char*)"<constant-declarations-list>", 0);
    parseConstantDeclarationsList();
    ReturnCursor();
    ReturnCursor();
}

void Parser::parseConstantDeclarationsList() 
{
    AddChild(Cursor, (char*)"<constant-declaration>", 0);
    parseConstantDeclaration();
    ReturnCursor();

    while (getCurrentToken().id >= 1001) 
    {
        AddChild(Cursor, (char*)"<constant-declarations-list>", 0);
        parseConstantDeclaration();
        ReturnCursor();
        ReturnCursor();
    }
}

void Parser::parseConstantDeclaration() 
{
    AddChild(Cursor, (char*)"<constant-identifier>", 0);
    parseConstantIdentifier();

    match(61); // =
    ReturnCursor();

    AddChild(Cursor, (char*)"<constant>", 0);
    parseConstant();
    ReturnCursor();

    match(59); // ;
    ReturnCursor(); 
}

void Parser::parseConstantIdentifier() {
    AddChild(Cursor, (char*)"<identifier>", 0);
    parseIdentifier();
    ReturnCursor();
}

void Parser::parseIdentifier() {
    if (getCurrentToken().id >= 1001) 
    {
        AddChild(Cursor, (char*)getCurrentToken().lexem.c_str(), 1);
        advance();
    } 
    else 
    {
        std::cerr << "Expected identifier at line " << getCurrentToken().line << std::endl;
    }
}

void Parser::parseConstant() 
{
    Lexem tok = getCurrentToken();
    if (tok.id <= 501 && tok.id > 1001) 
    {
        std::cerr << "Error: expected constant at line " << tok.line << std::endl;
        return;
    }

    std::string lex = tok.lexem;
    AddChild(Cursor, (char*)"<sign>", 0);
    parseSign(lex);
    ReturnCursor();

    AddChild(Cursor, (char*)"<unsigned-constant>", 0);
    parseUnsignedConstant(lex);
    ReturnCursor();

    advance(); 
}

void Parser::parseSign(std::string& str) 
{
    if (str.empty()) 
    {
        AddChild(Cursor, (char*)"<empty>", 0);
        ReturnCursor();
        ReturnCursor();
        return;
    }

    char first = str[0];
    if (first == '+' || first == '-') 
    {
        char signStr[2] = { first, 0 };
        AddChild(Cursor, signStr, 1);
        str = str.substr(1);
        ReturnCursor();
        ReturnCursor();
    } 
    else 
    {
        AddChild(Cursor, (char*)"<empty>", 0);
        ReturnCursor();
        ReturnCursor();
    }
}

void Parser::parseUnsignedConstant(std::string& str) 
{
    AddChild(Cursor, (char*)"<integer-part>", 0);
    parseIntegerPart(str);
    ReturnCursor();

    AddChild(Cursor, (char*)"<fractional-part>", 0);
    parseFractionalPart(str);
    ReturnCursor();
}

void Parser::parseIntegerPart(std::string& str) 
{
    AddChild(Cursor, (char*)"<unsigned-integer>", 0);
    parseUnsignedInteger(str);
    ReturnCursor();
}

void Parser::parseFractionalPart(std::string& str) 
{
    if (!str.empty() && str[0] == '#') 
    {
        AddChild(Cursor, (char*)"#", 1);
        str = str.substr(1);
        AddChild(Cursor, (char*)"<sign>", 0);
        parseSign(str);
        ReturnCursor();
        AddChild(Cursor, (char*)"<unsigned-integer>", 0);
        parseUnsignedInteger(str);
    } 
    else 
    {
        AddChild(Cursor, (char*)"<empty>", 0);
        ReturnCursor();
    }
}

void Parser::parseUnsignedInteger(std::string& str) 
{
    if (str.empty() || !isdigit(str[0])) 
    {
        AddChild(Cursor, (char*)"<empty>", 0);
        ReturnCursor();
        return;
    }

    char digitStr[2] = { str[0], 0 };
    AddChild(Cursor, digitStr, 1);
    str = str.substr(1);
    ReturnCursor();

    AddChild(Cursor, (char*)"<digits-string>", 0);
    parseDigitsString(str);
    ReturnCursor();
}

void Parser::parseDigitsString(std::string& str) 
{
    while (!str.empty() && isdigit(str[0])) 
    {
        char digitStr[2] = { str[0], 0 };
        AddChild(Cursor, digitStr, 1);
        str = str.substr(1);
    }
    if (str.empty() || !isdigit(str[0])) 
    {
        AddChild(Cursor, (char*)"<empty>", 0);
    }
    ReturnCursor();
}