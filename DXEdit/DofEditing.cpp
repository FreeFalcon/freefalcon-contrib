// DofEditing.cpp : implementation file
//

#include "stdafx.h"
#include "Lodconvert.h"
#include ".\dofediting.h"
#include "InputBox.h"

const	CString	DofTypes[]={"DISABLED", "ROTATE", "XROTATE", "TRANSLATE", "SCALE", ""};
// CDofEditing dialog
static	CDofEditing	*ThisDof;

IMPLEMENT_DYNAMIC(CDofEditing, CDialog)
CDofEditing::CDofEditing(CWnd* pParent /*=NULL*/)
	: CDialog(CDofEditing::IDD, pParent)
{
	Create(IDD, pParent);
	ThisDof=this;
	m_DofSlider.SetRange(-90, 90, true);

	UDACCEL	Acc[7]={{0,1},{1,5},{2,10},{3,50},{4,100},{5,500},{6,1000}};
	m_XSpin.SetAccel(7, Acc);
	m_XSpin.SetRange32(-2000, 2000);
	m_YSpin.SetAccel(7, Acc);
	m_YSpin.SetRange32(-2000, 2000);
	m_ZSpin.SetAccel(7, Acc);
	m_ZSpin.SetRange32(2000, -2000);
	m_XScaleSpin.SetAccel(7, Acc);
	m_XScaleSpin.SetRange32(-2000, 2000);
	m_YScaleSpin.SetAccel(7, Acc);
	m_YScaleSpin.SetRange32(-2000, 2000);
	m_ZScaleSpin.SetAccel(7, Acc);
	m_ZScaleSpin.SetRange32(-2000, 2000);
	m_MaxSpin.SetAccel(7, Acc);
	m_MaxSpin.SetRange32(-2000, 2000);
	m_MinSpin.SetAccel(7, Acc);
	m_MinSpin.SetRange32(-2000, 2000);
	m_CxSpin.SetAccel(7, Acc);
	m_CxSpin.SetRange32(-2000, 2000);

	m_YawSpin.SetAccel(7, Acc);
	m_YawSpin.SetRange32(-2000, 2000);
	m_PitchSpin.SetAccel(7, Acc);
	m_PitchSpin.SetRange32(-2000, 2000);
	m_RollSpin.SetAccel(7, Acc);
	m_RollSpin.SetRange32(-2000, 2000);

	m_DofCopy.dofNumber=-1;
	m_DofRef.dofNumber=-1;
	m_bDofMove=m_DofRec=false;

}

CDofEditing::~CDofEditing()
{
}

