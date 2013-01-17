#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDXTree dialog

class CDXTree : public CTreeCtrl
{
	DECLARE_DYNAMIC(CDXTree)

public:
	CDXTree(CWnd* pParent, RECT Area);						// standard constructor
	virtual ~CDXTree();
	void UpdateModel(E_Object *Obj, CSelectList *SelectList);
	void UpdateTree(void);
	void RemoteSelectNode(HTREEITEM item, bool Status);
	void ViewNode(HTREEITEM item = NULL);
	void UpdateTreeView(bool Compress=false);
	void ClearSelection(void);
	void CompressSelectedBranch(void);
	void OnEditCut();
	void OnPaste();
	void RemoteRemoveNode(HTREEITEM item);
	void UpdateBranches(void);
	void RemoteAddNode(DXNode *Node);
	void AddNode(DXNode *Node, HTREEITEM Parent=NULL,  HTREEITEM After=TVI_LAST, bool Reset=false);
	void RebuildBranch(HTREEITEM Parent);
	void CopySelection();
	void UpdateItemLabel(HTREEITEM item);
	void TreeSelectAll(void);
	DWORD DrawCalls(void) {return m_DrawCalls; };
	DWORD Surfaces(void) {return m_Surfaces; };
	E_Object	*Object(void) { return m_Object; }
	void KillSelection();




// Dialog Data
	enum { IDD = IDD_DXL_STRUCTURE };

protected:
	HTREEITEM TraverseModel(bool Reset=false , HTREEITEM Start=NULL, bool StopOnRoot=false);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	E_Object	*m_Object;
	CImageList	m_ImageList;
	CBitmap		m_BitMap;
	CSelectList	*m_SelectList;
	void		TreeSelect(HTREEITEM);
	bool		m_MouseMove;
	HTREEITEM	m_RootItem, m_LastItem;
	BOOL		GetParentStatus(HTREEITEM item);
	DWORD		m_DrawCalls, m_Surfaces;



	DECLARE_MESSAGE_MAP()
public:
	BOOL		m_ExpOnSel;
//	CTreeCtrl m_Tree;
	afx_msg void OnLButtonDown( UINT nFlags, CPoint point );
	afx_msg void OnLButtonUp( UINT nFlags, CPoint point );
	afx_msg void OnMouseMove( UINT nFlags, CPoint point );
	afx_msg void OnRButtonDown( UINT nFlags, CPoint point );
};
#pragma once


#pragma once


// CTreePanel dialog

class CTreePanel : public CDialog
{
	DECLARE_DYNAMIC(CTreePanel)

public:
	CTreePanel(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTreePanel();
	void UpdateModel(E_Object *Obj, DWORD Index, CSelectList *SelectList);
	void ShowModelTree(DWORD Index);
	void SelectModel(DWORD Index);
	void SelectNode(HTREEITEM item, bool Status);
	void ViewNode(HTREEITEM item = NULL);
	void RemoveNode(HTREEITEM item);
	void AddNode(HTREEITEM Prev, HTREEITEM item);
	void AddNode(DXNode *Node);
	void RebuildBranch(HTREEITEM item);
	void UpdateTree();
	void UpdateItemLabel(HTREEITEM item);




// Dialog Data
	enum { IDD = IDD_TREEPANEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CDXTree		 *TheTree[MAX_EDITABLE_OBJECTS];
	DWORD	m_SelectedModel;
	DECLARE_MESSAGE_MAP()

public:
	CStatic m_ModelArea;
	CButton m_ExpOnSel;
	afx_msg void OnBnClickedCollapset();
	afx_msg void OnBnClickedExponsel();
	afx_msg void OnBnClickedClearsel();
	afx_msg void OnBnClickedCollapseb();
	afx_msg void OnBnClickedCut();
	afx_msg void OnBnClickedPaste();
	afx_msg void OnBnClickedRebuildtree();
	afx_msg void OnBnClickedCopy();
	afx_msg void OnBnClickedTreeall();
	afx_msg void OnBnClickedKill();
	afx_msg void OnBnClickedAddsw();
};
#pragma once



