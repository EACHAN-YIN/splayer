/* 
 *	Copyright (C) 2003-2006 Gabest
 *	http://www.gabest.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *   
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *   
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. 
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

// PlayerToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "mplayerc.h"
#include <math.h>
#include <atlbase.h>
#include <afxpriv.h>
#include "PlayerToolBar.h"
#include "MainFrm.h"
#include "../../svplib/svplib.h"

typedef HRESULT (__stdcall * SetWindowThemeFunct)(HWND hwnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList);

// CPlayerToolBar

IMPLEMENT_DYNAMIC(CPlayerToolBar, CToolBar)
CPlayerToolBar::CPlayerToolBar() :
m_hovering(0),
holdStatStr(0),
iButtonWidth (30),
m_pbtnList(&m_btnList)
{
	 
}

CPlayerToolBar::~CPlayerToolBar()
{
}
#define ID_VOLUME_THUMB 126356

BOOL CPlayerToolBar::Create(CWnd* pParentWnd)
{
	int iToolBarID = IDB_PLAYERTOOLBAR;
	/*
	CRect rcDesktop;
		GetDesktopWindow()->GetWindowRect(&rcDesktop);
	
		if( rcDesktop.Width() < 1200){
			iToolBarID = IDB_PLAYERTOOLBAR_SMALL;
			iButtonWidth = 20;
		}*/
	
 
	if(!__super::CreateEx(pParentWnd,
		TBSTYLE_FLAT|TBSTYLE_TRANSPARENT|TBSTYLE_AUTOSIZE,
		WS_CHILD|WS_VISIBLE|CBRS_ALIGN_BOTTOM , CRect(0,0,0,0))  //CBRS_TOOLTIPS NEW UI
	) //|| !LoadToolBar(iToolBarID)
		return FALSE;

	GetToolBarCtrl().SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);

	CSUIButton* btnPlay = new CSUIButton(L"BTN_PLAY.BMP" , ALIGN_TOPLEFT, CRect(-50 , 6, 3,3)  , 0, ID_PLAY_PLAY, FALSE, 0, 0 );
	//btnPlay->m_stat = 3; //disabled
	m_btnList.AddTail( btnPlay );

	CSUIButton* btnPause = new CSUIButton(L"BTN_PAUSE.BMP" , ALIGN_TOPLEFT, CRect(-50 , 6, 3,3)  , 0, ID_PLAY_PAUSE, TRUE, 0, 0 );
	m_btnList.AddTail( btnPause );

	CSUIButton* btnStop = new CSUIButton(L"BTN_STOP.BMP" , ALIGN_TOPLEFT, CRect(-50 , 8, 3,3)  , 0, ID_PLAY_STOP, FALSE, ALIGN_RIGHT , btnPause  , CRect(10 , 10 , 10, 10));
	btnStop->addAlignRelButton(ALIGN_RIGHT, btnPlay , CRect(10 , 10 , 10, 10) );
	m_btnList.AddTail( btnStop );

	
	CSUIButton* btnStep = new CSUIButton(L"BTN_STEP.BMP" , ALIGN_TOPLEFT, CRect(-50 , 8, 3,3)  , 0, ID_PLAY_FRAMESTEP, FALSE, ALIGN_LEFT , btnPause  , CRect(10 , 10 , 10, 10));
	btnStep->addAlignRelButton(ALIGN_LEFT, btnPlay , CRect(10 , 10 , 10, 10) );
	m_btnList.AddTail( btnStep );

	CSUIButton* btnFFwd = new CSUIButton(L"FAST_FORWORD.BMP" , ALIGN_TOPLEFT, CRect(-52 , 10, 3,3)  , 0, ID_PLAY_FWD, FALSE, ALIGN_LEFT, btnPause , CRect(5 , 10 , 5, 10));
	btnFFwd->addAlignRelButton(  ALIGN_LEFT, btnPlay , CRect(5 , 10 , 5, 10) );
	btnFFwd->addAlignRelButton(ALIGN_LEFT, btnStep , CRect(5 , 10 , 5, 10) );
	m_btnList.AddTail( btnFFwd );

	
	CSUIButton* btnFFBack = new CSUIButton(L"FAST_BACKWORD.BMP" , ALIGN_TOPLEFT, CRect(-48 , 10, 3,3)  , 0, ID_PLAY_BWD, FALSE, ALIGN_RIGHT, btnPause , CRect(5 , 10 , 5, 10) );
	btnFFBack->addAlignRelButton(ALIGN_RIGHT, btnPlay , CRect(5 , 10 , 5, 10) );
	btnFFBack->addAlignRelButton(ALIGN_RIGHT, btnStop , CRect(5 , 10 , 5, 10) );
	m_btnList.AddTail( btnFFBack );

	CSUIButton* btnPrev = new CSUIButton(L"BTN_PREV.BMP" , ALIGN_TOPLEFT, CRect(-48 , 10, 3,3)  , 0, ID_NAVIGATE_SKIPBACK, FALSE, ALIGN_RIGHT, btnFFBack , CRect(10 , 10 , 10, 10) ) ;
	m_btnList.AddTail( btnPrev );

	m_btnList.AddTail( new CSUIButton(L"BTN_NEXT.BMP" , ALIGN_TOPLEFT, CRect(-48 ,10, 3,3)  , 0, ID_NAVIGATE_SKIPFORWARD, FALSE, ALIGN_LEFT, btnFFwd , CRect(10 , 10 , 10, 10) ) );
	
	CSUIButton* btnLogo =  new CSUIButton(L"SPLAYER.BMP" , ALIGN_TOPLEFT, CRect(20 , 7, 3,3)  , TRUE, 0, FALSE   ) ;
	m_btnList.AddTail(btnLogo);

	CSUIButton* btnSubFont =   new CSUIButton(L"BTN_FONT.BMP" , ALIGN_TOPLEFT, CRect(-47 , 5, 3,3)  , 0, ID_SUBSETFONTBOTH /*sub font*/, TRUE, ALIGN_RIGHT, btnPrev , CRect(20 , 10 , 30, 10) );
	btnSubFont->addAlignRelButton(ALIGN_RIGHT, btnFFBack   , CRect(20 , 10 , 30, 10) );
	m_btnList.AddTail( btnSubFont );

 	CSUIButton* btnSubFontPlus =   new CSUIButton(L"BTN_FONT_PLUS.BMP" , ALIGN_TOPLEFT, CRect(-10 , 5, 3,3)  , 0, ID_SUBFONTUPBOTH , TRUE, ALIGN_LEFT, btnSubFont , CRect(3 , 10 , 3, 10) );
 	m_btnList.AddTail( btnSubFontPlus );
 
 	CSUIButton* btnSubFontMinus =   new CSUIButton(L"BTN_FONT_MINUS.BMP" , ALIGN_TOPLEFT, CRect(-10 , 15, 3,3)  , 0, ID_SUBFONTDOWNBOTH , TRUE, ALIGN_LEFT, btnSubFont , CRect(3 , 10 , 3, 10) );
 	btnSubFontMinus->addAlignRelButton(ALIGN_TOP, btnSubFontPlus ,  CRect(3 , 0 , 3, 0) );
 	m_btnList.AddTail( btnSubFontMinus );

	CSUIButton* btnSubSwitch = new CSUIButton(L"BTN_SUB.BMP" , ALIGN_TOPLEFT, CRect(-37 , 5, 3,3)  , 0, ID_SUBTOOLBARBUTTON, TRUE, ALIGN_RIGHT, btnFFBack , CRect(20 , 10 , 80, 10) );
	btnSubSwitch->addAlignRelButton(ALIGN_RIGHT, btnSubFont ,  CRect(20 , 10 , 35, 10) );
	m_btnList.AddTail( btnSubSwitch );

	m_btnList.AddTail( new CSUIButton(L"BTN_SUB_DELAY_REDUCE.BMP" , ALIGN_TOPLEFT, CRect(-42 , 7, 3,3)  , 0, ID_SUBDELAYDEC, TRUE, ALIGN_RIGHT, btnSubSwitch , CRect(2 , 3 , 2, 3) ) );
	m_btnList.AddTail( new CSUIButton(L"BTN_SUB_DELAY_INCREASE.BMP" , ALIGN_TOPLEFT, CRect(-10 , 7, 3,3)  , 0, ID_SUBDELAYINC, TRUE, ALIGN_LEFT, btnSubSwitch , CRect(2 , 3 , 2, 3) ) );
	
	m_btnVolBG = new CSUIButton(L"VOLUME_BG.BMP" , ALIGN_TOPRIGHT, CRect(3 , 11, 20,3)  , TRUE, 0, FALSE ) ;
	m_btnList.AddTail( m_btnVolBG );

	BOOL bIsMuted = IsMuted();
	CSUIButton* btnMute = new CSUIButton(L"MUTED.BMP" , ALIGN_TOPRIGHT, CRect(3 , 10, 105,3)  , FALSE, ID_VOLUME_MUTE, !bIsMuted , ALIGN_RIGHT, m_btnVolBG,  CRect(3 , 3 , 3, 3)) ;
	m_btnList.AddTail( btnMute );

	CSUIButton* btnVol =  new CSUIButton(L"VOLUME.BMP" , ALIGN_TOPRIGHT, CRect(3 , 10, 105,3)  , FALSE, ID_VOLUME_MUTE, bIsMuted  , ALIGN_RIGHT, m_btnVolBG,  CRect(3 , 3 , 3, 3)) ;
	m_btnList.AddTail(btnVol);

	CSUIButton* btnSetting = new CSUIButton(L"BTN_SETTING.BMP" , ALIGN_TOPRIGHT, CRect(-70 , 5, 105,3)  , FALSE, ID_VIEW_OPTIONS, TRUE , ALIGN_RIGHT, btnMute , CRect(10 , 10 , 10, 10)) ;
	btnSetting->addAlignRelButton( ALIGN_RIGHT , btnVol , CRect(10 , 10 , 10, 10) );
	m_btnList.AddTail( btnSetting );

	CSUIButton* btnPlayList = new CSUIButton(L"BTN_PLAYLIST.BMP" , ALIGN_TOPRIGHT, CRect(3 , 5, 105,3)  , FALSE, ID_VIEW_PLAYLIST, TRUE , ALIGN_RIGHT, btnSetting , CRect(3 , 10 , 3, 10)) ;
	btnPlayList->addAlignRelButton( ALIGN_RIGHT , btnVol , CRect(10 , 10 , 10, 10) );
	btnPlayList->addAlignRelButton( ALIGN_RIGHT , btnMute , CRect(10 , 10 , 10, 10) );
	m_btnList.AddTail( btnPlayList );


	CSUIButton* btnCapture = new CSUIButton(L"BTN_CAPTURE.BMP" , ALIGN_TOPRIGHT, CRect(3 , 5, 105,3)  , FALSE, ID_FILE_SAVE_IMAGE_AUTO, TRUE , ALIGN_RIGHT, btnPlayList , CRect(3 , 10 , 3, 10)) ;
	m_btnList.AddTail( btnCapture );

	CSUIButton* btnOpenFile = new CSUIButton(L"BTN_OPENFILE_SMALL.BMP" , ALIGN_TOPRIGHT, CRect(3 , 5, 105,3)  , FALSE, ID_FILE_OPENQUICK, TRUE , ALIGN_RIGHT, btnCapture , CRect(3 , 10 , 3, 10)) ;
	btnOpenFile->addAlignRelButton(ALIGN_RIGHT, btnPlayList , CRect(3 , 10 , 3, 10));
	m_btnList.AddTail( btnOpenFile );


	
	
	m_btnVolTm = new CSUIButton(L"VOLUME_TM.BMP" , ALIGN_TOPRIGHT, CRect(3 , 10, 65,3)  , FALSE, ID_VOLUME_THUMB, FALSE );
	m_btnList.AddTail( m_btnVolTm );

	cursorHand = ::LoadCursor(NULL, IDC_HAND);

	GetSystemFontWithScale(&m_statft, 14.0);

	CDC ScreenDC;
	ScreenDC.CreateIC(_T("DISPLAY"), NULL, NULL, NULL);
	m_nLogDPIY = ScreenDC.GetDeviceCaps(LOGPIXELSY);
	

	
		m_volctrl.Create(this);
	
	EnableToolTips(TRUE);


	return TRUE;
}

