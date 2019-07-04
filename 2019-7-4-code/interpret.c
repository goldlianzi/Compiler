#include <stdlib.h>
#include <string.h>
#include "pl0.h"

void initialize(){
       // char in[50]='15';
        //printf("%d\n",char_int(in));
        int no;
        char tmp[80];
        while(!feof(infile)){
                fscanf(infile,"%s",tmp);//序号
                no=atoi(tmp);
                //printf("no:%d\n",no);
                fscanf(infile,"%s",tmp);//code.f
                //code[0].f=tmp;
                if(!strcmp(tmp,"opr")){
                        code[no].f=opr;
                        //printf("f:%d\n",code[no].f);
                }
                else if(!strcmp(tmp,"lit")){
                        code[no].f=lit;
                        //printf("d:%d\n",code[no].f);
                }
                else if(!strcmp(tmp,"lod")){
                        code[no].f=lod;
                }
                else if(!strcmp(tmp,"sto")){
                        code[no].f=sto;
                }
                else if(!strcmp(tmp,"cal")){
                        code[no].f=cal;
                }
                else if(!strcmp(tmp,"int")){
                        //printf("jjj");
                        code[no].f=Int;
                }
                else if(!strcmp(tmp,"jmp")){
                        code[no].f=jmp;
                }
                else if(!strcmp(tmp,"jpc")){
                        code[no].f=jpc;
                }
                else{
                        break;
                        printf("error!\n");
                }
                fscanf(infile,"%s",tmp);//code.l
                code[no].l=atoi(tmp);
                //printf("code.l:%d\n",code[no].l);
                //strcpy(code[0].f,tmp);
                fscanf(infile,"%s",tmp);//code.a
                code[no].a=atoi(tmp);
                //printf("code.a:%d\n",code[no].a);
                //printf("%s\n",tmp);
                //memset(tmp,0,80);

        }
        //printf("code.a:%d\n",code[3].a);

}

long base(long b, long l){
    long b1;

    b1=b;
    while (l>0){	// find base l levels down
	b1=s[b1]; l=l-1;
    }
    return b1;
}

void interpret(){
    long p,b,t;		// program-, base-, topstack-registers
    instruction i;	// instruction register

    printf("start interpreting\n");
    t=0; b=1; p=0;
    s[1]=0; s[2]=0; s[3]=0;
    do{
	i=code[p]; p=p+1;
	switch(i.f){
	    case lit:
		t=t+1; s[t]=i.a;
		break;
	    case opr:
		switch(i.a){ 	// operator
		    case 0:	// return
                t=b-1; p=s[t+3]; b=s[t+2];
                break;
		    case 1:
                s[t]=-s[t];
                break;
		    case 2:
                t=t-1; s[t]=s[t]+s[t+1];
                break;
		    case 3:
                t=t-1; s[t]=s[t]-s[t+1];
                break;
		    case 4:
                t=t-1; s[t]=s[t]*s[t+1];
                break;
		    case 5:
                t=t-1; s[t]=s[t]/s[t+1];
                break;
		    case 6:
                s[t]=s[t]%2;
                break;
		    case 8:
                t=t-1; s[t]=(s[t]==s[t+1]);
                break;
		    case 9:
                t=t-1; s[t]=(s[t]!=s[t+1]);
                break;
		    case 10:
                t=t-1; s[t]=(s[t]<s[t+1]);
                break;
		    case 11:
                t=t-1; s[t]=(s[t]>=s[t+1]);
                break;
		    case 12:
                t=t-1; s[t]=(s[t]>s[t+1]);
                break;
		    case 13:
                t=t-1; s[t]=(s[t]<=s[t+1]);
                break;
            case 14:  // 输出前几个，需要后面跟一个‘ ’来当分隔符
				printf("%d ",s[t]);
				t = t - 1;
				break;
			case 15: // 输出最后一个，不需要' '来当分隔符
				printf("%d\n", s[t]);
				t = t - 1;
		}
		break;
	    case lod:
		t=t+1; s[t]=s[base(b,i.l)+i.a];
		break;
	    case sto:
		s[base(b,i.l)+i.a]=s[t]; printf("%10d\n", s[t]); t=t-1;
		break;
	    case cal:		// generate new block mark
		s[t+1]=base(b,i.l); s[t+2]=b; s[t+3]=p;
		b=t+1; p=i.a;
		break;
	    case Int:
                //printf("okkk!!!\n");
		t=t+i.a;
		break;
	    case jmp:
		p=i.a;
		break;
	    case jpc:
		if(s[t]==0){
		    p=i.a;
		}
		t=t-1;
	}
    }while(p!=0);
    printf("end interpreting\n");
}

int main()
{
        printf("please input source program file name: ");
        scanf("%s", infilename);
        printf("\n");
        //strcpy(infilename,"tests.txt");
        if ((infile = fopen(infilename, "r")) == NULL)
        {
                printf("File %s can't be opened.\n", infilename);
                exit(1);
        }else{
                initialize();
                interpret();
        }
        printf("interpret success\n");
        fclose(infile);
        return 0;
}
