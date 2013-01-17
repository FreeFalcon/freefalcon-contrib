// DXLList.cpp : implementation file
//

#include "stdafx.h"
#include "LODConvert.h"
#include "DXLList.h"
#include ".\dxllist.h"
//#include "DXVBManager.h"
//#include "Device.h"
//#include "DXEngine.h"

// CDXLList dialog

IMPLEMENT_DYNAMIC(CDXLList, CDialog)
CDXLList::CDXLList(CWnd* pParent /*=NULL*/)
	: CDialog(CDXLList::IDD, pParent)
{
	Create(IDD_DXLLIST);
	ShowWindow(SW_SHOW);
}

CDXLList::~CDXLList()
{
}

void CDXLList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DXLIST, m_DXList);
	UpdateData();
	DDX_Control(pDX, IDC_DISABLECHECK, m_DisableCheck);
	DDX_Control(pDX, IDC_FRAMESURACE, m_FrameSurface);
}


BEGIN_MESSAGE_MAP(CDXLList, CDialog)
	ON_LBN_SELCHANGE(IDC_DXLIST, OnLbnSelchangeDxlist)
	ON_BN_CLICKED(IDC_DISABLECHECK, OnBnClickedDisablecheck)
	ON_BN_CLICKED(IDC_FRAMESURACE, OnBnClickedFramesurace)
END_MESSAGE_MAP()


// CDXLList message handlers


void CDXLList::UpdateData(void)
{
	ObjectInstance *objInst;
	objInst=DX->DrawObject[DX->SelectedObject];
	VBItemType	VB;
	if(objInst==NULL) return;

	TheVbManager.GetModelData(VB, objInst->id);

	NodeScannerType	NODE;
	NODE.BYTE=(BYTE*)VB.Nodes;
	m_DXList.ResetContent();

	CString	Label,Label2,Label3;
	CString	Spaces;
	DWORD	DofLevel=0;
	DWORD	Draws=0;
	memset(Selected, 0xff, sizeof(Selected));

	Spaces="";
	memset(LineListNode, NULL, sizeof(LineListNode));
	// Till end of Model
	while(NODE.HEAD->Type!=DX_MODELEND){
		
		//Spaces="";
		//for(a=0; a<DofLevel; a++) Spaces+="  ";

		// assign the node ID

		// Selects actions for each node
		switch(NODE.HEAD->Type){

			case	DX_ROOT:		break;

			
		// * SURFACE MANAGEMENT *	Alpha Surfaces are deferred to another Draw
			case	DX_SURFACE:		Draws++;
									LineListNode[m_DXList.GetCount()]=NODE.SURFACE;
									if(NODE.SURFACE->TexID[0]!=-1)
										Label.Format(" %03d - SURFACE - TEXTURE : %02d     ", Draws, NODE.SURFACE->TexID[0]);
									else 
										Label.Format(" %03d - SURFACE - VERTEX COLOR   ", Draws);
									if(NODE.SURFACE->dwFlags.b.Texture) Label+="T";
									if(NODE.SURFACE->dwFlags.b.VColor) Label+="V";
									if(NODE.SURFACE->dwFlags.b.Alpha) Label+="A";
									if(NODE.SURFACE->dwFlags.b.Lite) Label+="L";
									if(NODE.SURFACE->dwFlags.b.zBias) Label+="Z";
									if(NODE.SURFACE->dwFlags.b.ChromaKey) Label+="K";
									if(NODE.SURFACE->dwFlags.b.BillBoard) Label+="B";

									if(NODE.SURFACE->dwFlags.b.Point) Label+=" DOT";
									if(NODE.SURFACE->dwFlags.b.Poly) Label+=" TRI";
									if(NODE.SURFACE->dwFlags.b.Line) Label+=" LIN";

									if(NODE.SURFACE->dwzBias)
										Label3.Format("  Z= %01d",NODE.SURFACE->dwzBias);
									else 
										Label3="";
									Label2.Format("  Vert : %04.0d", NODE.SURFACE->dwVCount);
									m_DXList.AddString(Spaces+Label+Label2+Label3);
									break;

			
			case	DX_MATERIAL:	Label.Format(" MATERIAL %04.0d", NODE.MATERIAL->m);
									m_DXList.AddString(Spaces+Label);
									break;

			
									
			case	DX_TEXTURE:		Label.Format(" TEXTURE %04d", NODE.TEXTURE->Tex[0]);
									m_DXList.AddString(Spaces+Label);
									break;
			

			case	DX_SLOT:		Label.Format(" SLOT %04d", NODE.SLOT->SlotNr);
									m_DXList.AddString(Spaces+Label);
									break;

			case	DX_DOF:			DofLevel++;
									m_DXList.AddString(Spaces+"\\---------------------------\\");
									if(NODE.DOF->Type==SWITCH) {
										Label.Format("%02d ",NODE.DOF->SwitchNumber);
										Spaces+=Label;
										Label.Format(" SWITCH : %04d   BR : %04d", NODE.DOF->SwitchNumber,NODE.DOF->SwitchBranch);
									} else {
										Spaces+="  ";
										Label.Format(" DOF nr :%04d ", NODE.DOF->dofNumber);
										if(NODE.DOF->Type==ROTATE) Label+=" - ROTATION";
										if(NODE.DOF->Type==XROTATE) Label+=" - EXT. ROTATION";
										if(NODE.DOF->Type==TRANSLATE) Label+=" - TRANSLATION";
										if(NODE.DOF->Type==SCALE) Label+=" - SCALING";
										//LineListNode[m_DXList.GetCount()]=(DxSurfaceType*)NODE.DOF;
									}
									m_DXList.AddString(Spaces+Label);
									break;

			case	DX_ENDDOF:		DofLevel--;
									Spaces=Spaces.Left(Spaces.GetLength()-3);
									m_DXList.AddString(Spaces+"/---------------------------/");
									break;

			case	DX_SWITCH:		
									break;

			case	DX_LIGHT:		break;

			default			:		MessageBox("Corrupted Model!!!", "DX Engine", NULL);
			
		}

		NODE.BYTE+=NODE.HEAD->dwNodeSize;
		if(VB.NNodes<0) MessageBox("No Model End...", "Corrupted Model!!!", NULL);

	}

}



void CDXLList::OnLbnSelchangeDxlist()
{
	int a=0;
	while(Selected[a]!=-1){
		if(LineListNode[Selected[a]]){
			LineListNode[Selected[a]]->dwFlags.b.Disable=0;
			LineListNode[Selected[a]]->dwFlags.b.Frame=0;
		}
		a++;
	}
	memset(Selected, 0xff, sizeof(Selected));

	int	b=m_DXList.GetSelCount();
	m_DXList.GetSelItems(16384, Selected);
	for(a=0; a<b; a++){
		if(LineListNode[Selected[a]]){
			if(m_DisableCheck.GetCheck()) LineListNode[Selected[a]]->dwFlags.b.Disable=1;
			if(m_FrameSurface.GetCheck()) LineListNode[Selected[a]]->dwFlags.b.Frame=1;
		}
	}

}

void CDXLList::OnBnClickedDisablecheck()
{
	OnLbnSelchangeDxlist();
}


void CDXLList::OnBnClickedFramesurace()
{
	// TODO: Add your control notification handler code here
	OnLbnSelchangeDxlist();
}
