void getch() {
	if (cc == ll) {  //判断此字符是不是在上次读的一行里
		if (feof(infile)) {
			printf("************************************\n");
			printf("      program incomplete\n");
			printf("************************************\n");
			exit(1);
		}
		ll = 0; cc = 0;
		printf("%5d ", cx);

		int f1 = 0, f2 = 0;  // 用来判断 // 和 /* */的情况
		while (!feof(infile)) {  //读一行的字符
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