void CDofEditing::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_YAWANGLE, m_YawAngle);
	DDX_Control(pDX, IDC_PITCHANGLE, m_PitchAngle);
	DDX_Control(pDX, IDC_ROLLANGLE, m_RollAngle);
	DDX_Control(pDX, IDC_DOFSLIDER, m_DofSlider);
	DDX_Control(pDX, IDC_DOFANGLE, m_DofAngle);
	DDX_Control(pDX, IDC_XPOSITION, m_XPos);
	DDX_Control(pDX, IDC_YPOSITION, m_YPos);
	DDX_Control(pDX, IDC_ZPOSITION, m_ZPos);
	DDX_Control(pDX, IDC_XSPIN, m_XSpin);
	DDX_Control(pDX, IDC_YSPIN, m_YSpin);
	DDX_Control(pDX, IDC_ZSPIN, m_ZSpin);
	DDX_Control(pDX, IDC_DOFTYPELIST, m_DofList);

	m_DofList.ResetContent();
	m_DofList.Clear();
	m_DofList.SetExtendedUI(true);
	m_DofList.SetTopIndex(0);
	for(DWORD a=0; DofTypes[a]!=""; a++){
		m_DofList.AddString(DofTypes[a]);
	}

	DDX_Control(pDX, IDC_XSCALE, m_XScale);
	DDX_Control(pDX, IDC_YSCALE, m_YScale);
	DDX_Control(pDX, IDC_ZSCALE, m_ZScale);
	DDX_Control(pDX, IDC_XSCALESPIN, m_XScaleSpin);
	DDX_Control(pDX, IDC_YSCALESPIN, m_YScaleSpin);
	DDX_Control(pDX, IDC_ZSCALESPIN, m_ZScaleSpin);
	DDX_Control(pDX, IDC_DOFMAX, m_DofMax);
	DDX_Control(pDX, IDC_DOFMIN, m_DofMin);
	DDX_Control(pDX, IDC_MAXSPIN, m_MaxSpin);
	DDX_Control(pDX, IDC_MINSPIN, m_MinSpin);
	DDX_Control(pDX, IDC_DOFINVCHECK, m_DofInvCheck);
	DDX_Control(pDX, IDC_DOFLIMITS, m_DofLimits);
	DDX_Control(pDX, IDC_DOFDEGREES, m_DofDegrees);
	DDX_Control(pDX, IDC_DOFSCALE, m_DofScale);
	DDX_Control(pDX, IDC_CX, m_DofCx);
	DDX_Control(pDX, IDC_CXSPIN, m_CxSpin);
	DDX_Control(pDX, IDC_DOFMOVE, m_DofMoveButton);
	DDX_Control(pDX, IDC_YAWSPIN, m_YawSpin);
	DDX_Control(pDX, IDC_PITCHSPIN, m_PitchSpin);
	DDX_Control(pDX, IDC_ROLLSPIN, m_RollSpin);
}


BEGIN_MESSAGE_MAP(CDofEditing, CDialog)
	ON_WM_SYSCOMMAND()	
	ON_BN_CLICKED(IDC_DOFRESET, OnBnClickedDofreset)
	ON_BN_CLICKED(IDC_DOFRESTORE, OnBnClickedDofrestore)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_DOFSLIDER, OnNMCustomdrawDofslider)
	ON_NOTIFY(UDN_DELTAPOS, IDC_XSPIN, OnDeltaposXspin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_YSPIN, OnDeltaposYspin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ZSPIN, OnDeltaposZspin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_XSCALESPIN, OnDeltaposXscalespin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_YSCALESPIN, OnDeltaposYscalespin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ZSCALESPIN, OnDeltaposZscalespin)
	ON_CBN_SELCHANGE(IDC_DOFTYPELIST, OnCbnSelchangeDoftypelist)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MAXSPIN, OnDeltaposMaxspin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MINSPIN, OnDeltaposMinspin)
	ON_BN_CLICKED(IDC_DOFINVCHECK, OnBnClickedDofinvcheck)
	ON_BN_CLICKED(IDC_DOFLIMITS, OnBnClickedDoflimits)
	ON_BN_CLICKED(IDC_DOFDEGREES, OnBnClickedDofdegrees)
	ON_BN_CLICKED(IDC_DOFSCALE, OnBnClickedDofscale)
	ON_NOTIFY(UDN_DELTAPOS, IDC_CXSPIN, OnDeltaposCxspin)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_COMMAND(ID_TOOLS_CHANGEDOFNUMBER, OnToolsChangedofnumber)
	ON_COMMAND(ID_TOOLS_COPYDOF, OnToolsCopydof)
	ON_COMMAND(ID_TOOLS_PASTEDOF, OnToolsPastedof)
	ON_BN_CLICKED(IDC_DOFMOVE, OnBnClickedDofmove)
	ON_NOTIFY(UDN_DELTAPOS, IDC_YAWSPIN, OnDeltaposYawspin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PITCHSPIN, OnDeltaposPitchspin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ROLLSPIN, OnDeltaposRollspin)
	ON_COMMAND(ID_TOOLS_RECORDCHANGES, OnToolsRecordchanges)
	ON_COMMAND(ID_TOOLS_APPLYCHANGES, OnToolsApplychanges)
END_MESSAGE_MAP()


