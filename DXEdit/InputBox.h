#pragma once
#include "afxwin.h"


// CInputBox dialog

class CInputBox : public CDialog
{
	DECLARE_DYNAMIC(CInputBox)

public:
	CInputBox(CWnd* pParent = NULL, CString *String=NULL, CString *Text=NULL);   // standard constructor
	virtual ~CInputBox();

// Dialog Data
	enum { IDD = IDD_INPUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CString	*Dest, Mex;
	DECLARE_MESSAGE_MAP()

public:
	CEdit m_Input;
	CStatic m_Message;
	afx_msg void OnEnUpdateInput();
};
