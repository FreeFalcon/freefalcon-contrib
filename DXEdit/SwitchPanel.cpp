// SwitchPanel.cpp : implementation file
//

#include "stdafx.h"
#include "LodConvert.h"
#include "SwitchPanel.h"
#include ".\switchpanel.h"


const	DOFItemType	SWLabs[MAX_SWITCHES]={
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
	{ "", 0}
};



// CSwitchPanel dialog

IMPLEMENT_DYNAMIC(CSwitchPanel, CDialog)
CSwitchPanel::CSwitchPanel(CWnd* pParent /*=NULL*/)
	: CDialog(CSwitchPanel::IDD, pParent)
{
	Create(IDD, pParent);
	ShowWindow(SW_HIDE);
	m_SWSlider.SetRange(0,8);
	m_node = NULL;

}

CSwitchPanel::~CSwitchPanel()
{
	m_node = NULL;
}

void CSwitchPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SWITCHLIST, m_SwitchList);

	DWORD a=0;
	for(a=0; a<MAX_SWITCHES; a++){
		CString Label;
		if(SWLabs[a].Name!="") Label.Format("%0.3d  -  %s", SWLabs[a].Number, SWLabs[a].Name);
		else Label.Format("%0.3d  - SWITCH NR %0.3d", a, a);
		m_SwitchList.AddString(Label);
	}
	
	DDX_Control(pDX, IDC_SW_ENABLE, m_EnableSW);
	DDX_Control(pDX, IDC_SW_SELECT, m_SWSlider);
}


BEGIN_MESSAGE_MAP(CSwitchPanel, CDialog)
	ON_LBN_SELCHANGE(IDC_SWITCHLIST, OnLbnSelchangeSwitchlist)
	ON_BN_CLICKED(IDC_SW_ENABLE, OnBnClickedSwEnable)
	ON_BN_CLICKED(IDC_SW_ALLON, OnBnClickedSwAllon)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SW_SELECT, OnNMCustomdrawSwSelect)
	ON_BN_CLICKED(IDC_SW_ALLOFF, OnBnClickedSwAlloff)
	ON_BN_CLICKED(IDC_SW_DISABLE, OnBnClickedSwDisable)
	ON_BN_CLICKED(IDC_SW_ASSIGN, OnBnClickedSwAssign)
END_MESSAGE_MAP()


// CSwitchPanel message handlers
void CSwitchPanel::ActivateWindow(DXDof *node, DWORD SwitchNr)
{
	if(!OSelected) return;
	m_node = node;
	ShowWindow(SW_SHOW);
	m_SwitchList.SetCurSel(SwitchNr);
}



void CSwitchPanel::OnBnClickedSwEnable()
{
	DWORD	Index=m_SwitchList.GetCurSel();
	OSelected->Obj->SetSwitch(Index,1);
}

void CSwitchPanel::OnBnClickedSwAllon()
{
	if(!OSelected) return;
	for(DWORD a=0; a<MAX_SWITCHES; a++) OSelected->Obj->SetSwitch(a,1);
}

void CSwitchPanel::OnNMCustomdrawSwSelect(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	*pResult = 0;
	DWORD	Index=m_SwitchList.GetCurSel();
	DWORD	Value=m_SWSlider.GetPos();
	OSelected->Obj->SetSwitch(Index,(Value)?1<<(Value-1):0);

}

void CSwitchPanel::OnBnClickedSwAlloff()
{
	if(!OSelected) return;
	for(DWORD a=0; a<MAX_SWITCHES; a++) OSelected->Obj->SetSwitch(a,0);
}

void CSwitchPanel::OnBnClickedSwDisable()
{
	DWORD	Index=m_SwitchList.GetCurSel();
	OSelected->Obj->SetSwitch(Index,0);
}

void CSwitchPanel::OnLbnSelchangeSwitchlist()
{
	// TODO: Add your control notification handler code here
	DWORD	Index=OSelected->Obj->SwitchValues[m_SwitchList.GetCurSel()];
	DWORD	Value=0;
	while(Index) { Value++; Index>>=1; }
	m_SWSlider.SetPos(Value);
}

void CSwitchPanel::OnBnClickedSwAssign()
{
	if(m_node){
		DWORD	Index=m_SwitchList.GetCurSel();
		m_node->dof.SwitchNumber = Index;
		m_node->dof.SwitchBranch = 0;
	}
}
