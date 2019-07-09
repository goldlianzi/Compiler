// pl/0 compiler with code generation
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "pl0.h"

/* 函数声明 */
void error(long n);
void getch();
void getsym();
void gen(enum fct x, long y, long z);
void test(unsigned long long s1, unsigned long long s2, long n);
void enter(enum object k); // enter object into table
long position(char * id); // find identifier id in table
void constdeclaration();
void vardeclaration();
void typedeclaration();
void listcode(); // list code generated for this block
void factor(unsigned long long fsys); //依赖expression()
void term(unsigned long long fsys);
void expression(unsigned long long fsys);
void condition(unsigned long long fsys);
void statement(unsigned long long fsys);
void block(unsigned long long fsys);
long base(long b, long l);
void interpret();

void error(long n) {
    long i;

    printf(" ****");
    for (i = 1; i <= cc - 1; i++) {
        printf(" ");
    }
    printf("^%2d\n", n);
    err++;
}

void getch() {
	int f1 = 0, f2 = 0; // 用来判断 // 和 /* */的情况

	if (cc == ll) {  //判断此字符是不是在上次读的一行里
		if (feof(infile)) {
			printf("************************************\n");
			printf("      program incomplete\n");
			printf("************************************\n");
			exit(1);
		}
		ll = 0; cc = 0;
		printf("%5d ", cx);


		while (!feof(infile)) { //读一行的字符
			ch = getc(infile);
			if (f1) { // 如果是 // 的注释就一直读到此行结束
				if (ch != '\n') continue; else
					break;
			}

			if (f2) {  // 是 /*  的情况
				if (ch != '*') continue; else
					if (!feof(infile)) {
						ch = getc(infile);
						if (ch != '/') continue; else
							f2 = 0;
					}
			}

			if (ch == '/') {  // 第一次遇到 '/' 即进行判断
				if ((!feof(infile)) && ((ch = getc(infile)) != '\n')) {
					switch (ch) {
					case '/': f1 = 1; break;
					case '*': f2 = 1; break;
					default:
						printf("/"); ll = ll + 1; line[ll] = '/';
						printf("%c", ch); ll = ll + 1; line[ll] = ch;
						continue;
					}
				}
			}
			if (f1 || f2) continue;
			if (ch == '\n') break;
			printf("%c", ch);
			ll = ll + 1; line[ll] = ch;
		}
		printf("\n");
		ll = ll + 1; line[ll] = ' ';
	}
	cc = cc + 1; ch = line[cc];
}

void getsym() {
    long i, j, k;
    long m;

    while (ch == ' ' || ch == '\t') { //过滤空格和制表符 ch存放当前取出来的符号
        getch();
    }

    // 依次判断是否为 字母 数字 : < > {+ - * / ( ) = , . ;}
    if (isalpha(ch)) { // identified or reserved 小写字母为2，大写字母为1，若不是字母，返回0
        k = 0;
        do {
            if (k < al) {
                a[k] = ch;
                k = k + 1;
            }
            getch();
        } while (isalpha(ch) || isdigit(ch));
        if (k >= kk) { //疑似为a[]的空处(NULL)填充空格符
            kk = k;
        } else {
            do {
                kk = kk - 1;
                a[kk] = ' ';
            } while (k < kk);
        }
        strcpy(id, a);// id从a拷贝单词（一个单词）
        i = 0;
        j = norw - 1;
        do { //二分查找保留字
            k = (i + j) / 2;
            if (strcmp(id, word[k]) <= 0) {
                j = k - 1;
            }
            if (strcmp(id, word[k]) >= 0) {
                i = k + 1;
            }
        } while (i <= j);
        if (i - 1 > j) { //保留字匹配成功
            sym = wsym[k];
        } else { //保留字匹配失败，则当前单词是标识符（常量或变量）
            sym = ident;
        }
    } else if (isdigit(ch)) { // number 检查ch是否为十进制数字字符。
        k = 0;
        num = 0;
        sym = number;
        do {
            num = num * 10 + (ch - '0');
            k = k + 1;
            getch();
        } while (isdigit(ch));
        if (k > nmax) {
            error(31);
        }
    } else if (ch == ':') {
        getch();
        if (ch == '=') {
            sym = becomes;
            getch();
        } else {
            sym = nul;
        }
    } else if (ch == '<') {
        getch();
        if (ch == '=') {
            sym = leq;
            getch();
        } else if (ch == '>') {
            sym = neq;
            getch();
        } else {
            sym = lss;
        }
    } else if (ch == '>') {
        getch();
        if (ch == '=') {
            sym = geq;
            getch();
        } else {
            sym = gtr;
        }
    } else if (ch == '"') {

        getch();
        m = 0;
        strcpy(buff[buff_i], "");
        while (ch != '"') {
            buff[buff_i][m ++] = ch;
            getch();
        }
        buff_i++;
        sym = string;
        getch();
    } else {
        sym = ssym[(unsigned char) ch];
        getch();
    }
}

