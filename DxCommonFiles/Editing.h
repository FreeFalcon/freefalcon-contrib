#pragma once
#include	"DxDefines.h"
#include	"Dxnodes.h"
#define	MAX_EDITABLE_VERTICES	(128 * 1024)


class CEditing
{
public:
	CEditing(void *Owner = NULL, CEditing * = NULL);
	virtual ~CEditing(void);
public:	
	void			*m_List;
	CEditing		*Prev, *Next;
	DXNode			*Node;
	DWORD			EditID;
};

typedef	struct	{
	DXNode		*Node;
	DWORD		Nr;
}	VItem;



class CSelectList
{
public:
	CSelectList(void) { m_Start = m_Last=NULL; m_LastID=0; memset(m_VList, 0, sizeof(m_VList)); };
	virtual ~CSelectList(void) {};
	
	DWORD		AddSelection(CEditing *Item);
	void		RemoveSelection(DWORD ID);
	CEditing	*GetList(void) { return m_Start; }
	void		ClearSelection(void);
	void		HideSelection(void);
	void		UnHideSelection(void);
	void		ViewTreeSelection(void);
	void		StartParseList(void)	{ m_Traverse=m_Start; } ;
	CEditing	GetItemFromList(void);
	void		GetBoundVertices( DWORD PlaneMode, D3DVERTEXEX *VertexArray, DxSurfaceType *Surface);
	void		AssignUV( DWORD PlaneMode, DWORD Texture, D3DVERTEXEX *VertexArray, DxSurfaceType *Surface);

	void		GetSelectedFlags(DXFlagsType *AndFlags, DXFlagsType *ORFlags);
	void		GetZBiasValues(DWORD *Min, DWORD *Max) { *Min=m_MinZBias; *Max=m_MaxZBias; }
	void		GetSIValues(float *Min, float *Max) { *Min=m_MinSI; *Max=m_MaxSI; }
	void		UpdateValues(DXNode *Node, bool Add);
	void		RecalcValues();
	DWORD		m_MinZBias, m_MaxZBias;
	float		m_MinSI, m_MaxSI;
	bool		m_Recalc;
	float		m_MinZ, m_MaxZ, m_MinX, m_MaxX, m_MinY, m_MaxY;
	float		m_MinTU, m_MaxTU, m_MinTV, m_MaxTV;
	void		AddVertex(DXNode *Node, DWORD Nr);
	void		RemoveVertex(DXNode *Node, DWORD Nr);
	void		ClearVertexList(void);
	void		StartParseVertex(void)	{ m_VertexNr=0; } ;
	D3DVERTEXEX	*GetVertexFromList(void);
	void		UpdateTexUsedList(void);
	DWORD		m_TexUsedList[32];


private:
	CEditing	*m_Start, *m_Last;
	CEditing	*m_Traverse;
	CEditing	*LastSelection;
	DWORD		m_LastID;
	DWORD		m_VertexNr;
	DWORD		m_TexUsedCount[32];
	DWORD		m_TexUsedNr;
	DWORD		m_FlagsCount[32],m_SurfacesNr;

	VItem	m_VList[MAX_EDITABLE_VERTICES];
};


#define	HIDDEN_SURFACE(x)	(((DXTriangle*)x)->Surface.dwFlags.b.Disable)
#define	IS_SURFACE(x)		(x->Type==DOT || x->Type==LINE || x->Type==TRIANGLE)
#define	SURFACE(x)			((DXTriangle*)x)->Surface
#define	ADD					true
#define	REMOVE				false
#define	IS_VALID_SURFACE(x)	(IS_SURFACE(x) && (!HIDDEN_SURFACE(x)))