void CPlayerToolBar::OnMove(int x, int y){
	__super::OnMove(x, y);
	ReCalcBtnPos();
}
void CPlayerToolBar::ReCalcBtnPos(){
	CRect rc;
	GetWindowRect(&rc);
	m_btnList.OnSize( rc);
}
void CPlayerToolBar::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	ArrangeControls();

	CRect rc;
	GetWindowRect(&rc);
	long iWidth = rc.Width();
	BOOL hideT1 = TRUE;
	BOOL hideT2 = TRUE;
	BOOL hideT3 = TRUE;
	BOOL hideT4 = TRUE;
	if( iWidth > 510 ){
			hideT1 = false;
	}
	if( iWidth > 580 ){
		hideT2 = false;
	}
	if( iWidth > 635 ){
		hideT3 = false;
	}
	if( iWidth > 680 ){
		hideT4 = false;
	}

	m_btnList.SetHideStat(ID_NAVIGATE_SKIPBACK , hideT1);
	m_btnList.SetHideStat(ID_NAVIGATE_SKIPFORWARD , hideT1);

	m_btnList.SetHideStat(ID_VIEW_PLAYLIST , hideT2);
	m_btnList.SetHideStat(ID_SUBTOOLBARBUTTON , hideT2);
	m_btnList.SetHideStat(ID_SUBDELAYDEC , hideT2);
	m_btnList.SetHideStat(ID_SUBDELAYINC , hideT2);
	m_btnList.SetHideStat(ID_FILE_OPENQUICK , hideT2);

	
	m_btnList.SetHideStat(ID_FILE_SAVE_IMAGE , hideT3);
	m_btnList.SetHideStat(ID_VIEW_OPTIONS , hideT3);
	

	m_btnList.SetHideStat(ID_SUBSETFONTBOTH , hideT4);
	m_btnList.SetHideStat(ID_SUBFONTUPBOTH , hideT4);
	m_btnList.SetHideStat(ID_SUBFONTDOWNBOTH , hideT4);

	m_btnList.OnSize( rc);
}