void gen(enum fct x, long y, long z) { //生成中间代码，并放入结构体存储区code
    if (cx > cxmax) {
        printf("program too long\n");
        exit(1);
    }
    code[cx].f = x;
    code[cx].l = y;
    code[cx].a = z;
    cx = cx + 1;
}

void test(unsigned long long s1, unsigned long long s2, long n) {
    if (!(sym & s1)) {
        error(n);
        s1 = s1 | s2;
        while (!(sym & s1)) {
            getsym();
        }
    }
}

void enter(enum object k) { // enter object into table 登记名字表
    tx = tx + 1; // 表内的名字个数
    strcpy(table[tx].name, id);
    table[tx].kind = k;
    table[tx].Vkind = none;
    switch (k) { //依据名字的类型然后从 num dx lev取出相应数据
        case constant:
            if (num > amax) {
                error(31);
                num = 0;
            }
            table[tx].val = num;
            break;
        case variable:
            table[tx].level = lev;
            table[tx].addr = dx;
            dx = dx + 1;
            break;
        case proc:
            table[tx].level = lev;
            break;
        case typeExp:  //---------- 更改
			table[tx].level = lev;
			break;
    }
}

long position(char * id) { // find identifier id in table 查找标识符在名字表中的位置
    long i;

    strcpy(table[0].name, id);
    i = tx;
    while (strcmp(table[i].name, id) != 0) {
        i = i - 1;
    }
    return i;
}

void constdeclaration() {
    if (sym == ident) { // 再次确认是标识符
        getsym();
        if (sym == eql || sym == becomes) { //常量赋值
            if (sym == becomes) { // 常量定义不允许 :=
                error(1);
            }
            getsym();
            if (sym == number) {
                enter(constant); // 登记名字表
                getsym();
            } else {
                error(2);
            }
        } else {
            error(3);
        }
    } else {
        error(4);
    }
}

void vardeclaration() {
    long i;
    if (sym == ident) { // 表明声明不允许赋初值，故直接登记
        enter(variable);
        getsym();
        if(sym == nul){ // 下一个是冒号 *djm*
            getsym(); // 获取其变量的类型，是布尔、实型、整型（默认）
            if (sym == semicolon) { // :后面接;则报错
                error(100);
            }
            switch(sym){ //因为本函数enter(variable)已经登记过了，此处多分支只是对table表中的Vkind进行补充
                case boolean:
                    table[tx - 1].Vkind = boolean;
                    break;
                case real:
                    table[tx - 1].Vkind = real;
                    break;
                case integer:
                    table[tx - 1].Vkind = integer;
                    break;
                case typeExp:  // 更改
					i = position(id);
					if (i == 0) {
						error(11);
					}
					table[tx - 1].Vkind = table[i].Vkind;
            }
            getsym();
        }
    } else {
        error(4);
    }
}

void typedeclaration() { // TypeDef --> ident = TypeExp;
	if (sym == ident) { // 表明声明不允许赋初值，故直接登记
		enter(typeExp);
		getsym();
		if (sym == eql) { // 下一个是等号
			getsym(); // 获取 typeExp的类型
		}
		else {
			// 报错处理
		}

		switch (sym) { //因为本函数enter(typeExp)已经登记过了，此处多分支只是对table表中的Vkind进行补充
		case boolean:
			table[tx - 1].Vkind = boolean;
			break;
		case real:
			table[tx - 1].Vkind = real;
			break;
		case integer:
			table[tx - 1].Vkind = integer;
			break;
		default:
			// 报错处理
			break;
		}
		getsym();

		if (sym == semicolon) {
			getsym();
		}
		else {
			// 报错处理
		}
	}
	else {
		error(4);
	}
}

