// pl/0 compiler with code generation
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "pl0.h"

/* �������� */
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
void factor(unsigned long long fsys); //����expression()
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
	int f1 = 0, f2 = 0; // �����ж� // �� /* */�����

	if (cc == ll) {  //�жϴ��ַ��ǲ������ϴζ���һ����
		if (feof(infile)) {
			printf("************************************\n");
			printf("      program incomplete\n");
			printf("************************************\n");
			exit(1);
		}
		ll = 0; cc = 0;
		printf("%5d ", cx);


		while (!feof(infile)) { //��һ�е��ַ�
			ch = getc(infile);
			if (f1) { // ����� // ��ע�;�һֱ�������н���
				if (ch != '\n') continue; else
					break;
			}

			if (f2) {  // �� /*  �����
				if (ch != '*') continue; else
					if (!feof(infile)) {
						ch = getc(infile);
						if (ch != '/') continue; else
							f2 = 0;
					}
			}

			if (ch == '/') {  // ��һ������ '/' �������ж�
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

    while (ch == ' ' || ch == '\t') { //���˿ո���Ʊ�� ch��ŵ�ǰȡ�����ķ���
        getch();
    }

    // �����ж��Ƿ�Ϊ ��ĸ ���� : < > {+ - * / ( ) = , . ;}
    if (isalpha(ch)) { // identified or reserved Сд��ĸΪ2����д��ĸΪ1����������ĸ������0
        k = 0;
        do {
            if (k < al) {
                a[k] = ch;
                k = k + 1;
            }
            getch();
        } while (isalpha(ch) || isdigit(ch));
        if (k >= kk) { //����Ϊa[]�Ŀմ�(NULL)���ո��
            kk = k;
        } else {
            do {
                kk = kk - 1;
                a[kk] = ' ';
            } while (k < kk);
        }
        strcpy(id, a);// id��a�������ʣ�һ�����ʣ�
        i = 0;
        j = norw - 1;
        do { //���ֲ��ұ�����
            k = (i + j) / 2;
            if (strcmp(id, word[k]) <= 0) {
                j = k - 1;
            }
            if (strcmp(id, word[k]) >= 0) {
                i = k + 1;
            }
        } while (i <= j);
        if (i - 1 > j) { //������ƥ��ɹ�
            sym = wsym[k];
        } else { //������ƥ��ʧ�ܣ���ǰ�����Ǳ�ʶ���������������
            sym = ident;
        }
    } else if (isdigit(ch)) { // number ���ch�Ƿ�Ϊʮ���������ַ���
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

void gen(enum fct x, long y, long z) { //�����м���룬������ṹ��洢��code
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

void enter(enum object k) { // enter object into table �Ǽ����ֱ�
    tx = tx + 1; // ���ڵ����ָ���
    strcpy(table[tx].name, id);
    table[tx].kind = k;
    table[tx].Vkind = none;
    switch (k) { //�������ֵ�����Ȼ��� num dx levȡ����Ӧ����
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
        case typeExp:  //---------- ����
			table[tx].level = lev;
			break;
    }
}

long position(char * id) { // find identifier id in table ���ұ�ʶ�������ֱ��е�λ��
    long i;

    strcpy(table[0].name, id);
    i = tx;
    while (strcmp(table[i].name, id) != 0) {
        i = i - 1;
    }
    return i;
}

void constdeclaration() {
    if (sym == ident) { // �ٴ�ȷ���Ǳ�ʶ��
        getsym();
        if (sym == eql || sym == becomes) { //������ֵ
            if (sym == becomes) { // �������岻���� :=
                error(1);
            }
            getsym();
            if (sym == number) {
                enter(constant); // �Ǽ����ֱ�
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
    if (sym == ident) { // ����������������ֵ����ֱ�ӵǼ�
        enter(variable);
        getsym();
        if(sym == nul){ // ��һ����ð�� *djm*
            getsym(); // ��ȡ����������ͣ��ǲ�����ʵ�͡����ͣ�Ĭ�ϣ�
            if (sym == semicolon) { // :�����;�򱨴�
                error(100);
            }
            switch(sym){ //��Ϊ������enter(variable)�Ѿ��Ǽǹ��ˣ��˴����ֻ֧�Ƕ�table���е�Vkind���в���
                case boolean:
                    table[tx - 1].Vkind = boolean;
                    break;
                case real:
                    table[tx - 1].Vkind = real;
                    break;
                case integer:
                    table[tx - 1].Vkind = integer;
                    break;
                case typeExp:  // ����
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
	if (sym == ident) { // ����������������ֵ����ֱ�ӵǼ�
		enter(typeExp);
		getsym();
		if (sym == eql) { // ��һ���ǵȺ�
			getsym(); // ��ȡ typeExp������
		}
		else {
			// ������
		}

		switch (sym) { //��Ϊ������enter(typeExp)�Ѿ��Ǽǹ��ˣ��˴����ֻ֧�Ƕ�table���е�Vkind���в���
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
			// ������
			break;
		}
		getsym();

		if (sym == semicolon) {
			getsym();
		}
		else {
			// ������
		}
	}
	else {
		error(4);
	}
}

void listcode() { // list code generated for this block ��ӡ�м䣨Ŀ�꣩���� ��txt�ı�
    long i, j;

    j = 0;
    for(i = 0; i < cx; i ++){
        if(code[i].f == buf) { // ������ buf 0 0 �ļӹ�����
            fprintf(outfile,"%10d%5s%35s\n", i, mnemonic[code[i].f], buff[j++]);
        } else {
            fprintf(outfile,"%10d%5s%5d%15d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
        }

    }
}

void factor(unsigned long long fsys) { // �����ʶ�� ���� ������
    long i;
    long high_num, low_num, result_num, tmp_num; // ʵ��ʵ�����͵�ʱ���õ���

    test(facbegsys, fsys, 24);
    while (sym & facbegsys) {
        if ((strcmp(id, "true      ") == 0 || strcmp(id, "false     ") == 0)) { // ʵ�ֲ������� *djm*
            if(strcmp(id, "true      ") == 0){
                num = 0x00400001;
            } else {
                num = 0x00400000;
            }
            gen(lit, 0, num);
            getsym();
        } else if (sym == ident) { //��ǰ�������Ϊ��ʶ��
            i = position(id);
            if (i == 0) { // �Ҳ����ñ�ʶ��
                error(11);
            } else { // �ҵ��˱�ʶ���������ֱ�table��λ��
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
        } else if (sym == number) { //��ǰ�������Ϊ����
            if (num > amax) {
                error(31);
                num = 0;
            }

            /* ����ʵ��ʵ�� */
            tmp_num = num;
            getsym();
            if (sym == period) {
                // ʵ��
                getsym();
                if (sym == number) {
                    if (tmp_num > 0x00003FFF || num > 0x00003FFF) {
                        // ������
                    } else {
                        //printf("arrive here\n");
                        high_num = (tmp_num << 11) & 0x003FF800;
                        low_num = num & 0x000007FF;
                        //printf("high is %ld, low is %ld\n", high_num , low_num);
                        result_num = high_num | low_num; // �ߵ�ַ�͵͵�ַ�ϲ�����num��
                        result_num = result_num | 0x00800000; // ʵ��Ϊ10 ��ַ����Ϊ[1]..[2][11][11]
                        //printf("resultlow is %ld\n", result_num);
                        //printf("needresult is %ld\n", 0x00804008);
                        gen(lit, 0, result_num);
                        getsym();
                    }
                } else {
                    // ������
                }
            } else {
                // ����
                tmp_num = tmp_num & 0xFF3FFFFF; // ʵ��Ϊ00 ��ַ����Ϊ[1]..[2][11][11]
                gen(lit, 0, tmp_num);
            }
            /* end */
        } else if (sym == lparen) { //��ǰ�������Ϊ������
            getsym();
            expression(rparen | fsys);
            if (sym == rparen) {
                getsym();
            } else {
                error(22);
            }
        } else if (sym == string) { //��ǰ�������Ϊ��ʱ�ַ���
            //gen(lit, 0, num);
            getsym();
        }
        test(fsys, lparen, 23);
    }
}

void term(unsigned long long fsys) { // ����˳�����
    unsigned long long mulop;

    factor(fsys | times | slash | orsym | andsym | divsym | modsym);
    while (sym == times || sym == slash || sym == orsym || sym == andsym || sym == divsym || sym == modsym) {
        mulop = sym;
        getsym();
        factor(fsys | times | slash | orsym | andsym | divsym | modsym);
        if (mulop == times) { // �жϳ˳������ɲ�����
            gen(opr, 0, 4);
        } else if(mulop == slash){
            gen(opr, 0, 5);
        } else if(mulop == orsym){ // ��|����
            gen(opr, 0, 17);
        } else if(mulop == andsym){ // ��&����
            gen(opr, 0, 18);
        } else if(0){ // ��~����
            // ��Ϊ�ǲ����������ԣ��������expression()��ʵ��
        } else if(mulop == divsym){ // div���� ͬ/
            gen(opr, 0, 5);
        } else if(mulop == modsym){ // mod����
            gen(opr, 0, 20);
        }
    }
}

void expression(unsigned long long fsys) {
    unsigned long long addop;

    if (sym == plus || sym == minus || sym == notsym) { // �ж��� �򵥵ĸ������ʽ eg num = -1
        addop = sym;
        getsym();
        term(fsys | plus | minus | notsym);
        if (addop == minus) {
            gen(opr, 0, 1);
        } else if (addop == notsym) { // �ǲ���
            gen(opr, 0, 19);
        }
    } else {
        term(fsys | plus | minus | notsym);
    }
    while (sym == plus || sym == minus || sym == notsym) { // �����ļӼ�ʽ��
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
            error(20); // �ȽϷ����޷�ʶ�� ����
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

    if (sym == ident) { // ����Ϊ��ʶ��ʱ
        i = position(id);
        if (i == 0) {
            error(11);
        } else if (table[i].kind != variable) { // assignment to non-variable �Ǳ���
            error(12);
            i = 0;
        }
        getsym();
        if (sym == becomes) { // ����Ϊ :=
            getsym();
        } else {
            error(13);
        }
        expression(fsys);
        if (i != 0) {
            gen(sto, lev - table[i].level, table[i].addr);
        }
    } else if (sym == callsym) { // ����Ϊ������call
        getsym();
        if (sym != ident) { // ���������Ǳ�ʶ��
            error(14);
        } else {
            i = position(id);
            if (i == 0) { //�������������ֱ���
                error(11);
            } else if (table[i].kind == proc) { // ���� call ���������м����
                gen(cal, lev - table[i].level, table[i].addr);
            } else {
                error(15);
            }
            getsym();
        }
    } else if (sym == ifsym) { // ����Ϊ������if
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
    } else if (sym == beginsym) { // ����Ϊ������begin
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
    } else if (sym == whilesym) { //whileѭ�� ��ʵ��exit��׼��
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
    } else if (sym == exitsym) { // ʵ��exit
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
			// ������
		}
		if(sym == string){  // �����ʱ�ַ��� eg write("this is a test\n");
		    expression(fsys | comma | rparen | string);
            gen(buf, 0, 0);
            // getsym();
		} else { // �������
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
            // ������
        }
	} else if (sym == readsym) { // read(ident{,ident})
		getsym();
		if (sym == lparen) {
			getsym();
		}
		else {
			// ������
		}
		if (sym == ident) {
			i = position(id);
			if (i == 0) {
				error(11);
			}
			else {
				if (table[i].kind == variable) {  // ֻ�ܶ��������������������Ķ����ɶ���
					gen(opr, 0, 16);
					gen(sto, lev - table[i].level, table[i].addr);
				}
				else {
					// ������
				}
			}
			getsym();
		}
		else {
			// ������
		}
		while (sym == comma) {
			getsym();
			if (sym == ident) {
				i = position(id);
				if (i == 0) {
					error(11);
				}
				else {
					if (table[i].kind == variable) {  // ֻ�ܶ��������������������Ķ����ɶ���
						gen(opr, 0, 16);
						gen(sto, lev - table[i].level, table[i].addr);
					}
					else {
						// ������
					}
				}
				getsym();
			}
			else {
				// ������
			}
		}
		if (sym == rparen) {
			getsym();
		}
		else {
			// ������
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
    gen(jmp, 0, 0); //Ŀ�����ĳ�ʼ����ʼ��
    if (lev > levmax) { // ����Ƕ�ײ���
        error(32);
    }
    do {
        if (sym == constsym) { // �������͵ĵ���
            getsym(); // ȡ����һ������ ������Ҫ����ĳ������ţ�
            do {
                constdeclaration();
                while (sym == comma) { //�������� eg�� int a, b, c;
                    getsym();
                    constdeclaration();
                }
                if (sym == semicolon) { //�����ֺŽ�����������
                    getsym();
                } else {
                    error(5);
                }
            } while (sym == ident);
        }
        while (sym == varsym) { // �������͵ĵ��� ������һ��������������
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
        if (sym == typesym) { // �������͵ĵ���
			getsym(); // ȡ����һ������ ������Ҫ����ĳ������ţ�
			do {
				typedeclaration();
			} while (sym == ident);
		}
        while (sym == procsym) { // ���̱����ֵĵ���
            getsym();
            if (sym == ident) { //�ǼǶ���Ĺ�����
                enter(proc);
                getsym();
            } else {
                error(4);
            }
            if (sym == semicolon) { //��������ӵ��Ƿֺ� ����procedure multiply;
                getsym();
            } else {
                error(5);
            }
            lev = lev + 1; // Ƕ�ײ���+1 �����ֳ�
            tx1 = tx;
            dx1 = dx;
            block(fsys | semicolon); // ����������һ��procedure�����
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
        test(statbegsys | ident, declbegsys, 7); // ���Ե�ǰ���ʷ��Ƿ�Ϸ�
    } while (sym & declbegsys);
    code[table[tx0].addr].a = cx; // ���ڵ��м�������ɣ�Ϊʲô����д�ģ���������gen()��
    table[tx0].addr = cx; // start addr of code
    cx0 = cx;
    gen(Int, 0, dx); // �м��������
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
    for (i = 0; i < 256; i++) { //ĳ������ĳ�ʼ����
        ssym[i] = nul;
    }

    strcpy(word[0], "and       "); //word[]�ַ������� ��ű������ַ�
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

    wsym[0] = andsym; //wsym[]�޷��ų��������� �ؼ��ֶ�Ӧ��ʮ��������
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

    ssym['+'] = plus; //ssym[]����ͬwsym[] ��������Ӧ��ʮ�������룿
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

    strcpy(mnemonic[lit], "lit"); //mnemonic[]�ַ������� �����м����������Ķ�Ӧ���ţ�
    strcpy(mnemonic[opr], "opr");
    strcpy(mnemonic[lod], "lod");
    strcpy(mnemonic[sto], "sto");
    strcpy(mnemonic[cal], "cal");
    strcpy(mnemonic[Int], "int");
    strcpy(mnemonic[jmp], "jmp");
    strcpy(mnemonic[jpc], "jpc");
    strcpy(mnemonic[buf], "buf");

    declbegsys = constsym | varsym | procsym | typesym; //{���� ���� ������} ��������
    statbegsys = beginsym | callsym | ifsym | whilesym;  //{��ʼ ���� ���� ѭ��} ����������
    statbegsys = statbegsys | booleansym | integersym | realsym | writesym | readsym; // �����ӱ�����
    statbegsys = statbegsys | elsesym | exitsym | orsym | andsym | notsym; // �����ӱ�����
    statbegsys = statbegsys | divsym | modsym; // �����ӱ�����
    facbegsys = ident | number | lparen | string; // {��ʶ�� ���� ������}
    // printf("%lld\n", string);
    // printf("string is %lld\n", facbegsys & string);

    printf("please input source program file name: ");
    scanf("%s", infilename);
    printf("\n");

    if ((infile = fopen(infilename, "r")) == NULL) {
        printf("File %s can't be opened.\n", infilename);
        exit(1);
    }
    /* ������м����д��outfile */
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
