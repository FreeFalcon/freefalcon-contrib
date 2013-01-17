#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CDofEditing dialog

class CDofEditing : public CDialog
{
	DECLARE_DYNAMIC(CDofEditing)

public:
	CDofEditing(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDofEditing();
	void EditDof(E_Object *Object, DXDof *Dof);
	void DofMoveFunction(D3DXVECTOR3, D3DXVECTOR3, bool Remove);

// Dialog Data
	enum { IDD = IDD_DOFEDITING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void GetOriginalAngles(void);
	void UpdateAngles(void);
	DXDof	*m_Dof;
	DxDofType	m_OriginalDof;
	DxDofType	m_DofCopy;
	float	m_Yaw, m_Pitch, m_Roll;
	E_Object	*m_Object;
	CEdit m_XPos;
	CEdit m_YPos;
	CEdit m_ZPos;
	CEdit m_DofAngle;
	CSliderCtrl m_DofSlider;
	CEdit m_YawAngle;
	CEdit m_PitchAngle;
	CEdit m_RollAngle;
	bool		m_bDofMove;	
	DxDofType	m_DofRef;
	bool		m_DofRec;


	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedDofreset();
	afx_msg void OnBnClickedDofrestore();
	afx_msg void OnBnClickedYaw5m();
	afx_msg void OnBnClickedYaw5p();
	afx_msg void OnBnClickedPitch5m();
	afx_msg void OnBnClickedPitch5p();
	afx_msg void OnBnClickedRoll5m();
	afx_msg void OnBnClickedRoll5p();
	afx_msg void OnNMCustomdrawDofslider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposXspin(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	CSpinButtonCtrl m_XSpin;
	CSpinButtonCtrl m_YSpin;
	CSpinButtonCtrl m_ZSpin;
public:
	afx_msg void OnDeltaposYspin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposZspin(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	CComboBox m_DofList;
	CEdit m_XScale;
	CEdit m_YScale;
public:
	CEdit m_ZScale;
protected:
	CSpinButtonCtrl m_XScaleSpin;
	CSpinButtonCtrl m_YScaleSpin;
	CSpinButtonCtrl m_ZScaleSpin;
public:
	afx_msg void OnDeltaposXscalespin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposYscalespin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposZscalespin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeDoftypelist();
protected:
	CEdit m_DofMax;
	CEdit m_DofMin;
	CSpinButtonCtrl m_MaxSpin;
	CSpinButtonCtrl m_MinSpin;
public:
	afx_msg void OnDeltaposMaxspin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposMinspin(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	CButton m_DofInvCheck;
	CButton m_DofLimits;
	CButton m_DofDegrees;
	CButton m_DofScale;
public:
	afx_msg void OnBnClickedDofinvcheck();
	afx_msg void OnBnClickedDoflimits();
	afx_msg void OnBnClickedDofdegrees();
	afx_msg void OnBnClickedDofscale();
protected:
	CEdit m_DofCx;
	CSpinButtonCtrl m_CxSpin;
public:
	afx_msg void OnDeltaposCxspin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnToolsChangedofnumber();
	afx_msg void OnToolsCopydof();
	afx_msg void OnToolsPastedof();
	afx_msg void OnBnClickedDofmove();
	afx_msg void OnSysCommand( UINT nID, LPARAM lParam );
	protected:
	CButton m_DofMoveButton;

	CSpinButtonCtrl m_YawSpin;
	CSpinButtonCtrl m_PitchSpin;
	CSpinButtonCtrl m_RollSpin;
public:
	afx_msg void OnDeltaposYawspin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposPitchspin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposRollspin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnToolsRecordchanges();
	afx_msg void OnToolsApplychanges();
};
