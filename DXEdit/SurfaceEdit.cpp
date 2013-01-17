// SurfaceEdit.cpp : implementation file
//

#include "stdafx.h"
#include "LodConvert.h"
#include "SurfaceEdit.h"
#include ".\surfaceedit.h"
#include "DXEditing.h"



const	DWORD	SpecularColors[]=SURFACE_SPECULARCOLORS;
		

CSurfaceEdit	*ThisSurface;
// CSurfaceEdit dialog

IMPLEMENT_DYNAMIC(CSurfaceEdit, CDialog)
CSurfaceEdit::CSurfaceEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CSurfaceEdit::IDD, pParent)
{
	Create(IDD, pParent);
	ShowWindow(SW_HIDE);
	ThisSurface=this;

}

CSurfaceEdit::~CSurfaceEdit()
{
}

void CSurfaceEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_S_ALPHACKECK, m_AlphaCheck);
	DDX_Control(pDX, IDC_S_LITEDCHECK, m_LightedCheck);
	DDX_Control(pDX, IDC_S_VCOLORCHECK, m_VColorCheck);
	DDX_Control(pDX, IDC_S_TEXTURECHECK, m_TexturedCheck);
	DDX_Control(pDX, IDC_S_GOURAUDCHECK, m_GouraudCheck);
	DDX_Control(pDX, IDC_S_BBOARDCHECK, m_BillBoardCheck);
	DDX_Control(pDX, IDC_S_ZBIASSLIDER, m_ZBiasSlider);
	DDX_Control(pDX, IDC_S_SINDEXSLIDER, m_SIndexSlider);



	DDX_Control(pDX, IDC_S_CKEYCHECK, m_CKeyCheck);
	DDX_Control(pDX, IDC_SELECTIONMOVE, m_SurfaceMoveButton);
	DDX_Control(pDX, IDC_S_ZBIASCHECK, m_ZBiasCheck);
	DDX_Control(pDX, IDC_ZMAX, m_ZMaxCheck);
	DDX_Control(pDX, IDC_ZMIN, m_ZMinCheck);
	DDX_Control(pDX, IDC_SIMAX, m_SIMaxCheck);
	DDX_Control(pDX, IDC_SIMIN, m_SIMinCheck);
	DDX_Control(pDX, IDC_ZBIASVALUE, m_ZBiasValue);
	DDX_Control(pDX, IDC_S_XMOVE, m_XMMove);
	DDX_Control(pDX, IDC_S_YMOVE, m_YMMove);
	DDX_Control(pDX, IDC_S_ZMOVE, m_ZMMove);
	DDX_Control(pDX, IDC_S_RELMOVE, m_MRelCheck);
	DDX_Control(pDX, IDC_S_SWEMITCHECK, m_SwEmitCheck);
	DDX_Control(pDX, IDC_S_SWNR, m_SwNrText);
	DDX_Control(pDX, IDC_NORMALSSLIDER, m_NormalSlider);
	DDX_Control(pDX, IDC_NORMVALUE, m_NormValue);

	m_ZBiasSlider.SetRange( 0, MAX_ZBIAS, true );
	m_SIndexSlider.SetRange( 0, MAX_SINDEX, true );
	m_NormalSlider.SetRange(0, 99, true);
	m_NormalSlider.SetPos(9);
	DDX_Control(pDX, IDC_S_SWMASK, m_SWMaskText);
	DDX_Control(pDX, IDC_S_PFACES, m_PFaces);
	DDX_Control(pDX, IDC_S_PRADIUS, m_PRadius);
	DDX_Control(pDX, IDC_MIRROCHECK, m_MirrorCheck);
	DDX_Control(pDX, IDC_SCALEZVALUE, m_ScaleZ);
	DDX_Control(pDX, IDC_SCALEYVALUE, m_ScaleY);
	DDX_Control(pDX, IDC_SCALEXVALUE, m_ScaleX);
}


