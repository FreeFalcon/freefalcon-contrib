#include "stdafx.h"
#include "Lodconvert.h"
#include "TexSelect.h"
#include ".\texselect.h"


// CTexSelect dialog

IMPLEMENT_DYNAMIC(CTexSelect, CDialog)
CTexSelect::CTexSelect(CWnd* pParent, E_Object *Obj, CString *TexNr)
	: CDialog(CTexSelect::IDD, pParent)
{
	m_Obj=Obj;
	m_TexNr=TexNr;
	*m_TexNr="";
	//Create(IDD, pParent);
	DoModal();
}

CTexSelect::~CTexSelect()
{
}

void CTexSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TEXTURESELECTLIST, m_TexList);
	FillData();
}


BEGIN_MESSAGE_MAP(CTexSelect, CDialog)
	ON_LBN_SELCHANGE(IDC_TEXTURESELECTLIST, OnLbnSelchangeTextureselectlist)
	ON_BN_CLICKED(IDC_LOADTEXBUTTON, OnBnClickedLoadtexbutton)
END_MESSAGE_MAP()

void CTexSelect::FillData(void)
{
	DWORD	TexturesList[256];
	memset(TexturesList, 0xff, sizeof(TexturesList));

	for(DWORD a=0; a<m_Obj->TexNr; a++){
		DWORD	t=m_Obj->Textures[a];
		DWORD	c=0;
		while(TexturesList[c]!=-1 && c<256){
			if(TexturesList[c]==t) goto AlreadyIn;
			c++;
		}
		if(c<256) TexturesList[c]=t;
AlreadyIn:;
	}

	m_TexList.ResetContent();
	a=0;
	while(TexturesList[a]!=-1){
		char	Text[16];
		itoa(TexturesList[a], Text, 10);
		m_TexList.AddString(Text);
		a++;
	}

}


// CTexSelect message handlers

void CTexSelect::OnLbnSelchangeTextureselectlist()
{
	m_TexList.GetText(m_TexList.GetCurSel(), *m_TexNr);
}




void CTexSelect::OnBnClickedLoadtexbutton()
{
	*m_TexNr="LOAD";
	EndModalLoop(0);
}
