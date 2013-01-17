#pragma once
#include "afxwin.h"


// CAskBox dialog

class CAskBox : public CDialog
{
	DECLARE_DYNAMIC(CAskBox)

public:
	CAskBox(CString Label, CString *Dest, CWnd* pParent = NULL);   // standard constructor
	virtual ~CAskBox();

// Dialog Data
	enum { IDD = IDD_ASKBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CString *Destination, AskLabel;		
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_Label;
	CEdit m_text;
	afx_msg void OnEnChangeAsktext();
};
