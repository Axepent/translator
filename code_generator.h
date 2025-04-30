#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include <string>
#include "ParserTree.h"

void generateCode(ParserTreeItem_t* root, const std::string& filename);

#endif // CODE_GENERATOR_H
