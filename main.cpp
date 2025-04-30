#include "lexem.h"
#include "parser.h"
#include "ParserTree.h"
#include "code_generator.h"
#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

int main() 
{
    vector<Lexem> tokenList = runLexicalAnalysis("tests/test2.txt", "tokens.txt");
    cout << "\nLexical analysis completed. Lexems saved in tokens.txt\n";

    Parser parser(tokenList);
    parser.parse();

    FILE* f;
    fopen_s(&f, "tree_output.txt", "w");
    if (f) 
    {
        PrintTree(parser.root, 0, f);
        fclose(f);
        cout << "\nSyntax analysis completed. Tree saved in tree_output.txt\n";
    } 
    else 
    {
        cerr << "Failed to open file to write tree!\n";
    }

    if (parser.root) {
        generateCode(parser.root, "output.txt");
        std::cout << "\nCode generation completed! Output file: output.txt\n\n";
    }

    return 0;
}
