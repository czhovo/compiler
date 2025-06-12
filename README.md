# compiler
bianyiyuanli compiler

## lv1 
想了半天终于找到问题了，FuncType : INT的INT是常量字符串"int"而非str_val，是不含值的token，%token INT RETURN %token <str_val> IDENT是对的不用改

debug文件夹用来放src之外的东西，例如hello.c, hello.koopa

添加了处理块注释

deepseek目前还挺好用