void CDofEditing::UpdateAngles(void)
{
	CString	Angle;
	Angle.Format("%05.2f", D3DXToDegree(m_Yaw));
	m_YawAngle.SetWindowText(Angle);
	Angle.Format("%05.2f", D3DXToDegree(m_Pitch));
	m_PitchAngle.SetWindowText(Angle);
	Angle.Format("%05.2f", D3DXToDegree(m_Roll));
	m_RollAngle.SetWindowText(Angle);
	D3DXMatrixRotationYawPitchRoll(&m_Dof->dof.rotation, m_Yaw, m_Pitch, m_Roll);

	Angle.Format("%7.3f", m_Dof->dof.translation.x);
	m_XPos.SetWindowText(Angle);
	Angle.Format("%7.3f", m_Dof->dof.translation.y);
	m_YPos.SetWindowText(Angle);
	Angle.Format("%7.3f", m_Dof->dof.translation.z);
	m_ZPos.SetWindowText(Angle);

	Angle.Format("%7.3f", m_Dof->dof.scale.x);
	m_XScale.SetWindowText(Angle);
	Angle.Format("%7.3f", m_Dof->dof.scale.y);
	m_YScale.SetWindowText(Angle);
	Angle.Format("%7.3f", m_Dof->dof.scale.z);
	m_ZScale.SetWindowText(Angle);

	Angle.Format("%7.3f", m_Dof->dof.multiplier);
	m_DofCx.SetWindowText(Angle);


	m_DofInvCheck.SetCheck(m_Dof->dof.flags&XDOF_NEGATE);
	m_DofLimits.SetCheck(m_Dof->dof.flags&XDOF_MINMAX);
	m_DofDegrees.SetCheck(m_Dof->dof.flags&XDOF_ISDOF);
	m_DofScale.SetCheck(m_Dof->dof.flags&XDOF_SUBRANGE);

	Angle.Format("%7.3f", (m_Dof->dof.flags&XDOF_ISDOF)?D3DXToDegree(m_Dof->dof.max):m_Dof->dof.max);
	m_DofMax.SetWindowText(Angle);
	Angle.Format("%7.3f", (m_Dof->dof.flags&XDOF_ISDOF)?D3DXToDegree(m_Dof->dof.min):m_Dof->dof.min);
	m_DofMin.SetWindowText(Angle);

	CString Caption;
	Caption.Format("DOF Nr : %03d", m_Dof->dof.dofNumber);
	SetWindowText(Caption);

	m_DofList.SetCurSel(m_Dof->dof.Type);

}

void CDofEditing::GetOriginalAngles(void)
{
	m_Yaw=asin(m_OriginalDof.rotation.m20);
	m_Pitch=-atan2(m_OriginalDof.rotation.m21, m_OriginalDof.rotation.m22);
	m_Roll=-atan2(m_OriginalDof.rotation.m10, m_OriginalDof.rotation.m00);
}

void CDofEditing::EditDof(E_Object *Object, DXDof *Dof)
{
	m_Dof=Dof;
	m_OriginalDof=Dof->dof;
	m_Object=Object;
	if(!m_Object) {
		ShowWindow(SW_HIDE);
		return;
	}
	m_DofList.SetCurSel(2);
	m_DofSlider.SetPos((int)D3DXToDegree((float)m_Object->Obj->DOFValues[m_Dof->dof.dofNumber].rotation));
	ShowWindow(SW_SHOW);

	GetOriginalAngles();
	UpdateAngles();
}


// CDofEditing message handlers

void CDofEditing::OnBnClickedDofreset()
{
	m_Yaw=m_Pitch=m_Roll=0.0f;
	m_Dof->dof.translation.x=m_Dof->dof.translation.y=m_Dof->dof.translation.z=0.0f;
	m_Dof->dof.scale.x=m_Dof->dof.scale.y=m_Dof->dof.scale.z=0.0f;
	UpdateAngles();
}