void listcode() { // list code generated for this block 打印中间（目标）代码 到txt文本
    long i, j;

    j = 0;
    for(i = 0; i < cx; i ++){
        if(code[i].f == buf) { // 对形如 buf 0 0 的加工处理
            fprintf(outfile,"%10d%5s%35s\n", i, mnemonic[code[i].f], buff[j++]);
        } else {
            fprintf(outfile,"%10d%5s%5d%15d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
        }

    }
}

void factor(unsigned long long fsys) { // 处理标识符 数字 左括号
    long i;
    long high_num, low_num, result_num, tmp_num; // 实现实数类型的时候用到了

    test(facbegsys, fsys, 24);
    while (sym & facbegsys) {
        if ((strcmp(id, "true      ") == 0 || strcmp(id, "false     ") == 0)) { // 实现布尔类型 *djm*
            if(strcmp(id, "true      ") == 0){
                num = 0x00400001;
            } else {
                num = 0x00400000;
            }
            gen(lit, 0, num);
            getsym();
        } else if (sym == ident) { //当前单词类别为标识符
            i = position(id);
            if (i == 0) { // 找不到该标识符
                error(11);
            } else { // 找到了标识符的在名字表table的位置
                switch (table[i].kind) {
                    case constant:
                        gen(lit, 0, table[i].val);
                        break;
                    case variable:
                        gen(lod, lev - table[i].level, table[i].addr);
                        break;
                    case proc:
                        error(21);
                        break;
                }
            }
            getsym();
        } else if (sym == number) { //当前单词类别为数字
            if (num > amax) {
                error(31);
                num = 0;
            }

            /* 尝试实现实数 */
            tmp_num = num;
            getsym();
            if (sym == period) {
                // 实型
                getsym();
                if (sym == number) {
                    if (tmp_num > 0x00003FFF || num > 0x00003FFF) {
                        // 报错处理
                    } else {
                        //printf("arrive here\n");
                        high_num = (tmp_num << 11) & 0x003FF800;
                        low_num = num & 0x000007FF;
                        //printf("high is %ld, low is %ld\n", high_num , low_num);
                        result_num = high_num | low_num; // 高地址和低地址合并放在num中
                        result_num = result_num | 0x00800000; // 实型为10 地址码拆分为[1]..[2][11][11]
                        //printf("resultlow is %ld\n", result_num);
                        //printf("needresult is %ld\n", 0x00804008);
                        gen(lit, 0, result_num);
                        getsym();
                    }
                } else {
                    // 报错处理
                }
            } else {
                // 整型
                tmp_num = tmp_num & 0xFF3FFFFF; // 实型为00 地址码拆分为[1]..[2][11][11]
                gen(lit, 0, tmp_num);
            }
            /* end */
        } else if (sym == lparen) { //当前单词类别为左括号
            getsym();
            expression(rparen | fsys);
            if (sym == rparen) {
                getsym();
            } else {
                error(22);
            }
        } else if (sym == string) { //当前单词类别为临时字符串
            //gen(lit, 0, num);
            getsym();
        }
        test(fsys, lparen, 23);
    }
}

void term(unsigned long long fsys) { // 处理乘除操作
    unsigned long long mulop;

    factor(fsys | times | slash | orsym | andsym | divsym | modsym);
    while (sym == times || sym == slash || sym == orsym || sym == andsym || sym == divsym || sym == modsym) {
        mulop = sym;
        getsym();
        factor(fsys | times | slash | orsym | andsym | divsym | modsym);
        if (mulop == times) { // 判断乘除以生成操作码
            gen(opr, 0, 4);
        } else if(mulop == slash){
            gen(opr, 0, 5);
        } else if(mulop == orsym){ // 或|操作
            gen(opr, 0, 17);
        } else if(mulop == andsym){ // 与&操作
            gen(opr, 0, 18);
        } else if(0){ // 非~操作
            // 因为非操作的特殊性，将其放在expression()中实现
        } else if(mulop == divsym){ // div整除 同/
            gen(opr, 0, 5);
        } else if(mulop == modsym){ // mod求余
            gen(opr, 0, 20);
        }
    }
}

void expression(unsigned long long fsys) {
    unsigned long long addop;

    if (sym == plus || sym == minus || sym == notsym) { // 判断是 简单的负数表达式 eg num = -1
        addop = sym;
        getsym();
        term(fsys | plus | minus | notsym);
        if (addop == minus) {
            gen(opr, 0, 1);
        } else if (addop == notsym) { // 非操作
            gen(opr, 0, 19);
        }
    } else {
        term(fsys | plus | minus | notsym);
    }
    while (sym == plus || sym == minus || sym == notsym) { // 正常的加减式子
        addop = sym;
        getsym();
        term(fsys | plus | minus);
        if (addop == plus) {
            gen(opr, 0, 2);
        } else if (addop == minus) {
            gen(opr, 0, 3);
        } else if (addop == notsym) {
            gen(opr, 0, 19);
        }
    }
}

void condition(unsigned long long fsys) {
    unsigned long long relop;

    if (sym == oddsym) {
        getsym();
        expression(fsys);
        gen(opr, 0, 6);
    } else {
        expression(fsys | eql | neq | lss | gtr | leq | geq);
        if (!(sym & (eql | neq | lss | gtr | leq | geq))) {
            error(20); // 比较符号无法识别 报错
        } else {
            relop = sym;
            getsym();
            expression(fsys);
            switch (relop) {
                case eql:
                    gen(opr, 0, 8);
                    break;
                case neq:
                    gen(opr, 0, 9);
                    break;
                case lss:
                    gen(opr, 0, 10);
                    break;
                case geq:
                    gen(opr, 0, 11);
                    break;
                case gtr:
                    gen(opr, 0, 12);
                    break;
                case leq:
                    gen(opr, 0, 13);
                    break;
            }
        }
    }
}

void statement(unsigned long long fsys) {
    long i, cx1, cx2;

    if (sym == ident) { // 单词为标识符时
        i = position(id);
        if (i == 0) {
            error(11);
        } else if (table[i].kind != variable) { // assignment to non-variable 非变量
            error(12);
            i = 0;
        }
        getsym();
        if (sym == becomes) { // 单词为 :=
            getsym();
        } else {
            error(13);
        }
        expression(fsys);
        if (i != 0) {
            gen(sto, lev - table[i].level, table[i].addr);
        }
    } else if (sym == callsym) { // 单词为保留字call
        getsym();
        if (sym != ident) { // 过程名不是标识符
            error(14);
        } else {
            i = position(id);
            if (i == 0) { //过程名不在名字表中
                error(11);
            } else if (table[i].kind == proc) { // 生成 call 过程名的中间代码
                gen(cal, lev - table[i].level, table[i].addr);
            } else {
                error(15);
            }
            getsym();
        }
    } else if (sym == ifsym) { // 单词为保留字if
        getsym();
        condition(fsys | thensym | dosym);
        if (sym == thensym) {
            getsym();
        } else {
            error(16);
        }
        cx1 = cx;
        gen(jpc, 0, 0);
        statement(fsys | semicolon | endsym | elsesym);

        code[cx1].a = cx + 1;
        if(sym == elsesym) {
            getsym();
            code[cx1].a = cx+1; //jumps past if
            cx1 = cx;
            gen(jmp, 0, 0); // 7 is JMP for op, 0 is for L and cx1 for M
            //updates JPC M value
            statement(fsys | semicolon | endsym | elsesym);
        }
        code[cx1].a = cx; //jumps past else (if theres an else statement) otherwise jumps past if
    } else if (sym == beginsym) { // 单词为保留字begin
        getsym();
        statement(fsys | semicolon | endsym);
        while (sym == semicolon || (sym & statbegsys)) {
            if (sym == semicolon) {
                getsym();
            } else {
                error(10);
            }
            statement(fsys | semicolon | endsym);
        }
        if (sym == endsym) {
            getsym();
        } else {
            error(17);
        }
    } else if (sym == whilesym) { //while循环 并实现exit的准备
        whilenum++;
        cx1 = cx;
        getsym();
        condition(fsys | dosym);
        cx2 = cx;
        gen(jpc, 0, 0);
        if (sym == dosym) {
            getsym();
        }else{
            error(18);
        }
        statement(fsys);
        gen(jmp, 0, cx1);
        code[cx2].a = cx;
        if (exitcx != 0) {
            code[exitcx].a = cx;
            exitcx = 0;
        }
        whilenum--;
    } else if (sym == exitsym) { // 实现exit
        if (whilenum == 0) {
            error(35);
        } else {
            exitcx = cx;
            gen(jmp, 0, 0);
            getsym();
        }
    } else if (sym == writesym) { // write(Exp{,Exp})
		getsym();
		if (sym == lparen) {
			getsym();
		} else {
			// 报错处理
		}
		if(sym == string){  // 输出临时字符串 eg write("this is a test\n");
		    expression(fsys | comma | rparen | string);
            gen(buf, 0, 0);
            // getsym();
		} else { // 输出变量
            expression(fsys | comma | rparen | string);
            if (sym == comma) {
                gen(opr, 0, 14);
            } else {
                gen(opr, 0, 15);
            }
            while (sym == comma) {
                getsym();
                expression(fsys | comma | rparen | string);
                if (sym == comma) {
                    gen(opr, 0, 14);
                } else {
                    gen(opr, 0, 15);
                }
            }
		}
        if (sym == rparen) {
            getsym();
        } else {
            // 报错处理
        }
	} else if (sym == readsym) { // read(ident{,ident})
		getsym();
		if (sym == lparen) {
			getsym();
		}
		else {
			// 报错处理
		}
		if (sym == ident) {
			i = position(id);
			if (i == 0) {
				error(11);
			}
			else {
				if (table[i].kind == variable) {  // 只能读变量，常量或者其他的都不可读入
					gen(opr, 0, 16);
					gen(sto, lev - table[i].level, table[i].addr);
				}
				else {
					// 报错处理
				}
			}
			getsym();
		}
		else {
			// 报错处理
		}
		while (sym == comma) {
			getsym();
			if (sym == ident) {
				i = position(id);
				if (i == 0) {
					error(11);
				}
				else {
					if (table[i].kind == variable) {  // 只能读变量，常量或者其他的都不可读入
						gen(opr, 0, 16);
						gen(sto, lev - table[i].level, table[i].addr);
					}
					else {
						// 报错处理
					}
				}
				getsym();
			}
			else {
				// 报错处理
			}
		}
		if (sym == rparen) {
			getsym();
		}
		else {
			// 报错处理
		}
	}

    test(fsys, 0, 19);
}

void block(unsigned long long fsys) {
    long tx0; // initial table index
    long cx0; // initial code index
    long tx1; // save current table index before processing nested procedures
    long dx1; // save data allocation index

    dx = 3;
    tx0 = tx;
    table[tx].addr = cx;
    gen(jmp, 0, 0); //目标代码的初始化开始？
    if (lev > levmax) { // 限制嵌套层数
        error(32);
    }
    do {
        if (sym == constsym) { // 常量类型的单词
            getsym(); // 取出下一个单词 （就是要定义的常量符号）
            do {
                constdeclaration();
                while (sym == comma) { //连续定义 eg： int a, b, c;
                    getsym();
                    constdeclaration();
                }
                if (sym == semicolon) { //遇到分号结束常量定义
                    getsym();
                } else {
                    error(5);
                }
            } while (sym == ident);
        }
        while (sym == varsym) { // 变量类型的单词 步骤上一个常量定义类似
            getsym();
            do {
                vardeclaration();
                while (sym == comma) {
                    getsym();
                    vardeclaration();
                }
                if (sym == semicolon) {
                    getsym();
                } else {
                    error(5);
                }
            } while (sym == ident);
        }
        if (sym == typesym) { // 常量类型的单词
			getsym(); // 取出下一个单词 （就是要定义的常量符号）
			do {
				typedeclaration();
			} while (sym == ident);
		}
        while (sym == procsym) { // 过程保留字的单词
            getsym();
            if (sym == ident) { //登记定义的过程名
                enter(proc);
                getsym();
            } else {
                error(4);
            }
            if (sym == semicolon) { //过程名后接的是分号 形如procedure multiply;
                getsym();
            } else {
                error(5);
            }
            lev = lev + 1; // 嵌套层数+1 保护现场
            tx1 = tx;
            dx1 = dx;
            block(fsys | semicolon); // 迭代解释下一份procedure程序块
            lev = lev - 1;
            tx = tx1;
            dx = dx1;
            if (sym == semicolon) {
                getsym();
                test(statbegsys | ident | procsym, fsys, 6);
            } else {
                error(5);
            }
        }
        test(statbegsys | ident, declbegsys, 7); // 测试当前单词符是否合法
    } while (sym & declbegsys);
    code[table[tx0].addr].a = cx; // 出口的中间代码生成（为什么是裸写的，而不是用gen()）
    table[tx0].addr = cx; // start addr of code
    cx0 = cx;
    gen(Int, 0, dx); // 中间代码生成
    statement(fsys | semicolon | endsym);
    gen(opr, 0, 0); // return
    test(fsys, 0, 8);
}

long base(long b, long l) {
    long b1;

    b1 = b;
    while (l > 0) { // find base l levels down
        b1 = s[b1];
        l = l - 1;
    }
    return b1;
}

void main() {
    long i;
    for (i = 0; i < 256; i++) { //某个数组的初始化？
        ssym[i] = nul;
    }

    strcpy(word[0], "and       "); //word[]字符串数组 存放保留字字符
    strcpy(word[1], "begin     ");
    strcpy(word[2], "boolean   ");
    strcpy(word[3], "call      ");
    strcpy(word[4], "const     ");
    strcpy(word[5], "div       ");
    strcpy(word[6], "do        ");
    strcpy(word[7], "else      ");
    strcpy(word[8], "end       ");
    strcpy(word[9], "exit      ");
    strcpy(word[10], "if        ");
    strcpy(word[11], "integer   ");
    strcpy(word[12], "mod       ");
    strcpy(word[13], "not       ");
    strcpy(word[14], "odd       ");
    strcpy(word[15], "or        ");
    strcpy(word[16], "procedure ");
    strcpy(word[17], "read      ");
    strcpy(word[18], "real      ");
    strcpy(word[19], "then      ");
    strcpy(word[20], "type      ");
    strcpy(word[21], "var       ");
    strcpy(word[22], "while     ");
    strcpy(word[23], "write     ");

    wsym[0] = andsym; //wsym[]无符号长整型数组 关键字对应的十六进制码
    wsym[1] = beginsym;
    wsym[2] = booleansym;
    wsym[3] = callsym;
    wsym[4] = constsym;
    wsym[5] = divsym;
    wsym[6] = dosym;
    wsym[7] = elsesym;
    wsym[8] = endsym;
    wsym[9] = exitsym;
    wsym[10] = ifsym;
    wsym[11] = integersym;
    wsym[12] = modsym;
    wsym[13] = notsym;
    wsym[14] = oddsym;
    wsym[15] = orsym;
    wsym[16] = procsym;
    wsym[17] = readsym;
    wsym[18] = realsym;
    wsym[19] = thensym;
    wsym[20] = typesym;
    wsym[21] = varsym;
    wsym[22] = whilesym;
    wsym[23] = writesym;

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
    ssym['|'] = orsym;
    ssym['&'] = andsym;
    ssym['~'] = notsym;

    strcpy(mnemonic[lit], "lit"); //mnemonic[]字符串数组 放置中间代码操作符的对应符号？
    strcpy(mnemonic[opr], "opr");
    strcpy(mnemonic[lod], "lod");
    strcpy(mnemonic[sto], "sto");
    strcpy(mnemonic[cal], "cal");
    strcpy(mnemonic[Int], "int");
    strcpy(mnemonic[jmp], "jmp");
    strcpy(mnemonic[jpc], "jpc");
    strcpy(mnemonic[buf], "buf");

    declbegsys = constsym | varsym | procsym | typesym; //{常量 变量 过程名} 名字类型
    statbegsys = beginsym | callsym | ifsym | whilesym;  //{开始 调用 条件 循环} 保留字类型
    statbegsys = statbegsys | booleansym | integersym | realsym | writesym | readsym; // 新增加保留字
    statbegsys = statbegsys | elsesym | exitsym | orsym | andsym | notsym; // 新增加保留字
    statbegsys = statbegsys | divsym | modsym; // 新增加保留字
    facbegsys = ident | number | lparen | string; // {标识符 数字 左括号}
    // printf("%lld\n", string);
    // printf("string is %lld\n", facbegsys & string);

    printf("please input source program file name: ");
    scanf("%s", infilename);
    printf("\n");

    if ((infile = fopen(infilename, "r")) == NULL) {
        printf("File %s can't be opened.\n", infilename);
        exit(1);
    }
    /* 翻译的中间代码写入outfile */
    outfilename = NULL;
    outfilename = strtok(infilename,".");
    strcat(outfilename,".txt");
    if ((outfile = fopen(outfilename, "w")) == NULL){
        printf("can't output the code");
        exit(1);
    }
	/* end */
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
		listcode(); // zhj
        printf("compile success!\n");
		//interpret();
    } else {
        printf("errors in PL/0 program\n");
    }
    fclose(infile);
}
