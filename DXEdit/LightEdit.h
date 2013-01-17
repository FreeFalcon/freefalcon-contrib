#pragma once

#include "afxwin.h"
#include "LodConvert.h"
#include "afxcmn.h"

// CLightEdit dialog
#define	PARATABLE(name,strct,arg)	{#name, offsetof(strct, arg), sizeof(((strct*)0)->arg)}
typedef	struct	{CString Name; DWORD Offset; DWORD Size; } ParaTableType;

class CLightEdit : public CDialog
{
	DECLARE_DYNAMIC(CLightEdit)

public:
	CLightEdit(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLightEdit();
	void EditLight(E_Light *List);
	void	LightMoveFunction(D3DXVECTOR3 Transform, D3DXVECTOR3 Move, bool Remove);

	bool	Active;
// Dialog Data
	enum { IDD = IDD_LIGHTEDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void UpdateList(void);

	E_Light	*m_LightList;
	E_Light	*m_Selected;
	bool	Updating;
	DXLightType	LightCopy;

	DECLARE_MESSAGE_MAP()
	CListBox m_List;
public:
	afx_msg void OnLbnSelchangeLightlist();
	afx_msg void OnBnClickedOk();
protected:
	bool	m_bLightMove;
	CButton m_OwnLiteCheck;
private:
	CComboBox m_LiteTypeCombo;
public:
	afx_msg void OnCbnSelchangeLitetype();
	afx_msg void OnBnClickedOwnlitecheck();
	afx_msg void OnBnClickedAmbientlite();
	afx_msg void OnBnClickedDiffuselite();
	afx_msg void OnBnClickedSpecular();
protected:
	CSliderCtrl m_AmbientSlider;
private:
	CSliderCtrl m_SpecularSlider;
public:
	afx_msg void OnNMCustomdrawAmbientslider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSpecularslider(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	CSliderCtrl m_DistanceSlider;
	CStatic m_Distancetext;
public:
	afx_msg void OnNMCustomdrawDistanceslider(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	CButton m_LightMoveCheck;
public:
	afx_msg void OnBnClickedLightmove();
protected:
	CEdit m_LiteSwitchNrText;
public:
	afx_msg void OnEnChangeLiteswitchnr();
	afx_msg void OnNMCustomdrawAttenuateslider(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	CSliderCtrl m_AttenuateSlider;
	CStatic m_AttenuateText;
	CSliderCtrl m_ThetaSlider;
	CStatic m_ThetaText;
	CSliderCtrl m_PhySlider;
	CStatic m_Phytext;
public:
	afx_msg void OnNMCustomdrawThetaslider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawPhislider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedLitekill();
	afx_msg void OnBnClickedLiteadd();
protected:
	CSliderCtrl m_AttenuateSlider2;
	CStatic m_Attenuate2Text;
public:
	afx_msg void OnNMCustomdrawAttenuateslider2(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	CButton m_OthersLiteCheck;
	CButton m_StaticLiteCheck;
	CButton m_RndLiteCheck;
	CButton m_DofDimLiteCheck;
public:
	afx_msg void OnBnClickedOtherlitecheck();
	afx_msg void OnBnClickedOstaticlitecheck();
	afx_msg void OnBnClickedRndlitecheck();
	afx_msg void OnBnClickedDofdimlitecheck();
	afx_msg void OnBnClickedLitecopy();
	afx_msg void OnBnClickedLitepaste();
protected:
	CEdit m_LiteSwitchMask;
	void	SavePara(CStdioFile *F, ParaTableType *T, void *P);
	bool	LoadPara(CStdioFile *F, ParaTableType *T, void *P);
	void	RecalcAllStatic();

public:
	afx_msg void OnEnChangeLiteswitchmask();
	afx_msg void OnBnClickedCalcstatic();
	afx_msg void OnBnClickedClrstatic();
	afx_msg void OnBnClickedSaveset();
	afx_msg void OnBnClickedLoadset();
	afx_msg void OnBnClickedStaticrecalc();
	afx_msg void OnBnClickedKillalllites();
};


