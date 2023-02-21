
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include "lexer.h"
#include "lexer_output.h"
#include "utilities.h"

FILE* inFile = NULL;
char fileName[100];

int pendingError = 0;
int state = 0;
int endOfFile = 0;
int i, p = 0, id = 0;
int line = 0;
int column = 0;
char newChar;
int currentLine = 1;
int currentColumn = 0;
char text[1000];
char tokenText[34][100]={".","const",";",",","var","procedure",":=","call","begin","end",
"if","then","else","while","do","read","write","skip","odd","(",")","ident","number","=",
"<>","<","<=",">",">=","+","-","*","/","endofile"};
char tokenType[34][100] = {"periodsym","constsym","semisym","commasym","varsym","procsym",
"becomessym","callsym","beginsym","endsym","ifsym","thensym","elsesym","whilesym","dosym",
"readsym","writesym","skipsym","oddsym","lparensym","rparensym","identsym","numbersym","eqsym",
"neqsym","lessym","leqsym","gtrsym","geqsym","plussym","minussym","multsym","divsym","eofsym"};

// function prototypes
void lexer_open(const char *fname);
FILE* getStream();
void lexer_close();
bool lexer_done();
token lexer_next();
void initToken(token* nextToken);
void populateToken(token* nextToken, int currentLine, int currentColumn, char* text, bool isIdent, bool isNumber);
const char *lexer_filename();
unsigned int lexer_line();
unsigned int lexer_column();

// main
/*int main(int argc, char* argv[]) 
{
    // read arguments
    char* fname = argv[1]; 
    --argc;

    // open in lexer.c
    lexer_open(fname);

    lexer_output();

    // close file
    lexer_close();
}*/

// Requires: fname != NULL
// Requires: fname is the name of a readable file
// Initialize the lexer and start it reading
// from the given file name
void lexer_open(const char *fname)
{
    // copy fname into global fileName
    strcpy(fileName, fname);
    // open file 
    inFile = fopen(fname, "r");
    // error
    if(inFile == NULL) {
        bail_with_error(fname);
    }
}

// Close the file the lexer is working on
// and make this lexer be done
void lexer_close()
{
    fclose(inFile);
}

// Is the lexer's token stream finished
// (either at EOF or not open)?
bool lexer_done()
{
    char tempChar;
    tempChar = getc(inFile);
    if(tempChar == EOF)
        return true;
    else {
        ungetc(tempChar, inFile);
        return false;
    }   
}

