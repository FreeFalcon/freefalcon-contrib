// LightEdit.cpp : implementation file
//

#include "stdafx.h"
#include "LodConvert.h"
#include ".\lightedit.h"
#include "DXEditing.h"
//#include "LightEdit.h"

#include	"math.h"

static	CLightEdit	*ThisLight;
#define	ATTENUATION_RATE	100

// CLightEdit dialog




IMPLEMENT_DYNAMIC(CLightEdit, CDialog)
CLightEdit::CLightEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CLightEdit::IDD, pParent)
{
	Active=false;
	Create(IDD, pParent);
	ThisLight=this;
	m_LightList=NULL;
	m_Selected=NULL;
	m_OwnLiteCheck.SetCheck(false);
	m_bLightMove=false;

	m_LiteTypeCombo.ResetContent();
	m_LiteTypeCombo.AddString("OMNI");
	m_LiteTypeCombo.AddString("SPOT");

	m_AmbientSlider.SetRange(0,100);
	m_SpecularSlider.SetRange(0,100);
	m_AttenuateSlider.SetRange(0,200);
	m_AttenuateSlider2.SetRange(0,200);
	m_DistanceSlider.SetRange(0,20000);
	m_ThetaSlider.SetRange(0,180);
	m_PhySlider.SetRange(0,180);

}

CLightEdit::~CLightEdit()
{
	if(m_Selected) m_Selected->Selected=false;
	Active=false;
	m_Selected=NULL;
}

void CLightEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIGHTLIST, m_List);
	DDX_Control(pDX, IDC_OWNLITECHECK, m_OwnLiteCheck);
	DDX_Control(pDX, IDC_LITETYPE, m_LiteTypeCombo);
	DDX_Control(pDX, IDC_AMBIENTSLIDER, m_AmbientSlider);
	DDX_Control(pDX, IDC_SPECULARSLIDER, m_SpecularSlider);
	DDX_Control(pDX, IDC_DISTANCESLIDER, m_DistanceSlider);
	DDX_Control(pDX, IDC_DISTANCETEXT, m_Distancetext);
	DDX_Control(pDX, IDC_LIGHTMOVE, m_LightMoveCheck);
	DDX_Control(pDX, IDC_LITESWITCHNR, m_LiteSwitchNrText);
	DDX_Control(pDX, IDC_ATTENUATESLIDER, m_AttenuateSlider);
	DDX_Control(pDX, IDC_ATTENUATETEXT, m_AttenuateText);
	DDX_Control(pDX, IDC_THETASLIDER, m_ThetaSlider);
	DDX_Control(pDX, IDC_THETATETEXT, m_ThetaText);
	DDX_Control(pDX, IDC_PHISLIDER, m_PhySlider);
	DDX_Control(pDX, IDC_PHYTEXT, m_Phytext);
	DDX_Control(pDX, IDC_ATTENUATESLIDER2, m_AttenuateSlider2);
	DDX_Control(pDX, IDC_ATTENUATETEXT2, m_Attenuate2Text);
	DDX_Control(pDX, IDC_OTHERLITECHECK, m_OthersLiteCheck);
	DDX_Control(pDX, IDC_OSTATICLITECHECK, m_StaticLiteCheck);
	DDX_Control(pDX, IDC_RNDLITECHECK, m_RndLiteCheck);
	DDX_Control(pDX, IDC_DOFDIMLITECHECK, m_DofDimLiteCheck);
	DDX_Control(pDX, IDC_LITESWITCHMASK, m_LiteSwitchMask);
}


