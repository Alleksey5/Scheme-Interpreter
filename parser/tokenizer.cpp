#include <tokenizer.h>

bool IsStartSymbol(char a) {
    if (std::isalpha(a) || a == '<' || a == '=' || a == '>' || a == '*' || a == '/' || a == '#') {
        return true;
    } else {
        return false;
    }
}

bool IsSignSymbol(char a) {
    if (a == '+' || a == '-') {
        return true;
    } else {
        return false;
    }
}

bool IsSymbol(char a) {
    if (IsStartSymbol(a) || IsSignSymbol(a) || std::isdigit(a) || a == '?' || a == '!') {
        return true;
    } else {
        return false;
    }
}
