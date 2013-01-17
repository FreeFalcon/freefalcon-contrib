// Tools.cpp : implementation file
//

#include "stdafx.h"
#include "LODConvert.h"
#include "Tools.h"
#include ".\tools.h"


const	DOFItemType	DOFLabs[128]={
	{ "LT_STAB", 0 },
	{ "RT_STAB", 1 },
	{ "LT_FLAP", 2 }, 
	{ "RT_FLAP", 3 } ,
	{ "RUDDER", 4},
	{ "NOS_GEAR_ROT", 5},
	{ "NOS_GEAR_COMP", 6},
	{ "LT_GEAR_COMP", 7},
	{ "RT_GEAR_COMP", 8},
	{ "LT_LEF", 9},
	{ "RT_LEF", 10},
	{ "BROKEN_NOS_GEAR", 11},
	{ "BROKEN_LT_GEAR",  12},
	{ "BROKEN_RT_GEAR",  13},
	{ "LT_AIR_BRAKE_TOP", 15},
	{ "LT_AIR_BRAKE_BOT", 16},
	{ "RT_AIR_BRAKE_TOP", 17},
	{ "RT_AIR_BRAKE_BOT", 18},
	{ "NOS_GEAR", 19},
	{ "LT_GEAR", 20},
	{ "RT_GEAR", 21},
	{ "NOS_GEAR_DR", 22},
	{ "LT_GEAR_DR", 23},
	{ "RT_GEAR_DR", 24},
	{ "LT_TEF", 28},
	{ "RT_TEF",  29},
	{ "CANOPY_DOF", 30},
	{ "HOOK_DOF", 37},
	{ "AB",  38}, 
	{ "Nozzle ",   39},
	{ "PROPELLOR", 40}, 
	{ "LT_SPOILER1", 42},
	{ "RT_SPOILER1", 43},
	{ "LT_SPOILER2", 44},
	{ "RT_SPOILER2", 45},
	{ "SWINGWING", 46}, 

	{ "WHEEL1", 50},
	{ "", 0}};


const	DOFItemType	SWLabs[128]={
	{"AB", 0},
	{"NOS_GEAR_SW", 1},
	{"LT_GEAR_SW",2},
	{"RT_GEAR_SW",3},
	{"NOS_GEAR_ROD",4},
	{"CANOPY",5},
	{"WING_VAPOR",6},
	{"TAIL_STROBE",7},
	{"NAV_LIGHTS",8},
	{"LAND_LIGHTS",9},
	{"EXH_NOZZLE",10},
	{"TIRN_POD",11},
	{"HTS_POD",12},
	{"REFUEL_DR",13},
	{"NOS_GEAR_DR_SW",14},
	{"LT_GEAR_DR_SW",15},
	{"RT_GEAR_DR_SW",16},
	{"NOS_GEAR_HOLE",17},
	{"LT_GEAR_HOLE",18},
	{"RT_GEAR_HOLE",19},
	{"BROKEN_NOS_GEAR_SW",20},
	{"BROKEN_LT_GEAR_SW",21},
	{"BROKEN_RT_GEAR_SW",22},
	{"HOOK",23},
	{"DRAGCHUTE",24},
	{ "", 0}};


// CTools dialog

IMPLEMENT_DYNAMIC(CTools, CDialog)
CTools::CTools(CWnd* pParent /*=NULL*/)
	: CDialog(CTools::IDD, pParent)
{
	DofSelected=SWSelected=-1;
	Create(IDD_TOOLS);
	ShowWindow(SW_SHOW);
	ZeroMemory(SwitchValues,sizeof(SwitchValues));
}

CTools::~CTools()
{
}

