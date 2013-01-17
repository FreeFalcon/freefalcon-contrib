// DXLStructureDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LODConvertEx.h"
#include "DXLStructureDlg.h"
#include ".\dxlstructuredlg.h"


// CDXLStructureDlg dialog

IMPLEMENT_DYNAMIC(CDXLStructureDlg, CDialog)
CDXLStructureDlg::CDXLStructureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDXLStructureDlg::IDD, pParent)
{
	m_pDecoder = NULL;
}

CDXLStructureDlg::CDXLStructureDlg(CString strFileName, Decoder *pDecoder, CWnd* pParent)
: CDialog(CDXLStructureDlg::IDD, pParent)
{
	m_strFileName = strFileName;
	m_pDecoder = pDecoder;
}

CDXLStructureDlg::~CDXLStructureDlg()
{
}

void CDXLStructureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DXL_TREE, m_ctrlDXLTree);
}


BEGIN_MESSAGE_MAP(CDXLStructureDlg, CDialog)
END_MESSAGE_MAP()


// CDXLStructureDlg message handlers

BOOL CDXLStructureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	HTREEITEM hRootItem = m_ctrlDXLTree.InsertItem(_T("DXL Structure for file ") + m_strFileName, 0, 0);
	m_ctrlDXLTree.SetItemBold(hRootItem, FW_BOLD);
	m_ctrlDXLTree.SetItemsSelfDraw(TRUE);
	
	COLORREF clrMask = RGB(0, 0, 0);
	CBitmap bmpImage;

	m_hImageList.Create(16, 16, ILC_MASK|ILC_COLOR24, 7, 1);

	// Load tree icons
	m_hImageList.Add(theApp.LoadIcon(IDI_ROOT));
	m_hImageList.Add(theApp.LoadIcon(IDI_HEADER));
	m_hImageList.Add(theApp.LoadIcon(IDI_TEXTURE));
	m_hImageList.Add(theApp.LoadIcon(IDI_SURFACE));
	m_hImageList.Add(theApp.LoadIcon(IDI_DOF));
	m_hImageList.Add(theApp.LoadIcon(IDI_ENDDOF));
	m_hImageList.Add(theApp.LoadIcon(IDI_GENERAL));
	m_hImageList.Add(theApp.LoadIcon(IDI_NODE));

	m_ctrlDXLTree.SetImageList(&m_hImageList, LVSIL_NORMAL);

	DWORD dwTotalSurfaceNodes, dwTotalDOFNodes, dwTotalTextureNodes;
	BeginWaitCursor();

	m_pDecoder->Initialize(m_strFileName, &m_ctrlDXLTree);
	m_pDecoder->Decode(dwTotalSurfaceNodes, dwTotalDOFNodes, dwTotalTextureNodes);

	EndWaitCursor();

	CString strFormatString;
	strFormatString.Format(_T("%d"), dwTotalSurfaceNodes);
	GetDlgItem(IDC_SURFACE_STATIC)->SetWindowText(strFormatString);
	strFormatString.Format(_T("%d"), dwTotalTextureNodes);
	GetDlgItem(IDC_TEXTURE_STATIC)->SetWindowText(strFormatString);
	strFormatString.Format(_T("%d"), dwTotalDOFNodes);
	GetDlgItem(IDC_DOF_STATIC)->SetWindowText(strFormatString);

	strFormatString.Format(_T("%d bytes"), m_pDecoder->m_uiBufferSize);
	GetDlgItem(IDC_FILESIZE_STATIC)->SetWindowText(strFormatString);

	strFormatString.Format(_T("%d"), m_pDecoder->m_dwTotalVertexes);
	GetDlgItem(IDC_VERTEX_NUMBER_STATIC)->SetWindowText(strFormatString);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