BOOL CPlayerToolBar::PreCreateWindow(CREATESTRUCT& cs)
{
	if(!__super::PreCreateWindow(cs))
		return FALSE;

	m_dwStyle &= ~CBRS_BORDER_TOP;
	m_dwStyle &= ~CBRS_BORDER_LEFT;
	m_dwStyle &= ~CBRS_BORDER_RIGHT;
	m_dwStyle &= ~CBRS_BORDER_BOTTOM;
//	m_dwStyle |= CBRS_SIZE_FIXED;

	return TRUE;
}

void CPlayerToolBar::ArrangeControls()
{
	if(!::IsWindow(m_volctrl.m_hWnd)) return;

	/*
	CRect r;
		GetClientRect(&r);
	
		CRect br = GetBorders();
	
		CRect r10;
		GetItemRect(18, &r10);
	
		CRect vr;
		m_volctrl.GetClientRect(&vr);
		CRect vr2(r.right+br.right-iButtonWidth*2, r.top-1, r.right+br.right+6, r.bottom);
		m_volctrl.MoveWindow(vr2);
	
		UINT nID;
		UINT nStyle;
		int iImage;
		GetButtonInfo(20, nID, nStyle, iImage);
		SetButtonInfo(19, GetItemID(19), TBBS_SEPARATOR, vr2.left - iImage - r10.right - 19);*/
	
}

