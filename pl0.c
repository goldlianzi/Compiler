// pl/0 compiler with code generation
#include <stdlib.h>

#include <string.h>

#include "pl0.h"
#include "error/error.c"
#include "getsym/getsym.c"
#include "gen/gen.c"
#include "test/test.c"
#include "block/block.c"
#include "interpret/interpret.c"

int main() {
    long i;
    for (i = 0; i < 256; i++) { //某个数组的初始化？
        ssym[i] = nul;
    }
    strcpy(word[0], "begin     "); //word[]字符串数组 存放保留字字符？
    strcpy(word[1], "call      ");
    strcpy(word[2], "const     ");
    strcpy(word[3], "do        ");
    strcpy(word[4], "end       ");
    strcpy(word[5], "if        ");
    strcpy(word[6], "odd       ");
    strcpy(word[7], "procedure ");
    strcpy(word[8], "then      ");
    strcpy(word[9], "var       ");
    strcpy(word[10], "while     ");
    wsym[0] = beginsym; //wsym[]无符号长整型数组 关键字对应的十六进制码？
    wsym[1] = callsym;
    wsym[2] = constsym;
    wsym[3] = dosym;
    wsym[4] = endsym;
    wsym[5] = ifsym;
    wsym[6] = oddsym;
    wsym[7] = procsym;
    wsym[8] = thensym;
    wsym[9] = varsym;
    wsym[10] = whilesym;
    ssym['+'] = plus; //ssym[]类型同wsym[] 操作符对应的十六进制码？
    ssym['-'] = minus;
    ssym['*'] = times;
    ssym['/'] = slash;
    ssym['('] = lparen;
    ssym[')'] = rparen;
    ssym['='] = eql;
    ssym[','] = comma;
    ssym['.'] = period;
    ssym[';'] = semicolon;
    strcpy(mnemonic[lit], "lit"); //mnemonic[]字符串数组 放置中间代码操作符的对应符号？
    strcpy(mnemonic[opr], "opr");
    strcpy(mnemonic[lod], "lod");
    strcpy(mnemonic[sto], "sto");
    strcpy(mnemonic[cal], "cal");
    strcpy(mnemonic[Int], "int");
    strcpy(mnemonic[jmp], "jmp");
    strcpy(mnemonic[jpc], "jpc");
    declbegsys = constsym | varsym | procsym; //某种意义的长整型赋值？
    statbegsys = beginsym | callsym | ifsym | whilesym;
    facbegsys = ident | number | lparen;

    printf("please input source program file name: ");
    scanf("%s", infilename);
    printf("\n");

    if ((infile = fopen(infilename, "r")) == NULL) {
        printf("File %s can't be opened.\n", infilename);
        exit(1);
    }

    err = 0;
    cc = 0;
    cx = 0;

    ll = 0;
    ch = ' ';
    kk = al;
    getsym();
    lev = 0;
    tx = 0;
    block(declbegsys | statbegsys | period);
    if (sym != period) {
        error(9);
    }
    if (err == 0) {
        interpret();
    } else {
        printf("errors in PL/0 program\n");
    }
    fclose(infile);
    return 0;
}