BEGIN_MESSAGE_MAP(CLightEdit, CDialog)
	ON_LBN_SELCHANGE(IDC_LIGHTLIST, OnLbnSelchangeLightlist)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_LITETYPE, OnCbnSelchangeLitetype)
	ON_BN_CLICKED(IDC_OWNLITECHECK, OnBnClickedOwnlitecheck)
	ON_BN_CLICKED(IDC_DIFFUSELITE, OnBnClickedDiffuselite)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_AMBIENTSLIDER, OnNMCustomdrawAmbientslider)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SPECULARSLIDER, OnNMCustomdrawSpecularslider)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_DISTANCESLIDER, OnNMCustomdrawDistanceslider)
	ON_BN_CLICKED(IDC_LIGHTMOVE, OnBnClickedLightmove)
	ON_EN_CHANGE(IDC_LITESWITCHNR, OnEnChangeLiteswitchnr)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_ATTENUATESLIDER, OnNMCustomdrawAttenuateslider)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_THETASLIDER, OnNMCustomdrawThetaslider)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_PHISLIDER, OnNMCustomdrawPhislider)
	ON_BN_CLICKED(IDC_LITEKILL, OnBnClickedLitekill)
	ON_BN_CLICKED(IDC_LITEADD, OnBnClickedLiteadd)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_ATTENUATESLIDER2, OnNMCustomdrawAttenuateslider2)
	ON_BN_CLICKED(IDC_OTHERLITECHECK, OnBnClickedOtherlitecheck)
	ON_BN_CLICKED(IDC_OSTATICLITECHECK, OnBnClickedOstaticlitecheck)
	ON_BN_CLICKED(IDC_RNDLITECHECK, OnBnClickedRndlitecheck)
	ON_BN_CLICKED(IDC_DOFDIMLITECHECK, OnBnClickedDofdimlitecheck)
	ON_BN_CLICKED(IDC_LITECOPY, OnBnClickedLitecopy)
	ON_BN_CLICKED(IDC_LITEPASTE, OnBnClickedLitepaste)
	ON_EN_CHANGE(IDC_LITESWITCHMASK, OnEnChangeLiteswitchmask)
	ON_BN_CLICKED(IDC_CALCSTATIC, OnBnClickedCalcstatic)
	ON_BN_CLICKED(IDC_CLRSTATIC, OnBnClickedClrstatic)
	ON_BN_CLICKED(IDC_SAVESET, OnBnClickedSaveset)
	ON_BN_CLICKED(IDC_LOADSET, OnBnClickedLoadset)
	ON_BN_CLICKED(IDC_STATICRECALC, OnBnClickedStaticrecalc)
	ON_BN_CLICKED(IDC_KILLALLLITES, OnBnClickedKillalllites)
END_MESSAGE_MAP()


