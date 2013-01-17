#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CColorTool dialog

class CColorTool : public CDHtmlDialog
{
	DECLARE_DYNCREATE(CColorTool)

public:
	CColorTool(CWnd* pParent = NULL);   // standard constructor
	virtual ~CColorTool();
// Overrides
	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);

// Dialog Data
	enum { IDD = IDD_COLORTOOL, IDH = IDR_HTML_COLORTOOL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	void	SetSliders(D3DCOLORVALUE *Color);
	void	GetSliders(D3DCOLORVALUE *Color);
	D3DLIGHT7	Light;
	D3DCOLORVALUE	*TheColour;


	bool	LDiffuseOn, LSpecularOn, LAmbientOn;
	bool	MDiffuseOn, MSpecularOn, MAmbientOn, MEmissiveOn;
	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
public:
	CSliderCtrl m_AlphaSlide;
	CSliderCtrl m_RedSlide;
	CSliderCtrl m_GreenSlide;
	CSliderCtrl m_BlueSlide;
	CStatic m_Values;
	afx_msg void OnBnClickedLdiffuse();
	afx_msg void OnBnClickedLspecular();
	afx_msg void OnBnClickedLambient();
	afx_msg void OnBnClickedMdiffuse();
	afx_msg void OnBnClickedMspecular();
	afx_msg void OnBnClickedMambient();
	afx_msg void OnBnClickedMemissive();
	afx_msg void OnBnClickedDiscocheck();
	CButton m_DiscoCheck;
	afx_msg void OnNMCustomdrawBlueslide(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawGreendslide(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawRedslide(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawAlphaslide(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedNvg();
	CButton m_NVG;
};
