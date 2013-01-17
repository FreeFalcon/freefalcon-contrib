#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CSurfaceEdit dialog

class CSurfaceEdit : public CDialog
{
	DECLARE_DYNAMIC(CSurfaceEdit)

public:
	CSurfaceEdit(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSurfaceEdit();
	void EditSurface(void **Object, DxSurfaceType **Surface, CSelectList *SelectList);
	void UpdateEditValues(CSelectList *SelectList = NULL, DXNode *Node=NULL, DWORD VertexNr=-1);
	void SurfaceMoveFunction(D3DXVECTOR3 Transform, D3DXVECTOR3 Move, bool Remove);


// Dialog Data
	enum { IDD = IDD_SURFACEEDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void	UpdateFlags(DXFlagsType Flags, DXFlagsType Mask);

	void			**m_SurfaceOwner;
	DxSurfaceType	**m_Surface;
	CSelectList		*m_SelectList;
	DXTriangle		*m_NSelected;
	DWORD			m_VSelected;
	bool			Updating;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSVcolorcheck();
private:
	CButton m_AlphaCheck;
	bool	m_bSurfaceMove;

public:
	CButton m_LightedCheck;
	CButton m_VColorCheck;
	CButton m_TexturedCheck;
	CButton m_GouraudCheck;
	CButton m_BillBoardCheck;
	CSliderCtrl m_ZBiasSlider;
	CSliderCtrl m_SIndexSlider;
	CEdit m_Tex1;
	CEdit m_Tex2;
	afx_msg void OnBnClickedSAlphackeck();
	afx_msg void OnBnClickedSLitedcheck();
	afx_msg void OnBnClickedSTexturecheck();
	afx_msg void OnBnClickedSGouraudcheck();
	afx_msg void OnBnClickedSBboardcheck();
	afx_msg void OnBnClickedFacemirror();
	afx_msg void OnBnClickedBackface();
	afx_msg void OnBnClickedSCkeycheck();
private:
	CButton m_CKeyCheck;
public:
	afx_msg void OnBnClickedSelectionmove();
protected:
	CButton m_SurfaceMoveButton;
public:
	afx_msg void OnNMReleasedcaptureSSindexslider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSZbiasslider(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	CButton m_ZMaxCheck;
	CButton m_ZMinCheck;
public:
	afx_msg void OnBnClickedZmax();
	afx_msg void OnBnClickedZmin();
protected:
	CButton m_SIMaxCheck;
	CButton m_SIMinCheck;
public:
	afx_msg void OnBnClickedSimax();
	afx_msg void OnBnClickedSimin();
protected:
	CStatic m_ZBiasValue;
public:
	afx_msg void OnBnClickedManmove();
protected:
	CEdit m_XMMove;
	CEdit m_YMMove;
	CEdit m_ZMMove;
	CButton m_MRelCheck;
public:
	afx_msg void OnBnClickedMansnap();
	afx_msg void OnBnClickedMansnap2();
	afx_msg void OnBnClickedGroundzero();
protected:
	CButton m_SwEmitCheck;
public:
	afx_msg void OnBnClickedSSwemitcheck();
	afx_msg void OnEnChangeSSwnr();
protected:
	CEdit m_SwNrText;
public:
	afx_msg void OnBnClickedSpeccolor();
protected:
	CSliderCtrl m_NormalSlider;
	float   GetNormalLevel() { return (float)(99-m_NormalSlider.GetPos())/100.f; }
private:
	CStatic m_NormValue;
public:
	afx_msg void OnNMCustomdrawNormalsslider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedMnosmalbutton();
	afx_msg void OnBnClickedSnormalbutton();
protected:
	CEdit m_SWMaskText;
public:
	afx_msg void OnEnChangeSSwmask();
	void OnBnClickedSZbiascheck();
	CButton m_ZBiasCheck;
	afx_msg void OnBnClickedPcircle();
	CEdit m_PFaces;
	CEdit m_PRadius;
	afx_msg void OnBnClickedMirrox();
	afx_msg void OnBnClickedMirroy();
	afx_msg void OnBnClickedMirroz();
	CButton m_MirrorCheck;
	afx_msg void OnBnClickedPlanize();
	CEdit m_ScaleZ;
	CEdit m_ScaleY;
	afx_msg void OnBnClickedScalexup();
	CEdit m_ScaleX;
	afx_msg void OnBnClickedScalexdn();
	afx_msg void OnBnClickedScaleyup();
	afx_msg void OnBnClickedScaleydn();
	afx_msg void OnBnClickedScalezup();
	afx_msg void OnBnClickedScalezdn();
};
