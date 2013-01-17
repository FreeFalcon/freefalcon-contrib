// VBAskClass.cpp : implementation file
//

#include "stdafx.h"
#include "LODConvert.h"
#include "VBAskClass.h"
#include ".\vbaskclass.h"


// CVBAskClass dialog

IMPLEMENT_DYNAMIC(CVBAskClass, CDialog)
CVBAskClass::CVBAskClass(CWnd* pParent /*=NULL*/)
	: CDialog(CVBAskClass::IDD, pParent)
{
}

CVBAskClass::~CVBAskClass()
{
}

void CVBAskClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CVBAskClass, CDialog)
	ON_BN_CLICKED(IDC_RADIO1, OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnBnClickedRadio3)
END_MESSAGE_MAP()


// CVBAskClass message handlers

void CVBAskClass::OnBnClickedRadio1()
{
	((DxDbHeader*)ECD->GetModelBuffer())->VBClass=VB_CLASS_FEATURES;
}

void CVBAskClass::OnBnClickedRadio2()
{
	((DxDbHeader*)ECD->GetModelBuffer())->VBClass=VB_CLASS_DOMAIN_GROUND;
}

void CVBAskClass::OnBnClickedRadio3()
{
	((DxDbHeader*)ECD->GetModelBuffer())->VBClass=VB_CLASS_DOMAIN_AIR;
}
