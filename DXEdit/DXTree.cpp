// DXTree.cpp : implementation file
//

#include "stdafx.h"
#include "LodConvert.h"
#include "DXTree.h"
#include "DXEditing.h"
#include "DXFiles.h"
#include ".\dxtree.h"
#include "switchpanel.h"

// CDXTree dialog

IMPLEMENT_DYNAMIC(CDXTree, CTreeCtrl)
CDXTree::CDXTree(CWnd* pParent, RECT Area)
{
	m_Object=NULL;
	Create(WS_VISIBLE|WS_BORDER|TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS|TVS_SHOWSELALWAYS|TVS_CHECKBOXES, Area, pParent, IDD);
	m_ImageList.Create(IDB_TREEBITMAP, 16, 0, 0xffffffff);
	SetImageList(&m_ImageList, TVSIL_NORMAL);
	EnableWindow(false);
	m_ExpOnSel=false;
}

CDXTree::~CDXTree()
{
}


void CDXTree::DoDataExchange(CDataExchange* pDX)
{
}


BEGIN_MESSAGE_MAP(CDXTree, CTreeCtrl)

	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()

END_MESSAGE_MAP()


CString	GetFlagsString(DXFlagsType Flags, DXNode *Node)
{
	CString	Result="";

	if(Node->Type==DOT || Node->Type==LINE || Node->Type==TRIANGLE ){
		char	text[128];
		if(Flags.b.Alpha) Result+="A";
		if(Flags.b.BillBoard)Result+="B";
		if(Flags.b.ChromaKey)Result+="K";
		if(Flags.b.Lite)Result+="L";
		if(Flags.b.Texture)Result+="T";
		if(((DXTriangle*)Node)->Surface.dwzBias)Result+="Z";
		if(Flags.b.VColor) Result+="V";
		if(Flags.b.SwEmissive) Result+="S";

		while(strlen(Result)<8) Result+=" ";

		if(Flags.b.Texture){
			sprintf(text, " T:%2d ", ((DXTriangle*)Node)->Surface.TexID[0]);
			Result+=text;
		}
		if(((DXTriangle*)Node)->Surface.dwzBias){
			sprintf(text, " Z:%2d ", ((DXTriangle*)Node)->Surface.dwzBias);
			Result+=text;
		}
		if(Flags.b.SwEmissive){
			sprintf(text, " S:%2d ", ((DXTriangle*)Node)->Surface.SwitchNumber);
			Result+=text;
		}
	}

	if(Node->Type==DOF){
		char	Number[32];
		if(((DXDof*)Node)->dof.Type<SWITCH){
			sprintf(Number, "%03d",((DXDof*)Node)->dof.dofNumber);
		} else {
			CString Name="";
			for(DWORD x=0; x<MAX_SWITCHES; x++){
				if(((DXDof*)Node)->dof.SwitchNumber==SWLabs[x].Number) { Name=SWLabs[x].Name; break; }
			}

			sprintf(Number, "%03d %#.12s - %02d",((DXDof*)Node)->dof.SwitchNumber, Name, ((DXDof*)Node)->dof.SwitchBranch);
		}
		Result=Number;
	}

	if(Node->Type==SLOT){
		char	Number[32];
		sprintf(Number, "%03d",((DXSlot*)Node)->Slot.SlotNr);
		Result=Number;
	}
		
	return Result;
}


