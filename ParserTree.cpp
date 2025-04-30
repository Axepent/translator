#include "ParserTree.h"
#include <stdlib.h>
#include <stdio.h>

struct ParserTreeItem_t* ParserTreeStack[200];
int ParserStackSize = 0;
struct ParserTreeItem_t* Cursor = NULL;

void PrintTree(struct ParserTreeItem_t* root, int rd, FILE* f) 
{
    for (int i = 0; i < rd; i++) 
    {
        fprintf(f, ".");
    }

    if (root->isTerminal == 1) 
    {
        fprintf(f, "%s\n", root->name);
    }
    else 
    {
        fprintf(f, "<%s>\n", root->name);
    }

    for (int i = 0; i < root->childrenCount; i++) 
    {
        PrintTree(root->children[i], rd + 1, f);
    }
}

void AddChild(struct ParserTreeItem_t* root, char* name, int isTerminal) {
    if (Cursor == NULL) 
    {
        Cursor = root;
    }

    int number = Cursor->childrenCount;

    Cursor->children[number] = (struct ParserTreeItem_t*)calloc(1, sizeof(struct ParserTreeItem_t));

    Cursor->children[number]->isTerminal = isTerminal;
    Cursor->children[number]->name = static_cast<char*>(calloc(64, sizeof(char)));

    for (int i = 0; i < 64; i++) 
    {
        Cursor->children[number]->name[i] = name[i];
        if (name[i] == 0) break;
    }

    ParserTreeStack[ParserStackSize] = Cursor;
    ParserStackSize++;
    Cursor->childrenCount++;
    Cursor = Cursor->children[number];
}

void ReturnCursor() {
    ParserStackSize--;
    Cursor = ParserTreeStack[ParserStackSize];
}
