// TextureEdit.cpp : implementation file
//

#include "stdafx.h"
#include "Lodconvert.h"
#include "TextureEdit.h"
#include ".\textureedit.h"
#include "DXEditing.h"
#include "Texselect.h"
static	CTextureEdit	*ThisTex;


// CTextureEdit dialog

IMPLEMENT_DYNAMIC(CTextureEdit, CDialog)
CTextureEdit::CTextureEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CTextureEdit::IDD, pParent)
{
	Create(IDD, pParent);
	ThisTex=this;

	UDACCEL	Acc[3]={{0,1},{1,3},{2,6}};
	m_VOffsetSpin.SetAccel(3, Acc);
	m_VOffsetSpin.SetRange32(-2000, 2000);
	m_HOffsetSpin.SetAccel(3, Acc);
	m_HOffsetSpin.SetRange32(-2000, 2000);
	m_VScaleSpin.SetAccel(3, Acc);
	m_VScaleSpin.SetRange32(2000, -2000);
	m_HScaleSpin.SetAccel(3, Acc);
	m_HScaleSpin.SetRange32(-2000, 2000);
	m_RotoSpin.SetAccel(3, Acc);
	m_RotoSpin.SetRange32(-2000, 2000);

	m_bTexOffset=m_bTexScale=false;
	m_TexUsedListBox.ResetContent();
	m_SelectList=NULL;
	memset(m_TexRef, 0, sizeof(m_TexRef));
}

CTextureEdit::~CTextureEdit()
{
}

void CTextureEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VOFFSET, m_VOffsetSpin);
	DDX_Control(pDX, IDC_HOFFSET, m_HOffsetSpin);
	DDX_Control(pDX, IDC_VSCALE, m_VScaleSpin);
	DDX_Control(pDX, IDC_HSCALE, m_HScaleSpin);
	DDX_Control(pDX, IDC_HVSCALE, m_HVScaleCheck);
	DDX_Control(pDX, IDC_TEXMOVE, m_OffsetCheck);
	DDX_Control(pDX, IDC_TEXSCALE, m_ScaleCheck);
	DDX_Control(pDX, IDC_ROTOSPIN, m_RotoSpin);
	DDX_Control(pDX, IDC_LIST1, m_TexUsedListBox);
	DDX_Control(pDX, IDC_TEXSETSTEXT, m_TexSetsText);
	DDX_Control(pDX, IDC_USETEXSETNR, m_UseTexSetNrCombo);
	DDX_Control(pDX, IDC_USETEX, m_UseTexCombo);
}


BEGIN_MESSAGE_MAP(CTextureEdit, CDialog)
	ON_NOTIFY(UDN_DELTAPOS, IDC_VOFFSET, OnDeltaposVoffset)
	ON_NOTIFY(UDN_DELTAPOS, IDC_HOFFSET, OnDeltaposHoffset)
	ON_NOTIFY(UDN_DELTAPOS, IDC_VSCALE, OnDeltaposVscale)
	ON_NOTIFY(UDN_DELTAPOS, IDC_HSCALE, OnDeltaposHscale)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ROTOSPIN, OnDeltaposRotospin)
	ON_BN_CLICKED(IDC_90, OnBnClicked90)
	ON_BN_CLICKED(IDC_91, OnBnClicked91)
	ON_BN_CLICKED(IDC_92, OnBnClicked92)
	ON_CBN_SELCHANGE(IDC_USETEXSETNR, OnCbnSelchangeUsetexsetnr)
	ON_BN_CLICKED(IDC_ADDTEXSET, OnBnClickedAddtexset)
	ON_BN_CLICKED(IDC_DELTEXSET, OnBnClickedDeltexset)
	ON_BN_CLICKED(IDC_ADDTEX, OnBnClickedAddtex)
	ON_BN_CLICKED(IDC_DELTEX, OnBnClickedDeltex)
	ON_BN_CLICKED(IDC_ASSIGNTEX, OnBnClickedAssigntex)
	ON_BN_CLICKED(IDC_CHANGETEX, OnBnClickedChangetex)
	ON_BN_CLICKED(IDC_X2, OnBnClickedX2)
	ON_BN_CLICKED(IDC_X3, OnBnClickedX3)
	ON_BN_CLICKED(IDC_D2, OnBnClickedD2)
	ON_BN_CLICKED(IDC_D3, OnBnClickedD3)
	ON_BN_CLICKED(IDC_FLIPU, OnBnClickedFlipu)
	ON_BN_CLICKED(IDC_FLIPV, OnBnClickedFlipv)
	ON_BN_CLICKED(IDC_TEXSAVE, OnBnClickedTexsave)
	ON_BN_CLICKED(IDC_TEXLOAD, OnBnClickedTexload)
	ON_BN_CLICKED(IDC_TEXREF, OnBnClickedTexref)
	ON_BN_CLICKED(IDC_TEXLCALC, OnBnClickedTexlcalc)
	ON_BN_CLICKED(IDC_SELECTTEX, OnBnClickedSelecttex)
	ON_BN_CLICKED(IDC_DELUNUSEDTEX, OnBnClickedDelunusedtex)
