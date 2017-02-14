#include "../compiler.h"
#include "../Token.h"
#include "../lex.h"

using namespace std;

bool E();
bool E1();
bool T();
bool T1();
bool F();

Token *token;
bool E() {
    if(T()) {
        return E1();
    }
    undoTokenGet();
    return false;
}

bool E1() {
    token = getNextToken();
    if(token->getString() == "+") {
        if(T()) {
            return E1();
        }
    }
    undoTokenGet();
    return true;
}
bool T() {
    if(F()) {
        return T1();
    }
    cout<<token->getFormatted();

    return false;
}
bool T1() {
    token = getNextToken();
    if(token->getString() == "*") {
        if(F()) {
            return T1();
        }
    }
    undoTokenGet();
    return true;
}

bool F() {
    token = getNextToken();
    if (token->getString() == "(") {
        if (E()) {
            token = getNextToken();
            if (token->getString() == ")") {
                return true;
            }
        }
    } else {
        return token->type == Type::IDENTIFIER;
    }

    return false;

}


int main() {

    lex_initialize();
    if(E()) {
        token = getNextToken();
        if(token->type == Type::TEOF) {
            cout << "Success";
        }
        else {
            cout<<"Error";
        }
    }
    else {
        cout<<"Error";
    }

    lex_finalize();
    return 0;
}