BEGIN_MESSAGE_MAP(CSurfaceEdit, CDialog)
	ON_BN_CLICKED(IDC_S_VCOLORCHECK, OnBnClickedSVcolorcheck)
	ON_BN_CLICKED(IDC_S_ALPHACKECK, OnBnClickedSAlphackeck)
	ON_BN_CLICKED(IDC_S_LITEDCHECK, OnBnClickedSLitedcheck)
	ON_BN_CLICKED(IDC_S_TEXTURECHECK, OnBnClickedSTexturecheck)
	ON_BN_CLICKED(IDC_S_GOURAUDCHECK, OnBnClickedSGouraudcheck)
	ON_BN_CLICKED(IDC_S_BBOARDCHECK, OnBnClickedSBboardcheck)
	ON_BN_CLICKED(IDC_FACEMIRROR, OnBnClickedFacemirror)
	ON_BN_CLICKED(IDC_BACKFACE, OnBnClickedBackface)
	ON_BN_CLICKED(IDC_S_CKEYCHECK, OnBnClickedSCkeycheck)
	ON_BN_CLICKED(IDC_SELECTIONMOVE, OnBnClickedSelectionmove)
	ON_BN_CLICKED(IDC_S_ZBIASCHECK, OnBnClickedSZbiascheck)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_S_SINDEXSLIDER, OnNMReleasedcaptureSSindexslider)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_S_ZBIASSLIDER, OnNMReleasedcaptureSZbiasslider)
	ON_BN_CLICKED(IDC_ZMAX, OnBnClickedZmax)
	ON_BN_CLICKED(IDC_ZMIN, OnBnClickedZmin)
	ON_BN_CLICKED(IDC_SIMAX, OnBnClickedSimax)
	ON_BN_CLICKED(IDC_SIMIN, OnBnClickedSimin)
	ON_BN_CLICKED(IDC_MANMOVE, OnBnClickedManmove)
	ON_BN_CLICKED(IDC_MANSNAP, OnBnClickedMansnap)
	ON_BN_CLICKED(IDC_MANSNAP2, OnBnClickedMansnap2)
	ON_BN_CLICKED(IDC_GROUNDZERO, OnBnClickedGroundzero)
	ON_BN_CLICKED(IDC_S_SWEMITCHECK, OnBnClickedSSwemitcheck)
	ON_EN_CHANGE(IDC_S_SWNR, OnEnChangeSSwnr)
	ON_BN_CLICKED(IDC_SPECCOLOR, OnBnClickedSpeccolor)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_NORMALSSLIDER, OnNMCustomdrawNormalsslider)
	ON_BN_CLICKED(IDC_MNOSMALBUTTON, OnBnClickedMnosmalbutton)
	ON_BN_CLICKED(IDC_SNORMALBUTTON, OnBnClickedSnormalbutton)
	ON_EN_CHANGE(IDC_S_SWMASK, OnEnChangeSSwmask)
	ON_BN_CLICKED(IDC_PCIRCLE, OnBnClickedPcircle)
	ON_BN_CLICKED(IDC_MIRROX, OnBnClickedMirrox)
	ON_BN_CLICKED(IDC_MIRROY, OnBnClickedMirroy)
	ON_BN_CLICKED(IDC_MIRROZ, OnBnClickedMirroz)
	ON_BN_CLICKED(IDC_PLANIZE, OnBnClickedPlanize)
	ON_BN_CLICKED(IDC_SCALEXUP, OnBnClickedScalexup)
	ON_BN_CLICKED(IDC_SCALEXDN, OnBnClickedScalexdn)
	ON_BN_CLICKED(IDC_SCALEYUP, OnBnClickedScaleyup)
	ON_BN_CLICKED(IDC_SCALEYDN, OnBnClickedScaleydn)
	ON_BN_CLICKED(IDC_SCALEZUP, OnBnClickedScalezup)
	ON_BN_CLICKED(IDC_SCALEZDN, OnBnClickedScalezdn)
