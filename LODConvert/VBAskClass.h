#pragma once
#include "afxwin.h"


// CVBAskClass dialog

class CVBAskClass : public CDialog
{
	DECLARE_DYNAMIC(CVBAskClass)

public:
	CVBAskClass(CWnd* pParent = NULL);   // standard constructor
	virtual ~CVBAskClass();

// Dialog Data
	enum { IDD = IDD_VBASKCLASS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DWORD	TheClass;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
protected:
};
