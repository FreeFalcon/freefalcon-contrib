// ProgressBar.cpp : implementation file
//

#include "stdafx.h"
#include "LodConvert.h"
#include "ProgressBar.h"


// CProgressBar dialog

IMPLEMENT_DYNAMIC(CProgressBar, CDialog)
CProgressBar::CProgressBar(CWnd* pParent, DWORD Min, DWORD Max, CString Caption)
	: CDialog(CProgressBar::IDD, pParent)
{
	Create(IDD, pParent);
	m_ProgressBar.SetRange((short)Min, (short)Max);
	m_ProgressBar.SetPos(0);
	SetWindowText(Caption);
	ShowWindow(SW_SHOW);
}


CProgressBar::~CProgressBar()
{
}

void CProgressBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_ProgressBar);
}


BEGIN_MESSAGE_MAP(CProgressBar, CDialog)
END_MESSAGE_MAP()

void CProgressBar::SetValue(DWORD Value)
{
	m_ProgressBar.SetPos(Value);
	IdleMode();
}

// CProgressBar message handlers
