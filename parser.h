#ifndef PARSER_H
#define PARSER_H

#include "lexem.h"
#include "ParserTree.h"

struct Parser 
{
    vector<Lexem> tokens;
    int currentToken;
    ParserTreeItem_t* root;
    ParserTreeItem_t* Cursor;
    Parser(vector<Lexem> lexems);
    void parse();

private:
    Lexem getCurrentToken();
    void advance();
    bool match(int expectedToken);

    void parseSignalProgram();
    void parseProgram();
    void parseProcedureIdentifier();
    void parseBlock();
    void parseDeclarations();
    void parseConstantDeclarations();
    void parseConstantDeclarationsList();
    void parseConstantDeclaration();
    void parseConstantIdentifier();
    void parseIdentifier();
    void parseConstant();
    
    void parseSign(std::string& str);
    void parseUnsignedConstant(std::string& str);
    void parseIntegerPart(std::string& str);
    void parseFractionalPart(std::string& str);
    void parseUnsignedInteger(std::string& str);
    void parseDigitsString(std::string& str);
};

#endif // PARSER_H