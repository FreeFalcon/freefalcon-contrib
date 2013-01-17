#include "stdafx.h"
#include "LodConvert.h"
#include "editing.h"

CEditing::CEditing(void *Owner, CEditing *Data)
{
	m_List=Owner;
	if(Data){
		Node=Data->Node;
		EditID=Data->EditID;
		Prev=Next=NULL;
		if(m_List && IS_SURFACE(Node))
			((CSelectList*)m_List)->UpdateValues(Node, ADD);
	}
}

CEditing::~CEditing(void)
{
	if(m_List && IS_SURFACE(Node)){
		CSelectList	*List=((CSelectList*)m_List);
		List->UpdateValues(Node, REMOVE);
	}
}



void	CSelectList::UpdateTexUsedList(void)
{
	m_TexUsedNr=0;
	memset(m_TexUsedList, -1, sizeof(m_TexUsedList));
	for(DWORD a=0; a<MAX_TEXTURES_X_SET; a++){
		if(m_TexUsedCount[a]) m_TexUsedList[m_TexUsedNr++]=a;
	}

}



void	CSelectList::UpdateValues(DXNode *Node, bool Add)
{
	if(Add){
		for(DWORD a=0; a<32; a++) if(SURFACE(Node).dwFlags.w & (1<<a)) m_FlagsCount[a]++;
		m_SurfacesNr++;
		if(SURFACE(Node).dwFlags.b.zBias && SURFACE(Node).dwzBias>m_MaxZBias) m_MaxZBias=SURFACE(Node).dwzBias;
		if(SURFACE(Node).dwFlags.b.zBias && SURFACE(Node).dwzBias<m_MinZBias) m_MinZBias=SURFACE(Node).dwzBias;
		if(SURFACE(Node).SpecularIndex>m_MaxSI) m_MaxSI=SURFACE(Node).SpecularIndex;
		if(SURFACE(Node).SpecularIndex<m_MinSI) m_MinSI=SURFACE(Node).SpecularIndex;
		if(SURFACE(Node).dwFlags.b.Texture && SURFACE(Node).TexID[0]!=-1) m_TexUsedCount[SURFACE(Node).TexID[0]]++;
		
		D3DVERTEXEX		*Vertex=((DXTriangle*)Node)->Vertex;
		for(DWORD a=0; a<SURFACE(Node).dwVCount; a++){
			if(Vertex[a].vx>m_MaxX) m_MaxX=Vertex[a].vx;
			if(Vertex[a].vx<m_MinX) m_MinX=Vertex[a].vx;
			if(Vertex[a].vy>m_MaxY) m_MaxY=Vertex[a].vy;
			if(Vertex[a].vy<m_MinY) m_MinY=Vertex[a].vy;
			if(Vertex[a].vz>m_MaxZ) m_MaxZ=Vertex[a].vz;
			if(Vertex[a].vz<m_MinZ) m_MinZ=Vertex[a].vz;
			if(SURFACE(Node).dwFlags.b.Texture && SURFACE(Node).TexID[0]!=-1){
				if(Vertex[a].tu<m_MinTU) m_MinTU=Vertex[a].tu;
				if(Vertex[a].tu>m_MaxTU) m_MaxTU=Vertex[a].tu;
				if(Vertex[a].tv<m_MinTV) m_MinTV=Vertex[a].tv;
				if(Vertex[a].tv>m_MaxTV) m_MaxTV=Vertex[a].tv;
			}
		}

	} else {
		for(DWORD a=0; a<32; a++) if(SURFACE(Node).dwFlags.w & (1<<a)) m_FlagsCount[a]--;
		if(SURFACE(Node).SpecularIndex==m_MaxSI || SURFACE(Node).SpecularIndex==m_MinSI) m_Recalc=true;
		if(SURFACE(Node).dwFlags.b.zBias && (SURFACE(Node).dwzBias==m_MaxZBias || SURFACE(Node).dwzBias==m_MinZBias))
				m_Recalc=true;

		D3DVERTEXEX		*Vertex=((DXTriangle*)Node)->Vertex;
		for(DWORD a=0; a<SURFACE(Node).dwVCount; a++){
			if(Vertex[a].vx==m_MaxX) m_Recalc=true;
			if(Vertex[a].vx==m_MinX) m_Recalc=true;
			if(Vertex[a].vy==m_MaxY) m_Recalc=true;
			if(Vertex[a].vy==m_MinY) m_Recalc=true;
			if(Vertex[a].vz==m_MaxZ) m_Recalc=true;
			if(Vertex[a].vz==m_MinZ) m_Recalc=true;
			if(SURFACE(Node).dwFlags.b.Texture && SURFACE(Node).TexID[0]!=-1){
				if(Vertex[a].tu==m_MinTU) m_Recalc=true;
				if(Vertex[a].tu==m_MaxTU) m_Recalc=true;
				if(Vertex[a].tv==m_MinTV) m_Recalc=true;
				if(Vertex[a].tv==m_MaxTV) m_Recalc=true;
			}
		}

		if(SURFACE(Node).dwFlags.b.Texture  && SURFACE(Node).TexID[0]!=-1 && m_TexUsedCount[SURFACE(Node).TexID[0]]) m_TexUsedCount[SURFACE(Node).TexID[0]]--;
		m_SurfacesNr--;
	}
}