void CPlayerToolBar::SetMute(bool fMute)
{
	/*
CToolBarCtrl& tb = GetToolBarCtrl();
	TBBUTTONINFO bi;
	bi.cbSize = sizeof(bi);
	bi.dwMask = TBIF_IMAGE;
	bi.iImage = fMute?21:20;
	tb.SetButtonInfo(ID_VOLUME_MUTE, &bi);
*/
	if(fMute){
		m_btnList.SetHideStat(L"VOLUME.BMP", TRUE);
		m_btnList.SetHideStat(L"MUTED.BMP", FALSE);
	}else{
		m_btnList.SetHideStat(L"VOLUME.BMP", FALSE);
		m_btnList.SetHideStat(L"MUTED.BMP", TRUE);
	}
	AfxGetAppSettings().fMute = fMute;
}

bool CPlayerToolBar::IsMuted()
{
/*
	CToolBarCtrl& tb = GetToolBarCtrl();
	TBBUTTONINFO bi;
	bi.cbSize = sizeof(bi);
	bi.dwMask = TBIF_IMAGE;
	tb.GetButtonInfo(ID_VOLUME_MUTE, &bi);
	return(bi.iImage==21);
*/

	return AfxGetAppSettings().fMute;
}

int CPlayerToolBar::GetVolume()
{
	int volume = min( m_volctrl.GetPos() , 100);
	volume = (int)((log10(1.0*volume)-2)*5000);
	volume = max(min(volume, 0), -10000);
	return(IsMuted() ? -10000 : volume);
}

void CPlayerToolBar::SetVolume(int volume)
{
/*
	volume = (int)pow(10, ((double)volume)/5000+2);
	volume = max(min(volume, 100), 1);
*/
	m_volctrl.SetPosInternal(volume);
	
}

BEGIN_MESSAGE_MAP(CPlayerToolBar, CToolBar)
	ON_MESSAGE_VOID(WM_INITIALUPDATE, OnInitialUpdate)
	ON_COMMAND_EX(ID_VOLUME_MUTE, OnVolumeMute)
	ON_UPDATE_COMMAND_UI(ID_VOLUME_MUTE, OnUpdateVolumeMute)
	ON_COMMAND_EX(ID_VOLUME_UP, OnVolumeUp)
	ON_COMMAND_EX(ID_VOLUME_DOWN, OnVolumeDown)
	ON_WM_NCPAINT()
	ON_WM_MOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_NCCALCSIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_ERASEBKGND()
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnTtnNeedText)
END_MESSAGE_MAP()

// CPlayerToolBar message handlers


BOOL CPlayerToolBar::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message){
	
	CPoint pt;
	::GetCursorPos (&pt);
	ScreenToClient (&pt);

	if(m_nItemToTrack){	
		SetCursor(cursorHand );
		return TRUE;
	}
	return CWnd::OnSetCursor(pWnd, 0, 0);
}

