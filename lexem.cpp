#include "lexem.h"
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;
/*
    0 - whitespace (8-13, 32)
    1 - digits (48-57)
    2 - letters (65-90)
    3 - separetors (35, 43, 45, 46, 59, 61)
    4 - long separator (-)
    5 - comment starting symbol (40)
    6 - banned symbols
*/

int ascii[] = { 6,6,6,6,6,6,6,6,0,0,0,0,0,0,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6, // 0-31  
    0,6,6,3,6,6,6,6,5,6,6,3,6,3,3,6,1,1,1,1,1,1,1,1,1,1,6,3,6,3,6,6, // 32-63  
    6,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,6,6,6,6,6, // 64-95 
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6  // 96-127
};
map <string, int> keywords = { {"PROGRAM", 401}, {"BEGIN", 402}, {"END", 403}, {"CONST", 404} };
map <string, int> identificators = {};
map <string, int> constants = {};
map <string, int> separators = { {".", 46}, {";", 59}, {"#", 35}, {"=", 61}, {"+", 43}, {"-", 45}  }; 

int const_count = 501;
int identificators_count = 1001;

vector<string> errors;
bool flag_next_symbol = true;

void printLexem(Lexem lexem) 
{
    cout << "|" << setw(10) << lexem.line << "|" << setw(10) << lexem.column << "|" << setw(10) << lexem.id << "|" << setw(10) << lexem.lexem << "|" << '\n';
}

int searchConst(const string& str) 
{
    auto it = constants.find(str);

    if (it != constants.end()) 
        return it->second;
    return -1;
}

int searchKeyword(const string& str) 
{
    auto it = keywords.find(str);

    if (it != keywords.end())
        return it->second;
    return -1;
}

int searchIdentifier(const string& str) 
{
    auto it = identificators.find(str);

    if (it != identificators.end())
        return it->second;
    return -1;
}

int searchSeparator(string separators_str) 
{
    auto it = separators.find(separators_str);
    
    if (it != separators.end())
        return it->second;
    return -1;
}