// Requires: !lexer_done()
// Return the next token in the input file,
// advancing in the input
token lexer_next()
{
    // create a token
    token* nextToken = malloc(sizeof(token));
    initToken(nextToken);

    // done flag
    int done = 0;
    int error = 0;

    while(endOfFile != 1 && done == 0 && error == 0) {
        switch(state)
        {
            case 0:
                newChar = getc(inFile);
                currentColumn++;
                // Check if the character is alphanumeric
                if (isalnum(newChar)) 
                {
                    // If it's a letter, move to state 11 to read an identifier
                    if (isalpha(newChar))
                        state = 11;
                    // If it's a digit, move to state 13 to read a number
                    else
                        state = 13;
                }
                // lessym - but need to check for =
                // If it's '<', move to state 1 to check for '<=', '<>', or '<'
                else if (newChar == '<') {
                    state = 1;
                }
                // gtrsym - but need to check for =
                // If it's '>', move to state 4 to check for '>=', '>', or '<>'
                else if (newChar == '>') {
                    state = 4;
                }
                // lparensym
                else if (newChar == '(') {
                    populateToken(nextToken, currentLine, currentColumn, "(", false, false);
                    done = 1;
                    state = 0;
                }
                // rparensym
                else if (newChar == ')') {
                    populateToken(nextToken, currentLine, currentColumn, ")", false, false);
                    done = 1;
                    state = 0;
                }
                // plussym
                else if (newChar == '+') {
                    populateToken(nextToken, currentLine, currentColumn, "+", false, false);
                    done = 1;
                    state = 0;
                }
                // minussym
                else if (newChar == '-') {
                    populateToken(nextToken, currentLine, currentColumn, "-", false, false);
                    done = 1;
                    state = 0;
                }
                // multsym
                else if (newChar == '*') {
                    populateToken(nextToken, currentLine, currentColumn, "*", false, false);
                    done = 1;
                    state = 0;
                }
                // divsym
                else if (newChar == '/') {
                    populateToken(nextToken, currentLine, currentColumn, "/", false, false);
                    done = 1;
                    state = 0;
                }
                // colon - must become := becomessym
                // If it's ':', move to state 7 to check for ':='
                else if (newChar == ':') {
                    state = 7;
                }
                // eqsym
                // If it's '=', move to state 9 to handle it as a symbol
                else if (newChar == '=') {
                    state = 9;
                }
                // semisym
                // If it's ';', create a semicolon token and set done to 1
                else if (newChar == ';') {
                    populateToken(nextToken, currentLine, currentColumn, ";", false, false);
                    done = 1;
                    state = 0;
                }
                // periodsym
                // If it's '.', create a period token and set done to 1
                else if (newChar == '.') {
                    populateToken(nextToken, currentLine, currentColumn, ".", false, false);
                    done = 1;
                    state = 0;
                }
                // comment
                // If it's '#', move to state 15 to handle comments
                else if (newChar == '#') {
                    state = 15;
                }
                else if (newChar == '\r') 
                {
		            getc(inFile);
                    currentLine++;
                    currentColumn = 0;
                    state = 0;
                }
                // If it's a newline, increment the current line and reset the current column
                else if (newChar == '\n')
                {
                    currentLine++;
                    currentColumn = 0;
                    state = 0;
                }
                // If it's a space, stay in state 0
                else if (newChar == ' ') {
                    state = 0;
                }
                // commasym
                else if (newChar == ',') {
                    populateToken(nextToken, currentLine, currentColumn, ",", false, false);
                    done = 1;
                    state = 0;
                }
                // eofsym
                else if(newChar == EOF) {
                    populateToken(nextToken, currentLine, currentColumn, "endofile", false, false);
                    done = 1;
                    state = 0;
                }
                else {
                    // error - A character in the input is not one of the characters permitted by the lexical grammar
                    lexical_error(fileName, lexer_line(), lexer_column(), "Illegal character '%c' (%03o)", newChar, newChar);
                    done = 1;
                    state = 0;
                }
                break;
            case 1:
                newChar = getc(inFile);
                currentColumn++;
                if(newChar == '=')
                    state = 2;
                else if (newChar == '>')
                    state = 16;
                else {
                    ungetc(newChar, inFile);
                    currentColumn--;
                    state = 3;
                }
                break;
            // leqsym
            case 2: 
                populateToken(nextToken, currentLine, currentColumn - 1, "<=", false, false);
                done = 1;
                state = 0;
                break;
            // lessym
            case 3:
                populateToken(nextToken, currentLine, currentColumn, "<", false, false);
                done = 1;
                state = 0;
                break;
            case 4:
                newChar = getc(inFile);
                currentColumn++;
                if(newChar == '=')
                    state=5;
                else 
                {
                    ungetc(newChar, inFile);
                    currentColumn--;
                    state = 6;
                }   
                break;
            // geqsym
            case 5:
                populateToken(nextToken, currentLine, currentColumn - 1, ">=", false, false);
                done = 1;
                state = 0;
                break;
            // gtrsym
            case 6:
                populateToken(nextToken, currentLine, currentColumn, ">", false, false);
                done = 1;
                state = 0;
                break;
            case 7:
                newChar = getc(inFile);
                currentColumn++;
                if(newChar == '=')
                    state = 8;
                else if(newChar == ' ') {
                    // error - need = after :
                    lexical_error(fileName, lexer_line(), lexer_column(), "Expecting '=' after a colon, not ' '");
                }
                else {
                    ungetc(newChar, inFile);
                    currentColumn--;
                    state = 0;
                }
                break;
            // becomessym
            case 8:
                populateToken(nextToken, currentLine, currentColumn - 1, ":=", false, false);
                done = 1;
                state = 0;
                break;  
            // equalsym 
            case 9:
                populateToken(nextToken, currentLine, currentColumn, "=", false, false);
                done = 1;
                state = 0;
                break;   
            case 10:
                break;   
            case 11:
                text[p++] = newChar;

                while(isalnum(newChar = getc(inFile))) 
                {
                    currentColumn++;
                    text[p++] = newChar;
                }
                
                ungetc(newChar, inFile);
                text[p] = '\0';
                state = 12;
                p = 0;
                break;
            // identsym
            case 12:
                for(i = 0; i < 34; i++)
                    if(strcmp(tokenText[i], text) == 0) 
                    {
                        populateToken(nextToken, currentLine, currentColumn - strlen(text) + 1, text, false, false);
                        done = 1;
                        id = 1;
                        break;
                    }
                if(id == 0) { 
                    populateToken(nextToken, currentLine, currentColumn - strlen(text) + 1, text, true, false);
                    done = 1;
                    // error - an <ident> has more than MAX_IDENT_LENGTH characters
                    if(strlen(text) > MAX_IDENT_LENGTH) 
                    {
                        char textSubstr[MAX_IDENT_LENGTH + 1];
                        strncpy(textSubstr, text, MAX_IDENT_LENGTH);
                        lexical_error(fileName, lexer_line(), 1, "Identifier starting \"%s\" is too long!", textSubstr);
                    }
                }
                state = 0;
                id = 0;
                break;
            case 13:
                text[p++]=newChar;
                while(isdigit(newChar = getc(inFile))) 
                {
                    currentColumn++;
                    text[p++] = newChar;
                }
                ungetc(newChar, inFile);
                text[p]='\0';
                state = 14;
                p = 0;
                break;
            // numbersym
            case 14:
                populateToken(nextToken, currentLine, currentColumn - strlen(text) + 1, text, false, true);
                done = 1;
                state = 0;
                // error - number's value too large for a C short int
                if(atoi(text) > SHRT_MAX || atoi(text) < SHRT_MIN) 
                {
                    lexical_error(fileName, lexer_line(), 1, "The value of %s is too large for a short!", text);
                }
                break;
            case 15:
                while(newChar != '\n') {
                    if (newChar == EOF) {
                        // error - input ends during a comment
                        lexical_error(fileName, lexer_line(), lexer_column(), "File ended while reading comment!");
                    }
                    newChar = getc(inFile);
                    currentColumn++;
                }
                state = 0;
                currentLine++;
                currentColumn = 0;
                break;
            // neqsym
            case 16:   
                populateToken(nextToken, currentLine, currentColumn - 1, "<>", false, false);
                done = 1;
                state = 0;
                break;
            default:
                break;
        }
        if(newChar == EOF) 
        {
            endOfFile = 1;
        }
    }
    return *nextToken;
}

