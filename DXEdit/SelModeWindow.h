#pragma once
#include "afxwin.h"


// CSelModeWindow dialog

class CSelModeWindow : public CDialog
{
	DECLARE_DYNAMIC(CSelModeWindow)

public:
	CSelModeWindow(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelModeWindow();
	void	SetSelectMode(void);
	bool	CheckSelection(DXTriangle *Search, DXTriangle *Compare );

// Dialog Data
	enum { IDD = IDD_SELMODEWINDOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	bool	EndOfDialog;
	bool	CommonVertex;
	bool	SameAlpha;
	bool	SameTexture;
	bool	SameDiffuse;
	bool	SameSpecular;
	bool	SameRenderState;
	bool	SameEmitSwitch;


	DECLARE_MESSAGE_MAP()
public:
	CButton m_CommonVertex;
	CButton m_SameAlpha;
	CButton m_SameTexture;
	CButton m_SameDiffuse;
	CButton m_SameSpecular;
	CButton m_SameRenderState;
	afx_msg void OnBnClickedComvcheck();
	afx_msg void OnBnClickedSmaveacheck();
	afx_msg void OnBnClickedSametcheck();
	afx_msg void OnBnClickedSamescheck();
	afx_msg void OnBnClickedSamedcheck();
	afx_msg void OnBnClickedSamercheck();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSameemit();
	CButton m_SameEmitCheck;
};