// Construct the main Model Tree traversing the Model data
void CDXTree::AddNode(DXNode *Node, HTREEITEM Parent, HTREEITEM After, bool Reset)
{
	bool	NewSurface=True;
	DWORD Data,Image;
	char		Number[32];

	static	HTREEITEM	TreeStack[128],Surface, LastItem;
	static	DWORD		TreeLevel, NSurfaces, LastTex, LastZBias;
	static	DXFlagsType LastFlags;
	HTREEITEM			item;
	CString	Label;
	DXFlagsType	Flags,FlagsMask;

	// Resetting feature
	if(Reset){
		Surface=TVI_ROOT;
		LastItem=After;
		TreeStack[0]=Parent;
		TreeLevel=0;
		NSurfaces=0;
		LastTex=-1;
		LastZBias=0;
		LastFlags.w=0xffffffff;
		m_DrawCalls=m_Surfaces=0;
		return;
	}

	Flags.w=0;
	FlagsMask.w=0;
	FlagsMask.b.Alpha=true;
	FlagsMask.b.BillBoard=true;
	FlagsMask.b.ChromaKey=true;
	FlagsMask.b.Line=true;
	FlagsMask.b.Lite=true;
	FlagsMask.b.Point=true;
	FlagsMask.b.Poly=true;
	FlagsMask.b.Texture=true;
	FlagsMask.b.VColor=true;
	FlagsMask.b.zBias=true;
	FlagsMask.b.SwEmissive=true;

	switch(Node->Type){
		case	DOT:		
		case	LINE:		// Immediatly get the surface flags to be used later
		case	TRIANGLE:	Flags.w=((DXTriangle*)Node)->Surface.dwFlags.w & FlagsMask.w;
							// check for Texture and zBias
							if(LastTex!=((DXTriangle*)Node)->Surface.TexID[0] || LastZBias!=((DXTriangle*)Node)->Surface.dwzBias)
								goto AddItem;
							// 1st, if a surface from ROOT, for sure a new surface, not continuing a previous one
							if(Surface==TVI_ROOT) goto AddItem;
							// else check what surface is the parent
							Data=GetItemData(Surface);
							// if a SURFACE ITEM then get the child
							if(!Data) Data=GetItemData(GetChildItem(Surface));
							// is a valid surface/dof/switch
							if(Data){
								// if not same type a new surface
								if(((DXNode*)Data)->Type!=Node->Type) goto AddItem;
								// else check if same flags
								LastFlags.w=((DXTriangle*)Data)->Surface.dwFlags.w & FlagsMask.w;
								// if different flags, new surface
								if(Flags.w!=LastFlags.w) goto AddItem;
							}
							// if here any test for a new surface has failed
							NewSurface=false;
							
							AddItem:
							if(((DXTriangle*)Node)->Surface.dwFlags.b.Line) Image=1;
							if(((DXTriangle*)Node)->Surface.dwFlags.b.Point) Image=2;
							if(((DXTriangle*)Node)->Surface.dwFlags.b.Poly) Image=0;
							if(NewSurface) {
								LastItem=Surface=InsertItem(TVIF_TEXT|TVIF_SELECTEDIMAGE|TVIF_IMAGE|TVIF_PARAM, "", 5, 5, 0, 0, 0, TreeStack[TreeLevel], LastItem);
								NSurfaces=0;
								// a new call
								m_DrawCalls++;
							}
							LastItem=Node->TreeItem=InsertItem(TVIF_TEXT|TVIF_SELECTEDIMAGE|TVIF_IMAGE|TVIF_PARAM, GetFlagsString(Flags, Node), Image, Image, 0, 0, (DWORD)Node, Surface, LastItem);
							NSurfaces++;
							m_Surfaces++;
							Label=GetFlagsString(Flags, Node);
							sprintf(Number, "%03d : ", NSurfaces);														
							Label=Number+Label;
							SetItemText(Surface, Label);
							LastZBias=((DXTriangle*)Node)->Surface.dwzBias;
							LastTex=((DXTriangle*)Node)->Surface.TexID[0];
							break;

		case	DOF:		if(((DXDof*)Node)->dof.Type<SWITCH){
								sprintf(Number, "%03d",((DXDof*)Node)->dof.dofNumber);
								item=InsertItem(TVIF_TEXT|TVIF_SELECTEDIMAGE|TVIF_IMAGE|TVIF_PARAM, GetFlagsString(Flags, Node), 3, 3, 0, 0, (DWORD)Node, TreeStack[TreeLevel], TVI_LAST);
							} else {
								sprintf(Number, "%03d - %02d",((DXDof*)Node)->dof.SwitchNumber, ((DXDof*)Node)->dof.SwitchBranch);
								item=InsertItem(TVIF_TEXT|TVIF_SELECTEDIMAGE|TVIF_IMAGE|TVIF_PARAM, GetFlagsString(Flags, Node), 4, 4, 0, 0, (DWORD)Node, TreeStack[TreeLevel], TVI_LAST);
							}
							TreeLevel++;
							Node->TreeItem=TreeStack[TreeLevel]=item;
							Surface=TVI_ROOT;
							LastItem=TVI_LAST;
							LastFlags.w=0xffffffff;
							break;

		case	CLOSEDOF:	Node->TreeItem=InsertItem(TVIF_TEXT|TVIF_SELECTEDIMAGE|TVIF_IMAGE|TVIF_PARAM, GetFlagsString(Flags, Node), 7, 7, 0, 0, (DWORD)Node, TreeStack[TreeLevel], TVI_LAST);
							Surface=TVI_ROOT;
							TreeLevel--;
							LastItem=TreeStack[TreeLevel];
							LastFlags.w=0xffffffff;
							break;

		case	SLOT:		Surface=TVI_ROOT;
							LastItem=TVI_LAST;
							Node->TreeItem=InsertItem(TVIF_TEXT|TVIF_SELECTEDIMAGE|TVIF_IMAGE|TVIF_PARAM, GetFlagsString(Flags, Node), 6, 6, 0, 0, (DWORD)Node, TreeStack[TreeLevel], TVI_LAST);
							break;

		case	MODELEND:	Node->TreeItem=InsertItem(TVIF_SELECTEDIMAGE|TVIF_IMAGE|TVIF_PARAM, GetFlagsString(Flags, Node), 9, 9, 0, 0, (DWORD)Node, TreeStack[TreeLevel], TVI_LAST);
							break;
		default	:			break;
	}
}

