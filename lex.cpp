#include "include/common.h"
#include "include/Token.h"
#include <algorithm>

using namespace std;

FILE *fp, *tokensOutput;
int filePointerLocation;
extern int line;
extern int column;


const vector <std::string> RESERVED_KEYWORDS = {
        "auto", "break", "case", "char", "const",
        "continue", "default", "do", "double", "else",
        "enum", "extern", "float", "for", "goto",
        "if", "int", "long", "register", "return",
        "short", "signed", "sizeof", "static", "struct",
        "switch", "typedef", "union", "unsigned", "void",
        "volatile", "while"
};


void readNumber(Token *token, char c) {
    string num;
    num.push_back(c);
    while ((c = next(fp)) != EOF) {
        if (isDigit(c) || c == '.') {
            num.push_back(c);
        } else {
            shift(fp, -1);
            break;
        }
    }
    token->type = NUMBER;
    token->value.number = atoi(num.c_str());
}


char readEscapedChar() {
    switch (next(fp)) {
        case 'a':
            return '\a';
        case 'b':
            return '\b';
        case 'f':
            return '\f';
        case 'n':
            return '\n';
        case 'r':
            return '\r';
        case 't':
            return '\t';
        case 'v':
            return '\v';
        case '\\':
            return '\\';
    }
}

void readString(Token *token) {
    string s;
    char c, prev = 0;
    while ((c = next(fp)) != EOF) {
        if (c == '"' && prev != '\\') {
            break;
        }
        if (c == '\\') {
            c = readEscapedChar();
        }
        s.push_back(c);
        prev = c;
    }
    token->type = STRING;
    token->value.s = s;
}

void readChar(Token *token) {
    char c = next(fp);
    if (c == '\\') {
        c = readEscapedChar();
    }
    next(fp); //TODO Char Literal without closing single quote
    token->type = CHAR;
    token->value.c = c;
}


void readIdentifier(Token *token, char c) {
    string s;
    s.push_back(c);

    while ((c = next(fp)) != EOF) {
        if (isAlphabet(c) || isDigit(c) || c == '_') {
            s.push_back(c);
        } else {
            shift(fp, -1);
            break;
        }
    }
    auto index = distance(RESERVED_KEYWORDS.begin(),
                          find(RESERVED_KEYWORDS.begin(), RESERVED_KEYWORDS.end(), s));

    if (index < RESERVED_KEYWORDS.size()) {
        token->type = index;
    } else {
        token->type = IDENTIFIER;
        token->value.s = s;
    }
}


void skipWhitespaces() {
    char c;
    while ((c = next(fp)) != EOF) {
        if (!isWhitespace(c) && c != '\n') {
            shift(fp, -1);
            break;
        }
        if (c == '\n') {
            line++;
            column = 0;
        } else if (c == '\t') {
            column += 3;   //Don't increment by 4.next() function will increment by 1. 
        }
    }
}


Token *getNextToken() {
    skipWhitespaces();
    filePointerLocation = ftell(fp);

    char c = next(fp);
    Token *token = new Token;
    token->line = line;
    token->column = column;

    switch (c) {
        case ',':
            token->type = COMMA;
            break;
        case '(':
            token->type = OPEN_PAREN;
            break;
        case ')':
            token->type = CLOSE_PAREN;
            break;
        case '[':
            token->type = OPEN_SQUARE;
            break;
        case ']':
            token->type = CLOSE_SQUARE;
            break;
        case '{':
            token->type = OPEN_BRACE;
            break;
        case '}':
            token->type = CLOSE_BRACE;
            break;
        case ';':
            token->type = SEMICOLON;
            break;

        case '0' ... '9':
            readNumber(token, c);
            break;

        case '"':
            readString(token);
            break;

        case '\'' :
            readChar(token);
            break;

        case 'a' ... 'z':
        case 'A' ... 'Z':
        case '_':
            readIdentifier(token, c);
            break;


        case '-':
            c = next(fp);
            if (c == '>') {
                token->type = DEREF;
            } else if (c == '-') {
                token->type = MINUS_MINUS;
            } else if (c == '=') {
                token->type = MINUS_EQ;
            } else {
                shift(fp, -1);
                token->type = MINUS;
            }
            break;
        case '+':
            c = next(fp);
            if (c == '+') {
                token->type = PLUS_PLUS;
            } else if (c == '=') {
                token->type = PLUS_EQ;
            } else {
                shift(fp, -1);
                token->type = PLUS;
            }
            break;
        case '!':
            c = next(fp);
            if (c == '=') {
                token->type = NOT_EQ;
            } else {
                shift(fp, -1);
                token->type = NOT;
            }
            break;
        case '~':
            token->type = COMPL;
            break;

        case '/':
            c = next(fp);
            if (c == '=') {
                token->type = DIV_EQ;
            } else {
                shift(fp, -1);
                token->type = DIV;
            }
            break;

        case '*':
            c = next(fp);
            if (c == '=') {
                token->type = MULT_EQ;
            } else {
                shift(fp, -1);
                token->type = MULT;
            }
            break;

        case '%':
            c = next(fp);
            if (c == '=') {
                token->type = MOD_EQ;
            } else {
                shift(fp, -1);
                token->type = MOD;
            }
            break;

        case '<':
            c = next(fp);
            if (c == '<') {
                c = next(fp);
                if (c == '=') {
                    token->type = LSHIFT_EQ;
                } else {
                    shift(fp, -1);
                    token->type = LSHIFT;
                }
            } else if (c == '=') {
                token->type = LESS_EQ;
            } else {
                shift(fp, -1);
                token->type = LESS;
            }
            break;


        case '>':
            c = next(fp);
            if (c == '>') {
                c = next(fp);
                if (c == '=') {
                    token->type = RSHIFT_EQ;
                } else {
                    shift(fp, -1);
                    token->type = RSHIFT;
                }
            } else if (c == '=') {
                token->type = GREATER_EQ;
            } else {
                shift(fp, -1);
                token->type = GREATER;
            }
            break;

        case '=':
            c = next(fp);
            if (c == '=') {
                token->type = EQ_EQ;
            } else {
                shift(fp, -1);
                token->type = EQ;
            }
            break;

        case '&':
            c = next(fp);
            if (c == '&') {
                token->type = AND_AND;
            } else if (c == '=') {
                token->type = AND_EQ;
            } else {
                shift(fp, -1);
                token->type = LESS;
            }
            break;


        case EOF:
            token->type = TEOF;
            break;
    }

    fputs(token->getFormatted().c_str(), tokensOutput);
    fputc('\n', tokensOutput);
    return token;
}


void ungetToken() {
    fseek(fp, filePointerLocation, SEEK_SET);
}

void lex_initialize(const char *inputFileName) {
    fp = openFile(inputFileName, "r");
    tokensOutput = openFile("data/tokens.txt", "w");
}

void lex_finalize() {
    fclose(fp);
    fclose(tokensOutput);
}



