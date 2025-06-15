#pragma once

#include <sstream>
#include <string>
#include <vector>
#include <map>
#include "koopa.h"

class RiscVGenerator {
private:
    std::ostringstream code_;
    int temp_reg_counter_ = 0;
    std::map<const koopa_raw_value_t, std::string> reg_alloc_;


public:
    std::string AllocTempReg() {
        const std::vector<std::string> regs = {
            "t0", "t1", "t2", "t3", "t4", "t5", "t6",
            "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7"
        };
        return regs[temp_reg_counter_++ % regs.size()];
    }
    void GenerateHeader() {
        code_ << "  .text\n";
        code_ << "  .globl main\n";
    }

    void GenerateFunction(const koopa_raw_function_t &func) {
        temp_reg_counter_ = 0; // 函数开始时重置临时寄存器计数器
        reg_alloc_.clear();
        code_ << "main:\n"; // +1跳过@符号
    }

    // 处理二元运算
    void GenerateBinary(const koopa_raw_binary_t &binary, const koopa_raw_value_t &result) {
        std::string lhs_reg = GetOperandReg(binary.lhs);
        std::string rhs_reg = GetOperandReg(binary.rhs);
        std::string dest_reg = AllocTempReg();

        switch(binary.op) {
            case KOOPA_RBO_ADD:
                code_ << "  add " << dest_reg << ", " << lhs_reg << ", " << rhs_reg << "\n";
                break;
            case KOOPA_RBO_SUB:
                code_ << "  sub " << dest_reg << ", " << lhs_reg << ", " << rhs_reg << "\n";
                break;
            case KOOPA_RBO_MUL:
                code_ << "  mul " << dest_reg << ", " << lhs_reg << ", " << rhs_reg << "\n"; 
                break;
            case KOOPA_RBO_DIV:
                code_ << "  div " << dest_reg << ", " << lhs_reg << ", " << rhs_reg << "\n";
                break;
            case KOOPA_RBO_MOD:
                code_ << "  rem " << dest_reg << ", " << lhs_reg << ", " << rhs_reg << "\n";
                break;
            case KOOPA_RBO_EQ:
                code_ << "  xor " << dest_reg << ", " << lhs_reg << ", " << rhs_reg << "\n";
                code_ << "  seqz " << dest_reg << ", " << dest_reg << "\n";
                break;
            case KOOPA_RBO_NOT_EQ:
                code_ << "  xor " << dest_reg << ", " << lhs_reg << ", " << rhs_reg << "\n";
                code_ << "  snez " << dest_reg << ", " << dest_reg << "\n";
                break;
            case KOOPA_RBO_LT:
                code_ << "  slt " << dest_reg << ", " << lhs_reg << ", " << rhs_reg << "\n";
                break;
            case KOOPA_RBO_LE:
                code_ << "  sgt " << dest_reg << ", " << lhs_reg << ", " << rhs_reg << "\n";
                code_ << "  seqz " << dest_reg << ", " << dest_reg << "\n";
                break;
            case KOOPA_RBO_GT:
                code_ << "  slt " << dest_reg << ", " << rhs_reg << ", " << lhs_reg << "\n";
                break;
            case KOOPA_RBO_GE:
                code_ << "  slt " << dest_reg << ", " << lhs_reg << ", " << rhs_reg << "\n";
                code_ << "  seqz " << dest_reg << ", " << dest_reg << "\n";
                break;
            case KOOPA_RBO_AND:
                code_ << "  and " << dest_reg << ", " << lhs_reg << ", " << rhs_reg << "\n";
                break;
            case KOOPA_RBO_OR:
                code_ << "  or "  << dest_reg << ", " << lhs_reg << ", " << rhs_reg << "\n";
                break;
            case KOOPA_RBO_XOR:
                code_ << "  xor " << dest_reg << ", " << lhs_reg << ", " << rhs_reg << "\n";
                break;
            default:
                assert(false);
        }

        reg_alloc_[result] = dest_reg;
    }

    std::string GetOperandReg(const koopa_raw_value_t &value) {
        auto it = reg_alloc_.find(value);
        if (it != reg_alloc_.end()) {
            return it->second;
        }

        std::string reg = AllocTempReg();
        if (value->kind.tag == KOOPA_RVT_INTEGER) {
            code_ << "  li " << reg << ", " << value->kind.data.integer.value << "\n";
        }
        reg_alloc_[value] = reg;
        return reg;
    }

    void GenerateReturn(const koopa_raw_return_t &ret) {
        if (ret.value) {
            std::string reg = GetOperandReg(ret.value);
            if (reg != "a0") code_ << "  mv a0, " << reg << "\n";
        }
        code_ << "  ret\n";
    }

    std::string GetCode() const { return code_.str(); }
};