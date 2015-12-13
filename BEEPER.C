#define INCL_DOSINFOSEG
#define INCL_DOSPROCESS

#include <os2.h>

void cdecl main(int argc, char **argv)
{
	SEL gseg,lseg;
	GINFOSEG far *ginfo;
	unsigned char sg;
	int i;

	DosGetInfoSeg(&gseg,&lseg);
	ginfo = MAKEP(gseg,0);
	while (TRUE) {
		sg = ginfo->sgCurrent;
		for (i = 0; i < sg/16; i++) {
			DosBeep(1000,50);
			DosSleep(150L);
		}
		DosSleep(300L);
		for (i = 0; i < sg%16; i++) {
			DosBeep(2000,50);
			DosSleep(150L);
		}
		DosSleep(1500L);
	}
}
