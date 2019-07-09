#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "pl0.h"

void initialize(){
   // char in[50]='15';
    //printf("%d\n",char_int(in));
    int no;
    char tmp[80];
    fscanf(infile, "%s", tmp);//序号
    while (!feof(infile)) {
        no = atoi(tmp);
        //printf("no:%d\n",no);
        fscanf(infile, "%s", tmp);//code.f
        //code[0].f=tmp;
        if (!strcmp(tmp, "opr")) {
            code[no].f = opr;
        } else if (!strcmp(tmp, "lit")) {
            code[no].f = lit;
        } else if (!strcmp(tmp, "lod")) {
            code[no].f = lod;
        } else if (!strcmp(tmp, "sto")) {
            code[no].f = sto;
        } else if (!strcmp(tmp, "cal")) {
            code[no].f = cal;
        } else if (!strcmp(tmp, "int")) {
            code[no].f = Int;
        } else if (!strcmp(tmp, "jmp")) {
            code[no].f = jmp;
        } else if (!strcmp(tmp, "jpc")) {
            code[no].f = jpc;
        } else if (!strcmp(tmp, "buf")) {
            code[no].f = buf;
        } else {
            break;
            printf("error!\n");
        }

        if (code[no].f == buf) { // 打印字符串
            strcpy(buff[buff_i], "");
            fscanf(infile, "%s", tmp);
            while (atoi(tmp) != (no + 1)) {
                strcat(buff[buff_i], tmp);
                strcat(buff[buff_i], " ");
                fscanf(infile, "%s", tmp);
            }
            code[no].l = 0;
            code[no].a = 0;
            buff_i++;
            continue;
        }

        fscanf(infile, "%s", tmp);//code.l
        code[no].l = atoi(tmp);
        //printf("code.l:%d\n",code[no].l);
        //strcpy(code[0].f,tmp);
        fscanf(infile, "%s", tmp);//code.a
        code[no].a = atoi(tmp);
        //printf("code.a:%d\n",code[no].a);
        //printf("%s\n",tmp);
        //memset(tmp,0,80);
        fscanf(infile, "%s", tmp);
    }
    //printf("code.a:%d\n",code[3].a);
}

long base(long b, long l){
    long b1;

    b1 = b;
    while (l > 0) {	// find base l levels down
        b1 = s[b1];
        l = l - 1;
    }
    return b1;
}

