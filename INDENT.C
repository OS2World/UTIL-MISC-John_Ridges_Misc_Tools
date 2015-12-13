/* C file indentation cleaner */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void cdecl main(int argc, char **argv)
{
	FILE *fi,*fo;
	char name1[256];
	char name2[256];
	char line[256];
	char temp[256];
	char stack[256];
	char quote,c,brace,*pnt,*tempnt;
	int i,j;
	char level = 0;
	int stackpoint = 0;
	int comment = 0;
	int cont = 0;

	if (argc < 2) {
		puts("\007No file specified");
		exit(1);
	}
	strcpy(name1,argv[1]);
	strcat(name1,".C");
	strcpy(name2,argv[1]);
	strcat(name2,".OLD");
	if (rename(name1,name2)) {
		puts("\007File can not be opened or renamed");
		exit(1);
	}
	fi=fopen(name2,"rt");
	fo=fopen(name1,"wt");
	while (fgets(line,sizeof(line),fi)) {
		pnt = line+strspn(line," \t\n");
		strrev(pnt);
		pnt += strspn(pnt," \t\n");
		strrev(pnt);
		j = 0;
		for (i = 0; pnt[i]; i++) {
			if (comment) {
				if (pnt[i] == '*' && pnt[i+1] == '/') {
					comment = 0;
					i++;
				}
			}
			else {
				if (pnt[i] == '/' && pnt[i+1] == '*') {
					comment = 1;
					i++;
				}
				else if (pnt[i] == '\\' && pnt[i+1]) i++;
				else temp[j++]=pnt[i];
			}
		}
		temp[j] = 0;
		tempnt = temp+strspn(temp," \t\n");
		strrev(tempnt);
		tempnt += strspn(tempnt," \t\n");
		strrev(tempnt);
		brace = 0;
		if (tempnt[0] == '#') {
			if (tempnt[1] == 'i' && tempnt[2] == 'f') stack[stackpoint++] = level;
			if (tempnt[1] == 'e' && tempnt[2] == 'l') level = stack[stackpoint-1];
			if (tempnt[1] == 'e' && tempnt[2] == 'n') if (!(stackpoint--)) {
				puts("\007#endif before #if");
				exit(1);
			}
		}
		else {
			quote = 0;
			for (i = 0; tempnt[i]; i++) {
				if (tempnt[i] == quote) quote = 0;
				else if (!quote) {
					if (tempnt[i] == '"' || tempnt[i] == '\'') quote = tempnt[i];
					else if (tempnt[i] == '{') brace++;
					else if (tempnt[i] == '}') brace--;
				}
			}
		}
		if (pnt[0]) {
			c = tempnt[0] ? tempnt[strlen(tempnt)-1] : 0;
			for (i = 0; i < level+
				(cont && tempnt[0] != '{' && tempnt[0] != '#' && level)-
				(c == ':' || tempnt[0] == '}'); i++) fputc('\t',fo);
			fputs(pnt,fo);
			if (tempnt[0] && tempnt[0] != '#')
				cont = c != ';' && c != '{' && c != '}' && c != ':';
			level += brace;
		}
		fputc('\n',fo);
	}
	fcloseall();
	if (stackpoint) {
		puts("\007Unmatched #if");
		exit(1);
	}
	if (level) {
		puts("\007Unmatched brace");
		exit(1);
	}
	if (comment) {
		puts("\007Open comment");
		exit(1);
	}
}
