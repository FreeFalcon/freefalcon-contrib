// MainFrm.h : interface of the CMainFrame class
//


#pragma once
class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	float		m_Pitch, m_Yaw;
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

	long		m_MouseStartX, m_MouseStartY;
	bool		m_MouseMove,m_StartMove,m_MouseTranslate;
	CWnd		*PrevMouse;
// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown( UINT nFlags, CPoint point );
	afx_msg void OnRButtonDown( UINT nFlags, CPoint point );
	afx_msg void OnLButtonUp( UINT nFlags, CPoint point );
	afx_msg void OnRButtonUp( UINT nFlags, CPoint point );
	afx_msg void OnSetFocus( CWnd* pOldWnd );
	afx_msg void OnMouseMove( UINT nFlags, CPoint point );
	afx_msg void OnKillFocus( CWnd* pNewWnd );
	afx_msg int OnMouseActivate( CWnd* pDesktopWnd, UINT nHitTest, UINT message );
	afx_msg BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );



	
	DECLARE_MESSAGE_MAP()
};


