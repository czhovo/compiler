#pragma once

#include <sstream>
#include <fstream>
#include <string>

// RISC-V代码生成器类
class RiscVGenerator {
private:
    std::ostringstream code_;   

public:
    // 生成头部信息
    void GenerateHeader() {
        code_ << "  .text\n";
        code_ << "  .globl main\n";
    }

    // 生成函数入口
    void GenerateFunction(const koopa_raw_function_t &func) {
        code_ << "main:\n";
    }

    // 生成返回指令
    void GenerateReturn(const koopa_raw_return_t &ret) {
        if (ret.value != nullptr && ret.value->kind.tag == KOOPA_RVT_INTEGER) {
            int32_t val = ret.value->kind.data.integer.value;
            code_ << "  li a0, " << val << "\n";
        }
        code_ << "  ret\n";
    }

    // 获取生成的代码
    std::string GetCode() const {
        return code_.str();
    }
};