void	CSelectList::RecalcValues(void)
{
	if(!m_Start) return;
	CEditing	*ptr=m_Start;

	m_SurfacesNr=0;
	m_MaxZBias=0;
	m_MinZBias=16;
	m_MaxSI=0.0f;
	m_MinSI=999999.9f;
	m_MaxX=m_MaxY=m_MaxZ=-9999999999.9f;
	m_MinX=m_MinY=m_MinZ=9999999999.9f;
	m_MinTU=m_MinTV=999999.9f;
	m_MaxTU=m_MaxTV=-9999999.9f;
	memset(m_FlagsCount, 0, sizeof(m_FlagsCount));
	memset(m_TexUsedCount, 0, sizeof(m_TexUsedCount));
	while(ptr){
		if(IS_SURFACE(ptr->Node)) UpdateValues(ptr->Node, ADD);
		ptr=ptr->Next;
	}
	UpdateTexUsedList();
}




DWORD	CSelectList::AddSelection(CEditing *Item)
{
	CEditing	*ptr=new CEditing(this, Item);
	if(!m_LastID) m_LastID=1;
	ptr->EditID=m_LastID++;

	if(!m_Start){
		ptr->Prev=NULL;
		m_Start=m_Last=ptr;
		RecalcValues();
	} else {
		ptr->Prev=m_Last;
		m_Last->Next=ptr;
		m_Last=ptr;
	}

	// Tree selection
	TheTreeViews->SelectNode(Item->Node->TreeItem, true);
	ptr->Next=NULL;
	return	m_Last->EditID;
}


void	CSelectList::RemoveSelection(DWORD ID)
{
	if(!m_Start) return;
	CEditing	*ptr=m_Start;
	while(ptr){
		if(ptr->EditID==ID){
			// Tree Deselection
			if(ptr->Node->Type==DOT || ptr->Node->Type==LINE || ptr->Node->Type==TRIANGLE && (!((DXTriangle*)ptr->Node)->Surface.dwFlags.b.Disable)){
				TheTreeViews->SelectNode(ptr->Node->TreeItem, false);
				if(ptr->Node->SelectV){
					for(DWORD	a=0; a<SURFACE(ptr->Node).dwVCount; a++)
						RemoveVertex(ptr->Node, a);
				}
				if(ptr->Prev) ptr->Prev->Next=ptr->Next;
				else m_Start=ptr->Next;
				if(ptr->Next) ptr->Next->Prev=ptr->Prev;
				if(ptr==m_Last) m_Last=ptr->Prev;
				delete ptr;
				return;
			}
		}
		ptr=ptr->Next;
	}
}



void	CSelectList::ClearSelection(void)
{
	ClearVertexList();
	if(!m_Start) return;
	NodeType	Type;
	CEditing	*ptr=m_Start, *Next;
	while(ptr){
		// Tree Deselection
		Type=ptr->Node->Type;
		Next=ptr->Next;
		if((Type!=DOT && Type!=LINE && Type!=TRIANGLE) || (!((DXTriangle*)ptr->Node)->Surface.dwFlags.b.Disable)){
			TheTreeViews->SelectNode(ptr->Node->TreeItem, false);
			if(Type==DOT || Type==LINE || Type==TRIANGLE)((DXTriangle*)ptr->Node)->Surface.dwFlags.b.Frame=false;
			ptr->Node->SelectID=0;
			if(ptr->Prev) ptr->Prev->Next=ptr->Next;
			else m_Start=ptr->Next;
			if(ptr->Next) ptr->Next->Prev=ptr->Prev;
			if(ptr==m_Last) m_Last=ptr->Prev;
			delete ptr;
		}
		ptr=Next;
	}
	//TheTreeViews->ViewNode(NULL);
	//m_Start=NULL;
}


