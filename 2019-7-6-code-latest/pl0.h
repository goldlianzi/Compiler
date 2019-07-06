#include <stdio.h>

#define norw       19           // no. of reserved words
#define txmax      100          // length of identifier table
#define nmax       14           // max. no. of digits in numbers  ���ֵ����λ��
#define al         10           // length of identifiers  ���Ƶı�ʶ������
#define amax       2047         // maximum address
#define levmax     3            // maximum depth of block nesting ���Ƕ�ײ���
#define cxmax      2000         // size of code array

#define nul		   0x1          // :
#define ident      0x2          //��ʶ�����û�����ĳ����������
#define number     0x4          //����
#define plus       0x8          //�� +
#define minus      0x10         //�� -
#define times      0x20         //�� *
#define slash      0x40         //�� /
#define eql        0x100        //���� =
#define neq        0x200        //���� <>
#define lss        0x400        //С�� <
#define leq        0x800        //С�� <=
#define gtr        0x1000       //���� >
#define geq        0x2000       //���ڵ��� >=
#define lparen     0x4000       //������ (
#define rparen     0x8000       //������ )
#define comma      0x10000      //���� ,
#define semicolon  0x20000      //�ֺ� ;
#define period     0x40000      //С���� �� .
#define becomes    0x80000      //����Ϊ :=
#define oddsym     0x80         //������ odd
#define beginsym   0x100000     //������ begin
#define endsym     0x200000     //������ end
#define ifsym      0x400000     //������ if
#define thensym    0x800000     //������ then
#define whilesym   0x1000000    //������ while
#define dosym      0x2000000    //������ do
#define callsym    0x4000000    //������ call
#define constsym   0x8000000    //������ const
#define varsym     0x10000000   //������ var
#define procsym    0x20000000   //������ procedure
#define writesym   0x40000000   //������ write
#define readsym    0x80000000   //������ read
#define booleansym 0x100000000  //������ boolean
#define realsym    0x200000000  //������ real
#define integersym 0x400000000  //������ integer
#define exitsym    0x800000000  //������ exit
#define elsesym    0x1000000000  //������ else
#define typesym    0x2000000000  //������ else

enum object { //���ֱ����������  ������������������
    constant, variable, proc, typeExp
};

enum Var_object { //��������  ������ʵ�͡����ͣ�Ĭ�ϣ�
    boolean, real, integer, none
};

enum fct {
    lit, opr, lod, sto, cal, Int, jmp, jpc         // functions
};

typedef struct{ //�м����洢��
    enum fct f;		// function code
    long l; 		// level
    long a; 		// displacement address
} instruction;
/*  lit 0, a : load constant a ������ջ
    opr 0, a : execute operation a �Ӽ��˳��Ƚϴ�С�Ȳ���
    lod l, a : load variable l, a ������ջ
    sto l, a : store variable l, a ��ջ���ı���ֵ����a
    cal l, a : call procedure a at level l ���ù���a
    Int 0, a : increment t-register by a ����a�����ݿռ�
    jmp 0, a : jump to a ������ת��
    jpc 0, a : jump conditional to a ������ת�ƣ�aΪת�Ƶ�ַ��ջ������ʱת��       */

char ch;                // last character read   ��ŵ�ǰ��ȡ���ַ�����ֵΪ�գ�
unsigned long long sym;      // last symbol read  ���ÿ�����ʵ�������ڲ�������ʽ��ʾ��
char id[al+1];          // last identifier read  ����û�������ı�ʶ����ֵ������ʶ���ַ����Ļ��ڱ�ʾ��
long num;               // last number read  ����û����������ֵ
long cc;                // character count   ����������ֵΪ0
long ll;                // line length   ����������ֵΪ0�����ڷ��ż���
long kk, err;           // err�������
long cx;                // code allocation index Դ����������к�

char line[81];          // һά���飬������Ԫ�����ַ������Ϊ1��80�����ڶ���һ���ַ��Ļ�������
char a[al+1];           // һά���飬������Ԫ�����ַ������ڵ����ַ����ƴ��
instruction code[cxmax+1];
char word[norw][al+1];  // �����ֱ�һά���飬����Ԫ��Ϊ���ַ�ΪԪ�ص�һά���飨���ַ����������Ϊ1��13�����ҷ�ʽΪ���ַ���
unsigned long long wsym[norw];
unsigned long long ssym[256]; // ���������������ŵ����ͣ�eg:sym = ssym[(unsigned char) ch];

char mnemonic[8][3+1];
unsigned long long declbegsys, statbegsys, facbegsys; //�������� ���������� {��ʶ�������֣�������}

struct{ // ���ֱ� ��� ���ִ����������͡�[����ֵ]��[Ƕ�ײ���]��[��ַ���]
    char name[al+1];
    enum object kind;
    enum Var_object Vkind;
    long val;
    long level;
    long addr;
}table[txmax+1];

char infilename[80];
FILE* infile;
char *outfilename; // �м��������ļ���
FILE* outfile; // �м�������


// the following variables for block
long dx;		// data allocation index ��var���͵����ַ���ı��
long lev;		// current depth of block nesting ��ǰǶ�ײ���
long tx;		// current table index

// the following array space for interpreter
#define stacksize 50000
long s[stacksize];	// datastore

//2019-7-4 exit
long whilenum = 0;
long exitcx = 0;
