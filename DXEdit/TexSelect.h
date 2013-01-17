#pragma once
#include "afxwin.h"



class CTexSelect : public CDialog
{
	DECLARE_DYNAMIC(CTexSelect)

public:
	CTexSelect(CWnd* pParent = NULL, E_Object *Obj=NULL, CString *TexNr=NULL);   // standard constructor
	virtual ~CTexSelect();
	void FillData(void);

// Dialog Data
	enum { IDD = IDD_TEXSELECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	E_Object	*m_Obj;
	CString		*m_TexNr;
	DECLARE_MESSAGE_MAP()
	CListBox m_TexList;
public:
	afx_msg void OnLbnSelchangeTextureselectlist();
	afx_msg void OnBnClickedLoadtexbutton();
};