void CTools::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DOFLIST, m_DofList);
	DDX_Control(pDX, IDC_SWLIST, m_SWList);
	DDX_Control(pDX, IDC_SWVALUE, m_SWSlider);
	DDX_Control(pDX, IDC_SWTEXT, m_SWText);
	DDX_Control(pDX, IDC_DOFROT, m_RotSlider);
	DDX_Control(pDX, IDC_DOFTRANS, m_TransSlider);


	int	a=0;
	while(DOFLabs[a].Name!=""){
		CString Label;
		Label.Format("%0.2d  -  %s", DOFLabs[a].Number, DOFLabs[a].Name);
		m_DofList.AddString(Label);
		a++;
	}
	a=0;
	while(SWLabs[a].Name!=""){
		CString Label;
		Label.Format("%0.2d  -  %s", SWLabs[a].Number, SWLabs[a].Name);
		m_SWList.AddString(Label);
		a++;
	}

	m_SWSlider.SetRange(0, 12, false);
	m_RotSlider.SetRange(0, 100, false);
	m_TransSlider.SetRange(0, 100, false);
}


BEGIN_MESSAGE_MAP(CTools, CDialog)
	ON_LBN_SELCHANGE(IDC_DOFLIST, OnLbnSelchangeDoflist)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_DOFROT, OnNMCustomdrawDofrot)
	ON_LBN_SELCHANGE(IDC_SWLIST, OnLbnSelchangeSwlist)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SWVALUE, OnNMCustomdrawSwvalue)
	ON_BN_CLICKED(IDC_SWONBUTTON, OnBnClickedSwonbutton)
	ON_BN_CLICKED(IDC_SWOFFBUTTON2, OnBnClickedSwoffbutton2)
	ON_BN_CLICKED(IDC_DOFZERO, OnBnClickedDofzero)
END_MESSAGE_MAP()


// CTools message handlers

float	ScaleRotToF(DWORD Value)
{
	float	p;
	p=((float)(PI))/100.0f;
	p*=(float)Value-50;
	return(p);
}

DWORD	ScaleRotToI(float Value)
{
	DWORD	p;

	Value*=(100.f/((float)PI));

	p=(DWORD)Value+50;
	return(p);
}


void CTools::OnLbnSelchangeDoflist()
{
	// TODO: Add your control notification handler code here
	/*DofSelected=DOFLabs[m_DofList.GetCurSel()].Number;
	m_RotSlider.SetPos(ScaleRotToI(DOFs[DofSelected].rotation));
	m_TransSlider.SetPos(ScaleRotToI(DOFs[DofSelected].translation));
*/


}

void CTools::OnNMCustomdrawDofrot(NMHDR *pNMHDR, LRESULT *pResult)
{
/*	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	if(DofSelected<0) return;
	DOFs[DofSelected].rotation=ScaleRotToF(m_RotSlider.GetPos());*/
}

void CTools::OnLbnSelchangeSwlist()
{
	/*// TODO: Add your control notification handler code here
	SWSelected=SWLabs[m_SWList.GetCurSel()].Number;

	DWORD k=SwitchValues[SWSelected];
	m_SWSlider.SetPos(k);
	CString s;
	s.Format("%d",k);
	m_SWText.SetWindowText(s);*/
}

void CTools::OnNMCustomdrawSwvalue(NMHDR *pNMHDR, LRESULT *pResult)
{
	/*LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	if(SWSelected<0) return;
	DWORD	k=m_SWSlider.GetPos();
	SwitchValues[SWSelected]=k;
	if(k){
		SWITCHes[SWSelected]=(1<<(k-1));
	} else {
		SWITCHes[SWSelected]=0;
	}
	CString s;
	s.Format("%d",k);
	m_SWText.SetWindowText(s);*/
}

void CTools::OnBnClickedSwonbutton()
{
	// TODO: Add your control notification handler code here
	/*int	a;
	for(a=0; a<128; a++) SWITCHes[a]=1;
	*/

}


void CTools::OnBnClickedSwoffbutton2()
{
	// TODO: Add your control notification handler code here
/*	int	a;
	for(a=0; a<128; a++) SWITCHes[a]=0;*/
}

void CTools::OnBnClickedDofzero()
{
	// TODO: Add your control notification handler code here
	/*ZeroMemory(DOFs, sizeof(DOFs));*/
}
