// InputBox.cpp : implementation file
//

#include "stdafx.h"
#include "LodConvert.h"
#include "InputBox.h"
#include ".\inputbox.h"


// CInputBox dialog

IMPLEMENT_DYNAMIC(CInputBox, CDialog)
CInputBox::CInputBox(CWnd* pParent /*=NULL*/, CString *String, CString *Text)
	: CDialog(CInputBox::IDD, pParent)
{
	ShowWindow(SW_SHOW);

	Mex=*Text;
//	m_Message.Invalidate(false);
	Dest=String;
}

CInputBox::~CInputBox()
{
}

void CInputBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, Input, m_Input);
	DDX_Control(pDX, Message, m_Message);
	m_Message.SetWindowText(Mex);
}


BEGIN_MESSAGE_MAP(CInputBox, CDialog)
	ON_EN_UPDATE(Input, OnEnUpdateInput)
END_MESSAGE_MAP()


// CInputBox message handlers

void CInputBox::OnEnUpdateInput()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.

	// TODO:  Add your control notification handler code here
	char	Text[128];
	if(Dest){
		m_Input.GetWindowText(Text,sizeof(Text));
		*Dest=Text;
	}
}