CSize CPlayerToolBar::CalcFixedLayout(BOOL bStretch,BOOL bHorz ){

	
	CSize size( 32767, 33 * m_nLogDPIY / 96 );

	if ( CWnd* pParent = AfxGetMainWnd() )
	{
		CRect rc;
		pParent->GetWindowRect( &rc );
		size.cx = rc.Width() - 2;
	}

	CRect rc;
	GetWindowRect(&rc);
	m_btnList.OnSize( rc);

	return size;



	//return __super::CalcFixedLayout(bStretch,bHorz);
}
void CPlayerToolBar::OnPaint()
{
	//LONGLONG startTime = AfxGetMyApp()->GetPerfCounter();
	CPaintDC dc(this); // device context for painting
	CRect paintRect(dc.m_ps.rcPaint);

	
	CRect rcClient;
	GetClientRect(&rcClient);
	CMemoryDC hdc(&dc, rcClient);

	CRect rc;
	GetWindowRect(&rc);

	if( paintRect == m_btnVolBG->m_rcHitest){
		m_btnVolBG->OnPaint( &hdc, rc);
		m_btnVolTm->OnPaint( &hdc, rc);
		//SVP_LogMsg5(_T("Just Paint Vol"));
		return;
	}
	//SVP_LogMsg5(_T("Just Paint All %d %d ") , paintRect.left , paintRect.top);
	CRect rcBottomSqu = rcClient;
	rcBottomSqu.top = rcBottomSqu.bottom - 10;
	//hdc.FillSolidRect(rcBottomSqu, NEWUI_COLOR_BG);

	CRect rcUpperSqu = rcClient;
	//rcUpperSqu.bottom = rcUpperSqu.bottom - 10;
	hdc.FillSolidRect(rcUpperSqu, NEWUI_COLOR_TOOLBAR_UPPERBG);


 	UpdateButtonStat();
	
	int volume = min( m_volctrl.GetPos() , m_volctrl.GetRangeMax() );
	m_btnVolTm->m_rcHitest.MoveToX(m_btnVolBG->m_rcHitest.left +  ( m_btnVolBG->m_rcHitest.Width() * volume / m_volctrl.GetRangeMax() ) - m_btnVolTm->m_rcHitest.Width()/2);
	/*
	SVP_LogMsg5(_T("Vol left %d") , m_btnVolTm->m_rcHitest.left);
	
		CString szLog;
		szLog.Format(_T("TM POS %d %d"), volume , m_btnVolTm->m_rcHitest.left );
		SVP_LogMsg(szLog);*/
	
 	m_btnList.PaintAll(&hdc, rc);

	if(!m_timerstr.IsEmpty()){
		
		HFONT holdft = (HFONT)hdc.SelectObject(m_statft);

		hdc.SetTextColor(0xffffff);
		CSize size = hdc.GetTextExtent(m_timerstr);
		CRect frc ( rcClient );
		size.cx = min( rcClient.Width() /3, size.cx);
		frc.left += 20;
		frc.bottom -= 7;
		frc.right = frc.left + size.cx;
		
		::DrawText(hdc, m_timerstr, m_timerstr.GetLength(), frc,  DT_LEFT|DT_SINGLELINE| DT_VCENTER);
		hdc.SelectObject(holdft);

		
	}
	//LONGLONG costTime = AfxGetMyApp()->GetPerfCounter() - startTime;
	//SVP_LogMsg3("Toolbar Paint @ %I64d cost %I64d" , startTime , costTime);
}
void CPlayerToolBar::UpdateButtonStat(){
	CMainFrame* pFrame = ((CMainFrame*)GetParentFrame());
	BOOL fShow = pFrame->GetUIStat( ID_PLAY_STOP );
	m_btnList.SetHideStat( ID_PLAY_PLAY , fShow );
	//m_btnList.SetHideStat( ID_PLAY_STOP , !fShow );
	//m_btnList.SetHideStat( ID_PLAY_FRAMESTEP , !fShow );
	m_btnList.SetHideStat( ID_PLAY_PAUSE , !fShow );
	BOOL bLogo = pFrame->IsSomethingLoaded();
	m_btnList.SetHideStat(_T("SPLAYER.BMP"), bLogo);
	m_btnList.SetDisableStat(ID_SUBTOOLBARBUTTON, !bLogo);
	if(!bLogo){
		m_timerstr.Empty();
	}
	BOOL bSub = pFrame->IsSubLoaded();
	m_btnList.SetDisableStat( ID_SUBDELAYINC, !bSub);
	m_btnList.SetDisableStat( ID_SUBDELAYDEC, !bSub);
	ReCalcBtnPos();
}
void CPlayerToolBar::OnNcPaint() // when using XP styles the NC area isn't drawn for our toolbar...
{
	//New UI GetSysColor(COLOR_BTNFACE)

	//填充背景-----------------------------------------   
	//dc.FillSolidRect(wr, RGB(214,219,239) );   

	// Do not call CToolBar::OnNcPaint() for painting messages
}

void CPlayerToolBar::OnInitialUpdate()
{
	ArrangeControls();
}

