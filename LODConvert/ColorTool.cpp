// ColorTool.cpp : implementation file
//

#include "stdafx.h"
#include "LODConvert.h"
#include "ColorTool.h"
#include ".\colortool.h"


// CColorTool dialog

IMPLEMENT_DYNCREATE(CColorTool, CDHtmlDialog)

CColorTool::CColorTool(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(CColorTool::IDD, CColorTool::IDH, pParent)
{
	Create(IDD_COLORTOOL);
	ShowWindow(SW_SHOW);
}

CColorTool::~CColorTool()
{
}

void CColorTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ALPHASLIDE, m_AlphaSlide);
	DDX_Control(pDX, IDC_REDSLIDE, m_RedSlide);
	DDX_Control(pDX, IDC_GREENDSLIDE, m_GreenSlide);
	DDX_Control(pDX, IDC_BLUESLIDE, m_BlueSlide);
	DDX_Control(pDX, IDC_VALUES, m_Values);

	m_AlphaSlide.SetRange(0, 100, true);
	m_RedSlide.SetRange(0, 100, true);
	m_BlueSlide.SetRange(0, 100, true);
	m_GreenSlide.SetRange(0, 100, true);

	LDiffuseOn=LSpecularOn=LAmbientOn=false;
	MDiffuseOn=MSpecularOn=MAmbientOn=MEmissiveOn=false;
	TheColour=NULL;
	DDX_Control(pDX, IDC_DiscoCheck, m_DiscoCheck);
	DDX_Control(pDX, IDC_NVG, m_NVG);
}

BOOL CColorTool::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CColorTool, CDHtmlDialog)
	ON_BN_CLICKED(IDC_LDIFFUSE, OnBnClickedLdiffuse)
	ON_BN_CLICKED(IDC_LSPECULAR, OnBnClickedLspecular)
	ON_BN_CLICKED(IDC_LAMBIENT, OnBnClickedLambient)
	ON_BN_CLICKED(IDC_MDIFFUSE, OnBnClickedMdiffuse)
	ON_BN_CLICKED(IDC_MSPECULAR, OnBnClickedMspecular)
	ON_BN_CLICKED(IDC_MAMBIENT, OnBnClickedMambient)
	ON_BN_CLICKED(IDC_MEMISSIVE, OnBnClickedMemissive)
	ON_BN_CLICKED(IDC_DiscoCheck, OnBnClickedDiscocheck)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_BLUESLIDE, OnNMCustomdrawBlueslide)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_GREENDSLIDE, OnNMCustomdrawGreendslide)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_REDSLIDE, OnNMCustomdrawRedslide)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_ALPHASLIDE, OnNMCustomdrawAlphaslide)
	ON_BN_CLICKED(IDC_NVG, OnBnClickedNvg)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CColorTool)
	DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()



// CColorTool message handlers

HRESULT CColorTool::OnButtonOK(IHTMLElement* /*pElement*/)
{
	OnOK();
	return S_OK;  // return TRUE  unless you set the focus to a control
}

HRESULT CColorTool::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	OnCancel();
	return S_OK;  // return TRUE  unless you set the focus to a control
}

void CColorTool::SetSliders(D3DCOLORVALUE *Color)
{
		m_AlphaSlide.SetPos((int)(Color->a/2*100.f));
		m_RedSlide.SetPos((int)(Color->r/2*100.f));
		m_GreenSlide.SetPos((int)(Color->g/2*100.f));
		m_BlueSlide.SetPos((int)(Color->b/2*100.f));

		CString	t;
		t.Format("A=%f  R=%f  G=%f  B=%f",Color->a, Color->r,Color->g,Color->b);
		m_Values.SetWindowText(t);
}


void CColorTool::GetSliders(D3DCOLORVALUE *Color)
{
		Color->a=((float)m_AlphaSlide.GetPos())/100.0f*2;
		Color->r=((float)m_RedSlide.GetPos())/100.0f*2;
		Color->g=((float)m_GreenSlide.GetPos())/100.0f*2;
		Color->b=((float)m_BlueSlide.GetPos())/100.0f*2;

		CString	t;
		t.Format("A=%f  R=%f  G=%f  B=%f",Color->a, Color->r,Color->g,Color->b);
		m_Values.SetWindowText(t);
}


void CColorTool::OnBnClickedLdiffuse()
{
	// TODO: Add your control notification handler code here
	TheColour=&TheDXEngine.TheSun.dcvDiffuse;
	SetSliders(TheColour);

}

void CColorTool::OnBnClickedLspecular()
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	TheColour=&TheDXEngine.TheSun.dcvSpecular;
	SetSliders(TheColour);
}

void CColorTool::OnBnClickedLambient()
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	TheColour=&TheDXEngine.TheSun.dcvAmbient;
	SetSliders(TheColour);
}

