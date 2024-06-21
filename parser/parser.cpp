#include <parser.h>

std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    Token tmp = tokenizer->GetToken();
    if (tokenizer->IsEnd()) {
        throw SyntaxError("Error in reading");
    } else if (std::holds_alternative<ConstantToken>(tmp)) {
        tokenizer->Next();
        return std::make_shared<Number>(std::get<ConstantToken>(tmp).GetVal());
    } else if (std::holds_alternative<SymbolToken>(tmp)) {
        tokenizer->Next();
        return std::make_shared<Symbol>(std::get<SymbolToken>(tmp).GetVal());
    } else if (std::holds_alternative<BracketToken>(tmp)) {
        if (std::get<BracketToken>(tmp) == BracketToken::OPEN) {
            return ReadList(tokenizer);
        } else {
            throw SyntaxError("Error");
        }
    } else if (std::holds_alternative<DotToken>(tmp)) {
        tokenizer->Next();
        return std::make_shared<Symbol>(".");
    }
    throw SyntaxError("Invalid syntax");
}

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {
    std::shared_ptr<Cell> cell = std::make_shared<Cell>();
    std::shared_ptr<Cell> el = cell;
    int num = 0;
    tokenizer->Next();
    while (tokenizer->GetToken() != Token{BracketToken::CLOSE}) {
        std::shared_ptr<Object> obj = Read(tokenizer);
        if (obj == nullptr) {
            tokenizer->Next();
            return cell;
        }
        if (Is<Symbol>(obj)) {
            if (As<Symbol>(obj)->GetName() == ".") {
                Token tmp = tokenizer->GetToken();
                if (tmp == Token{BracketToken::OPEN}) {
                    el->GetSecond() = ReadList(tokenizer);
                } else if (std::holds_alternative<ConstantToken>(tmp)) {
                    tokenizer->Next();
                    if (tokenizer->GetToken() != Token{BracketToken::CLOSE}) {
                        throw SyntaxError("Invalid syntax");
                    }
                    el->GetSecond() =
                        std::make_shared<Number>(std::get<ConstantToken>(tmp).GetVal());
                } else {
                    throw SyntaxError("Invalid syntax");
                }
            }
        }
        if (Is<Cell>(obj) || Is<Number>(obj) ||
            (Is<Symbol>(obj) && As<Symbol>(obj)->GetName() != ".")) {
            if (num > 0) {
                el->GetSecond() = std::make_shared<Cell>();
                el = As<Cell>(el->GetSSecond());
            }
            el->GetFirst() = obj;
        }
        if (tokenizer->IsEnd()) {
            throw SyntaxError("Bad string");
        }
        num++;
    }
    tokenizer->Next();
    return num ? cell : nullptr;
}