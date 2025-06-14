# compiler
bianyiyuanli compiler

## lv1 
想了半天终于找到问题了，FuncType : INT的INT是常量字符串"int"而非str_val，是不含值的token，%token INT RETURN %token <str_val> IDENT是对的不用改

debug文件夹用来放src之外的东西，例如hello.c, hello.koopa

添加了处理块注释

deepseek目前还挺好用

## lv3
前半：本地测试ok，deepseek好坑

在sysy.y的产生式里用了很多非终结符号，例如 LOrExp | LOrExp OR LAndExp 被deepseek写成 LOrExp | LOrExp "||" LAndExp，修正这一错误后通过大部分测试点

对or和and的处理不正确，首先是输出应该是0或1，例如11 || 0应该输出1而非11，此外and不能直接按位求与，修正这两个错误后通过本地测试