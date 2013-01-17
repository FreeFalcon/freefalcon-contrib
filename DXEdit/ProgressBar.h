#pragma once
#include "afxcmn.h"


// CProgressBar dialog

class CProgressBar : public CDialog
{
	DECLARE_DYNAMIC(CProgressBar)

public:
	CProgressBar(CWnd* pParent = NULL, DWORD Min=0, DWORD Max=100, CString Caption="");   // standard constructor
	virtual ~CProgressBar();
	void	SetValue(DWORD Value);

// Dialog Data
	enum { IDD = IDD_PROGRESSBAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	CProgressCtrl m_ProgressBar;
};
