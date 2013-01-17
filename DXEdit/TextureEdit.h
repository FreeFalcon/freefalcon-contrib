#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CTextureEdit dialog

class CTextureEdit : public CDialog
{
	DECLARE_DYNAMIC(CTextureEdit)

public:
	CTextureEdit(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTextureEdit();
	void EditTex(CSelectList *List);
	void UpdateUsedTexList();
	void UpdateValues();

// Dialog Data
	enum { IDD = IDD_TEXTUREEDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	bool	m_bTexOffset, m_bTexScale;
	CSelectList		*m_SelectList;
	D3DVERTEXEX		m_TexRef[4];
	DxSurfaceType	m_TexSurface;


	DECLARE_MESSAGE_MAP()
	CSpinButtonCtrl m_VOffsetSpin;
	CSpinButtonCtrl m_HOffsetSpin;
	CSpinButtonCtrl m_VScaleSpin;
	CSpinButtonCtrl m_HScaleSpin;
	CButton m_HVScaleCheck;
	CButton m_OffsetCheck;
	CButton m_ScaleCheck;
	CString	GetTexturefile();
	void UpdateTexList();

public:
	afx_msg void OnDeltaposVoffset(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposHoffset(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposVscale(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposHscale(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	CSpinButtonCtrl m_RotoSpin;
public:
	afx_msg void OnDeltaposRotospin(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	CListBox m_TexUsedListBox;
public:
	afx_msg void OnBnClicked90();
	afx_msg void OnBnClicked91();
	afx_msg void OnBnClicked92();
protected:
	CStatic m_TexSetsText;
	CComboBox m_UseTexSetNrCombo;
public:
	afx_msg void OnCbnSelchangeUsetexsetnr();
	afx_msg void OnBnClickedAddtexset();
	afx_msg void OnBnClickedDeltexset();
	afx_msg void OnBnClickedAddtex();
	afx_msg void OnBnClickedDeltex();
protected:
	CComboBox m_UseTexCombo;
public:
	afx_msg void OnBnClickedAssigntex();
	afx_msg void OnBnClickedChangetex();
	afx_msg void OnBnClickedX2();
	afx_msg void OnBnClickedX3();
	afx_msg void OnBnClickedD2();
	afx_msg void OnBnClickedD3();
	afx_msg void OnBnClickedFlipu();
	afx_msg void OnBnClickedFlipv();
	afx_msg void OnBnClickedTexsave();
	afx_msg void OnBnClickedTexload();
	afx_msg void OnBnClickedTexref();
	afx_msg void OnBnClickedTexlcalc();
	afx_msg void OnBnClickedSelecttex();
	afx_msg void OnBnClickedDelunusedtex();
};