void CColorTool::OnBnClickedMdiffuse()
{
	// TODO: Add your control notification handler code here
	
	TheDXEngine.TheSun.dcvDiffuse.r=0.1f;
	TheDXEngine.TheSun.dcvDiffuse.g=0.2f;
	TheDXEngine.TheSun.dcvDiffuse.b=0.3f;
	
	TheDXEngine.TheSun.dcvAmbient.r=0.1f;
	TheDXEngine.TheSun.dcvAmbient.g=0.1f;
	TheDXEngine.TheSun.dcvAmbient.b=0.2f;

	TheDXEngine.TheSun.dcvSpecular.r=0.1f;
	TheDXEngine.TheSun.dcvSpecular.g=0.2f;
	TheDXEngine.TheSun.dcvSpecular.b=0.4f;


}

void CColorTool::OnBnClickedMspecular()
{
	TheDXEngine.TheSun.dcvDiffuse.r=0.7f;
	TheDXEngine.TheSun.dcvDiffuse.g=0.7f;
	TheDXEngine.TheSun.dcvDiffuse.b=0.7f;
	
	TheDXEngine.TheSun.dcvAmbient.r=0.6f;
	TheDXEngine.TheSun.dcvAmbient.g=0.6f;
	TheDXEngine.TheSun.dcvAmbient.b=0.6f;

	TheDXEngine.TheSun.dcvSpecular.r=1.0f;
	TheDXEngine.TheSun.dcvSpecular.g=1.0f;
	TheDXEngine.TheSun.dcvSpecular.b=1.0f;
}

void CColorTool::OnBnClickedMambient()
{
	TheDXEngine.TheSun.dcvDiffuse.r=0.7f;
	TheDXEngine.TheSun.dcvDiffuse.g=0.7f;
	TheDXEngine.TheSun.dcvDiffuse.b=0.7f;
	
	TheDXEngine.TheSun.dcvAmbient.r=0.3f;
	TheDXEngine.TheSun.dcvAmbient.g=0.25f;
	TheDXEngine.TheSun.dcvAmbient.b=0.3f;

	TheDXEngine.TheSun.dcvSpecular.r=1.0f;
	TheDXEngine.TheSun.dcvSpecular.g=0.5f;
	TheDXEngine.TheSun.dcvSpecular.b=1.0f;
}

void CColorTool::OnBnClickedMemissive()
{
	TheDXEngine.TheSun.dcvDiffuse.r=0.7f;
	TheDXEngine.TheSun.dcvDiffuse.g=0.4f;
	TheDXEngine.TheSun.dcvDiffuse.b=0.5f;
	
	TheDXEngine.TheSun.dcvAmbient.r=0.4f;
	TheDXEngine.TheSun.dcvAmbient.g=0.3f;
	TheDXEngine.TheSun.dcvAmbient.b=0.2f;

	TheDXEngine.TheSun.dcvSpecular.r=0.7f;
	TheDXEngine.TheSun.dcvSpecular.g=0.4f;
	TheDXEngine.TheSun.dcvSpecular.b=0.3f;
}

void CColorTool::OnBnClickedDiscocheck()
{
	// TODO: Add your control notification handler code here
	DX->DiscoEnable=m_DiscoCheck.GetCheck();
}

void CColorTool::OnNMCustomdrawBlueslide(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	if(!TheColour) return;
	TheColour->b=((float)m_BlueSlide.GetPos())/100.0f*2;
	CString t;
	t.Format("A=%f  R=%f  G=%f  B=%f",TheColour->a, TheColour->r,TheColour->g,TheColour->b);
	m_Values.SetWindowText(t);
}

void CColorTool::OnNMCustomdrawGreendslide(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	if(!TheColour) return;
	TheColour->g=((float)m_GreenSlide.GetPos())/100.0f*2;
	CString t;
	t.Format("A=%f  R=%f  G=%f  B=%f",TheColour->a, TheColour->r,TheColour->g,TheColour->b);
	m_Values.SetWindowText(t);
}

void CColorTool::OnNMCustomdrawRedslide(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	if(!TheColour) return;
	TheColour->r=((float)m_RedSlide.GetPos())/100.0f*2;
	CString t;
	t.Format("A=%f  R=%f  G=%f  B=%f",TheColour->a, TheColour->r,TheColour->g,TheColour->b);
	m_Values.SetWindowText(t);
}

void CColorTool::OnNMCustomdrawAlphaslide(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	if(!TheColour) return;
	TheColour->a=((float)m_AlphaSlide.GetPos())/100.0f*2;
	CString t;
	t.Format("A=%f  R=%f  G=%f  B=%f",TheColour->a, TheColour->r,TheColour->g,TheColour->b);
	m_Values.SetWindowText(t);
}

void CColorTool::OnBnClickedNvg()
{
	// TODO: Add your control notification handler code here
	TheDXEngine.SetNVG(m_NVG.GetCheck());
}