void CLightEdit::UpdateList(void)
{
	Updating=true;
	m_List.ResetContent();
	E_Light	*ptr=m_LightList=OSelected->LightList;
	if(!m_LightList) return;

	CString Text;
	DWORD	LNr=0;
	while(ptr){
		LNr++;
		Text.Format("%01d - %s   SW:%000d", LNr, (ptr->Light.Light.dltType==D3DLIGHT_SPOT)?"SPOT":"OMNI", ptr->Light.Switch);
		m_List.AddString(Text);
		if(ptr==m_Selected) m_List.SetCurSel(LNr-1);
		OSelected->Obj->SetSwitch(ptr->Light.Switch,1);
		ptr=ptr->Next;
	}

	RecalcAllStatic();

	if(m_Selected){
		m_OwnLiteCheck.SetCheck(m_Selected->Light.Flags.OwnLight);
		m_OthersLiteCheck.SetCheck(m_Selected->Light.Flags.NotSelfLight);
		m_StaticLiteCheck.SetCheck(m_Selected->Light.Flags.Static);
		m_RndLiteCheck.SetCheck(m_Selected->Light.Flags.Random);
		m_DofDimLiteCheck.SetCheck(m_Selected->Light.Flags.DofDim);

		if(m_Selected->Light.Light.dltType==D3DLIGHT_POINT) m_LiteTypeCombo.SetCurSel(0);
		if(m_Selected->Light.Light.dltType==D3DLIGHT_SPOT) m_LiteTypeCombo.SetCurSel(1);


		float	p1=m_Selected->Light.Light.dcvAmbient.r+m_Selected->Light.Light.dcvAmbient.g+m_Selected->Light.Light.dcvAmbient.b;
		float	p2=m_Selected->Light.Light.dcvDiffuse.r+m_Selected->Light.Light.dcvDiffuse.g+m_Selected->Light.Light.dcvDiffuse.b;
		m_AmbientSlider.SetPos(((DWORD)(p1/p2*100)));

		p1=m_Selected->Light.Light.dcvSpecular.r+m_Selected->Light.Light.dcvSpecular.g+m_Selected->Light.Light.dcvSpecular.b;
		m_SpecularSlider.SetPos(((DWORD)(p1/p2*100)));

		Text.Format("%4.1",m_Selected->Light.Light.dvRange);
		m_DistanceSlider.SetPos((DWORD)(m_Selected->Light.Light.dvRange*10));
		m_Distancetext.SetWindowText(Text);

		m_AttenuateSlider.SetPos((DWORD)(m_Selected->Light.Light.dvAttenuation0*ATTENUATION_RATE));
		Text.Format("%d", m_AttenuateSlider.GetPos());
		m_AttenuateText.SetWindowText(Text);

		m_AttenuateSlider2.SetPos((DWORD)(m_Selected->Light.Light.dvAttenuation1*ATTENUATION_RATE));
		Text.Format("%d", m_AttenuateSlider2.GetPos());
		m_Attenuate2Text.SetWindowText(Text);

		if(m_Selected->Light.Light.dltType==D3DLIGHT_SPOT){
			m_AttenuateSlider.EnableWindow(true);
			m_ThetaSlider.EnableWindow(true);
			m_PhySlider.EnableWindow(true);

			m_ThetaSlider.SetPos((DWORD)(m_Selected->Light.Light.dvTheta*180/PI));
			Text.Format("%d°", m_ThetaSlider.GetPos());
			m_ThetaText.SetWindowText(Text);

			m_PhySlider.SetPos((DWORD)(m_Selected->Light.Light.dvPhi*180/PI));
			Text.Format("%d°", m_PhySlider.GetPos());
			m_Phytext.SetWindowText(Text);

		} else{
			//m_AttenuateSlider.EnableWindow(false);
			m_ThetaSlider.EnableWindow(false);
			m_PhySlider.EnableWindow(false);
		}

		Text.Format("%d", m_Selected->Light.Switch);
		m_LiteSwitchNrText.SetWindowText(Text);

		Text.Format("%d", m_Selected->Light.SwitchMask);
		m_LiteSwitchMask.SetWindowText(Text);

	}
	Updating=false;
}


	
void CLightEdit::EditLight(E_Light *List)
{
	Active=true;
	m_LightList=List;
	UpdateList();
	ShowWindow(SW_SHOWNA);
}

// CLightEdit message handlers

void CLightEdit::OnLbnSelchangeLightlist()
{
	if(!m_LightList) return;
	DWORD c=m_List.GetCurSel();
	if(m_Selected) m_Selected->Selected=false;
	if(c<0) return;
	m_List.SetCurSel(c);
	m_Selected=OSelected->LightList;
	while(c--) m_Selected=m_Selected->Next;
	m_Selected->Selected=true;
	UpdateList();
	
}

void CLightEdit::OnBnClickedOk()
{
	if(m_Selected) m_Selected->Selected=false;
	m_Selected=NULL;
	Active=false;
	ShowWindow(SW_HIDE);
}

void CLightEdit::OnCbnSelchangeLitetype()
{
	CString	sType;
	
	if(!m_Selected) return;

	DWORD dwType=m_LiteTypeCombo.GetCurSel();
	if(dwType==0) m_Selected->Light.Light.dltType=D3DLIGHT_POINT;
	if(dwType==1) m_Selected->Light.Light.dltType=D3DLIGHT_SPOT;
	UpdateList();

}

void CLightEdit::OnBnClickedOwnlitecheck()
{
	if(m_Selected) m_Selected->Light.Flags.OwnLight=m_OwnLiteCheck.GetCheck()?true:false;
	UpdateList();
}


void	GetD3DColorValue(D3DCOLORVALUE *D3DColor)
{
	DWORD	Color;

	Color=	((WORD)(D3DColor->r*255.0f)<<16)|
			((WORD)(D3DColor->g*255.0f)<<8)|
			(WORD)(D3DColor->b*255.0f);

	Color=GetColour(Color);

	D3DColor->r=((float)((Color>>16)&0xff))/255.0f;
	D3DColor->g=((float)((Color>>8)&0xff))/255.0f;
	D3DColor->b=((float)(Color&0xff))/255.0f;
}



