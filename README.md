# compiler
bianyiyuanli compiler

## lv1 
想了半天终于找到问题了，FuncType : INT的INT是常量字符串"int"而非str_val，是不含值的token，%token INT RETURN %token <str_val> IDENT是对的不用改

debug文件夹用来放src之外的东西，例如hello.c, hello.koopa

添加了处理块注释

deepseek目前还挺好用

写lv3才发现还有下半部分（risc-v），忘记交了，交一下试试

ok过了


## lv3
前半：本地测试ok，deepseek好坑

在sysy.y的产生式里用了很多非终结符号，例如 LOrExp | LOrExp OR LAndExp 被deepseek写成 LOrExp | LOrExp "||" LAndExp，修正这一错误后通过大部分测试点

对or和and的处理不正确，首先是输出应该是0或1，例如11 || 0应该输出1而非11，此外and不能直接按位求与，修正这两个错误后通过本地测试

后半：在deepseek的帮助下轻松完成了，nb

gt不能直接用sgt指令处理，因为没有这一条指令，因此采用slt并交换两个寄存器