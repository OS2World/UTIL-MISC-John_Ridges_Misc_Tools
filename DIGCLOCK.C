

  /*------------------------------------
       DIGCLOCK.C -- Digital Clock
    -----------------------------------*/

  #define INCL_WIN
  #define INCL_GPI
  #define INCL_DOS
  #include <os2.h>
  #include <stdio.h>

  #define ID_TIMER 1

  MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;
  VOID SizeTheWindow (HWND) ;
      HWND      hwndFrame, hwndClient ;

  int main (void)
      {
      static CHAR szClientClass[] ="DigClock" ;
      static ULONG flFrameFlags = FCF_BORDER | FCF_TASKLIST ;

      HAB       hab ;
      HMQ       hmq ;
      QMSG      qmsg ;

      hab = WinInitialize (0) ;
      hmq = WinCreateMsgQueue (hab, 0) ;

      WinRegisterClass (hab, szClientClass, ClientWndProc, 0L, 0) ;

      hwndFrame = WinCreateStdWindow (HWND_DESKTOP, WS_VISIBLE,
                                      &flFrameFlags, szClientClass, NULL,
                                      0L, NULL, 0, &hwndClient) ;

      SizeTheWindow (hwndFrame) ;

      if (WinStartTimer (hab, hwndClient, ID_TIMER, 1000))
           {
           while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
                WinDispatchMsg (hab, &qmsg) ;

           WinStopTimer (hab, hwndClient, ID_TIMER) ;
           }
      else
           WinMessageBox (HWND_DESKTOP, hwndClient,
                          "Too many clocks or timers",
                          szClientClass, 0, MB_OK | MB_ICONEXCLAMATION) ;

      WinDestroyWindow (hwndFrame) ;
      WinDestroyMsgQueue (hmq) ;
      WinTerminate (hab) ;
      return 0 ;
      }

  VOID SizeTheWindow (HWND hwndFrame)
       {
       RECTL       rcl ;
		 HWND lastwinwithfocus;
       
       rcl.yTop    = WinQuerySysValue(HWND_DESKTOP,SV_CYSCREEN)-7 ;
       rcl.yBottom = rcl.yTop - 44;
       rcl.xRight  = WinQuerySysValue (HWND_DESKTOP, SV_CXSCREEN)-2 ;
       rcl.xLeft   = rcl.xRight - 117;

       WinCalcFrameRect (hwndFrame, &rcl, FALSE) ;

		lastwinwithfocus = WinQueryFocus(HWND_DESKTOP,FALSE);
       WinSetWindowPos (hwndFrame, NULL, (SHORT) rcl.xLeft, (SHORT) rcl.yBottom,
                        (SHORT) (rcl.xRight - rcl.xLeft),
                        (SHORT) (rcl.yTop - rcl.yBottom), SWP_SIZE | SWP_MOVE) ;
		WinSetFocus(HWND_DESKTOP,lastwinwithfocus);
       }

       VOID UpdateTime (HWND hwnd, HPS hps)
            {
            static BOOL         fHaveCtryInfo = FALSE ;
            static CHAR         *szDayName [] = { "Sun", "Mon", "Tue", "Wed",
                                                  "Thu", "Fri", "Sat" } ;
            static CHAR         szDateFormat [] = " %s  %d%s%02d%s%02d " ;
            static COUNTRYCODE  ctryc = { 0, 0 } ;
            static COUNTRYINFO  ctryi ;
            CHAR                szBuffer [20] ;
            DATETIME            dt ;
            RECTL               rcl ;
            USHORT              usDataLength ;

                /*------------------------------------------
                   Get Country Information, Date, and Time
                  -----------------------------------------*/

            if (!fHaveCtryInfo)
                 {
                 DosGetCtryInfo (sizeof ctryi, &ctryc, &ctryi, &usDataLength) ;
                 fHaveCtryInfo = TRUE ;
                 }
            DosGetDateTime (&dt) ;
            dt.year %= 100 ;

                     /* ---------------
                         Format Date
                        ---------------*/
                                           /*-----------------
                                              mm/dd/yy format
                                            -----------------*/

            if (ctryi.fsDateFmt == 0)

                sprintf (szBuffer, szDateFormat, szDayName [dt.weekday],
                         dt.month, ctryi.szDateSeparator,
                         dt.day,   ctryi.szDateSeparator, dt.year) ;

                                          /*--------------------
                                              dd/mm/yy format
                                            --------------------*/


            else if (ctryi.fsDateFmt == 1)

                sprintf (szBuffer, szDateFormat, szDayName [dt.weekday],
                         dt.day,   ctryi.szDateSeparator,
                         dt.month, ctryi.szDateSeparator, dt.year) ;

                                          /*----------------------
                                                 yy/mm/dd
                                            ---------------------*/

            else
                sprintf (szBuffer, szDateFormat, szDayName [dt.weekday],
                         dt.year,   ctryi.szDateSeparator,
                         dt.month,  ctryi.szDateSeparator, dt.day) ;


                       /*-------------
                         Display Date
                        --------------*/

            WinQueryWindowRect (hwnd, &rcl) ;
            rcl.yBottom += 5 * rcl.yTop / 11 ;
            WinDrawText (hps, -1, szBuffer, &rcl, CLR_NEUTRAL, CLR_BACKGROUND,
                         DT_CENTER | DT_VCENTER) ;


                        /*--------------
                           Format Time
                          -------------*/
                                        /*-----------------
                                            12-hour format
                                          -----------------*/

            if ((ctryi.fsTimeFmt & 1) == 0)

                 sprintf (szBuffer, " %d%s%02d%s%02d %cm ",
                                    (dt.hours + 11) % 12 + 1, ctryi.szTimeSeparator,
                                    dt.minutes, ctryi.szTimeSeparator,
                                    dt.seconds, dt.hours / 12 ? 'p' : 'a') ;

                                        /*--------------------
                                            24-hour format
                                         ----------------------*/

            else
                sprintf (szBuffer, " %02d%s%02d%s%02d ", 
                                   dt.hours,   ctryi.szTimeSeparator,
                                   dt.minutes, ctryi.szTimeSeparator, dt.seconds) ;

                        /*--------------------
                             Display Time
                          ------------------*/


            WinQueryWindowRect (hwnd, &rcl) ;
            rcl.yTop -= 5 * rcl.yTop / 11 ;
            WinDrawText (hps, -1, szBuffer, &rcl, CLR_NEUTRAL, CLR_BACKGROUND,
                         DT_CENTER | DT_VCENTER) ;
            }

  MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
       {
       HPS  hps;

       switch (msg)
            {
            case WM_TIMER:
                 hps = WinGetPS (hwnd) ;
                 GpiSetBackMix (hps, BM_OVERPAINT) ;

                 UpdateTime (hwnd, hps) ;

                 WinReleasePS (hps) ;
                 return 0 ;

            case WM_PAINT:
                 hps = WinBeginPaint (hwnd, NULL, NULL) ;
                 GpiErase (hps) ;

                 UpdateTime (hwnd, hps) ;

                 WinEndPaint (hps) ;
                 return 0 ;

	case WM_BUTTON1DOWN:
		return WinSendMsg(hwndFrame,WM_TRACKFRAME,
			(MPARAM)(LOUSHORT(mp2)|TF_MOVE),NULL);
	case WM_CHAR:
		if (!(SHORT1FROMMP(mp1)&KC_KEYUP) && SHORT2FROMMP(mp2) == VK_F4) {
			WinPostMsg(hwnd,WM_QUIT,0,0);
			break;
		}
		default:
		return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
        }
}



                






