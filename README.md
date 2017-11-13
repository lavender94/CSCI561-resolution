# CSCI561-resolution
logic resolution by refutation

## Parser
- Parser.h        header for parser

## AST(Abstract Syntax Tree)
- AST_class.h     definition of AST node
- AST_algorithm.h algorithms to build an AST from a giver parser
- AST.h           header for external use

## Resolution
- resolution.h    algorithms for resolution and unification

## Compilation
- g++ *.cpp -o resolution.exe
- In common.h
  - enable INFO for std print out
  - enable DEBUG for specific resolution procedure
