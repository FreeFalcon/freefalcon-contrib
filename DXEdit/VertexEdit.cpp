// VertexEdit.cpp : implementation file
//

#include "stdafx.h"
#include "LodConvert.h"
#include "VertexEdit.h"
#include "DXEditing.h"


CVertexEdit	*ThisVertex;
// CVertexEdit dialog

IMPLEMENT_DYNAMIC(CVertexEdit, CDialog)
CVertexEdit::CVertexEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CVertexEdit::IDD, pParent)
{
	m_VSelected=NULL;
	Create(IDD, pParent);
	ShowWindow(SW_HIDE);
	ThisVertex=this;
}

CVertexEdit::~CVertexEdit()
{
}

void CVertexEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SINGLEVERTEXCHECK, m_SingleVertexCheck);
	DDX_Control(pDX, IDC_SPECULARSLIDER, m_SpecularAlpha);
	DDX_Control(pDX, IDC_DIFFUSESLIDER, m_DiffuseSlider);
	m_SpecularAlpha.SetRange(0, 0xff, true);
	m_DiffuseSlider.SetRange(0, 0xff, true);
	DDX_Control(pDX, IDC_SVERTEXMOVE, m_VertexMove);
	DDX_Control(pDX, IDC_ONLYSELECTCHECK, m_OnlySelectCheck);
	DDX_Control(pDX, IDC_SVERTEXUV, m_VertexUV);

	m_DiffuseCopy=m_SpecularCopy=0;
	DDX_Control(pDX, IDC_SVERTEXSCALEOMNI, m_ScaleOmniCheck);
	DDX_Control(pDX, IDC_SVERTEXSCALEABS, m_ScaleAbsolute);
}

void CVertexEdit::UpdateEditValues(CSelectList *List, void **Object, D3DVERTEXEX **Vertex)
{
	if(!*Vertex) return;
	m_SelectList=List;
	m_VSelected=Vertex;
	m_SpecularAlpha.SetPos(((*m_VSelected)->dwSpecular&0xff000000)>>24);
	m_DiffuseSlider.SetPos(((*m_VSelected)->dwColour&0xff000000)>>24);
}


bool CVertexEdit::EditVertex(CSelectList *List, void **Object, D3DVERTEXEX **Vertex)
{
	if(!*Vertex) return false;
	m_SelectList=List;
	m_VertexOwner=Object;
	m_VSelected=Vertex;
	m_SpecularAlpha.SetPos(((*m_VSelected)->dwSpecular&0xff000000)>>24);
	m_DiffuseSlider.SetPos(((*m_VSelected)->dwColour&0xff000000)>>24);
	ShowWindow(SW_SHOWNA);
	return false;
}




BEGIN_MESSAGE_MAP(CVertexEdit, CDialog)
	ON_BN_CLICKED(IDC_DIFFUSECOLOR, OnBnClickedDiffusecolor)
	ON_BN_CLICKED(IDC_SPECULARCOLOR, OnBnClickedSpecularcolor)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_DIFFUSESLIDER, OnNMCustomdrawDiffuseslider)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SPECULARSLIDER, OnNMCustomdrawSpecularslider)
	ON_BN_CLICKED(IDC_SVERTEXMOVE, OnBnClickedSvertexmove)
	ON_BN_CLICKED(IDC_SVERTEXSCALE, OnBnClickedSvertexscale)
	ON_BN_CLICKED(IDC_DCOPY, OnBnClickedDcopy)
	ON_BN_CLICKED(IDC_SCOPY, OnBnClickedScopy)
	ON_BN_CLICKED(IDC_DPASTE, OnBnClickedDpaste)
	ON_BN_CLICKED(IDC_SPASTE, OnBnClickedSpaste)
	ON_BN_CLICKED(IDC_POSCOPY, OnBnClickedPoscopy)
	ON_BN_CLICKED(IDC_POSPASTE, OnBnClickedPospaste)
	ON_BN_CLICKED(IDC_COPYUV, OnBnClickedCopyuv)
	ON_BN_CLICKED(IDC_PASTEUV, OnBnClickedPasteuv)
	ON_BN_CLICKED(IDC_SVERTEXUV, OnBnClickedSvertexuv)
END_MESSAGE_MAP()



DWORD	GetColour(DWORD Original)
{
	CColorDialog *SetColor;
	SetColor=new CColorDialog;
	SetColor->m_cc.rgbResult=D3DTOWINCOLOUR(Original);
	SetColor->m_cc.Flags|=CC_RGBINIT | CC_ANYCOLOR | CC_FULLOPEN;
	DWORD	id=SetColor->DoModal();
	if(id==IDOK) Original=WINTOD3DCOLOUR(SetColor->GetColor());
	return Original;
}

// CVertexEdit message handlers