void CDXTree::UpdateItemLabel(HTREEITEM item)
{
	DXNode *Node=(DXNode*)GetItemData(item);
	DXFlagsType	Flags;
	Flags.w=0;
	if(Node->Type==DOT || Node->Type==LINE || Node->Type==TRIANGLE ) Flags.w=((DXTriangle*)Node)->Surface.dwFlags.w;
	if(Node)SetItemText(item,GetFlagsString(Flags, Node));
}


void CDXTree::RebuildBranch(HTREEITEM Parent)
{
	HTREEITEM item;
	static	HTREEITEM	TreeStack[128];
	static	DWORD		StackLevel=0;
	DXNode				*Node;

	// if Root, do not delete nothing
	if(Parent==m_RootItem) goto Rebuild;

	// 1st of all Kill the Branch
	if(!ItemHasChildren(Parent)) Parent=GetParentItem(Parent);
	
	// Get the indicated node
	Node=(DXNode*)GetItemData(Parent);
	if(!Node && Parent!=m_RootItem) Parent=GetParentItem(Parent);

	item=Parent;

	do{
		// if item got a child, enter it
		if(ItemHasChildren(item)){
			TreeStack[StackLevel++]=item;
			item=GetChildItem(item);
		} else 
			item=GetNextSiblingItem(item);

		// if end of a branch, get parent from stack
		while(!item && StackLevel){
			item=TreeStack[--StackLevel];
			item=GetNextSiblingItem(item);
		}

		// delete any link btw Node & Tree
		if(item){
			Node=(DXNode*)GetItemData(item);
			if(Node) Node->TreeItem=NULL;
		}

	} while(item && StackLevel);

	while(GetChildItem(Parent)) DeleteItem(GetChildItem(Parent));

Rebuild:
	
	if(Parent && (Parent!=m_RootItem)){
		Node=(DXNode*)GetItemData(Parent);
		if(Node) Node=Node->Next;
	} else Node=(DXNode*)m_Object->Nodes;
	
	if(!Node) return;


	// Initialize the Tree Builder with the Parent
	AddNode(Node, Parent, TVI_LAST, true);

	// Add each node without any tree link
	while((Node) && (!Node->TreeItem)){
		AddNode(Node);
		Node=Node->Next;
	}

}



// Construct the main Model Tree traversing the Model data
void CDXTree::UpdateTree(void)
{
	// clear the Tree
	DeleteAllItems();
	// Get the model Nodes
	DXNode	*Node=(DXNode*)m_Object->Nodes;

	//The Root Icon
	m_RootItem=InsertItem(TVIF_TEXT|TVIF_SELECTEDIMAGE|TVIF_IMAGE|TVIF_PARAM, m_Object->Name, 8, 8, 0, 0, NULL, TVI_ROOT, TVI_LAST);
	
	// Initialize the Tree Builder
	AddNode(Node, m_RootItem, TVI_LAST,true);

	while(Node){
		AddNode(Node);
		Node=Node->Next;
	}

	UpdateData(true);

	Expand(m_RootItem, TVE_EXPAND);
}