void CDofEditing::OnBnClickedDofrestore()
{
	m_Dof->dof=m_OriginalDof;
	UpdateAngles();
}

void CDofEditing::OnBnClickedYaw5m()
{
	m_Yaw-=D3DXToRadian(5);
	UpdateAngles();
}

void CDofEditing::OnBnClickedYaw5p()
{
	m_Yaw+=D3DXToRadian(5);
	UpdateAngles();
}

void CDofEditing::OnBnClickedPitch5m()
{
	m_Pitch-=D3DXToRadian(5);
	UpdateAngles();
}

void CDofEditing::OnBnClickedPitch5p()
{
	m_Pitch+=D3DXToRadian(5);
	UpdateAngles();
}

void CDofEditing::OnBnClickedRoll5m()
{
	m_Roll-=D3DXToRadian(5);
	UpdateAngles();
}

void CDofEditing::OnBnClickedRoll5p()
{
	m_Roll+=D3DXToRadian(5);
	UpdateAngles();
}

void CDofEditing::OnNMCustomdrawDofslider(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	float Angle=(float)m_DofSlider.GetPos();
	m_Object->Obj->DOFValues[m_Dof->dof.dofNumber].rotation=D3DXToRadian(Angle);
	CString DofAngle;
	DofAngle.Format("%3.0f", Angle);
	m_DofAngle.SetWindowText(DofAngle);

}


void CDofEditing::OnDeltaposXspin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	signed int XMove=(signed short)(m_XSpin.GetPos()&0xffff);
	m_XSpin.SetPos(0);
	m_Dof->dof.translation.x+=((float)XMove)/1000;
	UpdateAngles();
}

void CDofEditing::OnDeltaposYspin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	signed int YMove=(signed short)(m_YSpin.GetPos()&0xffff);
	m_YSpin.SetPos(0);
	m_Dof->dof.translation.y+=((float)YMove)/1000;
	UpdateAngles();
}

void CDofEditing::OnDeltaposZspin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	signed int ZMove=(signed short)(m_ZSpin.GetPos()&0xffff);
	m_ZSpin.SetPos(0);
	m_Dof->dof.translation.z+=((float)ZMove)/1000;
	UpdateAngles();
}

void CDofEditing::OnDeltaposXscalespin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	signed int XScale=(signed short)(m_XScaleSpin.GetPos()&0xffff);
	m_XScaleSpin.SetPos(0);
	m_Dof->dof.scale.x+=((float)XScale)/1000;
	UpdateAngles();
}

void CDofEditing::OnDeltaposYscalespin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	signed int YScale=(signed short)(m_YScaleSpin.GetPos()&0xffff);
	m_YScaleSpin.SetPos(0);
	m_Dof->dof.scale.y+=((float)YScale)/1000;
	UpdateAngles();
}

void CDofEditing::OnDeltaposZscalespin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	signed int ZScale=(signed short)(m_ZScaleSpin.GetPos()&0xffff);
	m_ZScaleSpin.SetPos(0);
	m_Dof->dof.scale.z+=((float)ZScale)/1000;
	UpdateAngles();

}

void CDofEditing::OnCbnSelchangeDoftypelist()
{
	// TODO: Add your control notification handler code here
	m_Dof->dof.Type=(DofType)m_DofList.GetCurSel();
}

void CDofEditing::OnDeltaposMaxspin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	signed int Max=(signed short)(m_MaxSpin.GetPos()&0xffff);
	m_MaxSpin.SetPos(0);
	m_Dof->dof.max+=((float)Max)/1000;
	UpdateAngles();
}

void CDofEditing::OnDeltaposMinspin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	signed int Min=(signed short)(m_MinSpin.GetPos()&0xffff);
	m_MinSpin.SetPos(0);
	m_Dof->dof.min+=((float)Min)/1000;
	UpdateAngles();
}

