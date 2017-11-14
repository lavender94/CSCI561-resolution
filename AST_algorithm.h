#pragma once

#include "common.h"

#include <string>
#include "Parser.h"

Expression *EXP(Parser &p);
Expression *EXP2(Parser &p);
Expression *EXP3(Parser &p);
Expression *OP(Parser &p);
Expression *FUNC(Parser &p, std::string name);

Expression *buildAST(Parser &p);
