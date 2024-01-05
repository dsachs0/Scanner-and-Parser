/*
    Scan and parse a json file
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int ERR = -1; //error when scanning
const int EOT = 0; // end of file
const int STRING = 1; // sequence of chars
const int IDENT = 2; // identifier
const int COMMA = 3; // ’,’
const int COLON = 4; // ’:’
const int LBRACE = 5; // ’{’
const int RBRACE = 6; // ’}’
const int LBRACK = 7; // ’[’
const int RBRACK = 8; // ’]'
const int CONST = 9; // ’true’, ’fase’, ’null’, numbers

//struct for Token
struct Token {
int kind;
int lineNo;
char lexeme[100];
};

//array for Token names
char *tokenNames[10] = {
    "<EOF>", "STRING", "IDENT", "COMMA", "COLON",
    "LBRACE", "RBRACE", "LBRACK", "RBRACK", "CONST",
};
//array for token symbols
char *tokenSymbols[10] = {
    "<EOF>", "STRING", "IDENT", ",", ":",
    "{", "}", "[", "]", "CONST",
};

int ind = 0; //global index
struct Token nextToken; //variable for look-ahead token
struct Token *listOfTokens; //array to keep track of tokens
int lineNum = 1; //variable to keep track 
int col, arr = 0; //check if identifiers or strings
int loopInd = 0; //keep track of index for loop
int parseErr = 0; //keep track of errors during parsing

//functions declarations
void match(int x);
void consume();
int readLine(char str[], int n);
void *myMalloc(void *ptr, size_t size);
char *readFile(const char *filename);
struct Token getNextToken(char *c);
//parser functions
void file();
void object();
void array();
void member();
void value();
void literal();
void name();

//main
int main(int argc, char *argv[]) {
    int erro = 0; //variable to check if error in scanning
    char *fileCont; //string for the fileRead buffer
    //copy contents of buffer
    fileCont = readFile(argv[1]);
    //allocate list of tokens
    size_t size = 100;
    listOfTokens = myMalloc(NULL, sizeof(struct Token) * size);

    printf("\nScanning the file...\n"); //output to initialize scanner
    //keep an index for token array
    int j = 0; //index for token array
    //iterate through string turning char into token
    while(ind < strlen(fileCont)){
        //use getNextToken and add to token list
        listOfTokens[j] = getNextToken(fileCont);
        
        //if an error was thrown, catch it and break the loop
        if(listOfTokens[j].kind == ERR){
            printf("Something went wrong! Unterminated STRING in line %d\n\n", listOfTokens[j].lineNo);
            erro = 1;
            break;
        }
        
        //increment variables
        j++;
        ind++;
    }

    //if no error, output successful scan
    if(erro == 0){
        printf("Scanning done!\n\n");
    }

    j = 0; //index for list of tokens
    int l = 0; //index for lexeme
    printf("List of Tokens:\n");
    while(listOfTokens[j].kind != EOT && listOfTokens[j].kind != ERR ){
        //print beginning
        printf("<@%d, <%d,",listOfTokens[j].lineNo, listOfTokens[j].kind);
        l = 0; //reset lexeme index

        //switch statement to output based on token kind
        switch(listOfTokens[j].kind){
            case STRING:
            printf("STRING>=\"");
            while(listOfTokens[j].lexeme[l] != '\0'){
                printf("%c", listOfTokens[j].lexeme[l]);
                l++;
            }
            break;

            case IDENT:
            printf("IDENT>=\"");
            while(listOfTokens[j].lexeme[l] != '\0'){
                printf("%c", listOfTokens[j].lexeme[l]);
                l++;
            }
            break;

            case COMMA:
            printf("COMMA>=\"%C",listOfTokens[j].lexeme[0]);
            break;

            case COLON:
            printf("COLON>=\"%C",listOfTokens[j].lexeme[0]);
            break;

            case LBRACE:
            printf("LBRACE>=\"%C",listOfTokens[j].lexeme[0]);
            break;

            case RBRACE:
            printf("RBRACE>=\"%C",listOfTokens[j].lexeme[0]);
            break;

            case LBRACK:
            printf("LBACK>=\"%C",listOfTokens[j].lexeme[0]);
            break;

            case RBRACK:
            printf("RBACK>=\"%C",listOfTokens[j].lexeme[0]);
            break;

            case CONST:
            printf("CONST>=\"");
            while(listOfTokens[j].lexeme[l] != '\0'){
                printf("%c", listOfTokens[j].lexeme[l]);
                l++;
            }
            break;
            
        }

        printf("\">\n");
        j++;
    }
    
    if(listOfTokens[j].kind == EOT){
        printf("<@%d, <0,EOF>=\"EOF\">\n",listOfTokens[j-1].lineNo);
    }
    else{
        printf("\nCould not parse file.\n");
        return 0;
    }

    /////////PARSER/////////
    printf("\nParsing the file...\n"); //output to initialize parser
    ind = 0;
    nextToken = listOfTokens[ind];
    file();

    //check if there was an error when parsing
    if(parseErr == 0){
        //if no error print done
        printf("Parsing done!\n");
    }
    else{
        //if error, print error, expected symbol and line number
        fprintf(stderr, "Something went wrong! Expecting ’%s’ but found ’%s’ in line %d\n", tokenSymbols[col],
        nextToken.lexeme, nextToken.lineNo);
    }
    return 0;
}

// If lookahead token type matches x, consume and return; otherwise error
void match(int x) {
    if (nextToken.kind == x) consume();
    else {
        col = x; //re-use variable to save index
        parseErr = 1; //record that error happened
    }
}

// Consume the next token in the input
void consume() {
    ind++;
    nextToken = listOfTokens[ind];
}

// Reads an entire line of characters not including the line-feed
int readLine(char str[], int n) {
    int ch, i = 0;
    while ((ch = getchar()) != '\n')
    if (i < n) str[i++] = ch;
        str[i] = '\0';
    return i;
}

// Allocates memory if ptr is null; otherwise, it attemps to
// reallocate memory for it
void *myMalloc(void *ptr, size_t size) {
    ptr = realloc(ptr, size);
    if (!ptr) {
        fprintf(stderr, "Realloc failed to allocated memory.\n");
        exit(1);
    }
    return ptr;
}

char *readFile(const char *filename) {
    FILE *file = fopen(filename, "r"); // opening a file
    if (!file) {
        printf(stderr, "Failed to open file ’%s’.", filename);
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long len = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buffer = myMalloc(NULL, sizeof(char) * (len + 1));
    if (!buffer) {
        fprintf(stderr, "Unable to allocate memory for buffer");
        fclose(file);
        return NULL;
    }
    fread(buffer, 1, len, file);
    buffer[len] = '\0';
    return buffer;
}

//getNextToken function
//uses lookahead char to add token to list
//param: char *c : look-ahead char
struct Token getNextToken(char *c){
    int i = 0; //index for loops
    while(c[ind] != EOF){
        //use switch to check which char index is at
        switch(c[ind]){
            //increment line number counter for newLine char
            case '\n':
                lineNum++;
                ind++;
                continue;
            //consume whitespace and tab chars
            case ' ':
            case '\t':
                ind++;
                continue;
            
            //comma char
            case ',':
            nextToken.kind = COMMA;
            nextToken.lineNo = lineNum;
            nextToken.lexeme[0] = c[ind];
            nextToken.lexeme[1] = '\0';
            return nextToken;

            //left brace char
            case '{':
            nextToken.kind = LBRACE;
            nextToken.lineNo = lineNum;
            nextToken.lexeme[0] = c[ind];
            nextToken.lexeme[1] = '\0';
            col = 0;
            return nextToken;

            //right brace char
            case '}':
            nextToken.kind = RBRACE;
            nextToken.lineNo = lineNum;
            nextToken.lexeme[0] = c[ind];
            nextToken.lexeme[1] = '\0';
            return nextToken;

            //left bracket char
            case '[':
            nextToken.kind = LBRACK;
            nextToken.lineNo = lineNum;
            nextToken.lexeme[0] = c[ind];
            nextToken.lexeme[1] = '\0';
            arr = 1; //set array bool to true
            return nextToken;

            //right bracket char
            case ']':
            nextToken.kind = RBRACK;
            nextToken.lineNo = lineNum;
            nextToken.lexeme[0] = c[ind];
            nextToken.lexeme[1] = '\0';
            arr = 0; //set array bool to false
            return nextToken;
            
            //colon char
            case ':':
            nextToken.kind = COLON;
            nextToken.lineNo = lineNum;
            nextToken.lexeme[0] = c[ind];
            nextToken.lexeme[1] = '\0';
            col = 1; //set colon bool to true
            return nextToken;

            //case for strings
            case '\"':
                ind++; //bypass "
                //iterate through string until second " adding to lexeme
                while(c[ind] != '\"'){
                    nextToken.lexeme[i] = c[ind];

                    //if no second " exists, throw error
                    if(c[ind] == '\0'){
                    nextToken.kind = ERR;
                    nextToken.lineNo = lineNum;
                    return nextToken;
                    }
                    i++;
                    ind++;
                }
                nextToken.lexeme[i] = '\0'; //null terminate lexeme
                //check if identifier or string using colon/arr bools and assign according 
                if(col == 1 || arr == 1){
                    nextToken.kind = STRING;
                    nextToken.lineNo = lineNum;
                    col = 0; //set colon bool to false
                }
                else{
                    nextToken.kind = IDENT;
                    nextToken.lineNo = lineNum;
                }

            return nextToken; 
            
            //default case for constants
            default:
                //consume chars in constants
                while(c[ind] != ',' && c[ind] != ' ' && c[ind] != '\n'){
                    nextToken.lexeme[i] = c[ind];
                    i++;
                    ind++;
                }
                col = 0;
                nextToken.lexeme[i] = '\0'; //null terminate lexeme
                ind--; //go back one so char after const can be checked
                nextToken.kind = CONST;
                nextToken.lineNo = lineNum;
            return nextToken;
        }
    }
    
    //return EOT token if no cases were true
    nextToken.kind = EOT;
    nextToken.lineNo = lineNum;
    return nextToken;
}

//file function to start descent
//if { -> object, } -> array and neither is an error
void file(){
    if(nextToken.kind == LBRACE){
        object();
    }
    else if(nextToken.kind == LBRACK){
        array();
    } 
    else{
        parseErr = 1;
    }
}

//object function for {}
//if { -> member
void object(){
    if(nextToken.kind == LBRACE){
        match(LBRACE);
        if(nextToken.kind == RBRACE){
            match(RBRACE);
        }
        //iterate through member(s)
        else{
            do{
                //consume comma
                if(nextToken.kind == COMMA){
                    match(COMMA);
                }
                member();
            }while(nextToken.kind == COMMA); //if there is no comma, then no more members
        }
    }
    match(RBRACE);
}

//object function for []
//if { -> value
void array(){
    if(nextToken.kind == LBRACK){
        match(LBRACK);
        if(nextToken.kind == RBRACK){
            match(RBRACK);
        }
        //iterate through value(s)
        else{
            do{
                //consume comma
                if(nextToken.kind == COMMA){
                    match(COMMA);
                }
                value();
            }while(nextToken.kind == COMMA); //if there is no comma, then no more value
        }
    }
    match(RBRACK);
}

//member function for object traversal
//if IDENT -> name, : -> match(colon), STRING/CONST -> value
void member(){
    if(nextToken.kind == IDENT){
        name();
    }
    if(nextToken.kind == COLON){
        match(COLON);
    }
    
    if(nextToken.kind != IDENT && nextToken.kind != COLON){
        value();
    }
}

//value function
//check if { or [ and traverse to object and array respectively
//if CONST/STRING -> literal
void value(){
    if(nextToken.kind == LBRACE){
        object();
    }
    if(nextToken.kind == LBRACK){
        array();
    }
    if(nextToken.kind == CONST || nextToken.kind == STRING){
        literal();
    }
}

//literal function for value
//check if STRING or CONST and match respectively
void literal(){
    if(nextToken.kind == STRING){
        match(STRING);
    }
    if(nextToken.kind == CONST){
        match(CONST);
    }
}

//name function
//check if IDENT matches
void name(){
    if(nextToken.kind == IDENT){
        match(IDENT);
    }
}
