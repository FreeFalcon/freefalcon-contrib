#pragma once
#include "afxwin.h"


// CVbList dialog

class CVbList : public CDHtmlDialog
{
	DECLARE_DYNCREATE(CVbList)

public:
	CVbList(CWnd* pParent = NULL);   // standard constructor
	virtual ~CVbList();
// Overrides
	void	UpdateList(void);

// Dialog Data
	enum { IDD = IDD_VBLIST, IDH = IDR_HTML_VBLIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
public:
	CListBox m_VBList;
	afx_msg void OnBnClickedRefreshvb();
};
