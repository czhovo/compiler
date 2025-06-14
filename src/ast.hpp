#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <cassert>


class BaseAST {
public:
    virtual ~BaseAST() = default;
    virtual void Dump() const = 0;
    virtual std::string GenerateIR() const = 0;
};

class ExpAST : public BaseAST {
protected:
    inline static int tmp_counter;
    mutable std::string result_var;

    std::string NewTemp() const {
        result_var = "%" + std::to_string(tmp_counter++);
        return result_var;
    }

public:
    static void ResetCounter() { tmp_counter = 0; }
    virtual std::string GetResult() const { return result_var; }
};


class CompUnitAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> func_def;

    void Dump() const override {
        std::cout << "CompUnitAST {\n  ";
        func_def->Dump();
        std::cout << "\n}";
    }

    std::string GenerateIR() const override {
        ExpAST::ResetCounter();
        return func_def->GenerateIR();
    }
};

class FuncDefAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> func_type;
    std::string ident;
    std::unique_ptr<BaseAST> block;

    void Dump() const override {
        std::cout << "FuncDefAST {\n  ";
        func_type->Dump();
        std::cout << " " << ident << ",\n  ";
        block->Dump();
        std::cout << "\n}";
    }

    std::string GenerateIR() const override {
        std::string ir;
        ir += "fun @" + ident + "(): i32 {\n";
        ir += "%entry:\n";
        ir += block->GenerateIR();
        ir += "}\n";
        return ir;
    }
};

class FuncTypeAST : public BaseAST {
public:
    std::string type;

    void Dump() const override {
        std::cout << "FuncTypeAST { " << type << " }";
    }

    std::string GenerateIR() const override {
        return "";
    }
};

class BlockAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> stmt;

    void Dump() const override {
        std::cout << "BlockAST { ";
        stmt->Dump();
        std::cout << " }";
    }

    std::string GenerateIR() const override {
        return stmt->GenerateIR();
    }
};

class StmtAST : public BaseAST {
public:
    std::unique_ptr<ExpAST> exp;

    void Dump() const override {
        std::cout << "StmtAST { return ";
        exp->Dump();
        std::cout << " }";
    }

    std::string GenerateIR() const override {
        return exp->GenerateIR() + "  ret " + exp->GetResult() + "\n";
    }
};

class PrimaryExpAST : public ExpAST {
public:
    int val;

    void Dump() const override {
        std::cout << "PrimaryExpAST { " << val << " }";
    }

    std::string GenerateIR() const override {
        // 常量直接内联，不生成临时变量
        return "";
    }

    std::string GetResult() const override {
        return std::to_string(val); // 直接返回字面值
    }
};

class UnaryExpAST : public ExpAST {
public:
    std::string op;
    std::unique_ptr<ExpAST> operand;


    UnaryExpAST(std::string op, ExpAST* operand) 
        : op(op), operand(operand) {}

    void Dump() const override {
        std::cout << "UnaryExpAST { " << op << " ";
        operand->Dump();
        std::cout << " }";
    }

    std::string GenerateIR() const override {
        std::string ir = operand->GenerateIR();
        std::string tmp = NewTemp();
        
        switch(op[0]) {
            case '+': 
                ir += "  " + tmp + " = add 0, " + operand->GetResult() + "\n"; 
                break;
            case '-': 
                ir += "  " + tmp + " = sub 0, " + operand->GetResult() + "\n"; 
                break;
            case '!': 
                ir += "  " + tmp + " = eq " + operand->GetResult() + ", 0\n"; 
                break;
            default: 
                assert(false);
        }
        return ir;
    }
};

class BinaryExpAST : public ExpAST {
public:
    std::string op; // "+", "-", "*", "/", "%", "&&", "||", 等
    std::unique_ptr<ExpAST> lhs;
    std::unique_ptr<ExpAST> rhs;

    BinaryExpAST(std::string op, ExpAST* lhs, ExpAST* rhs) 
        : op(op), lhs(lhs), rhs(rhs) {}
        
    void Dump() const override {
        std::cout << "BinaryExpAST { ";
        lhs->Dump();
        std::cout << " " << op << " ";
        rhs->Dump();
        std::cout << " }";
    }

    std::string GenerateIR() const override {
        std::string ir = lhs->GenerateIR() + rhs->GenerateIR();
        std::string tmp = NewTemp();
        std::string opcode;

        if (op.size() == 1) {
            switch(op[0]) {
                case '+': opcode = "add"; break;
                case '-': opcode = "sub"; break;
                case '*': opcode = "mul"; break;
                case '/': opcode = "div"; break;
                case '%': opcode = "mod"; break;
                case '<': opcode = "lt";  break;
                case '>': opcode = "gt";  break;
            }
        } 
        else if (op == "==") opcode = "eq";
        else if (op == "!=") opcode = "ne";
        else if (op == "<=") opcode = "le";
        else if (op == ">=") opcode = "ge";
        else if (op == "&&")  {
            std::string lhs_bool = NewTemp();
            ir += "  " + lhs_bool + " = ne " + lhs->GetResult() + ", 0\n";
            
            std::string rhs_bool = NewTemp();
            ir += "  " + rhs_bool + " = ne " + rhs->GetResult() + ", 0\n";
            
            tmp = NewTemp();
            ir += "  " + tmp + " = and " + lhs_bool + ", " + rhs_bool + "\n";
            return ir;
        }
        else if (op == "||") {
            std::string or_tmp = NewTemp();
            ir += "  " + or_tmp + " = or " + lhs->GetResult() + ", " + rhs->GetResult() + "\n";
            
            std::string bool_tmp = NewTemp();
            ir += "  " + bool_tmp + " = ne " + or_tmp + ", 0\n";
            return ir;
        }
        else assert(false);

        ir += "  " + tmp + " = " + opcode + " " + lhs->GetResult() + ", " + rhs->GetResult() + "\n";
        return ir;
    }
};
