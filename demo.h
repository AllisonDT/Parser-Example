/* $Id: demo.h,v 1.2 2023/02/20 21:20:53 leavens Exp leavens $ */
#ifndef _DEMO_H
#define _DEMO_H
#include "token.h"
#include "ast.h"
#include "unparser.h"

// build an AST for x := 3402, creating tokens manually
extern AST* demo();

// Requires: filename names a readable file containing the characters
//    write x
// as the first two (non-ignored) tokens in the file.
// Build an AST for the statment write x using the given file
// and its tokens.  This shows how to use the lexer to help build ASTs.
extern AST* demo_with_lexer(const char *filename);

#endif