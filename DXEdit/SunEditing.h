#pragma once


// CSunEditing dialog

class CSunEditing : public CDialog
{
	DECLARE_DYNAMIC(CSunEditing)

public:
	CSunEditing(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSunEditing();

// Dialog Data
	enum { IDD = IDD_SUNEDITING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	D3DCOLORVALUE	m_Ambient, m_Diffuse, m_Specular;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSundiffuse();
	afx_msg void OnBnClickedSunambient();
	afx_msg void OnBnClickedSunspecular();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSundark();
	afx_msg void OnBnClickedSunmid();
	afx_msg void OnBnClickedSunbright();
};
