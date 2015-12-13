/* source error scanning program */

#define SEARCH "*.err"
#define TEMPFILE "$$chkerr.tmp"
#define MAXFILES 250

#define INCL_DOS

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void cdecl main(int, char **);

void cdecl main(argc, argv)
int argc;
char **argv;
{
	FILE *fp,*fo;
	BOOL tempopened;
	int i;
	char line1[256];
	char line2[256];
	char tmpstr[64];
	FILEFINDBUF *files;
	SEL sel;
	HDIR hdir = HDIR_SYSTEM;
	BOOL anyerrs = FALSE;
	int numfiles = 0;
	USHORT n = 1;

	DosAllocSeg(MAXFILES*sizeof(FILEFINDBUF),&sel,SEG_NONSHARED);
	files = MAKEP(sel,0);
	if (DosFindFirst(SEARCH,&hdir,FILE_NORMAL,files,sizeof(FILEFINDBUF),&n,0L)) {
		puts("No error files");
		exit(0);
	}
	do {
		n = 1;
		numfiles++;
   } while (!DosFindNext(hdir,files+numfiles,sizeof(FILEFINDBUF),&n));
	DosFindClose(hdir);
	for (i = 0; i < numfiles; i++) {
		tempopened = FALSE;
		if (!(fp = fopen(files[i].achName,"rt"))) {
			printf("Can't open file %s\r\n",files[i].achName);
			exit(1);
		}
		if (!fgets(line1,sizeof(line1),fp) || !strstr(line1,".c")) {
			fclose(fp);
			continue;
		}
		while (fgets(line2,sizeof(line2),fp)) {
			for (n = 1; n < argc; n++) {
				strcpy(tmpstr,"warning ");
				strcat(tmpstr,argv[n]);
				if (strstr(line2,tmpstr)) goto skipit;
			}
			if (!tempopened) {
				tempopened = TRUE;
				if (!(fo = fopen(TEMPFILE,"wt"))) {
					puts("Can't open temporary file");
					exit(1);
				}
				fputs(line1,fo);
				printf("\r\n\n%s",line1);
			}
			fputs(line2,fo);
			printf("%s",line2);
		skipit: ;
		}
		fclose(fp);
		unlink(files[i].achName);
		if (tempopened) {
			anyerrs = TRUE;
			fclose(fo);
			if (rename(TEMPFILE,files[i].achName)) {
				printf("Can't rename file %s\r\n",files[i].achName);
				exit(1);
			}
		}
	}
	DosFreeSeg(sel);
	exit(anyerrs);
}
