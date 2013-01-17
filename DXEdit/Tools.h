#pragma once
#include "afxwin.h"
#include "afxcmn.h"



typedef	struct	{
		CString	Name;
		DWORD	Number;
}	DOFItemType;

// CTools dialog

class CTools : public CDialog
{
	DECLARE_DYNAMIC(CTools)

public:
	CTools(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTools();

// Dialog Data
	enum { IDD = IDD_TOOLS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DWORD	DofSelected,SWSelected;
	DWORD	SwitchValues[128];

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnSelchangeDoflist();
	CListBox m_DofList;
	CSliderCtrl m_RotSlider;
	CSliderCtrl m_TransSlider;
	afx_msg void OnNMCustomdrawDofrot(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLbnSelchangeSwlist();
	CListBox m_SWList;
	CSliderCtrl m_SWSlider;
	CStatic m_SWText;
	afx_msg void OnNMCustomdrawSwvalue(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSwonbutton();
	afx_msg void OnStnClickedSwtext();
	afx_msg void OnBnClickedSwoffbutton2();
	afx_msg void OnBnClickedDofzero();
};
