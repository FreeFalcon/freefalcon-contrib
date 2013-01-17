#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CVertexEdit dialog

class CVertexEdit : public CDialog
{
	DECLARE_DYNAMIC(CVertexEdit)

public:
	CVertexEdit(CWnd* pParent = NULL);   // standard constructor
	virtual ~CVertexEdit();
	bool EditVertex(CSelectList *List, void **Object, D3DVERTEXEX **Vertex);
	void UpdateEditValues(CSelectList *List, void **Object, D3DVERTEXEX **Vertex);
	void VertexScaleFunction(D3DXVECTOR3 Transform, D3DXVECTOR3 Move, bool Remove);
	void VertexUVFunction(D3DXVECTOR3 Transform, D3DXVECTOR3 Move, bool Remove);


// Dialog Data
	enum { IDD = IDD_VERTEXEDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	D3DVERTEXEX		**m_VSelected;
	void			**m_VertexOwner;
	DWORD			m_SAlpha, m_DAlpha;
	bool			m_bVertexMove, m_bVertexScale, m_bVertexUV;
	CSelectList		*m_SelectList;

	D3DCOLOR		m_DiffuseCopy, m_SpecularCopy;
	D3DVECTOR		m_PositionCopy;
	float			m_UCopy, m_VCopy;

	DECLARE_MESSAGE_MAP()
public:
	void			VertexMoveFunction(D3DXVECTOR3 Transform, D3DXVECTOR3 Move, bool Remove);
	afx_msg void OnBnClickedDiffusecolor();
	afx_msg void OnBnClickedSpecularcolor();
	afx_msg void OnNMCustomdrawDiffuseslider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSpecularslider(NMHDR *pNMHDR, LRESULT *pResult);
	CButton m_SingleVertexCheck;
	CSliderCtrl m_SpecularAlpha;
	CSliderCtrl m_DiffuseSlider;
protected:
	CButton m_VertexMove	;
	CButton m_VertexScale;
	CButton m_VertexUV	;
public:
	afx_msg void OnBnClickedSvertexmove();
	afx_msg void OnBnClickedSvertexscale();
protected:
	CButton m_OnlySelectCheck;
public:
	afx_msg void OnBnClickedDcopy();
	afx_msg void OnBnClickedScopy();
	afx_msg void OnBnClickedDpaste();
	afx_msg void OnBnClickedSpaste();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedPoscopy();
	afx_msg void OnBnClickedPospaste();
	afx_msg void OnBnClickedCopyuv();
	afx_msg void OnBnClickedPasteuv();
	afx_msg void OnBnClickedSvertexuv();
	CButton m_ScaleOmniCheck;
	CButton m_ScaleAbsolute;
};
