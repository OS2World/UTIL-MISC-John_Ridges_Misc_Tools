#define timer_delay 1

#define INCL_PM
#define INCL_DOSMEMMGR
#include <os2.h>
#include <stdio.h>

MRESULT EXPENTRY MemWndProc( HWND, USHORT, MPARAM, MPARAM );
SHORT cdecl main(void);
BOOL FAR MemInitApp( VOID );

HAB	hAB;
HMQ	hmqMem;
HWND	hwndMem;
HWND	hwndMemFrame;
ULONG avail;

SHORT cdecl main( )
{
	QMSG qmsg;

	hAB = WinInitialize(NULL);

	hmqMem = WinCreateMsgQueue(hAB, 0);
	MemInitApp();

	while( WinGetMsg( hAB, (PQMSG)&qmsg, (HWND)NULL, 0, 0 ) )
	{
		WinDispatchMsg( hAB, (PQMSG)&qmsg );
	}

	WinDestroyWindow( hwndMemFrame );
	WinDestroyMsgQueue( hmqMem );
	WinTerminate( hAB );
	return 0;
}

MRESULT EXPENTRY MemWndProc( hWnd, msg, mp1, mp2 )
HWND   hWnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
	HPS    hPS;
	RECTL   rect;
	LONG    clrFore;
	LONG    clrBack;
	CHAR	szMessage[80];
	USHORT  rgfCmd;
	ULONG ava;

	switch (msg)
	{
	case WM_CREATE:
		DosMemAvail(&avail);
		WinStartTimer(hAB,hWnd,1,timer_delay * 1000);
		break;
	case WM_TIMER:
		DosMemAvail(&ava);
		if (ava != avail) {
			avail = ava;
			WinInvalidateRegion(hWnd,NULL, TRUE);
		}
		break;
	case WM_PAINT:
		hPS = WinBeginPaint( hWnd, (HPS)NULL, (PWRECT)NULL );
		WinQueryWindowRect( hWnd, (PRECTL)&rect );
		clrFore = CLR_BLACK;
		clrBack = CLR_BACKGROUND;
		rgfCmd = DT_CENTER | DT_VCENTER | DT_TEXTATTRS |
			DT_ERASERECT;
		sprintf(szMessage,"%lu K free",avail/1024);
		WinDrawText(hPS, 0xFFFF, szMessage, &rect, clrFore, clrBack, rgfCmd);
		WinEndPaint( hPS );
		break;
	case WM_BUTTON1DOWN:
		return WinSendMsg(hwndMemFrame,WM_TRACKFRAME,
			(MPARAM)(LOUSHORT(mp2)|TF_MOVE),NULL);
	case WM_CHAR:
		if (!(SHORT1FROMMP(mp1)&KC_KEYUP) && SHORT2FROMMP(mp2) == VK_F4) {
			WinPostMsg(hWnd,WM_QUIT,0,0);
			break;
		}
	default:
		return( WinDefWindowProc( hWnd, msg, mp1, mp2 ) );
		break;
	}
	return(0L);
}