// The public call to update a model and build up its tree
void CDXTree::UpdateModel(E_Object *Obj, CSelectList *SelectList)
{
	m_Object=Obj;
	m_SelectList=SelectList;
	UpdateTree();
	ShowWindow(SW_SHOW);
}

// The main Model Traversing function, called with TRUE argument to reset
// the traversing parameters and a possible start point
// Stop On Root make it to stop if we r not in a deeper level
HTREEITEM CDXTree::TraverseModel(bool Reset, HTREEITEM Start, bool StopOnRoot)
{
	static	HTREEITEM	item;
	static	HTREEITEM	TreeStack[128];
	static	DWORD		StackLevel=0,Data;
	static	bool		Root;

	// if Reset just startup the parameters
	if(Reset){
		item=(Start)?Start:GetRootItem();
		StackLevel=0;
		Root=true;
		return	item;
	}

	////////////////////// The traversing code \\\\\\\\\\\\\\\\\\\\\\\\\\\\

	// if 1st step return the startup item
	if(Root){
		Root=false;
		return item;
	}

	// if item got a child, enter it
	if(ItemHasChildren(item)){
		TreeStack[StackLevel++]=item;
		item=GetChildItem(item);
	} else 
		item=GetNextSiblingItem(item);

	// if end of a branch, get parent from stack
	while(!item && StackLevel){
		item=TreeStack[--StackLevel];
		item=GetNextSiblingItem(item);
	}
	
	// if just deeper levels and we are back on root stop here
	if(StopOnRoot && (!StackLevel)) return NULL;
	// else return the new item
	return	item;
}


// This function removes unused branches under the Surface Descriptors
void CDXTree::UpdateBranches(void)
{
	HTREEITEM	item, DelItem=NULL;
	DWORD		Data;

	// Startup the tree traversing code
	TraverseModel(true);

	// Traverse thru all the tree
	while(item=TraverseModel(false)){
		// if something to delete then do it
		if(DelItem) DeleteItem(DelItem);
		DelItem=NULL;
		// Get the Item Application Data
		Data=GetItemData(item);
		// if it is a SURFACE DESCRIPTOR ( Data=0x00 ) and got no more children, kill it
		if((!Data) && (!ItemHasChildren(item))) DelItem=item;;
	}
	// Eventually Last to delete
	if(DelItem) DeleteItem(DelItem);
}


// This function ensures that selected subbranches are visible,
// if Compress=TRUE it will compress any expanded branch
void CDXTree::UpdateTreeView(bool Compress)
{
	HTREEITEM	item,parent;
	DWORD		Data;

	// Startup the tree traversing code
	TraverseModel(true);

	// Traverse thru all the tree
	while(item=TraverseModel(false)){
		// If COMPRESS collapse sub branches
		if(Compress && GetChildItem(item)) Expand(item, TVE_COLLAPSE);
		// get Item Type
		Data=GetItemData(item);
		// if a SURFACE DESCRIPTOR set the check based on it's child checkings
		if(!Data) SetCheck(item, GetParentStatus(item));
		// if item is checked and is a SURFACE or DOF/SWITCH ensure it is visible
		if(GetCheck(item)){
			if(((!Data) || ((DXNode*)Data)->Type>=DOF) && (parent=GetParentItem(item)))
				Expand(parent, TVE_EXPAND);
				//EnsureVisible(item);
		}
	}
}


// This function returns if any sub item of the passed item is checked
BOOL CDXTree::GetParentStatus(HTREEITEM item)
{
	// start with no check
	BOOL	State=false;
	// from root of branch
	item=GetChildItem(item);
	// thru all the sub branch
	while(item){
		// look for checked items
		State|=GetCheck(item);
		item=GetNextSiblingItem(item);
	}
	// return the result
	return State;
}


// This function just clears any selection in the tree
void CDXTree::ClearSelection(void)
{
	m_SelectList->ClearSelection();
	UpdateTreeView(true);
}


