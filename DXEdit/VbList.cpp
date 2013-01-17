// VbList.cpp : implementation file
//

#include "stdafx.h"
#include "LODConvert.h"
#include "DXVBManager.h"
#include "VbList.h"
#include ".\vblist.h"


// CVbList dialog

IMPLEMENT_DYNCREATE(CVbList, CDHtmlDialog)

CVbList::CVbList(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(CVbList::IDD, CVbList::IDH, pParent)
{
	Create(IDD_VBLIST);
	ShowWindow(SW_SHOW);

}

CVbList::~CVbList()
{
	VBList=NULL;
}

void CVbList::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VBLIST, m_VBList);
	UpdateList();
}

BOOL CVbList::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CVbList, CDHtmlDialog)
	ON_BN_CLICKED(IDC_REFRESHVB, OnBnClickedRefreshvb)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CVbList)
END_DHTML_EVENT_MAP()





void CVbList::UpdateList(void)
{
	VBufferListType	*VbList=TheVbManager.GetVbList();
	m_VBList.ResetContent();

	int i=0;
	CVbVAT *Vat;
	CString Text;
	while(i<MAX_VERTEX_BUFFERS && VbList[i].Vb){
		Vat=VbList[i].pVAT;
		Text="";
		m_VBList.AddString(Text);
		Text.Format("   --- VB Nr. %02d ---- Free : %05d ---", i, VbList[i].Free );
		m_VBList.AddString(Text);
		if(VbList[i].BootGap){
			Text.Format("Gap : 0x%04x", VbList[i].BootGap);
			m_VBList.AddString(Text);
		}
		while(Vat){
			Text.Format("ID : %03d - Start : 0x%04x - Size : 0x%04x", Vat->ID, Vat->VStart, Vat->VSize);
			m_VBList.AddString(Text);
			if(Vat->Gap) {
				Text.Format("Gap : 0x%04x", Vat->Gap);
				m_VBList.AddString(Text);
			}
			Vat=Vat->Next;
		}
		i++;
	}
}


void CVbList::OnBnClickedRefreshvb()
{
	// TODO: Add your control notification handler code here
	UpdateList();
}
