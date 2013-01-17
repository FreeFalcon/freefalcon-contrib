// LodInfo.cpp : implementation file
//

#include "stdafx.h"
#include "LODConvert.h"
#include "LodInfo.h"
#include "Converter.h"
#include ".\lodinfo.h"
#include "Encoder.h"
#include "DXVbManager.h"
#include "ObjectInstance.h"

ListItemType OldList[]={
	{"Vertices            ", &Converter::LODVerticesCount},
	{"Draw Calls          ", &Converter::LODDraws},
	{"Shader Normals      ", &Converter::ShaderNormals},
	{"DOFs                ", &Converter::LODDofsCount},
	{"Switches            ", &Converter::LODSwitches},

	{"", NULL}
};

ListItemType DXList[]={
	{"Vertices            ", &Converter::DXVerticesCount},
	{"Draw Calls          ", &Converter::DXDraws},
	{"Shader Normals      ", &Converter::ShaderNormals},
	{"DOFs                ", &Converter::DXDofsCount},
	{"Switches            ", &Converter::DXSwitches},
	{"Switch Branches     ", &Converter::DXSWBranches},
	{"V-Textured          ", &Converter::VTextured},
	{"V-Coloured          ", &Converter::VSolid},
	{"V-Gouraud           ", &Converter::VGouraud},
	{"V-Shaded            ", &Converter::VLite},
	{"V-Alpha             ", &Converter::VAlpha},
	{"Triangles           ", &Converter::TrianglesCount},
	{"Lines               ", &Converter::LinesCount},
	{"Points              ", &Converter::DotsCount},
	{"DOF Vertices        ", &Converter::DofVertices},

	{"", NULL}
};



DWORD	ModelID=0;

// CLodInfo dialog

void CLodInfo::Report(void)
{
	cv->LODStat(TagList, TagCount);
	cv->Statistics();

	CString Info;
	ListItemType *p=OldList;
	while(p->Label[0]){
		M_LODList.AddString(p->Label);
		Info.Format("%d", cv->*p->Variable);
		M_LodList2.AddString(Info);
		p++;
	}

	p=DXList;
	while(p->Label[0]){
		m_DXList.AddString(p->Label);
		Info.Format("%d", cv->*p->Variable);
		m_DXList2.AddString(Info);
		p++;
	}
}



IMPLEMENT_DYNAMIC(CLodInfo, CDialog)
CLodInfo::CLodInfo(CWnd* pParent, CString Title, Converter *conversion, BRoot* rt, BNodeType *TL, DWORD TC)
	: CDialog(CLodInfo::IDD, pParent)
{
	Processed=false;
	Create(IDD_LODINFO);
	ShowWindow(SW_SHOW);
	CString s=Title;
	int l=s.ReverseFind('.');
	if(l>0) 
		Title=s.Left(l);
	Title+=".DXL";
	ecd =new Encoder(Title);
	CV=conversion;
	cv=conversion;
	TagList=TL;
	TagCount=TC;
	Root=rt;
	GlobalBias=0.0f;
	TheStateStack.pvtDrawObject(Root);
	Report();
	ecd->Initialize(cv);
	ecd->Encode(ModelID, 1, 0);


	TestObj=new ObjectInstance(ModelID++);
	TestObj->DOFValues=DOFs;
	TestObj->SwitchValues=SWITCHes;
	TestObj->ParentObject->nDOFs=40;
	TestObj->ParentObject->nSwitches=40;
	TestObj->ParentObject->nTextureSets=1;

	ModelID&=0x1ff;
	DxDbHeader *Header=(DxDbHeader*)ecd->GetModelBuffer();
	TheVbManager.SetupModel(TestObj->id, (BYTE*)Header, Header->VBClass);
	TheDXEngine.LoadTextures(TestObj->id);
	TheVbManager.AssertValidModel(TestObj->id);
	DX->Draw(TestObj);

	CString RS;
	DWORD	*rs=(DWORD*)ecd->GetModelBuffer();

	DWORD	dwNodesNr=((DxDbHeader*)rs)->dwNodesNr;		
	DWORD	pVPool=((DxDbHeader*)rs)->pVPool;		
	DWORD	dwPoolSize=((DxDbHeader*)rs)->dwPoolSize;		
	DWORD	dwNVertices=((DxDbHeader*)rs)->dwNVertices;


	RS.Format(" - #%02d  - Vertex Pool Size : %d", ModelID-1, dwNVertices);
	SetWindowText(Title+ RS);
	delete cv;
	Processed=true;

	if(VBList) VBList->UpdateList();
}

CLodInfo::~CLodInfo()
{
	DX->UnDraw(TestObj);
	TheDXEngine.UnLoadTextures(TestObj->id);
	TheVbManager.ReleaseModel(TestObj->id);
}

void CLodInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LODLIST, M_LODList);
	DDX_Control(pDX, IDC_DXLIST, m_DXList);
	DDX_Control(pDX, IDC_LODLIST2, M_LodList2);
	DDX_Control(pDX, IDC_DXLIST2, m_DXList2);
}


BEGIN_MESSAGE_MAP(CLodInfo, CDialog)
	ON_WM_CLOSE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()





// CLodInfo message handlers

void CLodInfo::OnSetFocus(CWnd* pOldWnd )
{
	//CWnd *x=GetCapture();
	//if(x) SetCapture();
	if(Processed)DX->Draw(TestObj);
	//ReleaseCapture();
}

void CLodInfo::OnClose()
{
	// TODO: Add your message handler code here and/or call default

//	delete cv;
	delete this;
	
}