void interpret(){
    long p, b, t;	// program-, base-, topstack-registers
    instruction i;	// instruction register

    long numm = 0;

    int tmp; // 输入暂存处
    long tmp1 = 0, tmp2 = 0, tmp3 = 0, tmp4 = 0, opr_result = 0, amend_bit = 1; //修正器使用

    printf("### start interpreting ###\n");
    t = 0;
    b = 1;
    p = 0;
    s[1] = 0;
    s[2] = 0;
    s[3] = 0;
    do{
	i = code[p];
    p = p + 1;

	switch (i.f) {
	    case lit:
            t = t + 1;
            s[t] = i.a;
            break;
	    case opr:
            switch(i.a){ 	// operator
                case 0:	// return
                    t = b - 1;
                    p = s[t + 3];
                    b = s[t + 2];
                    break;
                case 1:
                    s[t] = -s[t];
                    break;
                case 2:
                    /* 加法修正器 */
                    if (s[t] & 0x00400000) { //布尔修正

                    } else if (s[t] & 0x00800000 || s[t + 1] & 0x00800000 ) { //实型修正
                        t = t - 1;
                        s[t] = s[t] + s[t+1];
                        s[t] = s[t] & 0x003FFFFF; // 保留低位，过滤高位
                        s[t] = s[t] | 0x00800000; // 标识位置数
                    } else if (s[t] & 0x00C00000) { //待扩展

                    } else { // 整型
                        t = t - 1;
                        s[t] = s[t] + s[t + 1];
                    }
                    break;
                case 3:
                    /* 减法修正器 */
                    if (s[t] & 0x00400000) { //布尔修正

                    } else if (s[t] & 0x00800000 || s[t + 1] & 0x00800000 ) { //实型修正
                        t = t - 1;
                        s[t] = s[t] - s[t+1];
                        s[t] = s[t] & 0x003FFFFF; // 保留低位，过滤高位
                        s[t] = s[t] | 0x00800000; // 标识位置数
                    } else if (s[t] & 0x00C00000) { //待扩展

                    } else { // 整型
                        t = t - 1;
                        s[t] = s[t] - s[t + 1];
                    }
                    break;
                case 4:
                    /* 乘法修正器 */
                    if (s[t] & 0x00400000) { //布尔修正

                    } else if (s[t] & 0x00800000 || s[t + 1] & 0x00800000 ) { //实型修正
                        t = t - 1;
                        tmp1 = (tmp2 = (tmp3 = (tmp4 = 0)));
                        opr_result = 0;
                        amend_bit = 0;
                        tmp1 = (s[t] & 0x003FF800) >> 11; // 第一个操作数的高11位，整数位
                        tmp2 = s[t] & 0x000007FF; // 第一个操作数的低11位，小数位
                        tmp3 = (s[t + 1] & 0x003FF800) >> 11; // 第二个操作数的高11位 整数位
                        tmp4 = s[t + 1] & 0x000007FF; // 第二个操作数的低11位，小数位
                        // printf("s[t] is %ld\n", s[t]);
                        // printf("tmp1=%ld,tmp2=%ld,tmp3=%ld,tmp4=%ld\n", tmp1,tmp2,tmp3,tmp4);
                        while (tmp2 > pow(10, amend_bit)) {
                            amend_bit++;
                        }
                        tmp2 = tmp2 * pow(10, -amend_bit);
                        amend_bit = 1;
                        while (tmp4 > pow(10, amend_bit)) {
                            amend_bit++;
                        }
                        tmp4 = tmp4 * pow(10, -amend_bit);
                        // printf("tmp1=%ld,tmp2=%ld,tmp3=%ld,tmp4=%ld\n", tmp1,tmp2,tmp3,tmp4);
                        opr_result = tmp1 * tmp3 + tmp1 * tmp4 + tmp2 * tmp3 + tmp2 * tmp4;
                        // printf("opr_result is %ld\n", opr_result);

                        s[t] = opr_result;
                        s[t] = s[t] & 0x003FFFFF; // 保留低位，过滤高位
                        s[t] = s[t] | 0x00800000; // 标识位置数
                    } else if (s[t] & 0x00C00000) { //待扩展

                    } else { // 整型
                        t = t - 1;
                        s[t] = s[t] * s[t + 1];
                    }
                    break;
                case 5:
                    t = t - 1;
                    s[t] = s[t] / s[t + 1];
                    break;
                case 6:
                    s[t] = s[t] % 2;
                    break;
                case 8:
                    t = t - 1;
                    s[t] = (s[t] == s[t + 1]);
                    break;
                case 9:
                    t = t - 1;
                    s[t] = (s[t] != s[t + 1]);
                    break;
                case 10:
                    t = t - 1;
                    s[t] = (s[t] < s[t + 1]);
                    break;
                case 11:
                    t = t - 1;
                    s[t] = (s[t] >= s[t + 1]);
                    break;
                case 12:
                    t = t - 1;
                    s[t] = (s[t] > s[t + 1]);
                    break;
                case 13:
                    t = t - 1;
                    s[t] = (s[t] <= s[t + 1]);
                    break;
                case 14:  // 输出前几个，需要后面跟一个‘ ’来当分隔符
                    if (s[t] & 0x00400000) {  //布尔修正
                        if (s[t] == 0x00400001) {
                            printf("true ");
                        } else if (s[t] == 0x00400000) {
                            printf("false ");
                        }
                    } else if (s[t] & 0x00800000) { //实型修正
                        s[t] = s[t] & 0xFF7FFFFF;
                        printf("%d.%d ", (s[t] & 0x003FF800) >> 11, (s[t] & 0x000007FF));
                    } else if (s[t] & 0x00C00000) { // 待扩展
                        // 待扩展
                    } else { // 整型
                        printf("%d ",s[t]);
                    }
                    t = t - 1;
                    break;
                case 15: // 输出最后一个，不需要' '来当分隔符
                    if (s[t] & 0x00400000) {  //布尔修正
                        if (s[t] == 0x00400001) {
                            printf("true\n");
                        } else if (s[t] == 0x00400000) {
                            printf("false\n");
                        }
                    } else if (s[t] & 0x00800000) { //实型修正
                        s[t] = s[t] & 0xFF7FFFFF;
                        printf("%d.%d\n", (s[t] & 0x003FF800) >> 11, (s[t] & 0x000007FF));
                    } else if (s[t] & 0x00C00000) { // 待扩展
                        // 待扩展
                    } else { // 整型
                        printf("%d\n",s[t]);
                    }
                    t = t - 1;
                    break;
                case 16: // read()读出一个数据
                    scanf("%d", &tmp);
                    s[++ t] = tmp;
                    break;
                case 17: // or 操作
                    t = t - 1;
                    s[t] = (s[t] | s[t + 1]);
                    break;
                case 18: // and 操作
                    t = t - 1;
                    s[t] = (s[t] & s[t + 1]);
                    break;
                case 19: // not 操作
                    s[t] = ~s[t];
                    s[t] = s[t] & 0x000007FF; //只保留数的低11位数
                    break;
                case 20: // mod 操作
                    t = t - 1;
                    s[t] = s[t] % s[t + 1];
                    break;
            }
            break;
	    case lod:
            t = t + 1;
            s[t] = s[base(b, i.l) + i.a];
            break;
	    case sto:
            s[base(b, i.l) + i.a] = s[t];
            //printf("%10d\n", s[t]); t=t-1; 若需查看中间代码输出，请消除此处注释，或者查看对应的txt文件即可
            break;
	    case cal:		// generate new block mark
            s[t + 1] = base(b, i.l);
            s[t + 2] = b;
            s[t + 3] = p;
            b = t + 1;
            p = i.a;
            break;
	    case Int:
            //printf("okkk!!!\n");
            t = t + i.a;
            break;
	    case jmp:
            p = i.a;
            break;
	    case jpc:
            if(s[t] == 0){
                p = i.a;
            }
            t = t - 1;
            break;
        case buf:
            printf("%s\n", buff[numm++]);
            break;
	}
    }while (p != 0) ;
    printf("### end interpreting ###\n");
}

int main()
{
        printf("please input source program file name: ");
        scanf("%s", infilename);
        printf("\n");
        //strcpy(infilename,"tests.txt");
        if ((infile = fopen(infilename, "r")) == NULL) {
            printf("File %s can't be opened.\n", infilename);
            exit(1);
        } else {
            initialize();
            interpret();
        }
        printf("interpret success\n");
        fclose(infile);
        return 0;
}
