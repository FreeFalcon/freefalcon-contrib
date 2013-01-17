#pragma once
#include "treectrlex.h"
#include "Decoder.h"


// CDXLStructureDlg dialog

class CDXLStructureDlg : public CDialog
{
	DECLARE_DYNAMIC(CDXLStructureDlg)
	Decoder *m_pDecoder;
	CString m_strFileName;
	CImageList m_hImageList;
public:
	CDXLStructureDlg(CString strFileName, Decoder *pDecoder, CWnd* pParent);
	CDXLStructureDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDXLStructureDlg();

// Dialog Data
	enum { IDD = IDD_DXL_STRUCTURE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrlEx m_ctrlDXLTree;
	virtual BOOL OnInitDialog();
};