void CDofEditing::OnBnClickedDofinvcheck()
{
	// TODO: Add your control notification handler code here
	m_Dof->dof.flags|=(m_DofInvCheck.GetCheck())?XDOF_NEGATE:0;
	m_Dof->dof.flags&=(!m_DofInvCheck.GetCheck())?~XDOF_NEGATE:0xffffffff;
	UpdateAngles();
}

void CDofEditing::OnBnClickedDoflimits()
{
	m_Dof->dof.flags|=(m_DofLimits.GetCheck())?XDOF_MINMAX:0;
	m_Dof->dof.flags&=(!m_DofLimits.GetCheck())?~XDOF_MINMAX:0xffffffff;
	UpdateAngles();
}

void CDofEditing::OnBnClickedDofdegrees()
{
	m_Dof->dof.flags|=(m_DofDegrees.GetCheck())?XDOF_ISDOF:0;
	m_Dof->dof.flags&=(!m_DofDegrees.GetCheck())?~XDOF_ISDOF:0xffffffff;
	UpdateAngles();

}

void CDofEditing::OnBnClickedDofscale()
{
	m_Dof->dof.flags|=(m_DofScale.GetCheck())?XDOF_SUBRANGE:0;
	m_Dof->dof.flags&=(!m_DofScale.GetCheck())?~XDOF_SUBRANGE:0xffffffff;
	UpdateAngles();
}

void CDofEditing::OnDeltaposCxspin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	signed int Cx=(signed short)(m_CxSpin.GetPos()&0xffff);
	m_CxSpin.SetPos(0);
	m_Dof->dof.multiplier+=((float)Cx)/1000;
	UpdateAngles();
}

void CDofEditing::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	m_Dof->dof=m_OriginalDof;
	OnCancel();
}

void CDofEditing::OnToolsChangedofnumber()
{
	// TODO: Add your command handler code here
	CString	NewDof;
	CString Mex="Input the new DOF Number";
	CInputBox	*Box;
	Box=new CInputBox(this, &NewDof, &Mex );
	int Result=Box->DoModal();
	delete Box;
	if(Result==IDOK){
		m_Dof->dof.dofNumber=atoi(NewDof);
		TheTreeViews->UpdateItemLabel(m_Dof->TreeItem);
	}
	UpdateAngles();

}

void CDofEditing::OnToolsCopydof()
{
	// TODO: Add your command handler code here
	m_DofCopy=m_Dof->dof;
}

void CDofEditing::OnToolsPastedof()
{
	// TODO: Add your command handler code here
	if(m_DofCopy.dofNumber!=-1){
		DWORD	DofNr=m_Dof->dof.dofNumber;
		m_Dof->dof=m_DofCopy;
		m_Dof->dof.dofNumber=DofNr;
	}
	UpdateAngles();
}

void DofMoveCallBack(D3DXVECTOR3 Transform, D3DXVECTOR3 Move, bool Remove)
{
	ThisDof->DofMoveFunction(Transform, Move, Remove);
}

void CDofEditing::OnBnClickedDofmove()
{
	// TODO: Add your control notification handler code here
	m_bDofMove^=true;
	m_DofMoveButton.SetCheck(m_bDofMove);
	if(!m_bDofMove)
		MoveFunction=NULL;
	else {
		D3DXVECTOR3	Null(0,0,0);
		if(MoveFunction) MoveFunction(Null, Null, true);
		MoveFunction=DofMoveCallBack;
	}
	
}

void CDofEditing::DofMoveFunction(D3DXVECTOR3 Transform, D3DXVECTOR3 Move, bool Remove)
{
	if(Remove){
		m_bDofMove=false;
		m_DofMoveButton.SetCheck(m_bDofMove);
		MoveFunction=NULL;
		return;
	}
	
	m_Dof->dof.translation.x+=Move.x;
	m_Dof->dof.translation.y+=Move.y;
	m_Dof->dof.translation.z+=Move.z;

	m_Yaw+=Transform.x;
	m_Pitch+=Transform.y;
	m_Roll+=Transform.z;

	UpdateAngles();

}


