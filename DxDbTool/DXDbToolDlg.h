// DXDbToolDlg.h : file di intestazione
//

#pragma once
#include "afxwin.h"
#include "LodConvert.h"
#include "afxcmn.h"
// finestra di dialogo CDXDbToolDlg
class CDXDbToolDlg : public CDialog
{
// Costruzione
public:
	CDXDbToolDlg(CWnd* pParent = NULL);	// costruttore standard
	void GetTexturesFolder(void);
	void GetCDFilesFolder(void);
	void ExecuteCommand(CString Command, CString Arg1, CString Arg2, CString Arg3);
	void *OpenModel(CString FileName);

// Dati della finestra di dialogo
	enum { IDD = IDD_DXDBTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// supporto DDX/DDV
	void		ReplaceModel(CString FileName, DWORD Paernt, DWORD Lod);	

// Implementazione
protected:
	HICON m_hIcon;

	// Funzioni generate per la mappa dei messaggi
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_DbFolderText;
	afx_msg void OnBnClickedDxhbutton();
	afx_msg void OnBnClickedDxmbutton();
	void	*ImportModel(E_Object *, void*, DWORD Class, DWORD Id);

protected:
	CProgressCtrl m_ProgressBar;
	CStatic m_StatusText;
public:
	afx_msg void OnBnClickedUpdatebutton();
protected:
	CButton m_BackupCheck;
	void	WriteModel(void *ptr, float *Radius=NULL, bool CalcSign = false);
	CButton m_ForceCheck;
	CString m_TexturesFolder;
	CString m_CDFilesFolder;
public:
	CButton m_Signatures;
};