void CLightEdit::OnBnClickedDiffuselite()
{
	if(!m_Selected) return;

	GetD3DColorValue(&m_Selected->Light.Light.dcvDiffuse);

	float	cx=((float)(m_AmbientSlider.GetPos()))/100;
	m_Selected->Light.Light.dcvAmbient.r=m_Selected->Light.Light.dcvDiffuse.r*cx;
	m_Selected->Light.Light.dcvAmbient.g=m_Selected->Light.Light.dcvDiffuse.g*cx;
	m_Selected->Light.Light.dcvAmbient.b=m_Selected->Light.Light.dcvDiffuse.b*cx;
	
	cx=((float)(m_AmbientSlider.GetPos()))/100;
	m_Selected->Light.Light.dcvSpecular.r=m_Selected->Light.Light.dcvDiffuse.r*cx;
	m_Selected->Light.Light.dcvSpecular.g=m_Selected->Light.Light.dcvDiffuse.g*cx;
	m_Selected->Light.Light.dcvSpecular.b=m_Selected->Light.Light.dcvDiffuse.b*cx;
}


void CLightEdit::OnNMCustomdrawAmbientslider(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(Updating) return;
	if(!m_Selected) return;
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	float	cx=((float)(m_AmbientSlider.GetPos()))/100;

	m_Selected->Light.Light.dcvAmbient.r=m_Selected->Light.Light.dcvDiffuse.r*cx;
	m_Selected->Light.Light.dcvAmbient.g=m_Selected->Light.Light.dcvDiffuse.g*cx;
	m_Selected->Light.Light.dcvAmbient.b=m_Selected->Light.Light.dcvDiffuse.b*cx;
}

void CLightEdit::OnNMCustomdrawSpecularslider(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(Updating) return;
	if(!m_Selected) return;
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	float	cx=((float)(m_AmbientSlider.GetPos()))/100;

	m_Selected->Light.Light.dcvSpecular.r=m_Selected->Light.Light.dcvDiffuse.r*cx;
	m_Selected->Light.Light.dcvSpecular.g=m_Selected->Light.Light.dcvDiffuse.g*cx;
	m_Selected->Light.Light.dcvSpecular.b=m_Selected->Light.Light.dcvDiffuse.b*cx;
}

void CLightEdit::OnNMCustomdrawDistanceslider(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	if(Updating) return;
	if(!m_Selected) return;
	m_Selected->Light.Light.dvRange=(float)m_DistanceSlider.GetPos()/10.f;
	CString Text;
	Text.Format("%4.1f",m_Selected->Light.Light.dvRange);
	m_Distancetext.SetWindowText(Text);


}


void LightMoveCallBack(D3DXVECTOR3 Transform, D3DXVECTOR3 Move, bool Remove)
{
	ThisLight->LightMoveFunction(Transform, Move, Remove);
}


void CLightEdit::LightMoveFunction(D3DXVECTOR3 Transform, D3DXVECTOR3 Move, bool Remove)
{
	if(!m_Selected) {
		m_bLightMove=false;
		MoveFunction=NULL;
		m_LightMoveCheck.SetCheck(FALSE);
		return;
	}
	if(Remove){
		m_bLightMove=false;
		m_LightMoveCheck.SetCheck(m_bLightMove);
		MoveFunction=NULL;
		return;
	}

	m_Selected->Light.Light.dvPosition.x+=Move.x;
	m_Selected->Light.Light.dvPosition.y+=Move.y;
	m_Selected->Light.Light.dvPosition.z+=Move.z;


	if(Transform.y || Transform.z || Transform. x){
		float x=m_Selected->Light.Light.dvDirection.x;
		float y=m_Selected->Light.Light.dvDirection.y;
		float z=m_Selected->Light.Light.dvDirection.z;

		float Yaw=atan2(y, x);
		float Pitch=atan2(z, sqrtf(x*x+y*y));

		if(PlaneMode==PLANE_Y) { Yaw+=Transform.x; Pitch+=Transform.y; }
		if(PlaneMode==PLANE_X) { Yaw-=Transform.x; Pitch+=Transform.z; }
		if(PlaneMode==PLANE_Z) { Yaw+=Transform.y; Pitch-=Transform.z; }
		m_Selected->Light.Light.dvDirection.x=cosf(Yaw);
		m_Selected->Light.Light.dvDirection.y=sinf(Yaw);
		m_Selected->Light.Light.dvDirection.z=sqrtf(cosf(Yaw)*cosf(Yaw)+sinf(Yaw)*sinf(Yaw))*tan(Pitch);



	}

}