END_MESSAGE_MAP()


void CSurfaceEdit::EditSurface(void **Object, DxSurfaceType **Surface,  CSelectList *SelectList)
{
	m_Surface=Surface;
	m_SurfaceOwner=Object;
	m_SelectList=SelectList;
	ShowWindow(SW_SHOWNA);
}


#define	CHECK(x)		((Flags_1.b.x)?1:(Flags_2.b.x)?2:0)
DWORD	TransformZBiasPos(DWORD Value) { return (MAX_ZBIAS-Value); }
DWORD	TransformSIPos(float Value) { return (DWORD)(MAX_SINDEX-(DWORD)((Value-BASE_SI_INDEX) / SI_INDEX_CX)); }

void CSurfaceEdit::UpdateEditValues(CSelectList *SelectList, DXNode *Node, DWORD VertexNr)
{
//	char	Text[128];
	if(SelectList) m_SelectList=SelectList;
	if(Node) m_NSelected=(DXTriangle*)Node;
	if(VertexNr>=0) m_VSelected=VertexNr;

	if(!m_SelectList) return;
	if(!m_NSelected) return;	

	Updating=true;
	// Check for selected Flags
	DXFlagsType	Flags_1, Flags_2;
	m_SelectList->GetSelectedFlags(&Flags_1, &Flags_2);

	m_ZBiasCheck.SetCheck(CHECK(zBias));
	m_CKeyCheck.SetCheck(CHECK(ChromaKey));
	m_AlphaCheck.SetCheck(CHECK(Alpha));
	m_LightedCheck.SetCheck(CHECK(Lite));
	m_VColorCheck.SetCheck(CHECK(VColor));
	m_TexturedCheck.SetCheck(CHECK(Texture));
	m_GouraudCheck.SetCheck(CHECK(Gouraud));
	m_BillBoardCheck.SetCheck(CHECK(BillBoard));
	m_SwEmitCheck.SetCheck(CHECK(SwEmissive));

	{
		DWORD	Min, Max;
		m_SelectList->GetZBiasValues( &Min, &Max);
		if(m_ZMaxCheck.GetCheck()) m_ZBiasSlider.SetPos(TransformZBiasPos(Max));
		else if(m_ZMinCheck.GetCheck()) m_ZBiasSlider.SetPos(TransformZBiasPos(Min));
			else m_ZBiasSlider.SetPos(TransformZBiasPos(Min+(Max-Min)/2));
	}
	{
		float	Min, Max;
		m_SelectList->GetSIValues( &Min, &Max);
		if(m_SIMaxCheck.GetCheck()) m_SIndexSlider.SetPos(TransformSIPos(Max));
		else if(m_SIMinCheck.GetCheck()) m_SIndexSlider.SetPos(TransformSIPos(Min));
			else m_SIndexSlider.SetPos(TransformSIPos(Min+(Max-Min)/2));
	}

	CString Value;
	Value.Format("%2d", TransformZBiasPos(m_ZBiasSlider.GetPos()));
	m_ZBiasValue.SetWindowText(Value);


	Value.Format("%3d", m_NSelected->Surface.SwitchNumber);
	m_SwNrText.SetWindowText(Value);

	Value.Format("%8x", m_NSelected->Surface.SwitchMask);
	m_SWMaskText.SetWindowText(Value);

	Value.Format("%3.2f", GetNormalLevel());
	m_NormValue.SetWindowText(Value);

	Updating=false;

}



void CSurfaceEdit::UpdateFlags(DXFlagsType Flags, DXFlagsType Mask)
{
	E_UpdateFlags(m_SelectList, Flags, Mask);
}