void CPlayerToolBar::SetStatusTimer(CString str , UINT timer )
{
	if(m_timerstr == str) return;

	str.Trim();
	
	if(holdStatStr && !timer){
		m_timerqueryedstr = str;
	}else{
		m_timerstr = str;
		Invalidate();
	}
	if(timer){
		KillTimer(TIMER_STATERASER); 
		holdStatStr = TRUE;
		SetTimer(TIMER_STATERASER, timer , NULL);
	}
	
}

void CPlayerToolBar::SetStatusTimer(REFERENCE_TIME rtNow, REFERENCE_TIME rtDur, bool fHighPrecision, const GUID* pTimeFormat, double playRate)
{
	ASSERT(pTimeFormat);

	CString str;
	CString posstr, durstr;

	if(*pTimeFormat == TIME_FORMAT_MEDIA_TIME)
	{
		DVD_HMSF_TIMECODE tcNow = RT2HMSF(rtNow);
		DVD_HMSF_TIMECODE tcDur = RT2HMSF(rtDur);

		if(tcDur.bHours > 0 || (rtNow >= rtDur && tcNow.bHours > 0)) 
			posstr.Format(_T("%02d:%02d:%02d"), tcNow.bHours, tcNow.bMinutes, tcNow.bSeconds);
		else 
			posstr.Format(_T("%02d:%02d"), tcNow.bMinutes, tcNow.bSeconds);

		if(tcDur.bHours > 0)
			durstr.Format(_T("%02d:%02d:%02d"), tcDur.bHours, tcDur.bMinutes, tcDur.bSeconds);
		else
			durstr.Format(_T("%02d:%02d"), tcDur.bMinutes, tcDur.bSeconds);

		if(fHighPrecision)
		{
			str.Format(_T("%s.%03d"), posstr, (rtNow/10000)%1000);
			posstr = str;
			str.Format(_T("%s.%03d"), durstr, (rtDur/10000)%1000);
			durstr = str;
			str.Empty();
		}
	}
	else if(*pTimeFormat == TIME_FORMAT_FRAME)
	{
		posstr.Format(_T("%I64d"), rtNow);
		durstr.Format(_T("%I64d"), rtDur);
	}

	str = (/*start <= 0 &&*/ rtDur <= 0) ? posstr : posstr + _T(" / ") + durstr;

	SYSTEM_POWER_STATUS status;
	GetSystemPowerStatus(&status);
	CString szPower ;
	if ( status.BatteryFlag != 128 && status.BatteryFlag != 255 && status.BatteryLifePercent < 91 ){
		szPower.Format(_T("  电量: %d%% "), status.BatteryLifePercent);
	}else{
		//szPower = _T("电量: ∽ ");
	}
	CString szPlayrate;
	if(fabs(playRate - 1.0) > 0.02 && playRate > 0.01)	{
		szPlayrate.Format(_T("  速率 %0.1fx "), playRate);
	}

	CMainFrame* pFrame = ((CMainFrame*)GetParentFrame());
	CString szPlayingFileName = pFrame->GetCurPlayingFileName();
	if(!szPlayingFileName.IsEmpty()){
		//szPlayingFileName.Append(_T("  "));
	}

//szPlayingFileName
	SetStatusTimer( str + szPlayrate + szPower  );
}
BOOL CPlayerToolBar::OnVolumeMute(UINT nID)
{
	SetMute(!IsMuted()); 
	return FALSE;
}

void CPlayerToolBar::OnUpdateVolumeMute(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
	pCmdUI->SetCheck(IsMuted());
}

BOOL CPlayerToolBar::OnVolumeUp(UINT nID)
{
	m_volctrl.IncreaseVolume();
	return FALSE;
}

BOOL CPlayerToolBar::OnVolumeDown(UINT nID)
{
	m_volctrl.DecreaseVolume();
	return FALSE;
}
static BOOL m_bMouseDown = FALSE;
bool  CPlayerToolBar::OnSetVolByMouse(CPoint point){
	CMainFrame* pFrame = ((CMainFrame*)GetParentFrame());
	long nTBPos = point.x - m_btnVolBG->m_rcHitest.left;
	long TBMax = m_btnVolBG->m_rcHitest.right-m_btnVolBG->m_rcHitest.left ;
	nTBPos = max(0 , min(TBMax , nTBPos) );
	int Vol = 	nTBPos * m_volctrl.GetRangeMax() / TBMax;

	m_volctrl.SetPosInternal( Vol );


	pFrame->OnPlayVolume(0);
	/*
	CRect rc;
		GetWindowRect(&rc);
		CRect pRect(m_btnVolBG->m_rcHitest);
		pRect -= rc.TopLeft();
		InvalidateRect( pRect,true);*/
	
	Invalidate(FALSE);

	return true;
}

