#pragma once

#include <variant>
#include <optional>
#include <istream>
#include "error.h"

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

struct SymbolToken {
    SymbolToken() = default;
    SymbolToken(const std::string& other) : word_(other) {
    }

    bool operator==(const SymbolToken& other) const {
        return word_ == other.word_;
    }

private:
    std::string word_;
};

struct ConstantToken {
    ConstantToken() = default;
    ConstantToken(int num) : number_(num) {
    }

    bool operator==(const ConstantToken& other) const {
        return number_ == other.number_;
    }

private:
    int number_;
};

struct QuoteToken {
    QuoteToken() = default;
    bool operator==(const QuoteToken& qt) const {
        return true;
    }
};

struct DotToken {
    DotToken() = default;
    bool operator==(const DotToken& dt) const {
        return true;
    }
};

enum class BracketToken { OPEN, CLOSE };

using Token = std::variant<SymbolToken, ConstantToken, QuoteToken, DotToken, BracketToken>;

class Tokenizer {
public:
    Tokenizer(std::istream* in) {
        str_ = in;
        flag_ = false;
        Next();
    }

    bool IsEnd() {
        return flag_;
    }

    void Next() {
        if (str_->peek() == EOF) {
            flag_ = true;
            return;
        }
        std::string str;
        while (str_->peek() != EOF) {
            if (std::isspace(str_->peek())) {
                str_->get();
                if (str_->peek() == EOF) {
                    flag_ = true;
                    return;
                }
            } else if (str_->peek() == ')') {
                str_->get();
                token_.emplace<BracketToken>(BracketToken::CLOSE);
                break;
            } else if (str_->peek() == '(') {
                str_->get();
                token_.emplace<BracketToken>(BracketToken::OPEN);
                break;
            } else if (str_->peek() == '.') {
                str_->get();
                token_.emplace<DotToken>();
                break;
            } else if (str_->peek() == '\'') {
                str_->get();
                token_.emplace<QuoteToken>();
                break;
            } else if (std::isdigit(str_->peek()) &&
                       (str.empty() || (str.size() == 1 && IsSignSymbol(str.back())) ||
                        std::isdigit(str.back()))) {
                str += str_->get();
                if (!std::isdigit(str_->peek()) || str_->peek() == EOF) {
                    token_.emplace<ConstantToken>(std::atoi(&str[0]));
                    break;
                }
            } else if ((str.empty() && IsStartSymbol(str_->peek())) ||
                       (!str.empty() && IsSymbol(str_->peek()))) {
                str += str_->get();
                if (!IsSymbol(str_->peek())) {
                    token_.emplace<SymbolToken>(str);
                    break;
                }
            } else if (IsSignSymbol(str_->peek())) {
                str += str_->get();
                if (!std::isdigit(str_->peek()) || str_->peek() == EOF) {
                    token_.emplace<SymbolToken>(str);
                    break;
                }
            } else {
                throw SyntaxError("error");
            }
        }
    }

    Token GetToken() {
        return token_;
    }

private:
    bool flag_;
    std::istream* str_;
    Token token_;
};