END_MESSAGE_MAP()

void CTextureEdit::EditTex(CSelectList *List)
{
	m_SelectList=List;
	UpdateTexList();
	UpdateValues();
	ShowWindow(SW_SHOWNA);
}


void CTextureEdit::UpdateTexList()
{
	CString Text;
	m_UseTexCombo.ResetContent();
	for(DWORD a=0; a<OSelected->TexPerSet; a++){
		Text.Format("%02d  (%04d)", a, OSelected->Textures[a+OSelected->TexSetNr*OSelected->TexPerSet]);
		m_UseTexCombo.AddString(Text);
	}

}


void CTextureEdit::UpdateValues()
{
	UpdateUsedTexList();
	CString Text;
	DWORD	TexSetsNr=(OSelected->TexPerSet)?OSelected->TexNr/OSelected->TexPerSet:0;
	Text.Format("%d",TexSetsNr);
	m_TexSetsText.SetWindowText(Text);
	m_UseTexSetNrCombo.ResetContent();
	for(DWORD a=0; a<TexSetsNr; a++){
		Text.Format("%#2d", a);
		m_UseTexSetNrCombo.AddString(Text);
	}
	m_UseTexSetNrCombo.SetCurSel(OSelected->TexSetNr);
	if(gSSelected && gSSelected->TexID[0]!=-1) m_UseTexCombo.SetCurSel(gSSelected->TexID[0]);
}


void CTextureEdit::OnDeltaposVoffset(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	signed int VMove=(signed short)(m_VOffsetSpin.GetPos()&0xffff);
	m_VOffsetSpin.SetPos(0);
	E_OffsetTexture(m_SelectList, 0, ((float)VMove)/1000.0f);
	*pResult = 0;
}

void CTextureEdit::OnDeltaposHoffset(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	signed int HMove=(signed short)(m_HOffsetSpin.GetPos()&0xffff);
	m_HOffsetSpin.SetPos(0);
	E_OffsetTexture(m_SelectList, ((float)HMove)/1000.0f, 0);
	*pResult = 0;
}

void CTextureEdit::OnDeltaposVscale(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	signed int VScale=(signed short)(m_VScaleSpin.GetPos()&0xffff);
	m_VScaleSpin.SetPos(0);
	float fScale=1.0f+((float)VScale)/1000.0f;
	E_ScaleTexture(m_SelectList, (m_HVScaleCheck.GetCheck())?fScale:1.0f,fScale );
	*pResult = 0;
}

void CTextureEdit::OnDeltaposHscale(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	signed int HScale=(signed short)(m_HScaleSpin.GetPos()&0xffff);
	m_HScaleSpin.SetPos(0);
	float fScale=1.0f+((float)HScale)/1000.0f;
	E_ScaleTexture(m_SelectList, fScale, (m_HVScaleCheck.GetCheck())?fScale:1.0f);
	*pResult = 0;
}

void CTextureEdit::OnDeltaposRotospin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	signed int Roto=(signed short)(m_RotoSpin.GetPos()&0xffff);
	m_RotoSpin.SetPos(0);
	float fRoto=PI*((float)Roto)/180.0f;
	E_RotateTexture(m_SelectList, fRoto, gVSelected->tv, gVSelected->tu);
	*pResult = 0;
}


void	CTextureEdit::UpdateUsedTexList()
{
	CString	Text;
	m_TexUsedListBox.ResetContent();
	if(!m_SelectList) return;
	DWORD a=0;
	while(m_SelectList->m_TexUsedList[a]!=-1){
		Text.Format("%02d  (%04d)",m_SelectList->m_TexUsedList[a],OSelected->Textures[m_SelectList->m_TexUsedList[a++]+OSelected->TexSetNr*OSelected->TexPerSet]); 
		m_TexUsedListBox.AddString(Text);
	}
}

void CTextureEdit::OnBnClicked90()
{
	E_RotateTexture(m_SelectList, (float)PI/2.0f, gVSelected->tv, gVSelected->tu);
}

void CTextureEdit::OnBnClicked91()
{
	E_RotateTexture(m_SelectList, -(float)PI/2.0f, gVSelected->tv, gVSelected->tu);
}

void CTextureEdit::OnBnClicked92()
{
	E_BlendTexture(m_SelectList, gVSelected);
}

void CTextureEdit::OnCbnSelchangeUsetexsetnr()
{
	OSelected->TexSetNr=m_UseTexSetNrCombo.GetCurSel();
	UpdateTexList();
	UpdateValues();
}