void CLightEdit::OnBnClickedLightmove()
{

	if(!m_Selected) {
		m_bLightMove=false;
		if(MoveFunction==LightMoveCallBack) MoveFunction=NULL;
		m_LightMoveCheck.SetCheck(FALSE);
		return;
	}
	m_bLightMove^=true;
	m_LightMoveCheck.SetCheck(m_bLightMove);
	if(!m_bLightMove)
		MoveFunction=NULL;
	else {
		if(!m_Selected) return;
		D3DXVECTOR3	Null(0,0,0);
		if(MoveFunction && MoveFunction!=LightMoveCallBack) MoveFunction(Null, Null, true);
		MoveFunction=LightMoveCallBack;
	}
}

void CLightEdit::OnEnChangeLiteswitchnr()
{
	if(Updating) return;
	CString Value;
	m_LiteSwitchNrText.GetWindowText(Value);
	DWORD SwNr=atoi(Value);
	m_Selected->Light.Switch=SwNr;
}

void CLightEdit::OnNMCustomdrawAttenuateslider(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	if(Updating) return;
	if(!m_Selected) return;
	float	cx=((float)(m_AttenuateSlider.GetPos()))/ATTENUATION_RATE;

	m_Selected->Light.Light.dvAttenuation0=cx;
	CString Text;
	Text.Format("%d",m_AttenuateSlider.GetPos());
	m_AttenuateText.SetWindowText(Text);

}

void CLightEdit::OnNMCustomdrawThetaslider(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	if(Updating) return;
	if(!m_Selected) return;
	float	cx=((float)(m_ThetaSlider.GetPos()))/180*PI;

	m_Selected->Light.Light.dvTheta=cx;
	CString Text;
	Text.Format("%d°", m_ThetaSlider.GetPos());
	m_ThetaText.SetWindowText(Text);
}

void CLightEdit::OnNMCustomdrawPhislider(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	if(Updating) return;
	if(!m_Selected) return;
	float	cx=((float)(m_PhySlider.GetPos()))/180*PI;

	m_Selected->Light.Light.dvPhi=cx;
	CString Text;
	Text.Format("%d°", m_PhySlider.GetPos());
	m_Phytext.SetWindowText(Text);
}

void CLightEdit::OnBnClickedLitekill()
{
	if(!m_Selected) return;
	if(!OSelected || !OSelected->Header.dwLightsNr) return;
	E_AddStaticLight(OSelected, &m_Selected->Light, m_List.GetCurSel(), true);
	if(m_Selected->Prev) m_Selected->Prev->Next=m_Selected->Next;	
	else OSelected->LightList=m_Selected->Next;

	if(m_Selected->Next) m_Selected->Next->Prev=m_Selected->Prev;
	m_Selected->Selected=false;
	delete(m_Selected);
	OSelected->Header.dwLightsNr--;
	m_Selected=NULL;

	UpdateList();

}

void CLightEdit::OnBnClickedLiteadd()
{
	if(!OSelected ) return;
	E_Light	*ptr=OSelected->LightList, *New=new E_Light;
	ZeroMemory(New, sizeof(E_Light));
	OSelected->LightList=New;
	New->Prev=NULL;
	New->Next=ptr;
	if(ptr) ptr->Prev=New;
	New->Light.Light.dltType=D3DLIGHT_POINT;
	New->Light.Light.dvRange=3.0f;
	New->Light.Light.dvAttenuation0=1.0f;
	New->Light.Light.dvAttenuation1=0.0f;
	New->Light.Light.dcvDiffuse.r=1.0f;
	New->Light.Light.dcvDiffuse.g=1.0f;
	New->Light.Light.dcvDiffuse.b=1.0f;
	New->Light.Light.dcvAmbient.r=1.0f;
	New->Light.Light.dcvAmbient.g=1.0f;
	New->Light.Light.dcvAmbient.b=1.0f;
	New->Light.Light.dcvSpecular.r=1.0f;
	New->Light.Light.dcvSpecular.g=1.0f;
	New->Light.Light.dcvSpecular.b=1.0f;

	OSelected->Header.dwLightsNr++;
	if(m_Selected) m_Selected->Selected=false;
	m_Selected=New;
	m_Selected->Selected=true;
	
	UpdateList();


}

