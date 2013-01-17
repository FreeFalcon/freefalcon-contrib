// AskBox.cpp : implementation file
//

#include "stdafx.h"
#include "LODConvert.h"
#include "AskBox.h"
#include ".\askbox.h"


// CAskBox dialog

IMPLEMENT_DYNAMIC(CAskBox, CDialog)
CAskBox::CAskBox(CString Label, CString *Dest, CWnd* pParent /*=NULL*/)
	: CDialog(CAskBox::IDD, pParent)
{
	Destination=Dest;
	*Destination="";
	AskLabel=Label;
}

CAskBox::~CAskBox()
{
}

void CAskBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ASKLABEL, m_Label);
	m_Label.SetWindowText(AskLabel);
	DDX_Control(pDX, IDC_ASKTEXT, m_text);
}


BEGIN_MESSAGE_MAP(CAskBox, CDialog)
	ON_EN_CHANGE(IDC_ASKTEXT, OnEnChangeAsktext)
END_MESSAGE_MAP()


// CAskBox message handlers

void CAskBox::OnEnChangeAsktext()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	m_text.GetWindowText(*Destination);
}