INT_PTR CPlayerToolBar::OnToolHitTest(	CPoint point,TOOLINFO* pTI 	) const
{
	if(!pTI){
		return -1;
	}
	
	CRect rc;
	CMainFrame* pFrame = ((CMainFrame*)GetParentFrame());
	GetWindowRect(&rc);
	point += rc.TopLeft() ;
	//CSUIBtnList* x = (CSUIBtnList*)&m_btnList;
	UINT ret = m_nItemToTrack;//m_pbtnList->OnHitTest(point,rc);
	//SendMessage(WM_USER+31, (point.x & 0xffff) << 16 | (0xffff & point.y), (UINT_PTR)&ret );
	if(ret){
		
		
			pTI->hwnd = m_hWnd;
			pTI->uId = (UINT) (ret);
			//pTI->uFlags = TTF_IDISHWND;
			pTI->lpszText = LPSTR_TEXTCALLBACK;
			RECT rcClient;
			GetClientRect(&rcClient);

			pTI->rect = rcClient;

			return pTI->uId;
		
	}
	return -1;

};
void CPlayerToolBar::OnMouseMove(UINT nFlags, CPoint point){

	CSize diff = m_lastMouseMove - point;
	BOOL bMouseMoved =  diff.cx || diff.cy ;
	if(bMouseMoved)
		m_lastMouseMove = point;

	CRect rc;
	CMainFrame* pFrame = ((CMainFrame*)GetParentFrame());
	GetWindowRect(&rc);
	point += rc.TopLeft() ;
	
	if( m_nItemToTrack == ID_VOLUME_THUMB && m_bMouseDown ){
		if( bMouseMoved){
			OnSetVolByMouse(point);
			
		}
	}else if(bMouseMoved){
		
		UINT ret = m_btnList.OnHitTest(point,rc);
		m_nItemToTrack = ret;
		if(ret){
			
		}else if(!m_tooltip.IsEmpty()){
			m_tooltip.Empty();
		}
		if( m_btnList.HTRedrawRequired ){
			Invalidate();
		}
	}
	
	return;
}

BOOL CPlayerToolBar::OnTtnNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
	UNREFERENCED_PARAMETER(id);

	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
	UINT_PTR nID = pNMHDR->idFrom;
	BOOL bRet = FALSE;

	
		// idFrom is actually the HWND of the tool
	CString toolTip;
	switch(nID){
				case ID_SUBDELAYDEC:
					toolTip = _T("减少字幕延时");
					break;
				case ID_SUBDELAYINC:
					toolTip = _T("增加字幕延时");
					break;
				case ID_SUBTOOLBARBUTTON:
					toolTip = _T("字幕切换或调用");
					break;
				case ID_VIEW_PLAYLIST:
					toolTip = _T("播放列表");
					break;
				case ID_VIEW_OPTIONS:
					toolTip = _T("设置面板");
					break;
				case ID_FILE_SAVE_IMAGE:
					toolTip = _T("快速截图");
					break;
				case ID_FILE_OPENQUICK:
					toolTip = _T("打开文件");
					break;		
				case ID_SUBSETFONTBOTH:
					toolTip = _T("修改字幕字体");
					break;	
				case ID_SUBFONTUPBOTH:
					toolTip = _T("放大字幕");
					break;	
				case ID_SUBFONTDOWNBOTH:
					toolTip = _T("缩小字幕");
					break;	
	}

	
	if(!toolTip.IsEmpty()){
		pTTT->lpszText = toolTip.GetBuffer();
		pTTT->hinst = AfxGetResourceHandle();
		bRet = TRUE;
	}

	

	*pResult = 0;

	return bRet;
}


void CPlayerToolBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	KillTimer(TIMER_FASTFORWORD);
	CMainFrame* pFrame = ((CMainFrame*)GetParentFrame());
	iBottonClicked = -1;
	m_bMouseDown = TRUE;
	CRect rc;
	GetWindowRect(&rc);

	point += rc.TopLeft() ;
	UINT ret = m_btnList.OnHitTest(point,rc);
	if( m_btnList.HTRedrawRequired ){
		if(ret)
			SetCapture();
		Invalidate();
	}
	m_nItemToTrack = ret;
	

	if(m_nItemToTrack == ID_PLAY_BWD || m_nItemToTrack == ID_PLAY_FWD){
		//pFrame->PostMessage( WM_COMMAND, ID_PLAY_PAUSE);
		iBottonClicked = m_nItemToTrack;
		iFastFFWCount = 0;
		SetTimer(TIMER_FASTFORWORD, 350, NULL);
	}else if(m_nItemToTrack == ID_SUBDELAYDEC || m_nItemToTrack == ID_SUBDELAYINC){
		iBottonClicked = m_nItemToTrack;
		iFastFFWCount = 0;
		SetTimer(TIMER_FASTFORWORD, 350, NULL);
	}else if(!ret){
		
		if( m_btnVolBG->m_rcHitest.PtInRect(point) ){
			OnSetVolByMouse(point);
		}
	}
	return;
	//New UI End
	
	
}

void CPlayerToolBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	CMainFrame* pFrame = ((CMainFrame*)GetParentFrame());
	KillTimer(TIMER_FASTFORWORD);
	ReleaseCapture();

	CRect rc;
	GetWindowRect(&rc);

	CPoint xpoint = point + rc.TopLeft() ;
	UINT ret = m_btnList.OnHitTest(xpoint,rc);
	if( m_btnList.HTRedrawRequired ){
		if(ret)
			pFrame->PostMessage( WM_COMMAND, ret);
		Invalidate();
	}
	m_nItemToTrack = ret;

	if(m_nItemToTrack == iBottonClicked ){
		if(iFastFFWCount == 0){
			int iMsg = 0;
			CMainFrame* pFrame = ((CMainFrame*)GetParentFrame());
			// not increase or decrease play rate
			if(iBottonClicked == ID_PLAY_BWD){
				iMsg = ID_PLAY_SEEKBACKWARDSMALL;
			}else if(iBottonClicked == ID_PLAY_FWD){
				iMsg = ID_PLAY_SEEKFORWARDSMALL;
			}
			if(iMsg)
				pFrame->PostMessage( WM_COMMAND, iMsg);
			
		}
	}

//	__super::OnLButtonUp(nFlags, point);
	m_bMouseDown = FALSE;
	return;//New UI End

	/*
	KillTimer(TIMER_FASTFORWORD);
		for(int i = 0, j = GetToolBarCtrl().GetButtonCount(); i < j; i++)
		{
			CRect r;
			GetItemRect(i, r);
			if(r.PtInRect(point))
			{
				UINT iButtonID, iStyle ;
				int iImage ;
				GetButtonInfo(i,iButtonID, iStyle , iImage );
				if(iButtonID == iBottonClicked ){
					if(iFastFFWCount == 0){
						int iMsg = 0;
						CMainFrame* pFrame = ((CMainFrame*)GetParentFrame());
						// not increase or decrease play rate
						if(iBottonClicked == ID_PLAY_BWD){
							iMsg = ID_PLAY_SEEKBACKWARDSMALL;
						}else if(iBottonClicked == ID_PLAY_FWD){
							iMsg = ID_PLAY_SEEKFORWARDSMALL;
						}
						if(iMsg)
							pFrame->PostMessage( WM_COMMAND, iMsg);
						// 					if( iBottonClicked == ID_PLAY_BWD || iBottonClicked == ID_PLAY_FWD) 
						// 						pFrame->PostMessage( WM_COMMAND, ID_PLAY_PLAY);
					}
				}
				break;
			}
		}
		iBottonClicked = -1;
	
		__super::OnLButtonUp(nFlags, point);
		return;*/
	
}
void CPlayerToolBar::OnTimer(UINT nIDEvent){
	switch(nIDEvent){
		case TIMER_STATERASER:
			KillTimer(TIMER_STATERASER);
			if(!m_timerqueryedstr.IsEmpty()){
				m_timerstr = m_timerqueryedstr;
				m_timerqueryedstr.Empty();
				Invalidate();
			}

			holdStatStr = FALSE;
			break;
		case TIMER_STOPFASTFORWORD:
			iFastFFWCount = 0;
			KillTimer(TIMER_STOPFASTFORWORD);
			{
// 				CMainFrame* pFrame = ((CMainFrame*)GetParentFrame());
// 				pFrame->PostMessage( WM_COMMAND, ID_PLAY_PLAY);
			}
			break;
		case TIMER_FASTFORWORD:
			if(iBottonClicked < 0 ){
				KillTimer(TIMER_FASTFORWORD);
				break;
			}
			iFastFFWCount++;
			//fast forward or backword
			{
				CMainFrame* pFrame = ((CMainFrame*)GetParentFrame());
				int iMsg;
				if(iBottonClicked == ID_PLAY_BWD){
					iMsg = ID_PLAY_SEEKBACKWARDSMALL;
				}else if(iBottonClicked == ID_PLAY_FWD){
					iMsg = ID_PLAY_SEEKFORWARDSMALL;
				}else{
					iMsg = iBottonClicked;
				}
				if(iFastFFWCount > 10 && ( iBottonClicked == ID_PLAY_BWD || iBottonClicked == ID_PLAY_FWD) ){

					int iStepPow = (int)(iFastFFWCount / 10) * 2;
					iStepPow = min(8, iStepPow);
					iMsg += iStepPow;
				}
				pFrame->PostMessage( WM_COMMAND, iMsg);
			}
			break;
	}

	__super::OnTimer(nIDEvent);
}
BOOL CPlayerToolBar::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return true;
	//return CToolBar::OnEraseBkgnd(pDC);
}