// macro to be used with surface flags
#define	UPDATEFLAGS(x,y)	{\
							DXFlagsType	Flags,Mask;\
							Flags.w=Mask.w=0;\
							if(y.GetCheck()>1) y.SetCheck(0);\
							Flags.b.x=y.GetCheck()?true:false;\
							Mask.b.x=1;\
							UpdateFlags(Flags, Mask);\
						}\

void CSurfaceEdit::OnBnClickedSVcolorcheck()
{
	UPDATEFLAGS(VColor, m_VColorCheck);
}

void CSurfaceEdit::OnBnClickedSAlphackeck()
{
	UPDATEFLAGS(Alpha, m_AlphaCheck);
}

void CSurfaceEdit::OnBnClickedSLitedcheck()
{
	UPDATEFLAGS(Lite, m_LightedCheck);
}

void CSurfaceEdit::OnBnClickedSTexturecheck()
{
	UPDATEFLAGS(Texture, m_TexturedCheck);
}

void CSurfaceEdit::OnBnClickedSGouraudcheck()
{
	UPDATEFLAGS(Gouraud, m_GouraudCheck);
}

void CSurfaceEdit::OnBnClickedSBboardcheck()
{
	UPDATEFLAGS(BillBoard, m_BillBoardCheck);
}

void CSurfaceEdit::OnBnClickedFacemirror()
{
	// TODO: Add your control notification handler code here
	E_MirrorSelection(m_SelectList);
}

void CSurfaceEdit::OnBnClickedBackface()
{
	// TODO: Add your control notification handler code here
	E_BackFaceSelection(m_SelectList);
}

void CSurfaceEdit::OnBnClickedSCkeycheck()
{
	UPDATEFLAGS(ChromaKey, m_CKeyCheck);
}


void SurfaceMoveCallBack(D3DXVECTOR3 Transform, D3DXVECTOR3 Move, bool Remove)
{
	ThisSurface->SurfaceMoveFunction(Transform, Move, Remove);
}

void CSurfaceEdit::OnBnClickedSelectionmove()
{
	// TODO: Add your control notification handler code here
	m_bSurfaceMove^=true;
	m_SurfaceMoveButton.SetCheck(m_bSurfaceMove);
	if(!m_bSurfaceMove)
		MoveFunction=NULL;
	else {
		D3DXVECTOR3	Null(0,0,0);
		if(MoveFunction) MoveFunction(Null, Null, true);
		MoveFunction=SurfaceMoveCallBack;
	}
}

void CSurfaceEdit::SurfaceMoveFunction(D3DXVECTOR3 Transform, D3DXVECTOR3 Move, bool Remove)
{
	if(Remove){
		m_bSurfaceMove=false;
		m_SurfaceMoveButton.SetCheck(m_bSurfaceMove);
		MoveFunction=NULL;
		return;
	}
	
	D3DXVECTOR3 Center;

	Center.x=(m_SelectList->m_MaxX-m_SelectList->m_MinX)/2+m_SelectList->m_MinX;
	Center.y=(m_SelectList->m_MaxY-m_SelectList->m_MinY)/2+m_SelectList->m_MinY;
	Center.z=(m_SelectList->m_MaxZ-m_SelectList->m_MinZ)/2+m_SelectList->m_MinZ;
	if(PlaneMode==PLANE_X) {Transform.y=0; Transform.z=0;}
	if(PlaneMode==PLANE_Y) {Transform.x=0; Transform.z=0;}
	if(PlaneMode==PLANE_Z) {Transform.y=0; Transform.x=0;}


	E_TranslateSelection(m_SelectList, Move.x, Move.y, Move.z);
	E_RotateSelection(m_SelectList, Transform.x,Transform.y,Transform.z, Center);
}

void CSurfaceEdit::OnBnClickedSZbiascheck()
{
	UPDATEFLAGS(zBias, m_ZBiasCheck);
}