// This function selects an item, and its sub itmes if it is a parent
void CDXTree::TreeSelect(HTREEITEM item)
{
	bool		CheckStatus=(GetCheck(item))?false:true;

	// Startup the tree traversing code
	TraverseModel(true, item);
	// Select the item
	SetCheck(item, CheckStatus);

	// Traverse thru all the tree
	while(item=TraverseModel(false, NULL, true)){
		// Select the item
		SetCheck(item, CheckStatus);
		// and if the item points to something solid
		DXNode	*Node=(DXNode*)GetItemData(item);
		// Select it
		if(Node) E_SelectNode(m_SelectList, Node, CheckStatus);
	}
	UpdateTreeView();
}




// This function selects all items
void CDXTree::TreeSelectAll(void)
{
	HTREEITEM	item, DelItem=NULL;

	// Startup the tree traversing code
	TraverseModel(true);

	// Traverse thru all the tree
	while(item=TraverseModel(false)){
		// Select the item
		SetCheck(item, true);
		// and if the item points to something solid
		DXNode	*Node=(DXNode*)GetItemData(item);
		// Select it
		if(Node && (!Node->SelectID)) E_SelectNode(m_SelectList, Node, true);
	}
	UpdateTreeView();
}


////////////////////////////////// DX TREE MANAGEMENT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void CDXTree::OnLButtonDown( UINT nFlags, CPoint point )
{
	m_MouseMove=false;
	TVHITTESTINFO	Test;
	Test.pt=point;
	HTREEITEM item=HitTest(&Test);
	DXNode *Node=(DXNode*)GetItemData(item);
	if(Node && Node->Type==MODELEND) { SetCheck(item, false); return; }
	if(Test.flags & (TVHT_ONITEMSTATEICON)){
		SetCheck(item, GetCheck(item));
		TreeSelect(item);
		return;
	}
	CTreeCtrl::OnLButtonDown( nFlags, point );
	//UpdateTreeView();
}


void CDXTree::OnRButtonDown( UINT nFlags, CPoint point )
{
	TVHITTESTINFO	Test;
	Test.pt=point;
	HTREEITEM item=HitTest(&Test);
	if(item){
		SelectItem(item);
		DXNode *node=(DXNode*) GetItemData(item);
		if(!node) return;
		if(node->Type==DOF){
			if(((DXDof*)node)->dof.Type==SWITCH || ((DXDof*)node)->dof.Type==XSWITCH) SwitchesPanel->ActivateWindow(((DXDof*)node), ((DXDof*)node)->dof.SwitchNumber);
			else TheDofEditor->EditDof(m_Object, (DXDof*)node);
		}
	}
}


void CDXTree::OnLButtonUp( UINT nFlags, CPoint point )
{
	
	TVHITTESTINFO	Test;
	Test.pt=point;
	HTREEITEM item=HitTest(&Test);
	
}

void CDXTree::CompressSelectedBranch(void)
{
	HTREEITEM item=GetSelectedItem();
	if(!item) return;
	if(!GetChildItem(item)) item=GetParentItem(item);
	if(item) Expand(item, TVE_COLLAPSE);
}


void CDXTree::RemoteSelectNode(HTREEITEM item, bool Status)
{
	SetCheck(item, Status);
	m_LastItem=item;
}


void CDXTree::RemoteRemoveNode(HTREEITEM item)
{
	DeleteItem(item);
}


void CDXTree::RemoteAddNode(DXNode *Node)
{
	AddNode(Node);
}


void CDXTree::ViewNode(HTREEITEM item)
{
	UpdateTreeView();
	if(m_ExpOnSel){
		if(item) EnsureVisible(item);
		else if(m_LastItem) EnsureVisible(m_LastItem);
	}
}


void CDXTree::OnMouseMove( UINT nFlags, CPoint point )
{
	m_MouseMove=true;
}


void CTreePanel::OnBnClickedCollapset()
{
	// TODO: Add your control notification handler code here
	TheTree[m_SelectedModel]->UpdateTreeView(true);
}

void CTreePanel::OnBnClickedExponsel()
{
	// TODO: Add your control notification handler code here
	BOOL	Status=m_ExpOnSel.GetCheck();
	for(DWORD a=0; a<MAX_EDITABLE_OBJECTS; a++) TheTree[a]->m_ExpOnSel=Status;
}