void CLightEdit::OnNMCustomdrawAttenuateslider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(Updating) return;
	if(!m_Selected) return;
	float	cx=((float)(m_AttenuateSlider2.GetPos()))/ATTENUATION_RATE;

	m_Selected->Light.Light.dvAttenuation1=cx;
	CString Text;
	Text.Format("%d",m_AttenuateSlider2.GetPos());
	m_Attenuate2Text.SetWindowText(Text);
}

void CLightEdit::OnBnClickedOtherlitecheck()
{
	if(m_Selected) m_Selected->Light.Flags.NotSelfLight=m_OthersLiteCheck.GetCheck()?true:false;
	UpdateList();
}

void CLightEdit::OnBnClickedOstaticlitecheck()
{
	if(m_Selected){
		m_Selected->Light.Flags.Static=m_StaticLiteCheck.GetCheck()?true:false;
		if(m_StaticLiteCheck.GetCheck()) OnBnClickedCalcstatic();
		else OnBnClickedClrstatic();
	}

	UpdateList();
}

void CLightEdit::OnBnClickedRndlitecheck()
{
	if(m_Selected) m_Selected->Light.Flags.Random=m_RndLiteCheck.GetCheck()?true:false;
	UpdateList();
}

void CLightEdit::OnBnClickedDofdimlitecheck()
{
	if(m_Selected) m_Selected->Light.Flags.DofDim=m_DofDimLiteCheck.GetCheck()?true:false;
	UpdateList();
}

void CLightEdit::OnBnClickedLitecopy()
{
	if(m_Selected) LightCopy=m_Selected->Light;
}

void CLightEdit::OnBnClickedLitepaste()
{
	if(m_Selected) m_Selected->Light=LightCopy;
	UpdateList();
}

void CLightEdit::OnEnChangeLiteswitchmask()
{
	if(Updating) return;
	CString Value;
	m_LiteSwitchMask.GetWindowText(Value);
	DWORD SwMask=atoi(Value);
	m_Selected->Light.SwitchMask=SwMask;
}

void CLightEdit::OnBnClickedCalcstatic()
{
	if(m_Selected && m_Selected->Light.Flags.Static) E_AddStaticLight(OSelected, &m_Selected->Light, m_List.GetCurSel());
}

void CLightEdit::OnBnClickedClrstatic()
{
	if(m_Selected) E_AddStaticLight(OSelected, &m_Selected->Light, m_List.GetCurSel(), true);
}

ParaTableType	LightsTable[]={
	PARATABLE(SWITCH, DXLightType, Switch),
	PARATABLE(SWITCHMASK, DXLightType, SwitchMask),
	PARATABLE(ARGUMENT, DXLightType, Argument),
	PARATABLE(FLAGS, DXLightType, Flags),
	PARATABLE(LITETYPE, DXLightType, Light.dltType),
	PARATABLE(LITEPOSITION, DXLightType, Light.dvPosition),
	PARATABLE(LITEDIRECTION, DXLightType, Light.dvDirection),
	PARATABLE(LITERANGE, DXLightType, Light.dvRange),
    PARATABLE(FALLOFF, DXLightType, Light.dvFalloff),
	PARATABLE(LITEATT0, DXLightType, Light.dvAttenuation0),
	PARATABLE(LITEATT1, DXLightType, Light.dvAttenuation1),
	PARATABLE(LITEDIFFUSE, DXLightType, Light.dcvDiffuse),
	PARATABLE(LITEAMBIENT, DXLightType, Light.dcvAmbient),
	PARATABLE(LITESPECULAR, DXLightType, Light.dcvSpecular),
    PARATABLE(LITETHETA, DXLightType, Light.dvTheta),
    PARATABLE(LITEPHI, DXLightType, Light.dvPhi),

	PARATABLE(, DXLightType, Switch)
};

