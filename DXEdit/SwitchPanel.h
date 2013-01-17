#pragma once
#include "afxwin.h"
#include "afxcmn.h"

typedef	struct	{
		CString	Name;
		DWORD	Number;
}	DOFItemType;



// CSwitchPanel dialog

class CSwitchPanel : public CDialog
{
	DECLARE_DYNAMIC(CSwitchPanel)

public:
	CSwitchPanel(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSwitchPanel();
	void ActivateWindow(DXDof *node = NULL, DWORD SwitchNr = 0);

// Dialog Data
	enum { IDD = IDD_SWITCHPANEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
	DXDof	*m_node;
public:
	CListBox m_SwitchList;
	afx_msg void OnBnClickedSwEnable();
	afx_msg void OnBnClickedSwAllon();
	afx_msg void OnNMCustomdrawSwSelect(NMHDR *pNMHDR, LRESULT *pResult);
	CButton m_EnableSW;
	afx_msg void OnBnClickedSwAlloff();
	afx_msg void OnBnClickedSwDisable();
	CSliderCtrl m_SWSlider;
	afx_msg void OnLbnSelchangeSwitchlist();
	afx_msg void OnBnClickedSwAssign();
};


extern	const DOFItemType	SWLabs[];