void CTreePanel::OnBnClickedClearsel()
{
	// TODO: Add your control notification handler code here
	TheTree[m_SelectedModel]->ClearSelection();

}

void CTreePanel::OnBnClickedCollapseb()
{
	// TODO: Add your control notification handler code here
	TheTree[m_SelectedModel]->CompressSelectedBranch();
}

void CDXTree::OnEditCut()
{
	// check if a balanced selection
	if(!E_CheckBalancedSelection(m_SelectList)){
		int result=MessageBox("Warning !!! Unbalanced selection,\nNOT ALL DOFs/SWITCHES are closed,\ngoing ahead may corrupt model...\n\nDo you want to continue ?","Unbalanced Selection !",MB_OKCANCEL|MB_ICONEXCLAMATION);
		if(result==IDCANCEL) return;
	}
	E_CutSelection(m_Object, m_SelectList);
	UpdateBranches();
}


void CDXTree::CopySelection()
{
	E_CopySelection(m_SelectList);
}


void CDXTree::KillSelection()
{
	// check if a balanced selection
	if(!E_CheckBalancedSelection(m_SelectList)){
		int result=MessageBox("Warning !!! Unbalanced selection,\nNOT ALL DOFs/SWITCHES are closed,\ngoing ahead may corrupt model...\n\nDo you want to continue ?","Unbalanced Selection !",MB_OKCANCEL|MB_ICONEXCLAMATION);
		if(result==IDCANCEL) return;
	}
	E_KillSelection(m_Object, m_SelectList);
	UpdateBranches();
}


void CDXTree::OnPaste()
{
	// Get the selected item
	HTREEITEM	item=GetSelectedItem(), Child;
	// get the pointed node
	DXNode	*Node=(DXNode*)GetItemData(item);
	// if not a Node it is a Surface Descriptor, then look for the end of it to add after it
	while(!Node) {
		// get the 1st triangle of surface
		Child=GetChildItem(item);
		// if Surface got childs
		if(Child){
			// Follow the children branch till end
			item=Child;
			while(item){
				item=GetNextSiblingItem(Child);
				// if a new item go to next
				if(item) Child=item;
				// else this is the Last Node
				else Node=(DXNode*)GetItemData(Child);
			}
			// restore last avlid pointer
			item=Child;
		}
	}

	
	if(Node) E_PasteSelection(m_Object, m_SelectList, Node);
}



void CTreePanel::OnBnClickedCut()
{
	// TODO: Add your control notification handler code here
	TheTree[m_SelectedModel]->OnEditCut();
}

void CTreePanel::OnBnClickedPaste()
{
	// TODO: Add your control notification handler code here
	TheTree[m_SelectedModel]->OnPaste();
}

IMPLEMENT_DYNAMIC(CTreePanel, CDialog)
CTreePanel::CTreePanel(CWnd* pParent /*=NULL*/)
	: CDialog(CTreePanel::IDD, pParent)
{
	Create(IDD, pParent);
	ShowWindow(SW_HIDE);
	RECT	Area;
	m_ModelArea.GetClientRect(&Area);

	for(DWORD a=0; a<MAX_EDITABLE_OBJECTS; a++)
		TheTree[a]=new CDXTree(&m_ModelArea, Area);

}

CTreePanel::~CTreePanel()
{
}

void CTreePanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MODELAREA, m_ModelArea);
	DDX_Control(pDX, IDC_EXPONSEL, m_ExpOnSel);
}


void CTreePanel::UpdateModel(E_Object *Obj, DWORD Index, CSelectList *SelectList)
{
	TheTree[Index]->UpdateModel(Obj, SelectList);
	CString	Head;
	Head.Format("Draws : %d  Surfaces : %d", TheTree[Index]->DrawCalls(), TheTree[Index]->Surfaces());
	SetWindowText(Head);
}

void CTreePanel::ViewNode(HTREEITEM item)
{
	TheTree[m_SelectedModel]->ViewNode(item);
}

void CTreePanel::ShowModelTree(DWORD Index)
{
	ShowWindow(SW_SHOW);
	SelectModel(Index);
}