vector<Lexem> runLexicalAnalysis(const string& inputFilename, const string& outputTokensFile)
{
    vector<Lexem> tokenList;
    Lexem token;
    int line = 1, column = 1;
    char symbol;
    int symbol_ascii;
    flag_next_symbol = true;

    ifstream file(inputFilename);
    if (!file.is_open()) 
    {
        cout << "File is not opening" << '\n';
        return tokenList;
    }

    ofstream tokenOut(outputTokensFile);
    if (!tokenOut.is_open()) {
        cout << "Failed to open tokens.txt for writing!" << '\n';
        return tokenList;
    }
    /*
    cout << "---------------------------------------------" << '\n';
    cout << "|" << setw(10) << "Line" << "|" << setw(10) << "Column" << "|" << setw(10) << "Id" << "|" << setw(10) << "Lexem" << "|" << '\n';
    cout << "---------------------------------------------" << '\n';
    */
    while (!file.eof()) 
    {
        if (flag_next_symbol) 
        {
            file.get(symbol);
            symbol_ascii = (int)symbol;
        }
        else
            flag_next_symbol = true;

        if (symbol_ascii >= 0 && symbol_ascii <= 127) 
        {
            switch (ascii[symbol_ascii]) 
            {
                // 0 - whitespace
                case 0:
                {
                    if (symbol == '\t') 
                        column = column - (column - 1) % 4 + 4;
                    else
                    { 
                        if (symbol == '\n') 
                        {
                            line++;
                            column = 1;
                        }
                        else
                            column++;
                    }

                    token.lexem = "";
                    break;
                }
                // 1 - constant
                case 1:
                {
                    token.line = line;
                    token.column = column;
                    token.lexem += symbol;
                    file.get(symbol);
                    symbol_ascii = (int)symbol;
                    column++;

                    while (ascii[symbol_ascii] == 1) 
                    {
                        token.lexem += symbol;
                        file.get(symbol);
                        symbol_ascii = (int)symbol;
                        column++;
                    }
                    
                    if (symbol == '#') 
                    {
                        token.lexem += symbol;
                        file.get(symbol);
                        symbol_ascii = (int)symbol;
                        column++;
                        
                        bool second_const = false;

                        while (ascii[symbol_ascii] == 1) 
                        {
                            second_const = true;
                            token.lexem += symbol;
                            file.get(symbol);
                            symbol_ascii = (int)symbol;
                            column++;
                        }

                        if (symbol == '+' || symbol == '-') 
                        {
                            second_const = true;
                            token.lexem += symbol;
                            file.get(symbol);
                            symbol_ascii = (int)symbol;
                            column++;

                            while (ascii[symbol_ascii] == 1) 
                            {
                                token.lexem += symbol;
                                file.get(symbol);
                                symbol_ascii = (int)symbol;
                                column++;
                            }
                        }

                        if (symbol == ';' && second_const) 
                        {
                            if (searchConst(token.lexem) == -1) 
                            {
                                token.id = constants.size() + const_count;
                                constants[token.lexem] = token.id;
                            }
                            else
                                token.id = searchConst(token.lexem);

                            tokenOut << token.line << " " << token.column << " " << token.id << " " << token.lexem << "\n";
                            tokenList.push_back(token);
                            token.lexem = "";
                            flag_next_symbol = false;
                            break;
                        }
                        else 
                        {
                            if (ascii[symbol_ascii] == 0) 
                            {
                                if (symbol == '\t') 
                                        column = column - (column - 1) % 4 + 4;
                                    else
                                    { 
                                        if (symbol == '\n') 
                                        {
                                            line++;
                                            column = 1;
                                        }
                                        else
                                            column++;
                                    }
                                    errors.push_back("Error! In line " + to_string(line) + " in column " + to_string(column) + " invalid constant " + token.lexem + "!");
                                    token.lexem = "";
                                    break;
                            }
                            else 
                            {
                                errors.push_back("Error! In line " + to_string(line) + " in column " + to_string(column) + " invalid constant " + token.lexem + "!");
                                token.lexem = "";
                                break;
                            }
                        }
                    }

                    else 
                    {
                        if (symbol == ';') 
                        {
                            if (searchConst(token.lexem) == -1) 
                            {
                                token.id = constants.size() + const_count;
                                constants[token.lexem] = token.id;
                            }
                            else
                                token.id = searchConst(token.lexem);

                            tokenOut << token.line << " " << token.column << " " << token.id << " " << token.lexem << "\n";
                            tokenList.push_back(token);
                            token.lexem = "";
                            flag_next_symbol = false;
                            break;
                        }

                        else 
                        {
                            if (ascii[symbol_ascii] == 0) 
                            {
                                if (symbol == '\t') 
                                        column = column - (column - 1) % 4 + 4;
                                    else
                                    { 
                                        if (symbol == '\n') 
                                        {
                                            line++;
                                            column = 1;
                                        }
                                        else
                                            column++;
                                    }
                                    errors.push_back("Error! In line " + to_string(line) + " in column " + to_string(column) + " invalid constant " + token.lexem + "!");
                                    token.lexem = "";
                                    break;
                            }
                            else 
                            {
                                errors.push_back("Error! In line " + to_string(line) + " in column " + to_string(column) + " invalid constant " + token.lexem + "!");
                                token.lexem = "";
                                break;
                            }
                        }
                    }
                }
                // 2 - identifier or keyword
                case 2:
                {
                    token.line = line;
                    token.column = column;
                    token.lexem += symbol;
                    file.get(symbol);
                    symbol_ascii = (int)symbol;
                    column++;

                    while (ascii[symbol_ascii] == 2 || ascii[symbol_ascii] == 1) 
                    {
                        token.lexem += symbol;
                        file.get(symbol);
                        symbol_ascii = (int)symbol;
                        column++;
                    }

                    if (ascii[symbol_ascii] == 0 || ascii[symbol_ascii] == 3 || ascii[symbol_ascii] == 5) 
                    {
                        if (searchKeyword(token.lexem) != -1)
                            token.id = searchKeyword(token.lexem);
                        else 
                        {
                            if (searchIdentifier(token.lexem) != -1) 
                                token.id = searchIdentifier(token.lexem);
                            else 
                            {
                                token.id = identificators.size() + identificators_count;
                                identificators[token.lexem] = token.id;
                            }       
                        }
                        
                        tokenOut << token.line << " " << token.column << " " << token.id << " " << token.lexem << "\n";
                        tokenList.push_back(token);
                        token.lexem = "";
                        flag_next_symbol = false;
                        break;
                    }

                    else 
                    {
                        token.lexem = "";
                        errors.push_back("Error! In line " + to_string(line) + " in column " + to_string(column) + " not valid identifier or keyword!");
                        break;
                    }
                }
                // 3 - separators
                case 3:
                {
                    token.line = line;
                    token.column = column;
                    token.lexem += symbol;

                    if (symbol == '+' || symbol == '-') 
                    {
                        file.get(symbol);
                        symbol_ascii = (int)symbol;
                        column++;

                        while (ascii[symbol_ascii] == 1) 
                        {
                            token.lexem += symbol;
                            file.get(symbol);
                            symbol_ascii = (int)symbol;
                            column++;
                        }

                        if (symbol == '#') 
                        {
                            token.lexem += symbol;
                            file.get(symbol);
                            symbol_ascii = (int)symbol;
                            column++;
                            bool second_const = false;

                            while (ascii[symbol_ascii] == 1) 
                            {
                                second_const = true;
                                token.lexem += symbol;
                                file.get(symbol);
                                symbol_ascii = (int)symbol;
                                column++;
                            }

                            if (symbol == '+' || symbol == '-') 
                            {
                                second_const = true;
                                token.lexem += symbol;
                                file.get(symbol);
                                symbol_ascii = (int)symbol;
                                column++;

                                while (ascii[symbol_ascii] == 1) 
                                {
                                    token.lexem += symbol;
                                    file.get(symbol);
                                    symbol_ascii = (int)symbol;
                                    column++;
                                }
                            }
                            if (symbol == ';' && second_const) 
                            {
                                if (searchConst(token.lexem) == -1) 
                                {
                                    token.id = constants.size() + const_count;
                                    constants[token.lexem] = token.id;
                                }
                                else
                                    token.id = searchConst(token.lexem);

                                tokenOut << token.line << " " << token.column << " " << token.id << " " << token.lexem << "\n";
                                tokenList.push_back(token);
                                token.lexem = "";
                                flag_next_symbol = false;
                                continue;
                            }
                            else 
                            {
                                errors.push_back("Error! In line " + to_string(line) + " in column " + to_string(column) + " invalid constant " + token.lexem);
                                token.lexem = "";
                                break;
                            }
                        }

                        else 
                        {
                            if (symbol == ';') 
                            {
                                if (searchConst(token.lexem) == -1) 
                                {
                                    token.id = constants.size() + const_count;
                                    constants[token.lexem] = token.id;
                                }
                                else
                                    token.id = searchConst(token.lexem);

                                tokenOut << token.line << " " << token.column << " " << token.id << " " << token.lexem << "\n";
                                tokenList.push_back(token);
                                token.lexem = "";
                                flag_next_symbol = false;
                                break;
                            }
                            else 
                            {
                                token.lexem = "";
                                errors.push_back("Error! In line " + to_string(line) + " in column " + to_string(column) + " invalid constant!");
                                break;
                            }
                        }
                    }

                    else 
                    {
                        token.id = searchSeparator(token.lexem);
                        column++;
                        tokenOut << token.line << " " << token.column << " " << token.id << " " << token.lexem << "\n";
                        tokenList.push_back(token);
                        token.lexem = "";
                        break;
                    }
                }
                // 5 - comments
                case 5:
                {
                    token.lexem += symbol;
                    file.get(symbol);
                    symbol_ascii = (int)symbol;
                    column++;

                    if (!file.eof()) 
                    {
                        if (symbol == '*') 
                        {
                            token.line = line;
                            token.column = column - 1;
                            token.lexem += symbol;
                            bool EndStar = false;
                            bool EndComment = false;
                            bool ErrorCheck = false;

                            while (true) 
                            {
                                file.get(symbol);
                                symbol_ascii = (int)symbol;
                                token.lexem += symbol;

                                if (!file.eof()) 
                                {
                                    if (symbol == '*') 
                                    {
                                        EndStar = true;
                                        column++;
                                    }
                                    else 
                                    {
                                        if (symbol == ')' && EndStar) 
                                        {
                                            EndComment = true;
                                            column++;
                                            break;
                                        }
                                        else 
                                        {
                                            EndStar = false;
                                            if (symbol == '\n') 
                                            {
                                                line++;
                                                column = 1;
                                            }
                                            else 
                                            {
                                                if (symbol == '\t')
                                                    column = column - (column - 1) % 4 + 4;
                                                else
                                                    column++;
                                            }
                                        }
                                    }
                                }

                                else 
                                {
                                    errors.push_back("Error! Incorrect comment! Comment is not closed!");
                                    token.lexem = "";
                                    ErrorCheck = true;
                                    break;
                                }
                            }

                            if (EndComment) 
                            {
                                token.lexem = "";
                                break;
                            }
                            else 
                            {
                                if (!ErrorCheck) 
                                {
                                    errors.push_back("Error! Incorrect comment! Comment is not closed!");
                                    token.lexem = "";
                                    break;
                                }
                            }
                        }
                    }

                    else 
                    {
                        errors.push_back("Error! Incorrect comment! Started at the end of file!");
                        flag_next_symbol = false;
                        break;
                    }
                }
                // 6 - banned symbol
                case 6:
                {
                    token.line = line;
                    token.column = column;
                    token.lexem += symbol;
                    errors.push_back("Error! In line " + to_string(line) + " in column " + to_string(column) + " invalid symbol " + token.lexem);
                    column++;
                    token.lexem = "";
                    break;
                }
            }
        }
        else
        {
            errors.push_back("Error! In line " + to_string(line) + " in column " + to_string(column) + " symbol is not from default ASCII!");
            column++;
        }
    }

    //cout << "---------------------------------------------" << "\n\n";
    for (size_t i = 0; i < errors.size(); ++i) {
        cout << errors[i] << '\n';
    }

    file.close();
    tokenOut.close();
    return tokenList;
}