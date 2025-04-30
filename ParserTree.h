#ifndef __PARSER_TREE_H__
#define __PARSER_TREE_H__

#include <stdio.h>

struct ParserTreeItem_t 
{
    int isTerminal;
    char* name;
    int childrenCount;
    struct ParserTreeItem_t* children[10];
};

void PrintTree(struct ParserTreeItem_t* root, int rd, FILE* f);
void AddChild(struct ParserTreeItem_t* root, char* name, int isTerminal);
void ReturnCursor();

#endif // __PARSER_TREE_H__