void	CSelectList::HideSelection(void)
{
	if(!m_Start) return;
	NodeType	Type;
	CEditing	*ptr=m_Start;
	while(ptr){
		if(IS_SURFACE(ptr->Node)) UpdateValues(ptr->Node, REMOVE);
		Type=ptr->Node->Type;
		if(Type==DOT || Type==LINE || Type==TRIANGLE)
			((DXTriangle*)ptr->Node)->Surface.dwFlags.b.Disable=true;
		ptr=ptr->Next;
	}
}


void	CSelectList::UnHideSelection(void)
{
	if(!m_Start) return;
	NodeType	Type;
	CEditing	*ptr=m_Start;
	while(ptr){
		if(IS_SURFACE(ptr->Node)) UpdateValues(ptr->Node, ADD);
		Type=ptr->Node->Type;
		if(Type==DOT || Type==LINE || Type==TRIANGLE)
			((DXTriangle*)ptr->Node)->Surface.dwFlags.b.Disable=false;
		ptr=ptr->Next;
	}
}


CEditing	CSelectList::GetItemFromList(void)
{
	CEditing	Result;

LookAgain:
	memset(&Result, 0, sizeof(Result));

	if(m_Traverse){
		Result=*m_Traverse;
		m_Traverse=m_Traverse->Next;
		if((Result.Node->Type==DOT || Result.Node->Type==LINE || Result.Node->Type==TRIANGLE) && ((DXTriangle*)Result.Node)->Surface.dwFlags.b.Disable)
			goto LookAgain;
	}
	return Result;
}

void	CSelectList::GetSelectedFlags(DXFlagsType *Flags_1, DXFlagsType *Flags_2)
{
	// Get the start of list
	CEditing	*ptr=m_Start;
	// Reset the Flags
	Flags_1->w=Flags_2->w=0;

	if(m_Recalc) RecalcValues();
	m_Recalc=false;

	for(DWORD	a=0; a<32; a++){
		if(m_FlagsCount[a]){
			if(m_FlagsCount[a]==m_SurfacesNr) Flags_1->w|=1<<a;
			else Flags_2->w|=1<<a;
		}
	}

}


void	CSelectList::AddVertex(DXNode *Node, DWORD Nr)
{
	if(Node->SelectV & (1<<Nr)) return;
	DWORD	a=0;
	while(a<MAX_EDITABLE_VERTICES){
		if(!m_VList[a].Node){
			m_VList[a].Node=Node;
			m_VList[a].Nr=Nr;
			Node->SelectV|=1<<Nr;
			return;
		}
		a++;
	}
}


void	CSelectList::RemoveVertex(DXNode *Node, DWORD Nr)
{
	if(!(Node->SelectV & (1<<Nr))) return;

	DWORD a=0;
	while(a<MAX_EDITABLE_VERTICES){
		if(m_VList[a].Node==Node && m_VList[a].Nr==Nr){
			m_VList[a].Node->SelectV&=~(1<<Nr);
			m_VList[a].Node=0;
			return;
		}
		a++;
	}
}

void	CSelectList::ClearVertexList(void)
{	
	DWORD	a=0;
	while(a<MAX_EDITABLE_VERTICES){
		if(m_VList[a].Node){
			m_VList[a].Node->SelectV&=~(1<<m_VList[a].Nr);
			m_VList[a].Node=0;
		}
		a++;
	}
}


D3DVERTEXEX *CSelectList::GetVertexFromList(void)
{
	while(m_VertexNr<MAX_EDITABLE_VERTICES && (!m_VList[m_VertexNr].Node)) m_VertexNr++;

	if(m_VertexNr==MAX_EDITABLE_VERTICES) return NULL;
	
	return &((DXTriangle*)m_VList[m_VertexNr].Node)->Vertex[m_VList[m_VertexNr++].Nr];
}


void	CSelectList::GetBoundVertices( DWORD PlaneMode, D3DVERTEXEX *VertexArray, DxSurfaceType *Surface)
{
	if(!m_Start) return;
	CEditing	*ptr=m_Start;
	while(ptr){
		if(IS_SURFACE(ptr->Node) && ptr->Node->Type==TRIANGLE){
			DXTriangle *S=(DXTriangle*)ptr->Node;
			DWORD	VertexNr=S->Surface.dwVCount;
			for(DWORD i=0; i<VertexNr; i++){
				VertexArray[i]=S->Vertex[i];
				/*while(VertexArray[i].tu>1.0f) VertexArray[i].tu-=2.0f;
				while(VertexArray[i].tu<-1.0f) VertexArray[i].tu+=2.0f;
				while(VertexArray[i].tv>1.0f) VertexArray[i].tv-=2.0f;
				while(VertexArray[i].tv<-1.0f) VertexArray[i].tv+=2.0f;*/
			}
			*Surface=((DXTriangle*)ptr->Node)->Surface;
			return;
		}
		ptr=ptr->Next;
	}
}

