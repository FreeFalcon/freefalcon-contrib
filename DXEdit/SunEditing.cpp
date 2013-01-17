// SunEditing.cpp : implementation file
//

#include "stdafx.h"
#include "Lodconvert.h"
#include "SunEditing.h"
#include ".\sunediting.h"


// CSunEditing dialog

IMPLEMENT_DYNAMIC(CSunEditing, CDialog)
CSunEditing::CSunEditing(CWnd* pParent /*=NULL*/)
	: CDialog(CSunEditing::IDD, pParent)
{
	ShowWindow(SW_SHOW);
	DoModal();
}

CSunEditing::~CSunEditing()
{
}

void CSunEditing::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	m_Ambient=TheDXEngine.TheSun.dcvAmbient;
	m_Specular=TheDXEngine.TheSun.dcvSpecular;
	m_Diffuse=TheDXEngine.TheSun.dcvDiffuse;
}


BEGIN_MESSAGE_MAP(CSunEditing, CDialog)
	ON_BN_CLICKED(IDC_SUNDIFFUSE, OnBnClickedSundiffuse)
	ON_BN_CLICKED(IDC_SUNAMBIENT, OnBnClickedSunambient)
	ON_BN_CLICKED(IDC_SUNSPECULAR, OnBnClickedSunspecular)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_SUNDARK, OnBnClickedSundark)
	ON_BN_CLICKED(IDC_SUNMID, OnBnClickedSunmid)
	ON_BN_CLICKED(IDC_SUNBRIGHT, OnBnClickedSunbright)
END_MESSAGE_MAP()


D3DCOLORVALUE	GetColour(D3DCOLORVALUE Original)
{
	CColorDialog *SetColor;
	D3DCOLORVALUE	NewColor;
	DWORD	RGBColor=(((DWORD)(Original.r*255))<<16)+(((DWORD)(Original.g*255))<<8)+((DWORD)(Original.b*255));
	SetColor=new CColorDialog;
	SetColor->m_cc.rgbResult=D3DTOWINCOLOUR(RGBColor);
	SetColor->m_cc.Flags|=CC_RGBINIT | CC_ANYCOLOR | CC_FULLOPEN;
	DWORD	id=SetColor->DoModal();
	if(id==IDOK){
		NewColor.a=1.0f;
		RGBColor=WINTOD3DCOLOUR(SetColor->GetColor());
		NewColor.r=(float)((RGBColor>>16)&0xff)/255.0f;
		NewColor.g=(float)((RGBColor>>8)&0xff)/255.0f;
		NewColor.b=(float)(RGBColor&0xff)/255.0f;
		return	NewColor;
	}
	return Original;
}


// CSunEditing message handlers

void CSunEditing::OnBnClickedSundiffuse()
{
	TheDXEngine.TheSun.dcvDiffuse=GetColour(TheDXEngine.TheSun.dcvDiffuse);
}

void CSunEditing::OnBnClickedSunambient()
{
	TheDXEngine.TheSun.dcvAmbient=GetColour(TheDXEngine.TheSun.dcvAmbient);
}

void CSunEditing::OnBnClickedSunspecular()
{
	TheDXEngine.TheSun.dcvSpecular=GetColour(TheDXEngine.TheSun.dcvSpecular);
}

void CSunEditing::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	TheDXEngine.TheSun.dcvDiffuse=m_Diffuse;
	TheDXEngine.TheSun.dcvAmbient=m_Ambient;
	TheDXEngine.TheSun.dcvSpecular=m_Specular;
	OnCancel();
}

void CSunEditing::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CSunEditing::OnBnClickedSundark()
{
	TheDXEngine.TheSun.dcvDiffuse.r=TheDXEngine.TheSun.dcvDiffuse.g=TheDXEngine.TheSun.dcvDiffuse.b=0.1f;
	TheDXEngine.TheSun.dcvAmbient.r=TheDXEngine.TheSun.dcvAmbient.g=TheDXEngine.TheSun.dcvAmbient.b=0.1f;
	TheDXEngine.TheSun.dcvSpecular.r=TheDXEngine.TheSun.dcvSpecular.g=TheDXEngine.TheSun.dcvSpecular.b=0.1f;
}

void CSunEditing::OnBnClickedSunmid()
{
	TheDXEngine.TheSun.dcvDiffuse.r=TheDXEngine.TheSun.dcvDiffuse.g=TheDXEngine.TheSun.dcvDiffuse.b=0.6f;
	TheDXEngine.TheSun.dcvAmbient.r=TheDXEngine.TheSun.dcvAmbient.g=TheDXEngine.TheSun.dcvAmbient.b=0.4f;
	TheDXEngine.TheSun.dcvSpecular.r=TheDXEngine.TheSun.dcvSpecular.g=TheDXEngine.TheSun.dcvSpecular.b=0.6f;
}

void CSunEditing::OnBnClickedSunbright()
{
	TheDXEngine.TheSun.dcvDiffuse.r=TheDXEngine.TheSun.dcvDiffuse.g=TheDXEngine.TheSun.dcvDiffuse.b=1.0f;
	TheDXEngine.TheSun.dcvAmbient.r=TheDXEngine.TheSun.dcvAmbient.g=TheDXEngine.TheSun.dcvAmbient.b=0.8f;
	TheDXEngine.TheSun.dcvSpecular.r=TheDXEngine.TheSun.dcvSpecular.g=TheDXEngine.TheSun.dcvSpecular.b=1.0f;
}