void	CLightEdit::SavePara(CStdioFile *F, ParaTableType *T, void *P)
{
	CString	Value,Line;
	while(T->Name!=""){
		Line=T->Name+"=";
		for(DWORD i=0; i<T->Size; i+=4){
			Value.Format("%u",*((DWORD*)(((char*)P)+T->Offset+i)));
			Line+=Value;
			if((i+4)<T->Size)Line+=",";
		}
		Line+="\n";
		F->WriteString(LPCSTR(Line));
		T++;
	}
	Value="#\n";
	F->WriteString(LPCSTR(Value));
}

bool	CLightEdit::LoadPara(CStdioFile *F, ParaTableType *T, void *P)
{
	CString	Line, Name, Value;
	ParaTableType *D;
	bool			NewData=false;

	do{	
		F->ReadString(Line);
		Name=Line.SpanExcluding("=");
		if(Name!="#"){
			Line=Line.Right(Line.GetLength()-Name.GetLength()-1);
			D=T;
			while(D->Name!="" && D->Name!=Name) D++;
			if(D->Name!=""){
				NewData=true;
				DWORD	Nr=0;
				while(Line.GetLength() && Nr<D->Size){
					Value=Line.SpanExcluding(",");
					Line=Line.Right(Line.GetLength()-Value.GetLength()-1);
					*((DWORD*)(((char*)P)+D->Offset+Nr))=atol(LPCSTR(Value));
					Nr+=4;
				}
			}
		}
	} while(Name!="#");
	return NewData;
}

void CLightEdit::OnBnClickedSaveset()
{
	if(Updating) return;
	if(!OSelected || !OSelected->Header.dwLightsNr) return;
	
	E_Light	*L=OSelected->LightList;
	if(!L) return;

	CString	FileName="*.lts", FileTitle;
	CFileDialog	F(true, "*.lts", FileName, 0, "Lights Set Files|*.lts");
	DWORD	Updated=0;
	DWORD	rs=(DWORD)F.DoModal();
	if(rs==IDOK) {
		FileName=F.GetPathName();
		FileTitle=F.GetFileTitle();
	} else
		return;

	CStdioFile	File;
	if(File.Open(FileName, CFile::modeWrite|CFile::modeCreate, NULL)){
		while(L){
			SavePara(&File, LightsTable, (void*)&L->Light);
			L=L->Next;
		}
	}
	// End Marker
	File.WriteString("#\n");
	File.WriteString("#\n");
	File.WriteString("#\n");
	File.Close();
}

void CLightEdit::OnBnClickedLoadset()
{
	if(!OSelected) return;
	
	DXLightType	L;

	CString	FileName="*.lts", FileTitle;
	CFileDialog	F(true, "*.lts", FileName, 0, "Lights Set Files|*.lts");
	DWORD	Updated=0;
	DWORD	rs=(DWORD)F.DoModal();
	if(rs==IDOK) {
		FileName=F.GetPathName();
		FileTitle=F.GetFileTitle();
	} else
		return;

	CStdioFile	File;
	if(File.Open(FileName, CFile::modeRead, NULL)){
		DWORD	FileSize=File.GetLength();
		while(File.GetPosition()<FileSize){
			if(LoadPara(&File, LightsTable, (void*)&L)){
				OnBnClickedLiteadd();
				m_Selected->Light=L;
			}
		}
	}
	File.Close();
	UpdateList();
	RecalcAllStatic();
}


void CLightEdit::RecalcAllStatic()
{
	if(!OSelected) return;


	E_ClearAllStaticLights(OSelected);
	E_Light	*DXL=OSelected->LightList;
	DWORD	LNr=0;
	while(DXL){
		E_AddStaticLight(OSelected, &DXL->Light, LNr, true);
		if(DXL->Light.Flags.Static) E_AddStaticLight(OSelected, &DXL->Light, LNr);
		DXL=DXL->Next;
		LNr++;
	}


}

void CLightEdit::OnBnClickedStaticrecalc()
{
	RecalcAllStatic();
}

void CLightEdit::OnBnClickedKillalllites()
{
	if(!OSelected) return;
	while(OSelected->LightList){
		m_List.SetCurSel(0);
		m_Selected=OSelected->LightList;
		OnBnClickedLitekill();
	}
}
