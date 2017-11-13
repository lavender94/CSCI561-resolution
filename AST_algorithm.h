#pragma once

#include "common.h"

#include <string>
#include "Parser.h"

Expression *EXP(Parser &p);
Expression *OP(Parser &p);
Expression *FUNC(Parser &p, std::string name);

Expression *buildAST(Parser &p);
