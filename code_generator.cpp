#include "code_generator.h"
#include <cstring>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

struct Token 
{
    int line;
    int column;
    int id;
    std::string lexem;
};

std::vector<Token> tokens;
size_t tokenIndex = 0;

void loadTokens(const std::string& filename) 
{
    tokens.clear();
    tokenIndex = 0;

    std::ifstream file(filename);
    if (!file.is_open()) 
    {
        std::cerr << "Cannot open tokens file!\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) 
    {
        std::istringstream iss(line);
        Token tok;
        iss >> tok.line >> tok.column >> tok.id >> tok.lexem;
        tokens.push_back(tok);
    }

    file.close();
}

void readTree(ParserTreeItem_t* root, FILE* f) {
    if (!root || !root->name) return;

    if (strcmp(root->name, "BEGIN") == 0) {
        fprintf(f, "\ndata ENDS\n\ncode SEGMENT\n\tASSUME cs:code, ds:data\n");
    }
    else if (strcmp(root->name, "END") == 0) {
        fprintf(f, "\n\tret 0\n\n\tmov ax, 4c00h\n\tint 21h\ncode ENDS\n\tend begin\n");
    }
    else if (strcmp(root->name, "<declarations>") == 0) {
        fprintf(f, "\ndata SEGMENT\n\n");
    }
    else if (strcmp(root->name, "<constant-declaration>") == 0) 
    {
        while (tokenIndex < tokens.size()) 
        {
            Token current = tokens[tokenIndex];
            if (current.id == 402) {
                break;
            }
    
            if (tokenIndex + 3 < tokens.size()) 
            {
                Token identifier = tokens[tokenIndex];
                Token assign = tokens[tokenIndex + 1];
                Token constant = tokens[tokenIndex + 2];
                Token semicolon = tokens[tokenIndex + 3];
    
                if (identifier.id >= 1001 && assign.id == 61 && constant.id >= 501 && semicolon.id == 59) 
                {
                    fprintf(f, "%s equ %s\n", identifier.lexem.c_str(), constant.lexem.c_str());
                    //fprintf(f, "MOV_%s, AX\n", constant.lexem.c_str());
                    tokenIndex += 4; 
                }
                else 
                {
                    tokenIndex++;
                }
            }
            else 
            {
                std::cerr << "Error! Imbosible to read CONST (generate)!\n";
                break;
            }
        }
    }
    

    for (int i = 0; i < root->childrenCount; i++) 
    {
        readTree(root->children[i], f);
    }
}

void generateCode(ParserTreeItem_t* root, const std::string& filename) {
    FILE* f;
    fopen_s(&f, filename.c_str(), "w");
    if (!f) {
        std::cerr << "Cannot open output file!\n";
        return;
    }

    loadTokens("tokens.txt");
    readTree(root, f);
    fclose(f);
}
