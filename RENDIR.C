/* rename directorys */

#define INCL_DOS

#include <os2.h>
#include <conio.h>

void cdecl main(int, char **);

void cdecl main(argc,argv)
int argc;
char **argv;
{
	if (argc != 3) cputs("Missing or invalid parameter");
	else if (DosMove(argv[1],argv[2],0L))
		cputs("No such directory or duplicate name");
}
