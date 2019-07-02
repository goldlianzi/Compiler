void gen(enum fct x, long y, long z) {
    if (cx > cxmax) {
        printf("program too long\n");
        exit(1);
    }
    code[cx].f = x;
    code[cx].l = y;
    code[cx].a = z;
    cx = cx + 1;
}