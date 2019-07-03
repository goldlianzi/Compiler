#include <stdio.h>

#define norw       11           // no. of reserved words
#define txmax      100          // length of identifier table
#define nmax       14           // max. no. of digits in numbers  数字的最大位数
#define al         10           // length of identifiers  限制的标识符长度
#define amax       2047         // maximum address
#define levmax     3            // maximum depth of block nesting
#define cxmax      2000         // size of code array

#define nul		   0x1          // :
#define ident      0x2          //标识符（用户定义的常量或变量）
#define number     0x4          //数字
#define plus       0x8          //加 +
#define minus      0x10         //减 -
#define times      0x20         //乘 *
#define slash      0x40         //除 /
#define eql        0x100        //等于 =
#define neq        0x200        //不等 <>
#define lss        0x400        //小于 <
#define leq        0x800        //小于 <=
#define gtr        0x1000       //大于 >
#define geq        0x2000       //大于等于 >=
#define lparen     0x4000       //左括号 (
#define rparen     0x8000       //右括号 )
#define comma      0x10000      //逗号 ,
#define semicolon  0x20000      //分号 ;
#define period     0x40000      //小数点 点 .
#define becomes    0x80000      //定义为 :=
#define oddsym     0x80         //保留字 odd
#define beginsym   0x100000     //保留字 begin
#define endsym     0x200000     //保留字 end
#define ifsym      0x400000     //保留字 if
#define thensym    0x800000     //保留字 then
#define whilesym   0x1000000    //保留字 while
#define dosym      0x2000000    //保留字 do
#define callsym    0x4000000    //保留字 call
#define constsym   0x8000000    //保留字 const
#define varsym     0x10000000   //保留字 var
#define procsym    0x20000000   //保留字 procedure

enum object {
    constant, variable, proc
};

enum fct {
    lit, opr, lod, sto, cal, Int, jmp, jpc         // functions
};

typedef struct{
    enum fct f;		// function code
    long l; 		// level
    long a; 		// displacement address
} instruction;
/*  lit 0, a : load constant a
    opr 0, a : execute operation a
    lod l, a : load variable l, a
    sto l, a : store variable l, a
    cal l, a : call procedure a at level l
    Int 0, a : increment t-register by a
    jmp 0, a : jump to a
    jpc 0, a : jump conditional to a       */

char ch;                // last character read   存放当前读取的字符，初值为空；
unsigned long sym;      // last symbol read  存放每个单词的类别，用内部编码形式表示。
char id[al+1];          // last identifier read  存放用户所定义的标识符的值。及标识符字符串的机内表示。
long num;               // last number read  存放用户定义的数的值
long cc;                // character count   计数器，初值为0
long ll;                // line length   计数器，初值为0，行内符号计数
long kk, err;           // err错误个数
long cx;                // code allocation index 源程序代码行行号

char line[81];          // 一维数组，其数组元素是字符，界对为1：80。用于读入一行字符的缓冲区；
char a[al+1];           // 一维数组，其数组元素是字符，用于单个字符存放拼接
instruction code[cxmax+1];
char word[norw][al+1];  // 保留字表，一维数组，数组元素为以字符为元素的一维数组（即字符串），界对为1：13，查找方式为二分法。
unsigned long wsym[norw];
unsigned long ssym[256]; // 用于搜索单个符号的类型？eg:sym = ssym[(unsigned char) ch];

char mnemonic[8][3+1];
unsigned long declbegsys, statbegsys, facbegsys;

struct{
    char name[al+1];
    enum object kind;
    long val;
    long level;
    long addr;
}table[txmax+1];

char infilename[80];
FILE* infile;

// the following variables for block
long dx;		// data allocation index
long lev;		// current depth of block nesting
long tx;		// current table index

// the following array space for interpreter
#define stacksize 50000
long s[stacksize];	// datastore