void CVertexEdit::OnBnClickedDiffusecolor()
{
	// TODO: Add your control notification handler code here
	if(!m_VertexOwner || !*m_VertexOwner) return;
	DWORD	Colour=0x00ffffff;
	if(m_VSelected && *m_VSelected)	Colour=(*m_VSelected)->dwColour&0x00ffffff;
	Colour=GetColour(Colour);
	if(m_VSelected && *m_VSelected) (*m_VSelected)->dwColour=(*m_VSelected)->dwColour&0xff000000|Colour;
	if(!m_SingleVertexCheck.GetCheck()) E_SetVertices(m_SelectList, m_OnlySelectCheck.GetCheck()?true:false, &Colour);
}



void CVertexEdit::OnBnClickedSpecularcolor()
{
	// TODO: Add your control notification handler code here
	if(!m_VertexOwner || !*m_VertexOwner) return;
	DWORD	Colour=0x00ffffff;
	if(m_VSelected && *m_VSelected)	Colour=(*m_VSelected)->dwSpecular&0x00ffffff;
	Colour=GetColour(Colour);
	if(m_VSelected && *m_VSelected) (*m_VSelected)->dwSpecular=(*m_VSelected)->dwSpecular&0xff000000|Colour;
	if(!m_SingleVertexCheck.GetCheck()) E_SetVertices(m_SelectList, m_OnlySelectCheck.GetCheck()?true:false, NULL, &Colour);
	
}

void CVertexEdit::OnNMCustomdrawDiffuseslider(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	if(!m_VertexOwner || !*m_VertexOwner) return;
	*pResult = 0;
	m_DAlpha=m_DiffuseSlider.GetPos();
	D3DVERTEXEX	*v=NULL;
	if(m_VSelected) v=*m_VSelected;
	if(m_VSelected && *m_VSelected) (*m_VSelected)->dwColour=((*m_VSelected)->dwColour&0x00ffffff)|(m_DAlpha<<24);
	if(!m_SingleVertexCheck.GetCheck())
		E_SetVertices(m_SelectList, m_OnlySelectCheck.GetCheck()?true:false, NULL, NULL, &m_DAlpha);
}

void CVertexEdit::OnNMCustomdrawSpecularslider(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	if(!m_VertexOwner || !*m_VertexOwner) return;
	*pResult = 0;
	m_SAlpha=m_SpecularAlpha.GetPos();
	D3DVERTEXEX	*v=NULL;
	if(m_VSelected) v=*m_VSelected;
	if(m_VSelected && *m_VSelected) (*m_VSelected)->dwSpecular=((*m_VSelected)->dwSpecular&0x00ffffff)|(m_SAlpha<<24);
	if(!m_SingleVertexCheck.GetCheck()) E_SetVertices(m_SelectList, m_OnlySelectCheck.GetCheck()?true:false, NULL, NULL, NULL, &m_SAlpha);
}


void VertexMoveCallBack(D3DXVECTOR3 Transform, D3DXVECTOR3 Move, bool Remove)
{
	ThisVertex->VertexMoveFunction(Transform, Move, Remove);

}

void CVertexEdit::VertexMoveFunction(D3DXVECTOR3 Transform, D3DXVECTOR3 Move, bool Remove)
{
	if(Remove){
		m_bVertexMove=false;
		m_VertexMove.SetCheck(m_bVertexMove);
		MoveFunction=NULL;
		return;
	}
	
	E_TranslateVertices(m_SelectList , Move.x, Move.y, Move.z);

}

void CVertexEdit::OnBnClickedSvertexmove()
{
	m_bVertexMove^=true;
	m_VertexMove.SetCheck(m_bVertexMove);
	if(!m_bVertexMove)
		MoveFunction=NULL;
	else {
		D3DXVECTOR3	Null(0,0,0);
		if(MoveFunction) MoveFunction(Null, Null, true);
		MoveFunction=VertexMoveCallBack;
	}
}




void VertexScaleCallBack(D3DXVECTOR3 Transform, D3DXVECTOR3 Move, bool Remove)
{
	ThisVertex->VertexScaleFunction(Transform, Move, Remove);

}

void CVertexEdit::VertexScaleFunction(D3DXVECTOR3 Transform, D3DXVECTOR3 Move, bool Remove)
{
	if(Remove){
		m_bVertexScale=false;
		m_VertexScale.SetCheck(m_bVertexScale);
		MoveFunction=NULL;
		return;
	}
	
	Move.x/=10;
	Move.y/=10;
	Move.z/=10;

	D3DXVECTOR3 Center;

	Center.x=(m_SelectList->m_MaxX-m_SelectList->m_MinX)/2;
	Center.y=(m_SelectList->m_MaxY-m_SelectList->m_MinY)/2;
	Center.z=(m_SelectList->m_MaxZ-m_SelectList->m_MinZ)/2;

	if(m_ScaleAbsolute.GetCheck()) Center.x=Center.y=Center.z=0.0f;


	if(m_ScaleOmniCheck.GetCheck()){
		float	MoveMax=max(Move.x, max(Move.y, Move.z));
		float	MoveMin=min(Move.x, min(Move.y, Move.z));
		if(-MoveMin>MoveMax) Move.x=Move.z=Move.y=MoveMin;
		else Move.x=Move.z=Move.y=MoveMax;
	}

	E_ScaleVertices(m_SelectList, &Center , Move.x, Move.y, Move.z);

}