void CSurfaceEdit::OnNMReleasedcaptureSSindexslider(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	DWORD	SIndex=MAX_SINDEX - m_SIndexSlider.GetPos();
	float	fIndex=(float)SIndex * SI_INDEX_CX + BASE_SI_INDEX;
	if(m_SIMinCheck.GetCheck()) E_SetMinMaxSI(m_SelectList, &fIndex, NULL, NULL);
	else if(m_SIMaxCheck.GetCheck()) E_SetMinMaxSI(m_SelectList, NULL, &fIndex, NULL);
		else E_SetMinMaxSI(m_SelectList, NULL, NULL, &fIndex);

	E_SelectCommonSurface(m_SelectList, NULL, NULL, NULL, NULL, NULL, NULL, NULL, (DWORD*)&SpecularColors[SIndex]);
	UpdateEditValues();
}

void CSurfaceEdit::OnNMReleasedcaptureSZbiasslider(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	DWORD	ZBias=MAX_ZBIAS - m_ZBiasSlider.GetPos();
	if(m_ZMinCheck.GetCheck()) E_SetMinMaxZB(m_SelectList, &ZBias, NULL, NULL);
	else if(m_ZMaxCheck.GetCheck()) E_SetMinMaxZB(m_SelectList, NULL, &ZBias, NULL);
		else E_SetMinMaxZB(m_SelectList, NULL, NULL, &ZBias);
	UpdateEditValues();
}



void CSurfaceEdit::OnBnClickedZmax()
{
	bool	Check=m_ZMaxCheck.GetCheck()?true:false;
	if(Check) m_ZMinCheck.SetCheck(false);
	UpdateEditValues(m_SelectList);
}

void CSurfaceEdit::OnBnClickedZmin()
{
	bool	Check=m_ZMinCheck.GetCheck()?true:false;
	if(Check) m_ZMaxCheck.SetCheck(false);
	UpdateEditValues(m_SelectList);
}

void CSurfaceEdit::OnBnClickedSimax()
{
	bool	Check=m_SIMaxCheck.GetCheck()?true:false;
	if(Check) m_SIMinCheck.SetCheck(false);
	UpdateEditValues(m_SelectList);
}

void CSurfaceEdit::OnBnClickedSimin()
{
	bool	Check=m_SIMinCheck.GetCheck()?true:false;
	if(Check) m_SIMaxCheck.SetCheck(false);
	UpdateEditValues(m_SelectList);
}

void CSurfaceEdit::OnBnClickedManmove()
{
	float	xm, ym, zm;
	
	CString	Text;
	m_XMMove.GetWindowText(Text);
	xm=(float)atof(Text);
	m_YMMove.GetWindowText(Text);
	ym=(float)atof(Text);
	m_ZMMove.GetWindowText(Text);
	zm=(float)atof(Text);

	if(!m_MRelCheck.GetCheck()){
		xm-=m_NSelected->Vertex[m_VSelected].vx;
		ym-=m_NSelected->Vertex[m_VSelected].vy;
		zm-=m_NSelected->Vertex[m_VSelected].vz;
	}

	E_TranslateSelection(m_SelectList, xm, ym, zm);


}

void CSurfaceEdit::OnBnClickedMansnap()
{
	float	xm, ym, zm;


	xm=-fmod(m_NSelected->Vertex[m_VSelected].vx, GridSize);
	ym=-fmod(m_NSelected->Vertex[m_VSelected].vy, GridSize);
	zm=-fmod(m_NSelected->Vertex[m_VSelected].vz, GridSize);
	E_TranslateSelection(m_SelectList, (PlaneMode==PLANE_X)?0:xm, (PlaneMode==PLANE_Y)?0:ym, (PlaneMode==PLANE_Z)?0:zm);
}