void CTextureEdit::OnBnClickedAddtexset()
{
	if((OSelected->TexPerSet+OSelected->TexNr)>=256) return;
	DWORD	nr=(OSelected->TexPerSet)?(OSelected->TexNr/OSelected->TexPerSet):0;
	
	memcpy(	&OSelected->Textures[nr*OSelected->TexPerSet],
			&OSelected->Textures[m_UseTexSetNrCombo.GetCurSel()*OSelected->TexPerSet],
			sizeof(DWORD)*OSelected->TexPerSet);

	OSelected->TexNr+=OSelected->TexPerSet;
	OSelected->TexSetNr=nr;

	for(DWORD a=0; a<OSelected->TexPerSet; a++){
		DX->LoadTexture(OSelected->Textures[nr*OSelected->TexPerSet+a]);
	}

	UpdateTexList();
	UpdateValues();
}

void CTextureEdit::OnBnClickedDeltexset()
{
	if(!OSelected->TexNr) return;
	DWORD	nr=m_UseTexSetNrCombo.GetCurSel();
	OSelected->TexSetNr=0;

	for(DWORD a=0; a<OSelected->TexPerSet; a++){
		DX->UnLoadTexture(OSelected->Textures[nr*OSelected->TexPerSet+a]);
	}

	memcpy(	&OSelected->Textures[nr*OSelected->TexPerSet],
			&OSelected->Textures[(nr+1)*OSelected->TexPerSet],
			sizeof(DWORD)*(256-(nr+1)*OSelected->TexPerSet));
	OSelected->TexNr-=OSelected->TexPerSet;
	UpdateTexList();
	UpdateValues();
	

	
}





CString	CTextureEdit::GetTexturefile()
{
	CString	FileName;
	CFileDialog	File(true, "*.DDS", NULL, 0, "DDS Textures|*.dds", NULL);
	File.DoModal();
	FileName=File.GetFileName();
	FileName.MakeUpper();
	if(FileName.Right(4)==".DDS") return FileName.Left(FileName.GetLength()-4);
	return "";
}

void CTextureEdit::OnBnClickedAddtex()
{
	DWORD	TexSet, TexSetNr=(OSelected->TexPerSet)?(OSelected->TexNr/OSelected->TexPerSet):0;
	if((TexSetNr*(OSelected->TexPerSet+1))>256) return;
	CString	Texture=GetTexturefile();
	if(Texture=="") return;
	if(!atoi(Texture)) return;
	DWORD	Tex=atoi(Texture);


	if(TexSetNr){
		for(TexSet=1; TexSet<=TexSetNr; TexSet++){
			DX->LoadTexture(Tex);
			DWORD	SIdx=TexSet*OSelected->TexPerSet+TexSet-1;
			memmove(&OSelected->Textures[SIdx+1], &OSelected->Textures[SIdx], (OSelected->TexNr-SIdx)*sizeof(DWORD));
			OSelected->Textures[SIdx]=Tex;
			OSelected->TexNr++;
		}
	} else {
		DX->LoadTexture(Tex);
		OSelected->Textures[0]=Tex;
		OSelected->TexNr++;
	}
	OSelected->TexPerSet++;
	m_SelectList->RecalcValues();
	UpdateTexList();
	UpdateValues();
}


void	DeleteTexture(E_Object *OSelected, DWORD TexNr)
{
	DWORD	TexSetNr=(OSelected->TexPerSet)?(OSelected->TexNr/OSelected->TexPerSet):0;

	E_ClearTexture(OSelected, TexNr);

	for(DWORD TexSet=0; TexSet<TexSetNr; TexSet++){
		DWORD	SIdx=TexSet*OSelected->TexPerSet-TexSet+TexNr;
		DX->UnLoadTexture(OSelected->Textures[SIdx]);
		memcpy(&OSelected->Textures[SIdx], &OSelected->Textures[SIdx+1], (OSelected->TexNr-SIdx-1)*sizeof(DWORD));
		OSelected->TexNr--;
	}

	OSelected->TexPerSet--;

}

void CTextureEdit::OnBnClickedDeltex()
{
	DWORD	TexNr;
	if(!OSelected->TexNr) return;
	TexNr=m_UseTexCombo.GetCurSel();
	if(TexNr==-1) return;

	DeleteTexture(OSelected, TexNr);

	m_SelectList->RecalcValues();
	UpdateTexList();
	UpdateValues();
}

void CTextureEdit::OnBnClickedAssigntex()
{
	DWORD TexNr=m_UseTexCombo.GetCurSel();
	if(TexNr==-1) return;
	E_AssignTexture(m_SelectList, TexNr);
	m_SelectList->RecalcValues();
	UpdateValues();
}