/*
void	CSelectList::AssignUV( DWORD PlaneMode, DWORD Texture, D3DVERTEXEX *VA, DxSurfaceType *Surface)
{
	if(!m_Start) return;
	PlaneType   Plane=(PlaneType)PlaneMode;

	double	OffsetU, VSX, VSY;
	double	OffsetV, USX, USY;
	double	XRef, YRef;

	switch(Plane){

		case	 PLANE_X	:		OffsetU=VA[0].tu;
									OffsetV=VA[0].tv;
									XRef=VA[0].vz;
									YRef=VA[0].vy;

									break;
		case	 PLANE_Y	:		OffsetU=VA[0].tu;
									OffsetV=VA[0].tv;
									XRef=VA[0].vz;
									YRef=VA[0].vx;
									break;
		case	 PLANE_Z	:		// choose 2 vertices with both different X & y
									DWORD i=0,n=0;
									while(VA[i].vx==VA[n].vx || VA[i].vy==VA[n].vy ){
										n++;
										if(n==3) { n=0; i++; }
									}
									XRef=VA[i].vx;
									YRef=VA[n].vy;

									VSX=(VA[n].tv-VA[i].tv)/(VA[n].vx-VA[i].vx);
									USX=(VA[n].tu-VA[i].tu)/(VA[n].vx-VA[i].vx);
									VSY=(VA[n].tv-VA[i].tv)/(VA[n].vy-VA[i].vy);
									USY=(VA[n].tu-VA[i].tu)/(VA[n].vy-VA[i].vy);
									
									OffsetU=VA[n].tu;
									OffsetV=VA[i].tv;
									break;
	}

	CEditing	*ptr=m_Start;
	while(ptr){
		if(IS_SURFACE(ptr->Node) && ptr->Node->Type==TRIANGLE){
			
			DXTriangle *S=(DXTriangle*)ptr->Node;
			S->Surface.dwFlags.b.Texture=1;
			S->Surface.TexID[0]=Texture;
			S->Surface.DefaultSpecularity=Surface->DefaultSpecularity;
			S->Surface.SpecularIndex=Surface->SpecularIndex;

			DWORD	VertexNr=S->Surface.dwVCount;
			for(DWORD i=0; i<VertexNr; i++){

				S->Vertex[i].dwColour=VA[0].dwColour;
									
				switch(Plane){

					case	 PLANE_X	:	break;
					
					case	 PLANE_Y	:	break;
					
					case	 PLANE_Z	:	
											double	dx=(S->Vertex[i].vx-XRef);
											double	dy=(S->Vertex[i].vy-YRef);

											S->Vertex[i].tu=dy*USY+dx*USX;
											S->Vertex[i].tv=dy*VSY+dx*VSX;

											break;
				}
			}
		}
		ptr=ptr->Next;

	}
}


*/