void CSurfaceEdit::OnBnClickedMansnap2()
{
	float	xm, ym, zm;
	
	xm=m_SelectList->m_MaxX-(m_SelectList->m_MaxX-m_SelectList->m_MinX)/2;
	ym=m_SelectList->m_MaxY-(m_SelectList->m_MaxY-m_SelectList->m_MinY)/2;
	zm=m_SelectList->m_MaxZ-(m_SelectList->m_MaxZ-m_SelectList->m_MinZ)/2;
	xm=-fmod(xm, GridSize);;
	ym=-fmod(ym, GridSize);;
	zm=-fmod(zm, GridSize);;
	E_TranslateSelection(m_SelectList, (PlaneMode==PLANE_X)?0:xm, (PlaneMode==PLANE_Y)?0:ym, (PlaneMode==PLANE_Z)?0:zm);


}

void CSurfaceEdit::OnBnClickedGroundzero()
{
	// remember...on models bigger Z is lower
	float zm=-m_SelectList->m_MaxZ;
	E_TranslateSelection(m_SelectList, 0, 0, zm);
}


void CSurfaceEdit::OnBnClickedSSwemitcheck()
{
	UPDATEFLAGS(SwEmissive, m_SwEmitCheck);
}

void CSurfaceEdit::OnEnChangeSSwnr()
{
	if(Updating) return;
	CString Value;
	m_SwNrText.GetWindowText(Value);
	DWORD SwNr=atoi(Value);
	E_SelectCommonSurface(m_SelectList, NULL, NULL, NULL, NULL, NULL, &SwNr);
}

void CSurfaceEdit::OnBnClickedSpeccolor()
{
	DWORD	Original=0xff202020;
	if(m_NSelected)	Original=m_NSelected->Surface.DefaultSpecularity;
	Original=GetColour(Original);
	E_SelectCommonSurface(m_SelectList, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &Original);

}

void CSurfaceEdit::OnNMCustomdrawNormalsslider(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	CString Value;
	Value.Format("%3.2f", GetNormalLevel());
	m_NormValue.SetWindowText(Value);
}

void CSurfaceEdit::OnBnClickedMnosmalbutton()
{
	E_CalculateModelVNormals(OSelected, GetNormalLevel());
}

void CSurfaceEdit::OnBnClickedSnormalbutton()
{
	E_CalculateModelVNormals(OSelected, GetNormalLevel(), true, m_SelectList);
}

void CSurfaceEdit::OnEnChangeSSwmask()
{
	if(Updating) return;
	CString Value;
	m_SWMaskText.GetWindowText(Value);
	DWORD SwMask=atoi(Value);
	E_SelectCommonSurface(m_SelectList, NULL, NULL, NULL, NULL, NULL, NULL, &SwMask);
}


void AssingSurfaceItems(DxSurfaceType *Surface)
{
}

void CSurfaceEdit::OnBnClickedPcircle()
{	
	CString Value;
	m_PRadius.GetWindowText(Value);
	float Radius=atof(Value);
	m_PFaces.GetWindowText(Value);
	DWORD Faces=atoi(Value);

	if(!Radius || !Faces) return;

	DXTriangle		*Object=NULL, *ptr=NULL, *LastPtr=NULL;
	DxSurfaceType	Surface;

	for(DWORD i=0; i<Faces-1; i++){
		ptr=new DXTriangle;

	}




}

void CSurfaceEdit::OnBnClickedMirrox()
{
	D3DXVECTOR3 Center;
	if(m_MirrorCheck.GetCheck()) Center.x=Center.z=Center.z=0.0f;
	else {
		Center.x=(m_SelectList->m_MaxX-m_SelectList->m_MinX)/2+m_SelectList->m_MinX;
		Center.y=(m_SelectList->m_MaxY-m_SelectList->m_MinY)/2+m_SelectList->m_MinY;
		Center.z=(m_SelectList->m_MaxZ-m_SelectList->m_MinZ)/2+m_SelectList->m_MinZ;
	}
	E_MirrorOnAxis(m_SelectList, Center, true);
}

