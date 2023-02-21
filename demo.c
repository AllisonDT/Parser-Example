/* $Id: demo.c,v 1.2 2023/02/20 21:20:53 leavens Exp leavens $ */
#include "token.h"
#include "ast.h"
#include "lexer.h"

#define SAMPLEFILE "myfile.pl0"

// build an AST for x := 3402, creating tokens manually
AST* demo()
{
    // construct the token for the identifier x
    token idxtok;
    idxtok.typ = identsym;
    idxtok.filename = SAMPLEFILE;
    idxtok.line = 1;
    idxtok.column = 1;
    idxtok.text = "x";
    idxtok.value = 0;

    // construct the token for the number 3402
    token num3402tok;
    num3402tok.typ = numbersym;
    num3402tok.filename = SAMPLEFILE;
    num3402tok.line = 2;
    num3402tok.column = 1;
    num3402tok.text = "3402";
    num3402tok.value = 3402;

    // construct the AST for the number expression
    AST *numexp = ast_number(num3402tok, num3402tok.value);

    // construct the AST for the assignment statement x := 3402
    AST *stmt = ast_assign_stmt(idxtok, idxtok.text, numexp);
    
    return stmt;
}

// Requires: filename names a readable file containing the characters
//    write x
// as the first two (non-ignored) tokens in the file.
// Build an AST for the statment write x using the given file
// and its tokens.  This shows how to use the lexer to help build ASTs.
AST* demo_with_lexer(const char *filename)
{
    printf("%s", filename);
    lexer_open(filename);
    token wrtok = lexer_next();
    token idxtok = lexer_next();
    lexer_close();

    AST *ident_exp = ast_ident(idxtok, idxtok.text);
    return ast_write_stmt(wrtok, ident_exp);
}