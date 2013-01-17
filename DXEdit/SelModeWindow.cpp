// SelModeWindow.cpp : implementation file
//

#include "stdafx.h"
#include "LodConvert.h"
#include "SelModeWindow.h"
#include ".\selmodewindow.h"


// CSelModeWindow dialog

IMPLEMENT_DYNAMIC(CSelModeWindow, CDialog)
CSelModeWindow::CSelModeWindow(CWnd* pParent /*=NULL*/)
	: CDialog(CSelModeWindow::IDD, pParent)
{
	//memset(this, 0, sizeof(this));
	CommonVertex=SameRenderState=true;
}

CSelModeWindow::~CSelModeWindow()
{
}

void CSelModeWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMVCHECK, m_CommonVertex);
	DDX_Control(pDX, IDC_SMAVEACHECK, m_SameAlpha);
	DDX_Control(pDX, IDC_SAMETCHECK, m_SameTexture);
	DDX_Control(pDX, IDC_SAMEDCHECK, m_SameDiffuse);
	DDX_Control(pDX, IDC_SAMESCHECK, m_SameSpecular);
	DDX_Control(pDX, IDC_SAMERCHECK, m_SameRenderState);
	DDX_Control(pDX, IDC_SAMEEMIT, m_SameEmitCheck);
	m_CommonVertex.SetCheck(CommonVertex?1:0);
	m_SameAlpha.SetCheck(SameAlpha?1:0);
	m_SameTexture.SetCheck(SameTexture?1:0);
	m_SameDiffuse.SetCheck(SameDiffuse?1:0);
	m_SameSpecular.SetCheck(SameSpecular?1:0);
	m_SameRenderState.SetCheck(SameRenderState?1:0);
	m_SameEmitCheck.SetCheck(SameEmitSwitch?1:0);
}


BEGIN_MESSAGE_MAP(CSelModeWindow, CDialog)
	ON_BN_CLICKED(IDC_COMVCHECK, OnBnClickedComvcheck)
	ON_BN_CLICKED(IDC_SMAVEACHECK, OnBnClickedSmaveacheck)
	ON_BN_CLICKED(IDC_SAMETCHECK, OnBnClickedSametcheck)
	ON_BN_CLICKED(IDC_SAMESCHECK, OnBnClickedSamescheck)
	ON_BN_CLICKED(IDC_SAMEDCHECK, OnBnClickedSamedcheck)
	ON_BN_CLICKED(IDC_SAMERCHECK, OnBnClickedSamercheck)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_SAMEEMIT, OnBnClickedSameemit)
END_MESSAGE_MAP()


void CSelModeWindow::SetSelectMode(void)
{
	ShowWindow(SW_SHOW);
	DoModal();
	
}


bool CSelModeWindow::CheckSelection(DXTriangle *Search, DXTriangle *Compare )
{
	if(SameTexture && *Search->Surface.TexID != *Compare->Surface.TexID) return false;
	if(SameRenderState && Search->Surface.dwFlags.StateFlags != Compare->Surface.dwFlags.StateFlags ) return false;
	if((SameRenderState && Search->Surface.dwFlags.b.SwEmissive) && (Search->Surface.SwitchNumber!=Compare->Surface.SwitchNumber)) return false;
	if(SameEmitSwitch && (!Search->Surface.dwFlags.b.SwEmissive || Search->Surface.SwitchNumber!=Compare->Surface.SwitchNumber ||
		Search->Surface.SwitchMask!=Compare->Surface.SwitchMask)) return false;
	if(CommonVertex) {
		for(DWORD a=0; a<Search->Surface.dwVCount; a++)
			for(DWORD b=0; b<Compare->Surface.dwVCount; b++)
				if(	*(D3DVECTOR*)&Search->Vertex[a].vx==*(D3DVECTOR*)&Compare->Vertex[b].vx ) goto Test_ok;
		return	false;
		Test_ok:;
	}

	if(SameAlpha) {
		for(DWORD a=0; a<Search->Surface.dwVCount; a++)
			for(DWORD b=0; b<Compare->Surface.dwVCount; b++)
				if(	(Search->Vertex[a].dwColour&0xff000000) == (Compare->Vertex[a].dwColour&0xff000000) ) goto Test_1_ok;
		return	false;
		Test_1_ok:;
	}

	if(SameDiffuse) {
		for(DWORD a=0; a<Search->Surface.dwVCount; a++)
			for(DWORD b=0; b<Compare->Surface.dwVCount; b++)
				if(	(Search->Vertex[a].dwColour&0x00ffffff) == (Compare->Vertex[a].dwColour&0x00ffffff) ) goto Test_2_ok;
		return	false;
		Test_2_ok:;
	}

	if(SameSpecular) {
		for(DWORD a=0; a<Search->Surface.dwVCount; a++)
			for(DWORD b=0; b<Compare->Surface.dwVCount; b++)
				if(	(Search->Vertex[a].dwSpecular) == (Compare->Vertex[a].dwSpecular )) goto Test_3_ok;
		return	false;
		Test_3_ok:;
	}


	return true;
}


void CSelModeWindow::OnBnClickedComvcheck()
{
	// TODO: Add your control notification handler code here
	CommonVertex=(m_CommonVertex.GetCheck())?true:false;
}

void CSelModeWindow::OnBnClickedSmaveacheck()
{
	// TODO: Add your control notification handler code here
	SameAlpha=(m_SameAlpha.GetCheck())?true:false;
}

void CSelModeWindow::OnBnClickedSametcheck()
{
	// TODO: Add your control notification handler code here
	SameTexture=(m_SameTexture.GetCheck())?true:false;
}

void CSelModeWindow::OnBnClickedSamescheck()
{
	// TODO: Add your control notification handler code here
	SameSpecular=(m_SameSpecular.GetCheck())?true:false;
}

void CSelModeWindow::OnBnClickedSamedcheck()
{
	// TODO: Add your control notification handler code here
	SameDiffuse=(m_SameDiffuse.GetCheck())?true:false;
}

void CSelModeWindow::OnBnClickedSamercheck()
{
	// TODO: Add your control notification handler code here
	SameRenderState=(m_SameRenderState.GetCheck())?true:false;
}

void CSelModeWindow::OnBnClickedOk()
{
	OnOK();
}

void CSelModeWindow::OnBnClickedSameemit()
{
	// TODO: Add your control notification handler code here
	SameEmitSwitch=(m_SameEmitCheck.GetCheck())?true:false;
}
