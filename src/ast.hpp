#pragma once
#include <iostream>

// 所有 AST 的基类
class BaseAST {
 public:
  virtual ~BaseAST() = default;

  virtual void Dump() const = 0;

  virtual std::string GenerateIR() const = 0;
};


class CompUnitAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_def;

  void Dump() const override {
    std::cout << "CompUnitAST { ";
    func_def->Dump();
    std::cout << " }";
  }

  std::string GenerateIR() const override {
    return func_def->GenerateIR();
  }
};

class FuncDefAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;

  void Dump() const override {
    std::cout << "FuncDefAST { ";
    func_type->Dump();
    std::cout << ", " << ident << ", ";
    block->Dump();
    std::cout << " }";
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
  int number;

  void Dump() const override {
    std::cout << "StmtAST { " << number << " }";
  }

  std::string GenerateIR() const override {
    return "  ret " + std::to_string(number) + "\n";
  }
};