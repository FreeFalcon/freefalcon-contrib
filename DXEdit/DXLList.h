#pragma once
#include "afxwin.h"


// CDXLList dialog

class CDXLList : public CDialog
{
	DECLARE_DYNAMIC(CDXLList)

public:
	CDXLList(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDXLList();
	void UpdateData(void);
	DxSurfaceType	*LineListNode[16384];
	int	Selected[16384];


// Dialog Data
	enum { IDD = IDD_DXLLIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


	DECLARE_MESSAGE_MAP()
public:
	CListBox m_DXList;
	afx_msg void OnLbnSelchangeDxlist();
	CButton m_DisableCheck;
	afx_msg void OnBnClickedDisablecheck();
	afx_msg void OnLbnDblclkDxlist();
	CButton m_FrameSurface;
	afx_msg void OnBnClickedFramesurace();
};
