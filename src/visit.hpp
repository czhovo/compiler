#pragma once
// 函数声明
void Visit(const koopa_raw_program_t &program, RiscVGenerator &gen);
void Visit(const koopa_raw_slice_t &slice, RiscVGenerator &gen);
void Visit(const koopa_raw_function_t &func, RiscVGenerator &gen);
void Visit(const koopa_raw_basic_block_t &bb, RiscVGenerator &gen);
void Visit(const koopa_raw_value_t &value, RiscVGenerator &gen);

void Visit(const koopa_raw_program_t &program, RiscVGenerator &gen) {
    Visit(program.values, gen);
    Visit(program.funcs, gen);
}

void Visit(const koopa_raw_slice_t &slice, RiscVGenerator &gen) {
    for (size_t i = 0; i < slice.len; ++i) {
        auto ptr = slice.buffer[i];
        switch (slice.kind) {
            case KOOPA_RSIK_FUNCTION:
                Visit(reinterpret_cast<koopa_raw_function_t>(ptr), gen);
                break;
            case KOOPA_RSIK_BASIC_BLOCK:
                Visit(reinterpret_cast<koopa_raw_basic_block_t>(ptr), gen);
                break;
            case KOOPA_RSIK_VALUE:
                Visit(reinterpret_cast<koopa_raw_value_t>(ptr), gen);
                break;
            default:
                assert(false);
        }
    }
}

void Visit(const koopa_raw_function_t &func, RiscVGenerator &gen) {
    gen.GenerateFunction(func);
    Visit(func->bbs, gen);
}

void Visit(const koopa_raw_basic_block_t &bb, RiscVGenerator &gen) {
    Visit(bb->insts, gen);
}

void Visit(const koopa_raw_value_t &value, RiscVGenerator &gen) {
    switch (value->kind.tag) {
        case KOOPA_RVT_RETURN:
            gen.GenerateReturn(value->kind.data.ret);
            break;
        case KOOPA_RVT_INTEGER:
            break;
        case KOOPA_RVT_BINARY: {
            gen.GenerateBinary(value->kind.data.binary, value);
            break;
        }
        default:
            assert(false);
    }
}