/*
void	CSelectList::AssignUV( DWORD PlaneMode, DWORD Texture, D3DVERTEXEX *VA, DxSurfaceType *Surface)
{
	if(!m_Start) return;
	PlaneType   Plane=(PlaneType)PlaneMode;

	double	OffsetU, VSX, VSY;
	double	OffsetV, USX, USY;
	double	XRef, YRef;

	switch(Plane){

		case	 PLANE_X	:		OffsetU=VA[0].tu;
									OffsetV=VA[0].tv;
									XRef=VA[0].vz;
									YRef=VA[0].vy;

									break;
		case	 PLANE_Y	:		OffsetU=VA[0].tu;
									OffsetV=VA[0].tv;
									XRef=VA[0].vz;
									YRef=VA[0].vx;
									break;

		case	 PLANE_Z	:		OffsetU=VA[0].tu;
									OffsetV=VA[0].tv;
									XRef=VA[0].vx;;
									YRef=VA[0].vy;
									break;
	}


	double	AlphaTex, AlphaSurface;
	AlphaTex=atan2( VA[0].tu - VA[1].tu, VA[0].tv - VA[1].tv );
	AlphaSurface=atan2( VA[0].vx - VA[1].vx, VA[0].vy - VA[1].vy );
	double	Rate= sqrt((VA[0].tu - VA[1].tu)*(VA[0].tu - VA[1].tu) + (VA[0].tv - VA[1].tv)*(VA[0].tv - VA[1].tv));
	Rate/=sqrt((VA[0].vx - VA[1].vx)*(VA[0].vx - VA[1].vx) + (VA[0].vy - VA[1].vy)*(VA[0].vy - VA[1].vy));
	double	DeltaAlpha=AlphaTex-AlphaSurface;

	CEditing	*ptr=m_Start;
	while(ptr){
		if(IS_SURFACE(ptr->Node) && ptr->Node->Type==TRIANGLE){
			
			DXTriangle *S=(DXTriangle*)ptr->Node;
			S->Surface.dwFlags.b.Texture=1;
			S->Surface.TexID[0]=Texture;
			S->Surface.DefaultSpecularity=Surface->DefaultSpecularity;
			S->Surface.SpecularIndex=Surface->SpecularIndex;

			DWORD	VertexNr=S->Surface.dwVCount;
			for(DWORD i=0; i<VertexNr; i++){

				S->Vertex[i].dwColour=VA[0].dwColour;
									
				switch(Plane){

					case	 PLANE_X	:	break;
					
					case	 PLANE_Y	:	break;
					
					case	 PLANE_Z	:	
											double	dx=(XRef-S->Vertex[i].vx);
											double	dy=(YRef-S->Vertex[i].vy);
											double	TexAngle=atan2(dx, dy)+ DeltaAlpha;
											double	Len=sqrt(dx*dx+dy*dy);

											S->Vertex[i].tu=OffsetU + cos(TexAngle) * Rate * Len;
											S->Vertex[i].tv=OffsetV + sin(TexAngle) * Rate * Len;

											break;
				}
			}
		}
		ptr=ptr->Next;

	}
}
*/

void	CSelectList::AssignUV( DWORD PlaneMode, DWORD Texture, D3DVERTEXEX *VA, DxSurfaceType *Surface)
{
	if(!m_Start) return;

	D3DXMATRIX	Tri, Tex, Inv, M;
	D3DXVECTOR3	Point, UV;


	Tri.m00=VA[0].vx, Tri.m01=VA[1].vx, Tri.m02=VA[2].vx, Tri.m03=0.0f;
	Tri.m10=VA[0].vy, Tri.m11=VA[1].vy, Tri.m12=VA[2].vy, Tri.m13=0.0f;
	Tri.m20=1, Tri.m21=1, Tri.m22=1, Tri.m23=0.0f;
	Tri.m30=0, Tri.m31=0, Tri.m32=0, Tri.m33=1.0f;

	Tex.m00=VA[0].tu, Tex.m01=VA[1].tu, Tex.m02=VA[2].tu, Tex.m03=0.0f;
	Tex.m10=VA[0].tv, Tex.m11=VA[1].tv, Tex.m12=VA[2].tv, Tex.m13=0.0f;
	Tex.m20=1, Tex.m21=1, Tex.m22=1, Tex.m23=0.0f;
	Tex.m30=0, Tex.m31=0, Tex.m32=0, Tex.m33=0.0f;

	D3DXMatrixInverse(&Inv, NULL, &Tri);
	D3DXMatrixMultiply(&M, &Tex, &Inv);

	D3DXMatrixMultiply(&Tex, &M, &Tri);


	CEditing	*ptr=m_Start;
	while(ptr){
		if(IS_SURFACE(ptr->Node) && ptr->Node->Type==TRIANGLE){
			
			DXTriangle *S=(DXTriangle*)ptr->Node;
			S->Surface.dwFlags.b.Texture=1;
			S->Surface.TexID[0]=Texture;
			S->Surface.DefaultSpecularity=Surface->DefaultSpecularity;
			S->Surface.SpecularIndex=Surface->SpecularIndex;

			DWORD	VertexNr=S->Surface.dwVCount;
			for(DWORD i=0; i<VertexNr; i++){

				S->Vertex[i].dwColour=VA[0].dwColour;
					
				Tri.m00=S->Vertex[i].vx, Tri.m01==1.0f, Tri.m02==1.0f, Tri.m03==0.0f;
				Tri.m10=S->Vertex[i].vy, Tri.m11==1.0f, Tri.m12==1.0f, Tri.m13==0.0f;
				Tri.m20=1, Tri.m21==1.0f, Tri.m22==1.0f, Tri.m23==0.0f;
				Tri.m30=0, Tri.m31==0.0f, Tri.m32==0.0f, Tri.m33==0.0f;

				D3DXMatrixMultiply(&Tex, &M, &Tri);

				S->Vertex[i].tu=Tex.m00;
				S->Vertex[i].tv=Tex.m10;

			}
		}
		ptr=ptr->Next;

	}
}