void CSurfaceEdit::OnBnClickedMirroy()
{
	D3DXVECTOR3 Center;
	if(m_MirrorCheck.GetCheck()) Center.x=Center.z=Center.z=0.0f;
	else {
		Center.x=(m_SelectList->m_MaxX-m_SelectList->m_MinX)/2+m_SelectList->m_MinX;
		Center.y=(m_SelectList->m_MaxY-m_SelectList->m_MinY)/2+m_SelectList->m_MinY;
		Center.z=(m_SelectList->m_MaxZ-m_SelectList->m_MinZ)/2+m_SelectList->m_MinZ;
	}
	E_MirrorOnAxis(m_SelectList, Center, false, true);
}

void CSurfaceEdit::OnBnClickedMirroz()
{
	D3DXVECTOR3 Center;
	if(m_MirrorCheck.GetCheck()) Center.x=Center.z=Center.z=0.0f;
	else {
		Center.x=(m_SelectList->m_MaxX-m_SelectList->m_MinX)/2+m_SelectList->m_MinX;
		Center.y=(m_SelectList->m_MaxY-m_SelectList->m_MinY)/2+m_SelectList->m_MinY;
		Center.z=(m_SelectList->m_MaxZ-m_SelectList->m_MinZ)/2+m_SelectList->m_MinZ;
	}
	E_MirrorOnAxis(m_SelectList, Center, false, false, true);
}

void CSurfaceEdit::OnBnClickedPlanize()
{
	float xm=m_SelectList->m_MinX;
	float ym=m_SelectList->m_MinY;
	float zm=m_SelectList->m_MinZ;
	E_PlaneSelection(m_SelectList, (PlaneMode==PLANE_X)?&xm:NULL, (PlaneMode==PLANE_Y)?&ym:NULL, (PlaneMode==PLANE_Z)?&zm:NULL);
}

void CSurfaceEdit::OnBnClickedScalexup()
{
	char Buffer[16];
	m_ScaleX.GetWindowText(Buffer, sizeof(Buffer));
	float	Scale=atof(Buffer);
	if(!Scale) return;
	E_ScaleSelection(m_SelectList, Scale, 1.0f, 1.0f);
}

void CSurfaceEdit::OnBnClickedScalexdn()
{
	char Buffer[16];
	m_ScaleX.GetWindowText(Buffer, sizeof(Buffer));
	float	Scale=atof(Buffer);
	if(!Scale) return;
	Scale = 1/Scale;
	E_ScaleSelection(m_SelectList, Scale, 1.0f, 1.0f);
}

void CSurfaceEdit::OnBnClickedScaleyup()
{
	char Buffer[16];
	m_ScaleY.GetWindowText(Buffer, sizeof(Buffer));
	float	Scale=atof(Buffer);
	if(!Scale) return;
	E_ScaleSelection(m_SelectList, 1.0f, Scale, 1.0f);
}

void CSurfaceEdit::OnBnClickedScaleydn()
{
	char Buffer[16];
	m_ScaleY.GetWindowText(Buffer, sizeof(Buffer));
	float	Scale=atof(Buffer);
	if(!Scale) return;
	Scale = 1/Scale;
	E_ScaleSelection(m_SelectList, 1.0f, Scale, 1.0f);
}

void CSurfaceEdit::OnBnClickedScalezup()
{
	char Buffer[16];
	m_ScaleZ.GetWindowText(Buffer, sizeof(Buffer));
	float	Scale=atof(Buffer);
	if(!Scale) return;
	E_ScaleSelection(m_SelectList, 1.0f, 1.0f, Scale);
}

void CSurfaceEdit::OnBnClickedScalezdn()
{
	char Buffer[16];
	m_ScaleZ.GetWindowText(Buffer, sizeof(Buffer));
	float	Scale=atof(Buffer);
	if(!Scale) return;
	Scale = 1/Scale;
	E_ScaleSelection(m_SelectList, 1.0f, 1.0f, Scale);
}
