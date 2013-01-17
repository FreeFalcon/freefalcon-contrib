// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "LODConvert.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_MOUSEACTIVATE()
	ON_WM_MOUSEMOVE()
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_KILLFOCUS()
	ON_WM_ACTIVATE()
	ON_WM_NCLBUTTONDOWN()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,	
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	ToolBar=NULL;
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//SetWindowPos(NULL, 0, 0, 800, 600, 0);
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_TRANSPARENT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | CBRS_FLOAT_MULTI) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	RECT	Rect;
    GetClientRect(&Rect );
    //ClientToScreen((POINT*)&Rect.top);
    //ClientToScreen((POINT*)&Rect.left );

	FloatControlBar(&m_wndToolBar, ((Rect.right-Rect.left)/2,(Rect.bottom-Rect.top)/2));

	DWORD c=m_wndToolBar.GetCount();
	DWORD ID;
	while(c){
		ID=m_wndToolBar.GetItemID(c);
		if(ID>=ID_M1 && ID<=ID_M8) m_wndToolBar.SetButtonStyle(c, TBBS_CHECKGROUP);
		if(ID>=ID_V1 && ID<=ID_V8) m_wndToolBar.SetButtonStyle(c, TBBS_CHECKBOX);
		if(ID>=ID_XPLANE && ID<=ID_ZPLANE) m_wndToolBar.SetButtonStyle(c, TBBS_CHECKGROUP);
		if(ID>=ID_VERTEXMODE && ID<=ID_JOINTFACEMODE) m_wndToolBar.SetButtonStyle(c, TBBS_CHECKGROUP);
		if(ID==ID_3DTOGGLE || ID==ID_GRIDTOGGLE || ID==ID_FRAMEMODE) m_wndToolBar.SetButtonStyle(c, TBBS_CHECKBOX);
		c--;
	}
	ToolBar=&m_wndToolBar;
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

void CMainFrame::OnLButtonDown( UINT nFlags, CPoint point )
{	
	m_MouseStartX=point.x;
	m_MouseStartY=point.y;
	m_MouseMove=true;
	MouseLeftClick=point;
	MouseLClick=true;
	CWnd::OnLButtonDown( nFlags, point );
}

void CMainFrame::OnLButtonUp( UINT nFlags, CPoint point )
{	// if mous has not been moved
	if(m_StartMove){
		MouseLClick=true;
		MouseLeftClick=point;
	}
	m_MouseMove=false;
	ReleaseCapture();
	CWnd::OnLButtonUp( nFlags, point );
}

void CMainFrame::OnRButtonUp( UINT nFlags, CPoint point )
{
	if(m_StartMove){
		MouseRClick=true;
		MouseRightClick=point;
	}
	m_MouseMove=false;
	ReleaseCapture();
	CWnd::OnRButtonUp( nFlags, point );
}

void CMainFrame::OnRButtonDown( UINT nFlags, CPoint point )
{	
	m_MouseStartX=point.x;
	m_MouseStartY=point.y;
	m_MouseMove=true;
	MouseRightClick=point;
	MouseRClick=true;
	CWnd::OnRButtonDown( nFlags, point );
}






void CMainFrame::OnMouseMove( UINT nFlags, CPoint point )
{

	if(m_MouseMove){
		if(m_StartMove){
			m_StartMove=false;
			m_MouseStartX=point.x;
			m_MouseStartY=point.y;
		}
		long	dX=point.x-m_MouseStartX, dY=point.y-m_MouseStartY;
		if(nFlags&MK_RBUTTON){
			MouseYaw=PI/100*(float)dX;
			MousePitch=PI/100*(float)dY;
			if(nFlags&MK_CONTROL){
				MouseYaw*=0.1f;
				MousePitch*=0.1f;
			}
		}
		if(nFlags&MK_LBUTTON){
			if(nFlags&MK_CONTROL) MouseMoveY=-0.1f*(float)dY;
			else{
				MouseMoveX=0.1f*(float)dX;
				MouseMoveZ=0.1f*(float)dY;
			}
		}
		m_MouseStartX=point.x;
		m_MouseStartY=point.y;

	}
}

BOOL CMainFrame::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	if(nFlags&MK_SHIFT && nFlags&MK_CONTROL){
		SelectionAlpha+=0.0001f*zDelta;
		if(SelectionAlpha>1.0f) SelectionAlpha=1.0f;
		if(SelectionAlpha<0.0f) SelectionAlpha=0.0f;
	} else {
		CameraMoveZ=0.05f*(float)zDelta;
		if(nFlags&MK_SHIFT) CameraMoveZ=0.5f*(float)zDelta;
		if(nFlags&MK_CONTROL) CameraMoveZ=0.001f*(float)zDelta;
	}
	//if(nFlags&MK_ALT) { MouseMoveY=0.5*(float)zDelta; CameraMoveZ=0.0f; }
	return true;
}

void CMainFrame::OnSetFocus( CWnd* pOldWnd )
{
		CWnd::OnSetFocus( pOldWnd );
}

void CMainFrame::OnKillFocus( CWnd* pOldWnd )
{
		CWnd::OnKillFocus( pOldWnd );
}

int CMainFrame::OnMouseActivate( CWnd* pDesktopWnd, UINT nHitTest, UINT message )
{
	if(nHitTest==HTCLIENT){
		m_MouseMove=true;
		m_StartMove=true;
		this->ActivateFrame();
		PrevMouse=SetCapture();
	} else
		ReleaseCapture();
	
	return MA_ACTIVATE;
}


#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}



#endif //_DEBUG


// CMainFrame message handlers

