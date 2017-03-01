#pragma once

FILE *openFile(const char *, const char *);

void copyFile(const char *, const char *);

char next(FILE *);

void undo(FILE *, char);

void shift(FILE *, int);

char peek(FILE *,int);


inline bool isWhitespace(int c) {
    return c == ' ' || c == '\n' || c == '\t' || c == '\f' || c == '\v';
}

inline bool isAlphabet(char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

inline bool isDigit(char c) {
    return (c >= '0' && c <= '9');
}