// initalize an empty token
void initToken(token* nextToken) 
{
    nextToken->line = 0;
    nextToken->column = 0;
    nextToken->text = " ";
    nextToken->filename = " ";
    nextToken->typ = 0;
    nextToken->value = 0;
}

// populate the empty token
void populateToken(token* nextToken, int currentLine, int currentColumn, char* text, bool isIdent, bool isNumber) 
{
    token_type tt = 0;
    char *searchTerm = text;
    // if the token is an identifier, set the search term to "ident"
    if (isIdent) searchTerm = "ident";
    // if the token is a number, set the search term to "number"
    if (isNumber) searchTerm = "number"; 
    // loop through the predefined list of 34 token types
    for(i = 0; i < 34; i++) 
    {
        if(strcmp(tokenText[i], searchTerm) == 0) 
        {
            tt = i;
            break;
        }  
    }
    
    nextToken->line = currentLine;
    nextToken->column = currentColumn;
    // remove text/value for eof
    if(strcmp(text, "endofile") == 0)
        nextToken->text = NULL;
    else 
        nextToken->text = text;

    nextToken->filename = fileName;
    nextToken->typ = tt;
    // if the token is not a number, set the value field of the token structure to 0
    if (!isNumber) 
    {
        nextToken->value = 0;
    } 
    // if the token is a number, convert the token text to an integer and set the value field of the token structure to that value
    else 
    {
        nextToken->value = atoi(text);
    }
}

// Requires: !lexer_done()
// Return the name of the current file
const char *lexer_filename()
{
    // function called from lexer_output.c
    // return the name of the input file
    return &fileName[0];
}

// Requires: !lexer_done()
// Return the line number of the next token
unsigned int lexer_line()
{
    return currentLine;
}

// Requires: !lexer_done()
// Return the column number of the next token
unsigned int lexer_column()
{
    return currentColumn;
}



