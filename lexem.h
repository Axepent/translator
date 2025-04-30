#ifndef LAB1_LEXEM_H
#define LAB1_LEXEM_H

#include <map>
#include <vector>
#include <string>

using namespace std;

struct Lexem 
{
    int line;
    int column;
    int id;
    string lexem;
};

extern int ascii[];
extern map<string, int> keywords;
extern map<string, int> identificators;
extern map<string, int> constants;
extern map<string, int> separators;

extern int const_count;
extern int identificators_count;

extern vector<string> errors;
extern bool flag_next_symbol;

vector<Lexem> runLexicalAnalysis(const string& inputFilename, const string& outputTokensFile);

#endif