void CVertexEdit::OnBnClickedSvertexscale()
{
	m_bVertexScale^=true;
	m_VertexScale.SetCheck(m_bVertexScale);
	if(!m_bVertexScale)
		MoveFunction=NULL;
	else {
		D3DXVECTOR3	Null(0,0,0);
		if(MoveFunction) MoveFunction(Null, Null, true);
		MoveFunction=VertexScaleCallBack;
	}
}


void CVertexEdit::OnBnClickedDcopy()
{
	if((!m_VSelected) || (!*m_VSelected)) return;
	m_DiffuseCopy=(*m_VSelected)->dwColour;
}

void CVertexEdit::OnBnClickedScopy()
{
	if((!m_VSelected) || (!*m_VSelected)) return;
	m_SpecularCopy=(*m_VSelected)->dwSpecular;
}

void CVertexEdit::OnBnClickedDpaste()
{
	if((!m_VSelected) || (!*m_VSelected)) return;
	(*m_VSelected)->dwColour=m_DiffuseCopy;
	DWORD	Alpha=m_DiffuseCopy&0xff000000;
	if(!m_SingleVertexCheck.GetCheck()) E_SetVertices(m_SelectList, m_OnlySelectCheck.GetCheck()?true:false, &m_DiffuseCopy, NULL, &Alpha);
}

void CVertexEdit::OnBnClickedSpaste()
{
	if((!m_VSelected) || (!*m_VSelected)) return;
	(*m_VSelected)->dwSpecular=m_SpecularCopy;
	DWORD	Alpha=m_SpecularCopy&0xff000000;
	if(!m_SingleVertexCheck.GetCheck()) E_SetVertices(m_SelectList, m_OnlySelectCheck.GetCheck()?true:false, NULL, &m_SpecularCopy, NULL, &Alpha);
}


void CVertexEdit::OnBnClickedPoscopy()
{
	if((!m_VSelected) || (!*m_VSelected)) return;
	m_PositionCopy.x=(*m_VSelected)->vx;
	m_PositionCopy.y=(*m_VSelected)->vy;
	m_PositionCopy.z=(*m_VSelected)->vz;
}

void CVertexEdit::OnBnClickedPospaste()
{
	if((!m_VSelected) || (!*m_VSelected)) return;
	D3DVECTOR	Move;
	Move.x=m_PositionCopy.x-(*m_VSelected)->vx;
	Move.y=m_PositionCopy.y-(*m_VSelected)->vy;
	Move.z=m_PositionCopy.z-(*m_VSelected)->vz;
	
	E_TranslateVertices(m_SelectList , Move.x, Move.y, Move.z);
}

void CVertexEdit::OnBnClickedCopyuv()
{
	if((!m_VSelected) || (!*m_VSelected)) return;
	m_UCopy=(*m_VSelected)->tu;
	m_VCopy=(*m_VSelected)->tv;
}

void CVertexEdit::OnBnClickedPasteuv()
{
	if((!m_VSelected) || (!*m_VSelected)) return;
	(*m_VSelected)->tv=m_VCopy;
	(*m_VSelected)->tu=m_UCopy;
}

void VertexUVCallBack(D3DXVECTOR3 Transform, D3DXVECTOR3 Move, bool Remove)
{
	ThisVertex->VertexUVFunction(Transform, Move, Remove);

}

void CVertexEdit::OnBnClickedSvertexuv()
{
	m_bVertexUV^=true;
	m_VertexUV.SetCheck(m_bVertexUV);
	if(!m_bVertexUV)
		MoveFunction=NULL;
	else {
		D3DXVECTOR3	Null(0,0,0);
		if(MoveFunction) MoveFunction(Null, Null, true);
		MoveFunction=VertexUVCallBack;
	}
}

void CVertexEdit::VertexUVFunction(D3DXVECTOR3 Transform, D3DXVECTOR3 Move, bool Remove)
{
	if(Remove){
		m_bVertexUV=false;
		m_VertexUV.SetCheck(m_bVertexUV);
		MoveFunction=NULL;
		return;
	}
	float	u,v;
	if(PlaneMode==PLANE_X) {u=Move.y; v=Move.z;};
	if(PlaneMode==PLANE_Y) {u=Move.x; v=Move.z;};
	if(PlaneMode==PLANE_Z) {u=Move.x; v=Move.y;};
	u/=100.0f; v/=100.0f;
	E_TranslateUV(m_SelectList , u, v);

}