void CTextureEdit::OnBnClickedChangetex()
{
	CString	TexNr;
	CTexSelect TexSelect(this, OSelected, &TexNr);
	if(TexNr!=""){
		DWORD	Tex;
		DWORD	Sel=m_UseTexCombo.GetCurSel();
		DWORD	SIdx=OSelected->TexSetNr*OSelected->TexPerSet+Sel;
		DX->UnLoadTexture(OSelected->Textures[SIdx]);
		if(TexNr=="LOAD"){
			CString	Texture=GetTexturefile();
			if(Texture=="") return;
			if(!atoi(Texture)) return;
			Tex=atoi(Texture);
		} else {
			 Tex=atoi(TexNr);
		}
		DX->LoadTexture(Tex);
		OSelected->Textures[SIdx]=Tex;
	}

	UpdateTexList();
	UpdateValues();
}

void CTextureEdit::OnBnClickedX2()
{
	E_ScaleTexture(m_SelectList, 1.0f/2.0f ,1.0f/2.0f  );
}

void CTextureEdit::OnBnClickedX3()
{
	E_ScaleTexture(m_SelectList, 1.0f/3.0f ,1.0f/3.0f  );

}

void CTextureEdit::OnBnClickedD2()
{
	E_ScaleTexture(m_SelectList, 2 ,2  );
}

void CTextureEdit::OnBnClickedD3()
{
	E_ScaleTexture(m_SelectList, 3 ,3  );
}

void CTextureEdit::OnBnClickedFlipu()
{
	E_FlipTexture(m_SelectList, true);
}

void CTextureEdit::OnBnClickedFlipv()
{
	E_FlipTexture(m_SelectList, false, true);
}

void CTextureEdit::OnBnClickedTexsave()
{
	CString	FileName;
	CFileDialog	File(true, "*.TXS", NULL, 0, "Texture Set|*.TXS", NULL);
	File.DoModal();
	FileName=File.GetFileName();
	FileName.MakeUpper();
	if(FileName.Right(4)==".TXS"){
		FileName.Left(FileName.GetLength()-4);

		CFile File;
		File.Open(FileName, CFile::modeWrite|CFile::modeCreate, &CFileException(CFileException::generic, NULL));
		
		File.Write(&OSelected->TexNr,sizeof(OSelected->TexNr));
		File.Write(&OSelected->TexPerSet,sizeof(OSelected->TexPerSet));
		File.Write(&OSelected->Textures,sizeof(OSelected->Textures[0])*OSelected->TexNr);

		File.Close();
	}
}

void CTextureEdit::OnBnClickedTexload()
{
	CString	FileName;
	CFileDialog	File(true, "*.TXS", NULL, 0, "Texture Set|*.TXS", NULL);
	File.DoModal();
	FileName=File.GetFileName();
	FileName.MakeUpper();
	if(FileName.Right(4)==".TXS"){

		DrawDisable=true;

		while(RenderPhase) IdleMode();

		DX->UnLoadTextures(OSelected->TexNr, OSelected->Textures);

		FileName.Left(FileName.GetLength()-4);

		CFile File;
		File.Open(FileName, CFile::modeRead, &CFileException(CFileException::generic, NULL));
		
		File.Read(&OSelected->TexNr,sizeof(OSelected->TexNr));
		File.Read(&OSelected->TexPerSet,sizeof(OSelected->TexPerSet));
		File.Read(&OSelected->Textures,sizeof(OSelected->Textures[0])*OSelected->TexNr);

		File.Close();

		DX->LoadTextures(OSelected->TexNr, OSelected->Textures);
		DrawDisable=false;

	}
}

void CTextureEdit::OnBnClickedTexref()
{
	m_SelectList->GetBoundVertices((DWORD)PlaneMode, m_TexRef, &m_TexSurface);
}

void CTextureEdit::OnBnClickedTexlcalc()
{
	m_SelectList->AssignUV((DWORD)PlaneMode, m_UseTexCombo.GetCurSel(), m_TexRef, &m_TexSurface);
 
}

void CTextureEdit::OnBnClickedSelecttex()
{
	DWORD TexNr=m_UseTexCombo.GetCurSel();
	if(TexNr==-1) return;
	D3DCOLORVALUE Color;

	Color.a=Color.r=Color.g=Color.g=1.0F;
	E_SelectByTexture(OSelected, m_SelectList, TexNr, &Color);
}

void CTextureEdit::OnBnClickedDelunusedtex()
{
	DWORD TexNr=0;

	do{
		if(E_CheckUsedTexture(OSelected, TexNr)) TexNr++;
		else DeleteTexture(OSelected, TexNr);
	} while(TexNr<OSelected->TexPerSet);

	m_SelectList->RecalcValues();
	UpdateTexList();
	UpdateValues();

}