void CTreePanel::SelectModel(DWORD Index)
{	
	m_SelectedModel=Index;
	if(!IsWindowVisible()) return;
	for(DWORD a=0; a<MAX_EDITABLE_OBJECTS; a++) {
		TheTree[a]->ShowWindow(SW_HIDE);
		TheTree[a]->EnableWindow(false);
	}
	TheTree[Index]->EnableWindow(true);
	TheTree[Index]->ShowWindow(SW_SHOW);
	CString	Head;
	Head.Format("Draws : %d  Surfaces : %d", TheTree[Index]->DrawCalls(), TheTree[Index]->Surfaces());
	SetWindowText(Head);

}

void CTreePanel::RebuildBranch(HTREEITEM item)
{
	if(item)TheTree[m_SelectedModel]->RebuildBranch(item);
}


void CTreePanel::SelectNode(HTREEITEM item, bool Status)
{
	if(item)TheTree[m_SelectedModel]->RemoteSelectNode(item, Status);
}


void CTreePanel::RemoveNode(HTREEITEM item)
{
	if(item)TheTree[m_SelectedModel]->RemoteRemoveNode(item);
}

void CTreePanel::AddNode(DXNode *Node)
{
	if(Node)TheTree[m_SelectedModel]->RemoteAddNode(Node);
}


BEGIN_MESSAGE_MAP(CTreePanel, CDialog)
	ON_BN_CLICKED(IDC_COLLAPSET, OnBnClickedCollapset)
	ON_BN_CLICKED(IDC_EXPONSEL, OnBnClickedExponsel)
	ON_BN_CLICKED(IDC_CLEARSEL, OnBnClickedClearsel)
	ON_BN_CLICKED(IDC_COLLAPSEB, OnBnClickedCollapseb)
	ON_BN_CLICKED(IDC_CUT, OnBnClickedCut)
	ON_BN_CLICKED(IDC_PASTE, OnBnClickedPaste)
	ON_BN_CLICKED(IDC_REBUILDTREE, OnBnClickedRebuildtree)
	ON_BN_CLICKED(IDC_COPY, OnBnClickedCopy)
	ON_BN_CLICKED(IDC_TREEALL, OnBnClickedTreeall)
	ON_BN_CLICKED(IDC_KILL, OnBnClickedKill)
	ON_BN_CLICKED(IDC_ADDSW, OnBnClickedAddsw)
END_MESSAGE_MAP()



void CTreePanel::UpdateTree()
{
	TheTree[m_SelectedModel]->UpdateTree();
}


void CTreePanel::OnBnClickedRebuildtree()
{
	// resort the model
	TheTree[m_SelectedModel]->Object()->Nodes=(void*)E_SortModel((DXNode*)TheTree[m_SelectedModel]->Object()->Nodes);

	TheTree[m_SelectedModel]->UpdateTree();
	CString	Head;
	Head.Format("Draws : %d  Surfaces : %d", TheTree[m_SelectedModel]->DrawCalls(), TheTree[m_SelectedModel]->Surfaces());
	SetWindowText(Head);
}

void CTreePanel::OnBnClickedCopy()
{
	TheTree[m_SelectedModel]->CopySelection();
}

void CTreePanel::UpdateItemLabel(HTREEITEM item)
{
	TheTree[m_SelectedModel]->UpdateItemLabel(item);
}

void CTreePanel::OnBnClickedTreeall()
{
	TheTree[m_SelectedModel]->TreeSelectAll();
}

void CTreePanel::OnBnClickedKill()
{
	TheTree[m_SelectedModel]->KillSelection();
}

void CTreePanel::OnBnClickedAddsw()
{
	// Get the selected item
	HTREEITEM	item=TheTree[m_SelectedModel]->GetSelectedItem(), Child;
	// get the pointed node
	DXNode	*Paste=(DXNode*)TheTree[m_SelectedModel]->GetItemData(item);
	DXDof		*Node;
	DXEndDof	*End;

	Node=new DXDof();
	Node->dof.Type=SWITCH;
	Node->dof.SwitchBranch=Node->dof.SwitchNumber=0;
	E_InsertNode(TheTree[m_SelectedModel]->Object(), Paste, (DXNode*)Node);
	Paste=(DXNode*)Node;
	End=new DXEndDof();
	E_InsertNode(TheTree[m_SelectedModel]->Object(), Paste, (DXNode*)End);


	TheTree[m_SelectedModel]->UpdateTree();
}