void CDofEditing::OnSysCommand( UINT nID, LPARAM lParam )
{
	if(nID==SC_CLOSE){
		if(m_bDofMove){
			MoveFunction=NULL;
			m_bDofMove=false;
			m_DofMoveButton.SetCheck(m_bDofMove);
		}
	}
	CWnd::OnSysCommand(nID, lParam);
}

void CDofEditing::OnDeltaposYawspin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	signed int Cx=(signed short)(m_YawSpin.GetPos()&0xffff);
	m_YawSpin.SetPos(0);
	m_Yaw+=(D3DXToRadian((float)Cx)/100);
	UpdateAngles();
}

void CDofEditing::OnDeltaposPitchspin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	signed int Cx=(signed short)(m_PitchSpin.GetPos()&0xffff);
	m_PitchSpin.SetPos(0);
	m_Pitch+=(D3DXToRadian((float)Cx)/100);
	UpdateAngles();
}

void CDofEditing::OnDeltaposRollspin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	signed int Cx=(signed short)(m_RollSpin.GetPos()&0xffff);
	m_RollSpin.SetPos(0);
	m_Roll+=(D3DXToRadian((float)Cx)/100);
	UpdateAngles();

}

void CDofEditing::OnToolsRecordchanges()
{
	m_DofRec^=true;

	CMenu	*Menu=this->GetMenu();
	Menu->CheckMenuItem(ID_TOOLS_RECORDCHANGES, (m_DofRec)?MF_CHECKED:MF_UNCHECKED);
	if(m_DofRec){
		m_DofRef=m_Dof->dof;
	} else {
		DxDofType NewDof=m_Dof->dof;
		NewDof.translation.x=m_Dof->dof.translation.x-m_DofRef.translation.x;
		NewDof.translation.y=m_Dof->dof.translation.y-m_DofRef.translation.y;
		NewDof.translation.z=m_Dof->dof.translation.z-m_DofRef.translation.z;
		NewDof.scale.x=m_Dof->dof.scale.x-m_DofRef.scale.x;
		NewDof.scale.y=m_Dof->dof.scale.y-m_DofRef.scale.y;
		NewDof.scale.z=m_Dof->dof.scale.z-m_DofRef.scale.z;
		for(DWORD a=0; a<4; a++){
			for(DWORD b=0; b<4; b++){
				NewDof.rotation.m[a][b]=m_Dof->dof.rotation.m[a][b]-m_DofRef.rotation.m[a][b];
			}
		}
		NewDof.max=m_Dof->dof.max-m_DofRef.max;
		NewDof.min=m_Dof->dof.min-m_DofRef.min;
		NewDof.multiplier=m_Dof->dof.multiplier-m_DofRef.multiplier;
		NewDof.max=m_Dof->dof.max-m_DofRef.max;
		m_DofRef=NewDof;
	}
}

void CDofEditing::OnToolsApplychanges()
{
	m_Dof->dof.flags=m_DofRef.flags;
	m_Dof->dof.translation.x+=m_DofRef.translation.x;
	m_Dof->dof.translation.y+=m_DofRef.translation.y;
	m_Dof->dof.translation.z+=m_DofRef.translation.z;
	m_Dof->dof.scale.x+=m_DofRef.scale.x;
	m_Dof->dof.scale.y+=m_DofRef.scale.y;
	m_Dof->dof.scale.z+=m_DofRef.scale.z;
	for(DWORD a=0; a<4; a++){
		for(DWORD b=0; b<4; b++){
			m_Dof->dof.rotation.m[a][b]+=m_DofRef.rotation.m[a][b];
		}
	}
	m_Dof->dof.max+=m_DofRef.max;
	m_Dof->dof.min+=m_DofRef.min;
	m_Dof->dof.multiplier+=m_DofRef.multiplier;
	m_Dof->dof.max+=m_DofRef.max;
	UpdateAngles();
}
