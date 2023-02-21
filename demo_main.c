#include <stdlib.h>
#include "demo.h"
#include "unparser.h"

// Run the unparser on the demo AST,
// sending the output to stdout.
int main()
{
    printf("first demo...\n");
    AST *stmt1 = demo();
    unparseStmt(stdout, stmt1, 0, false);
    printf("\n");

    printf("second demo...\n");
    AST *stmt2 = demo_with_lexer("write_x.pl0");
    // To show the parameters of unparseStmt,
    // the following call indents the output 1 level
    // and adds a semicolon to the end
    unparseStmt(stdout, stmt2, 1, true);
    return EXIT_SUCCESS;
}