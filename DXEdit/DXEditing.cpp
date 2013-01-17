#include "stdafx.h"
#include <math.h>
//#include "TimeMgr.h"
#include "ObjectInstance.h"
#include <dxdefines.h>
#include "DxTools.h"
#include "mltrig.h"
#include "dxengine.h"
#include "DXnodes.h"
#include "DXEditing.h"
#include "DXFiles.h"
#include "ProgressBar.h"
/////////////////////////////////////////// EDITING TOOL FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\


#undef sqrt
#undef sqrtf

float	Scaling=1;
DWORD	gDebugTextureID;
D3DVERTEXEX	*VCursorList[MAX_EDITABLE_VERTICES];
D3DXMATRIX	VCursorStates[MAX_EDITABLE_VERTICES];
DWORD		VCursorNr;
DXNode		*ClipBoard=NULL;


char	DrawBuffer[ sizeof(DxSurfaceType) + sizeof(D3DVERTEXEX) * MAX_BUFFERIZABLE_VERTEX_COUNT];
char	FrameBuffer[ sizeof(DxSurfaceType) + sizeof(D3DVERTEXEX) * MAX_BUFFERIZABLE_VERTEX_COUNT];
DWORD	DrawVertexNr, FrameVertexNr;
char	DrawPool[DRAW_POOL_SIZE];
char	FramePool[DRAW_POOL_SIZE];
DWORD	DrawPoolNr, FramePoolNr;

// Passed in DXFILES.CPP
//DWORD		Version;


void	E_SetScale(float Scale)
{
	Scaling=Scale;
}

// Passed in DXFILES.CPP

/*void	AssignVertex(E_Object *Obj, D3DVERTEXEX *Dest, D3DVERTEXEX *Source, Int16 *VIdx)
{
	*Dest=Source[*VIdx];
	if(Source[*VIdx].vx>Obj->DnRight.x) Obj->DnRight.x=Source[*VIdx].vx;
	if(Source[*VIdx].vy>Obj->DnRight.y) Obj->DnRight.y=Source[*VIdx].vy;
	if(Source[*VIdx].vz>Obj->DnRight.z) Obj->DnRight.z=Source[*VIdx].vz;

	if(Source[*VIdx].vx<Obj->UpLeft.x) Obj->UpLeft.x=Source[*VIdx].vx;
	if(Source[*VIdx].vy<Obj->UpLeft.y) Obj->UpLeft.y=Source[*VIdx].vy;
	if(Source[*VIdx].vz<Obj->UpLeft.z) Obj->UpLeft.z=Source[*VIdx].vz;
}


void	ClearModel(E_Object *Obj)
{
	E_Light	*DxL=Obj->LightList, *NDxL;

	while(DxL){
		NDxL=DxL->Next;
		delete DxL;
		DxL=NDxL;
	}


	DXNode	*Node, *NNode;
	Node=(DXNode*)Obj->Nodes;
	while(Node){
		NNode=Node->Next;
		delete Node;
		Node=NNode;
	}

	DX->UnLoadTextures(Obj->TexNr, Obj->Textures);

	Obj->NodeNr=0;
	Obj->Nodes=NULL;
	Obj->LightList=NULL;

	Obj->DnRight.x=Obj->DnRight.y=Obj->DnRight.z=0;
	Obj->UpLeft.x=Obj->UpLeft.y=Obj->UpLeft.z=0;
	Obj->TexNr=0;

	Obj->Name="";

}


void	E_CalculateFaceNormal(DXTriangle *Node)
{
	DOUBLEVECTOR	N, U, V;
	
	// calculate the 2 vectors from vertex 0 of the Triangle
	U.x=Node->Vertex[0].vx - Node->Vertex[1].vx;
	U.y=Node->Vertex[0].vy - Node->Vertex[1].vy;
	U.z=Node->Vertex[0].vz - Node->Vertex[1].vz;
	V.x=Node->Vertex[0].vx - Node->Vertex[2].vx;
	V.y=Node->Vertex[0].vy - Node->Vertex[2].vy;
	V.z=Node->Vertex[0].vz - Node->Vertex[2].vz;
	
	// Calculate the plane Normal
	N.x=U.y * V.z - U.z * V.y;
	N.y=U.z * V.x - U.x * V.z;
	N.z=U.x * V.y - U.y * V.x;

	// Normalize the Normal
	double	l=sqrt(N.x * N.x + N.y * N.y + N.z * N.z);
	N.x/=l; N.y/=l; N.z/=l;

	Node->FaceNormal.x=(float)N.x;
	Node->FaceNormal.y=(float)N.y;
	Node->FaceNormal.z=(float)N.z;
}

*/
// END OF Passed in DXFILES.CPP

/*
void E_CalculateModelVNormals(E_Object *Obj, float Limit, bool Selection, CSelectList *List)
{

	DOUBLEVECTOR	N;
	DWORD			Normals;

	DXNode		*Node;
	DXNode		*NSearch;
	D3DVECTOR	VSearch;

	// STOP DRAWING
	//DrawDisable=true;

	Limit*=2.0f;

	CProgressBar Bar(NULL, 0, Obj->NodeNr, "Calculating Face Normals...");

	DWORD NodeNr=0;
	Node=(DXNode*)Obj->Nodes;
	while(Node->Type!=DX_MODELEND){
		if(Node->Type==TRIANGLE) E_CalculateFaceNormal((DXTriangle*)Node);
		Bar.SetValue(++NodeNr);
		Node=Node->Next;
		
	}

	CProgressBar Bar1(NULL, 0, Obj->NodeNr, "Calculating Vertex Normals...");
	NodeNr=0;
	// Get the start of the model
	Node=(DXNode*)Obj->Nodes;
	while(Node->Type!=DX_MODELEND){
		Bar1.SetValue(++NodeNr);
		if(Node->Type==TRIANGLE){
			for(DWORD i=0; i<SURFACE(Node).dwVCount; i++){
				N.x=0; N.y=0; N.z=0; Normals=0;
				memcpy( &VSearch.x, &(((DXTriangle*)Node)->Vertex[i].vx), sizeof(D3DVECTOR));
				NSearch=(DXNode*)Obj->Nodes;
				
				while(NSearch->Type!=DX_MODELEND){
					if(NSearch->Type==TRIANGLE){
						for(DWORD s=0; s<SURFACE(NSearch).dwVCount; s++){
							if(!memcmp(&(((DXTriangle*)NSearch)->Vertex[s].vx), &VSearch.x, sizeof(D3DVECTOR))){
								if(fabs(((DXTriangle*)Node)->FaceNormal.x-((DXTriangle*)NSearch)->FaceNormal.x)>=Limit) break;
								if(fabs(((DXTriangle*)Node)->FaceNormal.y-((DXTriangle*)NSearch)->FaceNormal.y)>=Limit) break;
								if(fabs(((DXTriangle*)Node)->FaceNormal.z-((DXTriangle*)NSearch)->FaceNormal.z)>=Limit) break;
								
								N.x+=((DXTriangle*)NSearch)->FaceNormal.x;
								N.y+=((DXTriangle*)NSearch)->FaceNormal.y;
								N.z+=((DXTriangle*)NSearch)->FaceNormal.z;
								Normals++;
							}
						}
					}
					NSearch=NSearch->Next;
				}

				((DXTriangle*)Node)->Vertex[i].nx=(float)(N.x/(double)Normals);
				((DXTriangle*)Node)->Vertex[i].ny=(float)(N.y/(double)Normals);
				((DXTriangle*)Node)->Vertex[i].nz=(float)(N.z/(double)Normals);

			}
		}
		Node=Node->Next;
	}

	//DrawDisable=false;
}

*/
void E_CalculateModelVNormals(E_Object *Obj, float Limit, bool ListMode, CSelectList *List)
{

	DOUBLEVECTOR	N;
	DWORD			Normals;

	DXNode		*Node;
	DXNode		*NSearch;
	D3DVECTOR	VSearch;

	#define	MAX_VSTACK	256

	D3DVERTEXEX	*VStack[MAX_VSTACK];
	DWORD		 VStackPtr;
	
	// if update by selection, create an alternate search list
	CSelectList	SearchList;
	if(ListMode) SearchList=*List;


	// STOP DRAWING
//	DrawDisable=true;

	CProgressBar Bar(NULL, 0, Obj->NodeNr, "Calculating Face Normals...");

	DWORD NodeNr=0;
	// Selection list
	if(ListMode) {
		List->StartParseList();
		Node=List->GetItemFromList().Node;
	} else {
		Node=(DXNode*)Obj->Nodes;
	}
	while(Node && Node->Type!=DX_MODELEND){
		if(Node->Type==TRIANGLE){
			E_CalculateFaceNormal((DXTriangle*)Node);
			for(DWORD a=0; a<SURFACE(Node).dwVCount; a++){
				((DXTriangle*)Node)->Vertex[a].nx=0.0f;
				((DXTriangle*)Node)->Vertex[a].ny=0.0f;
				((DXTriangle*)Node)->Vertex[a].nz=0.0f;
			}
		}
		Bar.SetValue(++NodeNr);
		Node=(ListMode)?List->GetItemFromList().Node:Node->Next;
	}

	CProgressBar Bar1(NULL, 0, Obj->NodeNr, "Calculating Vertex Normals...");
	NodeNr=0;
	// Get the start of the model
	if(ListMode) {
		List->StartParseList();
		Node=List->GetItemFromList().Node;
	} else {
		Node=(DXNode*)Obj->Nodes;
	}
	Limit*=2;
	while(Node && Node->Type!=DX_MODELEND){
		Bar1.SetValue(++NodeNr);
		if(Node->Type==TRIANGLE){
			for(DWORD i=0; i<SURFACE(Node).dwVCount; i++){
				if(((DXTriangle*)Node)->Vertex[i].nx==0 && ((DXTriangle*)Node)->Vertex[i].ny==0 && ((DXTriangle*)Node)->Vertex[i].nz==0){
					VStackPtr=0;
					N.x=0; N.y=0; N.z=0; Normals=0;
					memcpy( &VSearch.x, &(((DXTriangle*)Node)->Vertex[i].vx), sizeof(D3DVECTOR));
					
					if(ListMode) {
						SearchList.StartParseList();
						NSearch=SearchList.GetItemFromList().Node;
					} else {
						NSearch=(DXNode*)Obj->Nodes;
					}
					//VStack[VStackPtr++]=&(((DXTriangle*)Node)->Vertex[i]);
					
					while(NSearch && NSearch->Type!=DX_MODELEND){
						if(NSearch->Type==TRIANGLE){
							for(DWORD s=0; s<SURFACE(Node).dwVCount; s++){
								if(!memcmp(&(((DXTriangle*)NSearch)->Vertex[s].vx), &VSearch.x, sizeof(D3DVECTOR))){
									if(fabs(((DXTriangle*)Node)->FaceNormal.x-((DXTriangle*)NSearch)->FaceNormal.x)>Limit) break;
									if(fabs(((DXTriangle*)Node)->FaceNormal.y-((DXTriangle*)NSearch)->FaceNormal.y)>Limit) break;
									if(fabs(((DXTriangle*)Node)->FaceNormal.z-((DXTriangle*)NSearch)->FaceNormal.z)>Limit) break;
									
									N.x+=((DXTriangle*)NSearch)->FaceNormal.x;
									N.y+=((DXTriangle*)NSearch)->FaceNormal.y;
									N.z+=((DXTriangle*)NSearch)->FaceNormal.z;
									VStack[VStackPtr++]=&(((DXTriangle*)NSearch)->Vertex[s]);
									if(VStackPtr>=MAX_VSTACK) VStackPtr=MAX_VSTACK-1;
									Normals++;
								}
							}
						}
						NSearch=(ListMode)?SearchList.GetItemFromList().Node:NSearch->Next;
					}
					
					for(DWORD n=0; n<VStackPtr; n++){
						VStack[n]->nx=(float)(N.x/(double)Normals);
						VStack[n]->ny=(float)(N.y/(double)Normals);
						VStack[n]->nz=(float)(N.z/(double)Normals);
					}
				}
			}
		}
		Node=(ListMode)?List->GetItemFromList().Node:Node->Next;
	}

	// ENABLE DRAWING
//	DrawDisable=false;
}



// Passed in DXFILES.CPP
/*
void	CopySurface(DxSurfaceType *D, void *Source)
{
	// Same version

	switch(Version){


		// ************* FROM ORIGINAL BETA REVISION ***************
		case	-1:		{
							DxSurfaceType_00 *S=(DxSurfaceType_00*)Source;
							D->DefaultSpecularity=(DWORD)(((DWORD)(S->DefaultSpecularity.r*255.0f)<<16)+((DWORD)(S->DefaultSpecularity.g*255.0f)<<8)+(DWORD)(S->DefaultSpecularity.b*255.0f));
							D->dwFlags=S->dwFlags;
							D->dwPrimType=S->dwPrimType;
							D->dwStride=S->dwStride;
							D->dwVCount=S->dwVCount;
							D->dwzBias=S->dwzBias;
							D->h=S->h;
							D->SpecularIndex=S->SpecularIndex;
							D->SwitchMask=(S->dwFlags.b.SwEmissive)?1:0;
							D->SwitchNumber=S->SwitchNumber;
							D->TexID[0]=S->TexID[0];
							D->TexID[1]=S->TexID[1];
						}
							break;

		// Same version
		default:		*D=*((DxSurfaceType*)Source);
	}


}


void	CopyLight(DXLightType *D, void *Source)
{

	switch(Version){


		// ************* FROM ORIGINAL BETA REVISION ***************
		case	01:		{
							DXLightType_01 *S=(DXLightType_01*)Source;
							D->Light=S->Light;
							D->Flags.w=0x00000000;
							D->Flags.OwnLight=S->OwnLight;
							D->Switch=S->Switch;
							D->SwitchMask=0xffff;
							D->Argument=0;
						}
							break;
		// Same version
		default:		*D=*((DXLightType*)Source);
	}

}



// This function convert a DXM model from Indexed to Linear
void	DeIndexModel(void *Source, E_Object *Obj)
{
	// *********** Types used in translations - will be eventually changed by Version ****************
	DWORD			DxDbHeaderSize=sizeof(DxDbHeader);
	void			*DxDbHeaderTarget=(void*)&Obj->Header;
	DWORD			SurfaceSize=sizeof(DxSurfaceType), DXLightSize=sizeof(DXLightType);
	DxSurfaceType	Surface;

	bool	TexUsed[256];

	memset(TexUsed, false, sizeof(TexUsed));


	// Version Check, version has to be reversed in upper 16 bits
	Version=((DxDbHeader*)Source)->Version;
	Version^=0xffff0000;
	// eventually OLD NO MORE USED VERSION
	if((Version>>16)!=(Version&0xffff)) Version=-1;
	else Version&=0xffff;


	// SWITCHING BASED ON RELEASE... NOTE, NO BREAK, this to follow sequentially all statements starting from a release
	switch(Version){
		
		case	0:
		case	-1:		DxDbHeaderSize=sizeof(DxDbHeader_00);
						DxDbHeaderTarget=(void*)&Obj->Header.Id;
						SurfaceSize=sizeof(DxSurfaceType_00);
						DXLightSize=sizeof(DXLightType_01);


		case	1:		DXLightSize=sizeof(DXLightType_01);
	}


	memcpy(DxDbHeaderTarget, Source, DxDbHeaderSize);
	
	// update pointers
	D3DVERTEXEX	*Vertices;
	Vertices=(D3DVERTEXEX*)((char*)Source + Obj->Header.pVPool);
	memcpy((void*)Obj->Textures, ((char*)Source+DxDbHeaderSize), sizeof(DWORD) * Obj->Header.dwTexNr);

	// Build up the lights list
	E_Light	*DxL=NULL, *DxLp;
	for(DWORD l=0; l<Obj->Header.dwLightsNr; l++){
		DxL=new E_Light();
		if( l==0 ) { Obj->LightList=DxL; DxLp=NULL; }
		else DxLp->Next=DxL;
		CopyLight(&DxL->Light, ((char*)Source+Obj->Header.pLightsPool + l * DXLightSize));
		DxL->Prev=DxLp;
		DxL->Next=NULL;
		DxL->Selected=false;
		DxLp=DxL;

	}

	// copy the scripts
	*Obj->Scripts=*Obj->Header.Scripts;

	// and assign the number of textures
	Obj->TexNr=Obj->Header.dwTexNr;

	// then linearize the model
	NodeScannerType N;
	// get the start of nodes
	N.BYTE=(BYTE*)((char*)Source + DxDbHeaderSize + Obj->Header.dwTexNr * sizeof(DWORD));

	// the start of the list
	DXNode *DxN=NULL, *New;
	Obj->Nodes=NULL;
	Obj->NodeNr=0;
	Obj->TexPerSet=0;
	memset(&Obj->DnRight, 0, sizeof(Obj->DnRight));
	memset(&Obj->UpLeft, 0, sizeof(Obj->UpLeft));

	// Till end of Model
	while(N.HEAD->Type!=DX_MODELEND){
		
		// Selects actions for each node
		switch(N.HEAD->Type){

			case	DX_SWITCH:		
			case	DX_LIGHT:	
			case	DX_TEXTURE:		
			case	DX_MATERIAL:	
			case	DX_ROOT:		break;

			case	DX_SURFACE:		// 1st of all, revert surface to new format
									CopySurface(&Surface, N.SURFACE);
									Surface.dwFlags.b.Frame=Surface.dwFlags.b.Disable=0;
				
									// check for used texture
									if(Surface.TexID[0]!=-1){
										if(Obj->TexPerSet<(Surface.TexID[0]+1))
											Obj->TexPerSet=(Surface.TexID[0]+1); 
									}
				
									if(!Surface.dwFlags.b.Texture) Surface.TexID[0]=Surface.TexID[1]=-1;

									// * MANAGE THE POINT LIST *
									if(Surface.dwPrimType==D3DPT_POINTLIST){
										Int16	*VIdx=(Int16*)(N.BYTE+SurfaceSize);
										// add each vertex with its flags and features
										for(DWORD a=0; a<Surface.dwVCount; a++){
											New=new DXVertex();
											New->NodeNumber=Obj->NodeNr++;
											((DXVertex*)New)->Surface=Surface;
											((DXVertex*)New)->Surface.dwVCount=1;
											AssignVertex(Obj, &((DXVertex*)New)->Vertex, Vertices, VIdx++);
											((DXVertex*)New)->Prev=DxN;
											if(!DxN) { Obj->Nodes=New; DxN=NULL; }
											else DxN->Next=New;
											New->Prev=DxN;
											DxN=New;
										}
										break;
									}

									// * MANAGE THE LINE LIST *
									if(Surface.dwPrimType==D3DPT_LINELIST){
										Int16	*VIdx=(Int16*)(N.BYTE+SurfaceSize);
										// add each vertex with its flags and features
										for(DWORD a=0; a<Surface.dwVCount; a+=2){
											New=new DXLine();
											New->NodeNumber=Obj->NodeNr++;
											((DXLine*)New)->Surface=Surface;
											((DXLine*)New)->Surface.dwVCount=2;
											AssignVertex(Obj, &((DXLine*)New)->Vertex[0], Vertices, VIdx++);
											AssignVertex(Obj, &((DXLine*)New)->Vertex[1], Vertices, VIdx++);
											((DXLine*)New)->Prev=DxN;
											if(!DxN) { Obj->Nodes=New; DxN=NULL; }
											else DxN->Next=New;
											New->Prev=DxN;
											DxN=New;
										}
										break;
									}

									// * MANAGE THE TRIANGLE LIST *
									if(Surface.dwPrimType==D3DPT_TRIANGLELIST){
										Int16	*VIdx=(Int16*)(N.BYTE+SurfaceSize);
										// add each vertex with its flags and features
										for(DWORD a=0; a<Surface.dwVCount; a+=3){
											New=new DXTriangle();
											New->NodeNumber=Obj->NodeNr++;
											((DXTriangle*)New)->Surface=Surface;
											((DXTriangle*)New)->Surface.dwVCount=3;
											AssignVertex(Obj, &((DXTriangle*)New)->Vertex[0], Vertices, VIdx++);
											AssignVertex(Obj, &((DXTriangle*)New)->Vertex[1], Vertices, VIdx++);
											AssignVertex(Obj, &((DXTriangle*)New)->Vertex[2], Vertices, VIdx++);
											if(!DxN) { Obj->Nodes=New; DxN=NULL; }
											else DxN->Next=New;
											New->Prev=DxN;
											DxN=New;
										}
										E_CalculateFaceNormal((DXTriangle*)DxN);
										break;
									}

			
									break;

									

			
			case	DX_DOF:			New=new DXDof();
									New->NodeNumber=Obj->NodeNr++;
									if(!DxN) { Obj->Nodes=New; DxN=NULL; }
									else DxN->Next=New;
									New->Prev=DxN;
									DxN=New;
									((DXDof*)New)->dof=*N.DOF;
									break;

			case	DX_ENDDOF:		New=new DXEndDof();
									New->NodeNumber=Obj->NodeNr++;
									((DXEndDof*)New)->Sign=*N.DOFEND;
									if(!DxN) { Obj->Nodes=New; DxN=NULL; }
									else DxN->Next=New;
									New->Prev=DxN;
									DxN=New;
									break;
									
									
			case	DX_SLOT:		New=new DXSlot();
									New->NodeNumber=Obj->NodeNr++;
									if(!DxN) { Obj->Nodes=New; DxN=NULL; }
									else DxN->Next=New;
									New->Prev=DxN;
									DxN=New;
									((DXSlot*)New)->Slot=*N.SLOT;
									break;
									

			default			:		char s[128]; 
									printf(s, "Corrupted Model !!!");
									MessageBox(NULL, s, "DX Engine", NULL);
			
		}
		// Traverse the model
		N.BYTE+=N.HEAD->dwNodeSize;
	}

	if(N.HEAD->Type==DX_MODELEND){
		New=new DXNode(MODELEND);
		New->NodeNumber=Obj->NodeNr++;
		if(!DxN) { Obj->Nodes=New; DxN=NULL; }
		else DxN->Next=New;
		New->Prev=DxN;
		DxN=New;
	}

}

*/
// END OF Passed in DXFILES.CPP

// This Function Setup lights
void	E_SetupLights(E_Object *Obj, DWORD ID, D3DXMATRIX *State, D3DVECTOR *Pos)
{	
	// The light list pointer and number
	E_Light	*Lights=Obj->LightList;
	DWORD	LightNr=Obj->Header.dwLightsNr;
	
	// distance of object from camera
	float	Range=sqrtf(Pos->x * Pos->x + Pos->y * Pos->y + Pos->z * Pos->z);

	// Setup all the lights
	while(LightNr--){
		if(Obj->Obj->SwitchValues[Lights->Light.Switch] || TheLightEdit->Active) TheLightEngine.AddDynamicLight(ID, &Lights->Light, State, Pos, Range);
		Lights=Lights->Next;
	}

}


void	E_CreateVertexCursor(D3DVERTEXEX *Vertex, D3DXMATRIX *State)
{
	// The Vertex Cursor
	D3DVERTEXEX	VCursor[6];
	memset(VCursor, 0x00, sizeof(VCursor));
	for(DWORD a=0; a<6; a++){
		VCursor[a].dwColour=VCursor[a].dwSpecular=0xffff0000;
		*((D3DVECTOR*)&VCursor[a].vx)=*((D3DVECTOR*)&Vertex->vx);

	}

	float	VSize=Scaling*0.01f;

	VCursor[0].vx=Vertex->vx-VSize; 	VCursor[1].vx=Vertex->vx+VSize;
	VCursor[2].vy=Vertex->vy-VSize; 	VCursor[3].vy=Vertex->vy+VSize;
	VCursor[4].vz=Vertex->vz-VSize; 	VCursor[5].vz=Vertex->vz+VSize;

	DX->DrawFrame(VCursor, 6, State);

}



void	E_InitDraw(void)
{
	FrameVertexNr = DrawVertexNr = 0;

}


void	E_FlushDrawBuffer(ObjectInstance *objInst, DWORD LightOwner, DWORD LodID)
{
	if(DrawVertexNr){
		DxSurfaceType	*N = (DxSurfaceType*)DrawBuffer;
		N->dwVCount = DrawVertexNr;
		// calculate the whole node size
		DWORD	Size = sizeof(DxSurfaceType) + sizeof(D3DVERTEXEX) * DrawVertexNr;
		// check if stape in draw pool, if not reset
		if(Size + DrawPoolNr >= DRAW_POOL_SIZE) DrawPoolNr = 0;
		memcpy(&DrawPool[DrawPoolNr], DrawBuffer, Size);
		NodeScannerType K;
		K.SURFACE = (DxSurfaceType*)&DrawPool[DrawPoolNr];
		TheDXEngine.DrawNodeEx(&K, objInst, LightOwner, LodID);
		DrawVertexNr = 0;
		DrawPoolNr += Size;
	}
}



void	E_FlushFrameBuffer(ObjectInstance *objInst, DWORD LightOwner, DWORD LodID)
{
	if(FrameVertexNr){
		DxSurfaceType	*N = (DxSurfaceType*)FrameBuffer;
		N->dwVCount = FrameVertexNr;
		// calculate the whole node size
		DWORD	Size = sizeof(DxSurfaceType) + sizeof(D3DVERTEXEX) * FrameVertexNr;
		// check if stape in draw pool, if not reset
		if(Size + FramePoolNr >= DRAW_POOL_SIZE) FramePoolNr = 0;
		memcpy(&FramePool[FramePoolNr], FrameBuffer, Size);
		NodeScannerType K;
		K.SURFACE = (DxSurfaceType*)&FramePool[FramePoolNr];
//		TheDXEngine.DrawFrameSurfaces(&K, SelectionAlpha);
		FrameVertexNr = 0;
		FramePoolNr += Size;
	}
}



void	E_CloseDraw(ObjectInstance *objInst, DWORD LightOwner, DWORD LodID)
{
	E_FlushDrawBuffer(objInst, LightOwner, LodID);
	E_FlushFrameBuffer(objInst, LightOwner, LodID);
}



void	E_BufferizedDraw(NodeScannerType *NODE, ObjectInstance *objInst, DWORD LightOwner, DWORD LodID)
{
	bool	ChangeSurface = false;
	DxSurfaceType	*N = (DxSurfaceType*)DrawBuffer;
	D3DVERTEXEX		*V = (D3DVERTEXEX*)(&DrawBuffer[sizeof(DxSurfaceType)]);
	D3DVERTEXEX		*F = (D3DVERTEXEX*)(&FrameBuffer[sizeof(DxSurfaceType)]);

	// Check for node type
	DxSurfaceType	*S= NODE->SURFACE;
	DXFlagsType	Flags;
	Flags.w=S->dwFlags.w;

	if(S->dwFlags.b.Disable) return;

	// Check about continuity
	if(S->dwPrimType != N->dwPrimType) ChangeSurface = true;
	if(Flags.b.Texture != N->dwFlags.b.Texture) ChangeSurface=true;
	if(Flags.b.Texture && N->TexID[0]!=S->TexID[0]) ChangeSurface = true;
	if(Flags.b.Alpha != N->dwFlags.b.Alpha) ChangeSurface=true;
	if(Flags.b.Gouraud != N->dwFlags.b.Gouraud ) ChangeSurface=true;
	if(Flags.b.Lite != N->dwFlags.b.Lite ) ChangeSurface=true;
	if((Flags.b.VColor) != (N->dwFlags.b.VColor)) ChangeSurface=true;
	if(Flags.b.zBias != N->dwFlags.b.zBias) ChangeSurface=true;
	if(S->dwzBias!=N->dwzBias) ChangeSurface=true;
	if(Flags.b.ChromaKey != N->dwFlags.b.ChromaKey) ChangeSurface=true;
	if(Flags.b.SwEmissive != N->dwFlags.b.SwEmissive) ChangeSurface=true;
	if(Flags.b.SwEmissive && S->SwitchNumber!= N->SwitchNumber) ChangeSurface=true;
	if(S->DefaultSpecularity !=  N->DefaultSpecularity) ChangeSurface=true;

	// if changed, or buffer overflow, flush
	if(DrawVertexNr && (ChangeSurface || (DrawVertexNr + S->dwVCount >= MAX_BUFFERIZABLE_VERTEX_COUNT))){
		E_FlushDrawBuffer(objInst, LightOwner, LodID);
		E_FlushFrameBuffer(objInst, LightOwner, LodID);
	}

	// Framed surface manament
	if(S->dwFlags.b.Frame){
		if(!FrameVertexNr) memcpy(FrameBuffer, NODE->SURFACE, sizeof(DxSurfaceType));	
		// Add vertices
		memcpy(&F[FrameVertexNr], NODE->BYTE+sizeof(DxSurfaceType), sizeof(D3DVERTEXEX) * S->dwVCount);
		FrameVertexNr += S->dwVCount;
	}


	// if 1st vertex... create the surface node
	if(!DrawVertexNr) memcpy(DrawBuffer, NODE->SURFACE, sizeof(DxSurfaceType)), N->dwFlags.b.Frame = 0;	

	// Add vertices
	memcpy(&V[DrawVertexNr], NODE->BYTE+sizeof(DxSurfaceType), sizeof(D3DVERTEXEX) * S->dwVCount);
	DrawVertexNr += S->dwVCount;


}




// This Function draws a Model
void	E_DrawModel(E_Object *Obj, DWORD ID, D3DXMATRIX *State, D3DVECTOR *Pos, bool FrameMode)
{	
	// Get the start of the model
	DXNode			*Node=(DXNode*)Obj->Nodes;
	NodeScannerType	NODE;

	
	Obj->Obj->TextureSet=Obj->TexSetNr;

	D3DXMATRIX	World;
	D3DXMatrixTranslation(&World, Pos->x, Pos->y, Pos->z );
	D3DXMatrixMultiply(&World, State, &World);
//	D3DXMatrixMultiply(&World, &World, WorldState);

	TheDXEngine.m_FrameDrawMode=FrameMode;

	E_InitDraw();
	// Initialize the Engine
	TheDXEngine.ModelInit(Obj->Obj, &Obj->Header, Obj->Textures, &World, ID, Obj->TexPerSet);
	// reset VCursor stack
	VCursorNr=0;

	// Traverse all the model
	while(Node){
		
		switch(Node->Type){
			
			case	DOT:
			case	LINE:		// Break Check
			case	TRIANGLE:	NODE.BYTE=(BYTE*)&((DXTriangle*)Node)->Surface;
								//TheDXEngine.DrawNodeEx(&NODE, Obj->Obj, ID, ID);
								E_BufferizedDraw(&NODE, Obj->Obj, ID, ID);
								if(Node->SelectV){
									for(DWORD a=0; a<((DXTriangle*)Node)->Surface.dwVCount; a++)
										if(Node->SelectV&(1<<a)){
											VCursorList[VCursorNr]=&((DXTriangle*)Node)->Vertex[a];
											VCursorStates[VCursorNr++]=TheDXEngine.AppliedState;
										}
								}
								break;

			case	DOF:		E_CloseDraw(Obj->Obj, ID, ID);
								NODE.BYTE=(BYTE*)&((DXDof*)Node)->dof;
								TheDXEngine.DrawNodeEx(&NODE, Obj->Obj, ID, ID);
								break;

			case	CLOSEDOF:	E_CloseDraw(Obj->Obj, ID, ID);
								NODE.BYTE=(BYTE*)&((DXEndDof*)Node)->Sign;
								TheDXEngine.DrawNodeEx(&NODE, Obj->Obj, ID, ID);
								break;



			case	SLOT:		E_CloseDraw(Obj->Obj, ID, ID);
								NODE.BYTE=(BYTE*)&((DXSlot*)Node)->Slot;
								TheDXEngine.DrawNodeEx(&NODE, Obj->Obj, ID, ID);
								break;
		}
		Node=Node->Next;

	}

	E_CloseDraw(Obj->Obj, ID, ID);
	// now draw Vertex Cursors
	while(VCursorNr--){
		E_CreateVertexCursor(VCursorList[VCursorNr], &VCursorStates[VCursorNr]);
	}
}



void	E_DrawLight(DXLightType *Light, D3DXMATRIX *State, D3DVECTOR *Pos)
{
	E_Object *Obj=(Light->Light.dltType==D3DLIGHT_SPOT)?&TheSpotLight:&TheOmniLight;
	DWORD	LColor=	((WORD)(Light->Light.dcvDiffuse.b*255.0f))|
					(((WORD)(Light->Light.dcvDiffuse.g*255.0f))<<8)|
					(((WORD)(Light->Light.dcvDiffuse.r*255.0f))<<16);
	LColor|=0x40000000;	//Alpha

	D3DVERTEXEX		Vertices[1024];
	DWORD			Vnr=0;

	DXNode			*Node=(DXNode*)Obj->Nodes;
	DxSurfaceType	*Surface;
	D3DVERTEXEX		*Vertex;


	// Traverse all the model and assign vertex color
	while(Node){
		switch(Node->Type){
			case	DOT:
			case	LINE:
			case	TRIANGLE:	Surface=&((DXTriangle*)Node)->Surface;
								Vertex=&((DXTriangle*)Node)->Vertex[0];
								for(DWORD i=0; i<Surface->dwVCount; i++){
									Vertices[Vnr]=Vertex[i];
									Vertices[Vnr].dwSpecular=Vertices[Vnr].dwColour=LColor;
									Vnr++;
								}

								break;
		}
		Node=Node->Next;
	}


	D3DXVECTOR3	Position;
	D3DXVec3TransformCoord(&Position, (D3DXVECTOR3*)&Light->Light.dvPosition, State);
	Position.x+=Pos->x;
	Position.y+=Pos->y;
	Position.z+=Pos->z;

	D3DXMATRIX	VectorY, VectorZ, Vector;
	float	x=Light->Light.dvDirection.x;
	float	y=Light->Light.dvDirection.y;
	float	z=-Light->Light.dvDirection.z;

	float	Yaw=(float)(atan2(y,x));
	float	Pitch=float(atan2(z, sqrtf(x*x + y*y)));

	D3DXMatrixRotationZ(&VectorY, Yaw);
	D3DXMatrixRotationY(&VectorZ, Pitch);
	D3DXMatrixMultiply(&Vector, &VectorZ, &VectorY);
	
	D3DXMatrixMultiply(&Vector, &Vector, State);
	DX->DrawLightNode((void*)Vertices, Vnr, &Vector, &Position, Light->Flags.Static);

}






// this finction retrun tru/false if a point P is withing a triangle ABC
bool	PointWithinTriangle(D3DVECTOR *Point, D3DVERTEXEX *Tri, D3DVECTOR *Mask)
{
	DOUBLEVECTOR	P,T[3];
	float		*pX, *pY;
	// Check which vertex is to be ignored ( we are on 2D plane projections )
	// and Vertex Mask to be ignore could not be 0
	if(!Mask->x){ P.x=Point->y; P.y=Point->z; pX=&Tri->vy; pY=&Tri->vz; }
	if(!Mask->y){ P.x=Point->x; P.y=Point->z; pX=&Tri->vx; pY=&Tri->vz; }
	if(!Mask->z){ P.x=Point->x; P.y=Point->y; pX=&Tri->vx; pY=&Tri->vy; }
	// get the triangle vertex
	for(int a=0; a<3; a++){
		T[a].x=(double)*pX;
		T[a].y=(double)*pY;
		pX=(float*)(((BYTE*)pX)+sizeof(D3DVERTEXEX));
		pY=(float*)(((BYTE*)pY)+sizeof(D3DVERTEXEX));
	}

	// now calculate triangle vectors
	DOUBLEVECTOR	AB, BC, CA, AP, BP, CP;
	double		Pa, Pb, Pc;

	// Calculate vectors for the Triangle vertices
	AB.x=T[0].x-T[1].x; AB.y=T[0].y-T[1].y; 
	BC.x=T[1].x-T[2].x; BC.y=T[1].y-T[2].y; 
	CA.x=T[2].x-T[0].x; CA.y=T[2].y-T[0].y; 

	// calculate vectors btw Point and Triangle vertices
	AP.x=T[0].x-P.x; AP.y=T[0].y-P.y; 
	BP.x=T[1].x-P.x; BP.y=T[1].y-P.y; 
	CP.x=T[2].x-P.x; CP.y=T[2].y-P.y; 

	// Vector products
	Pa=(AP.x * AB.y) - (AP.y * AB.x );
	Pb=(BP.x * BC.y) - (BP.y * BC.x );
	Pc=(CP.x * CA.y) - (CP.y * CA.x );

	// if all products have same sign P is in ABC
	if((Pa>=0 && Pb>=0 && Pc>=0) ||(Pa<=0 && Pb<=0 && Pc<=0)){
		// now check for normal directions based on View
//		if((!Mask->x) && Tri->nx<=0) return false;
//		if((!Mask->y) && Tri->ny<=0) return false;
//		if((!Mask->z) && Tri->nz>=0) return false;
		return true;
	}

	return false;
}



// this function returns the averaged distance of a triangle vertices from reference point
float	DistanceWithinTriangle(D3DVECTOR *Point, D3DVERTEXEX *Tri, D3DVECTOR *Mask)
{
	DOUBLEVECTOR	N, U, V;
	
	// calculate the 2 vectors from vertex 0 of the Triangle
	U.x=Tri[0].vx - Tri[1].vx; U.y=Tri[0].vy - Tri[1].vy; U.z=Tri[0].vz - Tri[1].vz;
	V.x=Tri[0].vx - Tri[2].vx; V.y=Tri[0].vy - Tri[2].vy; V.z=Tri[0].vz - Tri[2].vz;
	
	// Calculate the plane Normal
	N.x=U.y * V.z - U.z * V.y;
	N.y=U.z * V.x - U.x * V.z;
	N.z=U.x * V.y - U.y * V.x;

	// Normalize the Normal
	double	l=sqrt(N.x * N.x + N.y * N.y + N.z * N.z);
	N.x/=l; N.y/=l; N.z/=l;

	// Calculate the distance of vertex 0
	double	d=N.x * Tri[0].vx + N.y * Tri[0].vy + N.z * Tri[0].vz;

	if(!Mask->x) d=-(d - N.y*Point->y - N.z*Point->z)/N.x;
	if(!Mask->y) d=-(d - N.x*Point->x - N.z*Point->z)/N.y;
	if(!Mask->z) d=(d - N.x*Point->x - N.y*Point->y)/N.z;
	
	return (float)d;

}


DWORD NearVertex(D3DVECTOR *Point, D3DVERTEXEX *Tri, D3DVECTOR *Mask, DWORD NVertices)
{
	DOUBLEVECTOR	P;
	float		*pX, *pY;
	double		dx, dy, Dist, Near=999999999.0;
	DWORD		Index;
	// Check which vertex is to be ignored ( we are on 2D plane projections )
	// and Vertex Mask to be ignore could not be 0
	if(!Mask->x){ P.x=Point->y; P.y=Point->z; pX=&Tri->vy; pY=&Tri->vz; }
	if(!Mask->y){ P.x=Point->x; P.y=Point->z; pX=&Tri->vx; pY=&Tri->vz; }
	if(!Mask->z){ P.x=Point->x; P.y=Point->y; pX=&Tri->vx; pY=&Tri->vy; }
	// get the triangle vertex
	for(DWORD a=0; a<NVertices; a++){
		dx=(double)*pX-P.x;
		dy=(double)*pY-P.y;
		Dist=sqrt(dx*dx+dy*dy);
		if(Dist<Near){
			Index=a;
			Near=Dist;
		}
		pX=(float*)(((BYTE*)pX)+sizeof(D3DVERTEXEX));
		pY=(float*)(((BYTE*)pY)+sizeof(D3DVERTEXEX));
	}

	return Index;
}
	




// This function scans for the nearest vertex to the one passed
DWORD	E_FindVertex(E_Object *Obj, D3DVECTOR *Search, CEditing *EditNode, D3DVECTOR *CoordMask)
{
	DxSurfaceType	*Current=NULL;
	double			CurrentDistance=99999999.0, LastZ=99999999.0;
	D3DVERTEXEX		*Last=NULL;
	D3DXMATRIX		AppliedState=Obj->State;
	bool			SkipSwitch=false;
	DWORD			DofLevel;
	DWORD			LastVSelected;

	// Get the start of the model
	DXNode			*Node=(DXNode*)Obj->Nodes;
	NodeScannerType	NODE;
	EditNode->Node=NULL;

	// Traverse all the model
	while(Node){
		
		switch(Node->Type){
			
			case	DOT:
			case	LINE:		

			case	TRIANGLE:	if(SkipSwitch) break;
								if(((DXTriangle*)Node)->Surface.dwFlags.b.Disable) break;
								D3DVERTEXEX	Vertices[3];
								for(DWORD i=0; i<SURFACE(Node).dwVCount; i++){
									// copy vertices transforming them
									D3DXVec3TransformCoord((D3DXVECTOR3*)&Vertices[i].vx, (D3DXVECTOR3*)&((DXTriangle*)Node)->Vertex[i].vx, &AppliedState);
//									D3DXVec3TransformCoord((D3DXVECTOR3*)&Vertices[1].vx, (D3DXVECTOR3*)&((DXTriangle*)Node)->Vertex[1].vx, &AppliedState);
//									D3DXVec3TransformCoord((D3DXVECTOR3*)&Vertices[2].vx, (D3DXVECTOR3*)&((DXTriangle*)Node)->Vertex[2].vx, &AppliedState);
								
									// transform normals too
									D3DXVec3TransformCoord((D3DXVECTOR3*)&Vertices[i].nx, (D3DXVECTOR3*)&((DXTriangle*)Node)->Vertex[i].nx, &AppliedState);
//									D3DXVec3TransformCoord((D3DXVECTOR3*)&Vertices[1].nx, (D3DXVECTOR3*)&((DXTriangle*)Node)->Vertex[1].nx, &AppliedState);
//									D3DXVec3TransformCoord((D3DXVECTOR3*)&Vertices[2].nx, (D3DXVECTOR3*)&((DXTriangle*)Node)->Vertex[2].nx, &AppliedState);
								}
								
								if(SURFACE(Node).dwVCount==3){
									// look for triangle
									if(PointWithinTriangle(Search, Vertices, CoordMask)){
										// calculate the distance from Camera
										float	d=DistanceWithinTriangle(Search, Vertices, CoordMask);
										if(d<=CurrentDistance){
											CurrentDistance=d;
											EditNode->Node=Node;
											LastVSelected=NearVertex(Search, Vertices, CoordMask, 3);
										}
									}
								}
								if(SURFACE(Node).dwVCount<3){
									Vertices[0].tu=0;
									Vertices[0].tv=0;
									Vertices[1].tu=1.0f;
									Vertices[1].tv=1.0f;
									for(i=0; i<SURFACE(Node).dwVCount; i++){
										float	x=Vertices[i].vx-Search->x;
										float	y=Vertices[i].vy-Search->y;
										float	z=Vertices[i].vz-Search->z;
										float	d=sqrtf(x*x+y*y+z*z);
										if(d<=CurrentDistance){
											CurrentDistance=d;
											EditNode->Node=Node;
											LastVSelected=i;
										}
									}
								}


								break;

			case	DOF:		if(SkipSwitch) { DofLevel++; break; }
								NODE.BYTE=(BYTE*)&((DXDof*)Node)->dof;
								if(NODE.DOF->Type<SWITCH){
									TheDXEngine.PushMatrixEx(&AppliedState);
									TheDXEngine.DofManageEx(&NODE, Obj->Obj, &AppliedState);
								} else {
									if(SkipSwitch=TheDXEngine.SwitchManageEx(&NODE, Obj->Obj, &AppliedState))
										DofLevel=1;

								}
								break;

			case	CLOSEDOF:	if(SkipSwitch){
									if(DofLevel) DofLevel--;
									if(!DofLevel) SkipSwitch=false;
									break;
								}
								
								TheDXEngine.PopMatrixEx(&AppliedState);

			default	:			break;
		}
		Node=Node->Next;
	}
	
	// indicate nearest vertex selection
	return(LastVSelected);
}




// This function scans for the nearest vertex to the one passed
DXNode *E_TraverseModelSurfaces(E_Object *Obj=NULL, bool Reset=false)
{
	static	bool			SkipSwitch=false;
	static	DWORD			DofLevel=0;
	static	DXNode			*Node;
	static	D3DXMATRIX		AppliedState;
	DXNode					*Value=NULL;
	static	E_Object		*TheObject;
	
	if(Reset){
		Node=(DXNode*)Obj->Nodes;
		AppliedState=Obj->State;
		TheObject=Obj;
		return NULL;
	}



	// Get the start of the model
	NodeScannerType	NODE;
	while((Node->Type!=DX_MODELEND) && (!Value)){
		switch(Node->Type){
			
			case	DOT:
			case	LINE:		break;

			case	TRIANGLE:	if(SkipSwitch) break;
								if(((DXTriangle*)Node)->Surface.dwFlags.b.Disable) break;
								Value=Node;
								break;

			case	DOF:		if(SkipSwitch) { DofLevel++; break; }
								NODE.BYTE=(BYTE*)&((DXDof*)Node)->dof;
								if(NODE.DOF->Type>=SWITCH){
									if(SkipSwitch=TheDXEngine.SwitchManageEx(&NODE, TheObject->Obj, &AppliedState)){
										DofLevel=1;
									} else 
										TheDXEngine.PopMatrixEx(&AppliedState);
								}
								break;

			case	CLOSEDOF:	if(SkipSwitch){
									if(DofLevel) DofLevel--;
									if(!DofLevel) SkipSwitch=false;
									break;
								}
								
			default	:			break;
		}
		Node=Node->Next;
	}
	
	return(Value);
}





DXNode *E_GetModelSurfaces(E_Object *Obj=NULL, bool Reset=false)
{
	static	DXNode			*Node;
	DXNode					*Value=NULL;
	static	E_Object		*TheObject;
	
	if(Reset){
		Node=(DXNode*)Obj->Nodes;
		TheObject=Obj;
		return NULL;
	}



	// Get the start of the model
	while((Node->Type!=DX_MODELEND) && (!Value)){
		switch(Node->Type){
			
			case	DOT:
			case	LINE:		
			case	TRIANGLE:	Value=Node;
								break;
			case	DOF:		
			case	CLOSEDOF:	break;

			default	:			break;
		}
		Node=Node->Next;
	}
	
	return(Value);
}









// This function links vertices common with Search vertex to the passed list
void	E_SelectNode(CSelectList *List, DXNode *Node, bool Select)
{
	CEditing	EditNode;
	D3DCOLORVALUE	Color;
	Color.a=Color.r=1.0f; Color.b=Color.g=0.0f;

	switch(Node->Type){
		
		case	DOT:
		case	LINE:		
		case	TRIANGLE:	if(Select){
								if(Node->SelectID) List->RemoveSelection(Node->SelectID);
								EditNode.Node=Node;
								EditNode.Node->FrameColour=Color;
								EditNode.Node->SelectID=List->AddSelection(&EditNode);
								((DXTriangle*)Node)->Surface.dwFlags.b.Frame=true;
							} else {
								List->RemoveSelection(Node->SelectID);
								Node->SelectID=0;
								((DXTriangle*)Node)->Surface.dwFlags.b.Frame=false;
							}
							break;

		case	DOF:		
		case	SLOT:
		case	CLOSEDOF:	if(Select){
								if(Node->SelectID) List->RemoveSelection(Node->SelectID);
								EditNode.Node=Node;
								EditNode.Node->SelectID=List->AddSelection(&EditNode);
							} else {
								List->RemoveSelection(Node->SelectID);
								Node->SelectID=0;
							}
							break;


		default	:			break;
	}
	List->UpdateTexUsedList();
	TheTextureEdit->UpdateValues();
	
	// view last selected on trees
}




// This function links vertices common with Search vertex to the passed list
void	E_ToggleNodeSelect(CSelectList *List, DXNode *Node)
{
	CEditing	EditNode;
	D3DCOLORVALUE	Color;
	Color.a=Color.r=1.0f; Color.b=Color.g=0.0f;
	List->ClearVertexList();


	switch(Node->Type){
		
		case	DOT:
		case	LINE:		
		case	TRIANGLE:	if(!Node->SelectID){
								EditNode.Node=Node;
								EditNode.Node->FrameColour=Color;
								EditNode.Node->SelectID=List->AddSelection(&EditNode);
								((DXTriangle*)Node)->Surface.dwFlags.b.Frame=true;
							} else {
								List->RemoveSelection(Node->SelectID);
								Node->SelectID=0;
								((DXTriangle*)Node)->Surface.dwFlags.b.Frame=false;
							}
							break;

		case	DOF:		
		case	SLOT:
		case	CLOSEDOF:	if(!Node->SelectID){
								EditNode.Node=Node;
								EditNode.Node->SelectID=List->AddSelection(&EditNode);
							} else {
								List->RemoveSelection(Node->SelectID);
								Node->SelectID=0;
							}
							break;


		default	:			break;
	}
	List->UpdateTexUsedList();
	TheTextureEdit->UpdateValues();
	
	// view last selected on trees
}




// This function links vertices common with Search vertex to the passed list
void	E_ExtendedSelect(E_Object *Obj, CSelectList *List, DXNode *Search, bool Select)
{
	DxSurfaceType	*Current=NULL;
	bool	SkipSwitch=false;
	DWORD	DofLevel;
	DXNode	*Node=(DXNode*)Obj->Nodes;
	CEditing	EditNode;
	D3DCOLORVALUE	Color;
	Color.a=Color.r=1.0f; Color.b=Color.g=0.0f;
	D3DXMATRIX	Ident;
	D3DXMatrixIdentity(&Ident);

	// Get the start of the model
	NodeScannerType	NODE;

	// Traverse all the model
	while(Node){
		
		switch(Node->Type){
			
			case	DOT:
			case	LINE:		
			case	TRIANGLE:	if(SkipSwitch) break;
								if(((DXTriangle*)Node)->Surface.dwFlags.b.Disable) break;
								if(TheSelector.CheckSelection((DXTriangle*)Search, (DXTriangle*)Node)){
									if(!Node->SelectID){
										if(!Select) break;
										EditNode.Node=Node;
										EditNode.Node->FrameColour=Color;
										EditNode.Node->SelectID=List->AddSelection(&EditNode);
										((DXTriangle*)Node)->Surface.dwFlags.b.Frame=true;
									} else {
										if(Select) break;
										List->RemoveSelection(Node->SelectID);
										Node->SelectID=0;
										((DXTriangle*)Node)->Surface.dwFlags.b.Frame=false;
									}
								}
								break;

			case	DOF:		if(SkipSwitch) { DofLevel++; break; }
								NODE.BYTE=(BYTE*)&((DXDof*)Node)->dof;
								if(NODE.DOF->Type>=SWITCH){
									if(SkipSwitch=TheDXEngine.SwitchManageEx(&NODE, Obj->Obj, &Ident))
										DofLevel=1;
								} else
									TheDXEngine.PushMatrixEx(&Ident);
								break;

			case	CLOSEDOF:	if(SkipSwitch){
									if(DofLevel) DofLevel--;
									if(!DofLevel) SkipSwitch=false;
									break;
								}
								TheDXEngine.PopMatrixEx(&Ident);

			default	:			break;
		}
		Node=Node->Next;
	}
	// view last selected on trees
	TheTreeViews->ViewNode();
	List->UpdateTexUsedList();
	TheTextureEdit->UpdateValues();

}


////////////////////////////////////////////////////////////////////////////////////////////////////


// This function fixes to right values any passed model
void	E_FixModelFeatures(E_Object *Obj)
{
	DXNode	*Node;
	E_TraverseModelSurfaces(Obj, true);

	DWORD	SpecularColors[]=SURFACE_SPECULARCOLORS;

	
	// Traverse all the model
	while(Node=E_TraverseModelSurfaces()){
		if(IS_VALID_SURFACE(Node)){
			DXFlagsType Flags=SURFACE(Node).dwFlags;
			DXTriangle	&Triangle=*((DXTriangle*)Node);

			// TEXTURE diffuse color
			if(Flags.b.Texture) for(DWORD i=0; i<SURFACE(Node).dwVCount; i++) {Triangle.Vertex[i].dwColour = Triangle.Vertex[i].dwColour & 0xff000000 | TEXTURED_DEFAULT_DIFFUSE; }
			
			// SPECULARITY - get the specularity index from OFF_SI to GLASS_SI
			DWORD	SpecularIndex=(DWORD)(( SURFACE(Node).SpecularIndex - BASE_SI_INDEX )/SI_INDEX_CX);
			if(SpecularIndex>GLASS_SI) SpecularIndex=GLASS_SI;
			DWORD	SpecularColor=SpecularColors[SpecularIndex];
			// finally setup standard Index and color
			SURFACE(Node).SpecularIndex=(float)SpecularIndex * SI_INDEX_CX + BASE_SI_INDEX;
			SURFACE(Node).DefaultSpecularity=SpecularColor;



		}

		Node=Node->Next;
	}
}


void	E_SelectCommonVertex(E_Object *Obj, CSelectList *List)
{
	DXNode	*Node;
	E_TraverseModelSurfaces(Obj, true);
	
	while(Node=E_TraverseModelSurfaces()) E_ToggleNodeSelect(List, Node);
}


void	E_SelectCommonVertex(bool Set, E_Object *Obj, CSelectList *List, D3DVERTEXEX *Vertex)
{
	DXNode	*Node;
	if(!Vertex) return;
	E_TraverseModelSurfaces(Obj, true);
	

	while(Node=E_TraverseModelSurfaces()){
		if(IS_VALID_SURFACE(Node)){
			for(DWORD a=0; a< SURFACE(Node).dwVCount; a++){
				if(!memcmp(&(((DXTriangle*)Node)->Vertex[a].vx), &Vertex->vx, sizeof(D3DVECTOR))){
					if(Set) List->AddVertex(Node, a);
					else List->RemoveVertex(Node, a);
				}
			}
		}
	}
}


void	E_ClearFaceSelection(CSelectList *List, DWORD *Emissive, DWORD *ZBias)
{
	List->StartParseList();
	DXTriangle	*Node;
	
	while(Node=(DXTriangle*)(List->GetItemFromList()).Node){
		
		switch(Node->Type){
			case	DOT	:	
			case	LINE:	
			case	TRIANGLE:	for(DWORD i=0; i<SURFACE(Node).dwVCount; i++){
									if(Emissive) ((DXTriangle*)Node)->Vertex[i].dwSpecular=*Emissive;
								}								
								if(ZBias){
									SURFACE(Node).dwzBias=*ZBias;
									SURFACE(Node).dwFlags.b.zBias=(*ZBias)?1:0;
								}
							break;


		}

	}
}




void	E_SelectCommonSurface(CSelectList *List, DXFlagsType *Flags, DWORD *ZBias, float *SIndex, DWORD *Tex1, DWORD *Tex2, DWORD *SwitchNr, DWORD *SwitchMask, DWORD *SpecularDefault)
{
	// Get The Model start
	DXNode	*Node;

	List->StartParseList();

	// Traverse all the model
	while(Node=List->GetItemFromList().Node){
		if(IS_VALID_SURFACE(Node)){
			if(Flags) ((DXTriangle*)Node)->Surface.dwFlags.w=Flags->w;
			if(ZBias) ((DXTriangle*)Node)->Surface.dwzBias=*ZBias;
			if(SIndex) ((DXTriangle*)Node)->Surface.SpecularIndex=*SIndex;
			if(Tex1) ((DXTriangle*)Node)->Surface.TexID[0]=*Tex1;
			if(Tex2) ((DXTriangle*)Node)->Surface.TexID[1]=*Tex2;
			if(SwitchNr) SURFACE(Node).SwitchNumber=*SwitchNr;
			if(SwitchMask) SURFACE(Node).SwitchMask=*SwitchMask;
			if(SpecularDefault) SURFACE(Node).DefaultSpecularity=*SpecularDefault;
		}
	}
}


void	E_UpdateFlags(CSelectList *List, DXFlagsType Flags, DXFlagsType FlagMask)
{
	// Initialize the list parsing
	List->StartParseList();
	DXNode	*Node;
	
	// look thru all the list
	while(Node=(List->GetItemFromList()).Node){
		// if a surface
		if(IS_VALID_SURFACE(Node)){
			List->UpdateValues(Node, REMOVE);
			SURFACE(Node).dwFlags.w&=~(FlagMask.w);
			SURFACE(Node).dwFlags.w|=Flags.w;
			List->UpdateValues(Node, ADD);
		}
	}
}





void	E_ClearClip(void)
{
	DXNode	*Node=ClipBoard, *Last;
	ClipBoard=NULL;

	while(Node){
		Last=Node;
		Node=Node->Next;
		delete Last;
	}
}


void	E_CopySelection(CSelectList *List)
{
	// Initialize the list parsing
	List->StartParseList();
	DXNode	*Node, *New=NULL, *Last=NULL;
	E_ClearClip();
	
	while(Node=(List->GetItemFromList()).Node){
		
		switch(Node->Type){

			case	DOT:		New=new DXVertex();
								*(DXVertex*)New=*(DXVertex*)Node;
								((DXVertex*)New)->Surface.dwFlags.b.Frame=false;
								break;
		
			case	LINE:		New=new DXLine();
								*(DXLine*)New=*(DXLine*)Node;
								((DXLine*)New)->Surface.dwFlags.b.Frame=false;
								break;

			case	TRIANGLE:	New=new DXTriangle();
								*(DXTriangle*)New=*(DXTriangle*)Node;
								((DXTriangle*)New)->Surface.dwFlags.b.Frame=false;
								break;

			case	DOF:		New=new DXDof();
								*(DXDof*)New=*(DXDof*)Node;
								break;

			case	SLOT:		New=new DXSlot();
								*(DXSlot*)New=*(DXSlot*)Node;
								break;

			case	CLOSEDOF:	New=new DXEndDof();
								*(DXEndDof*)New=*(DXEndDof*)Node;
								break;
		}
		
		New->Prev=Last;
		if(Last) Last->Next=New;
		else ClipBoard=New;
		New->Next=NULL;
		New->TreeItem=NULL;
		New->SelectV=New->SelectID=0;
		Last=New; 
	}
	List->ClearSelection();
}


void	E_CutSelection(E_Object *Obj, CSelectList *List)
{
	DXNode	*Node, *New=NULL, *Last=NULL;
	E_ClearClip();
	List->StartParseList();
	while(Node=(List->GetItemFromList()).Node){
		TheTreeViews->RemoveNode(Node->TreeItem);
		if(IS_SURFACE(Node) && Node->SelectV){
			for(DWORD a=0; a<SURFACE(Node).dwVCount; a++){
				if(Node->SelectV & (1<<a)) List->RemoveVertex(Node, a);
			}
		}
		// untie the Node from the original list
		if(Node->Prev) Node->Prev->Next=Node->Next;
		else Obj->Nodes=Node->Next;
		if(Node->Next) Node->Next->Prev=Node->Prev;
		New=Node;
		New->Prev=Last;
		New->Next=NULL;
		if(Last) Last->Next=New;
		if(!Last) ClipBoard=New;
		Last=New;
	}
	List->ClearSelection();
}



void	E_KillSelection(E_Object *Obj, CSelectList *List)
{
	DXNode	*Node;
	List->StartParseList();
	while(Node=(List->GetItemFromList()).Node){
		TheTreeViews->RemoveNode(Node->TreeItem);
		if(IS_SURFACE(Node) && Node->SelectV){
			for(DWORD a=0; a<SURFACE(Node).dwVCount; a++){
				if(Node->SelectV & (1<<a)) List->RemoveVertex(Node, a);
			}
		}
		List->RemoveSelection(Node->SelectID);
		// untie the Node from the original list
		if(Node->Prev) Node->Prev->Next=Node->Next;
		else Obj->Nodes=Node->Next;
		if(Node->Next) Node->Next->Prev=Node->Prev;
		delete Node;
	}
}



void	E_InsertNode(E_Object *Obj, DXNode *PastePoint, DXNode *Node)
{
	Node->Prev=Node->Next=NULL;
	if(PastePoint) { 
		Node->Prev=PastePoint;
		Node->Next=PastePoint->Next;
		if(PastePoint->Next) PastePoint->Next->Prev=Node;
		PastePoint->Next=Node;
	} else {
		Node->Next=(DXNode*)Obj->Nodes;
		Obj->Nodes=Node;
		if(Node->Next) Node->Next->Prev=Node;
	}
	
}


void	E_PasteSelection(E_Object *Obj, CSelectList *List, DXNode *PastePoint)
{
	DXNode	*Node=ClipBoard, *New, *Last=PastePoint, *End=PastePoint->Next;
	CEditing	EditNode;
	D3DCOLORVALUE	Color;
	Color.a=Color.r=1.0f; Color.b=Color.g=0.0f;
	
	if(!ClipBoard) return;

	// if at the end of model, insert before end marker
	if(PastePoint->Type==MODELEND){
		Last=PastePoint->Prev;
		End=PastePoint;
	}

	List->ClearSelection();

	// Find last node in cache
	while(Node){ 
		switch(Node->Type){

			case	DOT:		New=new DXVertex();
								*(DXVertex*)New=*(DXVertex*)Node;
								((DXVertex*)New)->Surface.dwFlags.b.Frame=true;
								if(((DXVertex*)New)->Surface.dwFlags.b.Texture){
									if(((DXVertex*)New)->Surface.TexID[0] >= Obj->TexPerSet) ((DXVertex*)New)->Surface.TexID[0] = Obj->TexPerSet-1;
									if(((DXVertex*)New)->Surface.TexID[1] >= Obj->TexPerSet) ((DXVertex*)New)->Surface.TexID[1] = Obj->TexPerSet-1;
								}
								break;
		
			case	LINE:		New=new DXLine();
								*(DXLine*)New=*(DXLine*)Node;
								((DXLine*)New)->Surface.dwFlags.b.Frame=true;
								if(((DXVertex*)New)->Surface.dwFlags.b.Texture){
									if(((DXVertex*)New)->Surface.TexID[0] >= Obj->TexPerSet) ((DXVertex*)New)->Surface.TexID[0] = Obj->TexPerSet-1;
									if(((DXVertex*)New)->Surface.TexID[1] >= Obj->TexPerSet) ((DXVertex*)New)->Surface.TexID[1] = Obj->TexPerSet-1;
								}
								break;

			case	TRIANGLE:	New=new DXTriangle();
								*(DXTriangle*)New=*(DXTriangle*)Node;
								((DXTriangle*)New)->Surface.dwFlags.b.Frame=true;
								if(((DXVertex*)New)->Surface.dwFlags.b.Texture){
									if(((DXVertex*)New)->Surface.TexID[0] >= Obj->TexPerSet) ((DXVertex*)New)->Surface.TexID[0] = Obj->TexPerSet-1;
									if(((DXVertex*)New)->Surface.TexID[1] >= Obj->TexPerSet) ((DXVertex*)New)->Surface.TexID[1] = Obj->TexPerSet-1;
								}
								break;

			case	DOF:		New=new DXDof();
								*(DXDof*)New=*(DXDof*)Node;
								break;

			case	SLOT:		New=new DXSlot();
								*(DXSlot*)New=*(DXSlot*)Node;
								break;

			case	CLOSEDOF:	New=new DXEndDof();
								*(DXEndDof*)New=*(DXEndDof*)Node;
								break;
		}

		EditNode.Node=New;
		EditNode.Node->FrameColour=Color;
		EditNode.Node->SelectID=List->AddSelection(&EditNode);


		if(Last) Last->Next=New;
		else Obj->Nodes=New;
		New->Prev=Last;
		New->Next=NULL;
		New->TreeItem=NULL;
		Last=New;
		Node=Node->Next;
	}

	Last->Next=End;
	if(End) End->Prev=Last;

	TheTreeViews->RebuildBranch(PastePoint->TreeItem);
	List->UpdateTexUsedList();
	TheTextureEdit->UpdateValues();
}



bool	E_CheckBalancedSelection(CSelectList *List)
{
	// Initialize the list parsing
	List->StartParseList();
	DWORD	DofLevel=0;
	DXNode	*Node;
	
	while(Node=(List->GetItemFromList()).Node){
		
		switch(Node->Type){

			case	DOF:		DofLevel++;
								break;
			
			case	CLOSEDOF:	DofLevel--;
								break;
		}
		
	}

	return (DofLevel)?false:true;

}



void	ReverseVertex(D3DVERTEXEX *Vertex)
{
	Vertex->nx=-Vertex->nx;
	Vertex->ny=-Vertex->ny;
	Vertex->nz=-Vertex->nz;
}


void	E_MirrorSelection(CSelectList *List)
{
	List->StartParseList();
	D3DVERTEXEX	Vertex;
	DXTriangle	*Node;
	
	while(Node=(DXTriangle*)(List->GetItemFromList()).Node){
		
		switch(Node->Type){
			case	DOT	:	ReverseVertex(Node->Vertex);
							break;

			case	LINE:	ReverseVertex(&Node->Vertex[0]);
							ReverseVertex(&Node->Vertex[1]);
							Vertex=Node->Vertex[0];
							Node->Vertex[0]=Node->Vertex[1];
							Node->Vertex[1]=Vertex;
							break;

			case	TRIANGLE:	ReverseVertex(&Node->Vertex[0]);
							ReverseVertex(&Node->Vertex[1]);
							ReverseVertex(&Node->Vertex[2]);
							Vertex=Node->Vertex[1];
							Node->Vertex[1]=Node->Vertex[2];
							Node->Vertex[2]=Vertex;
							break;
		}

	}
}


void	E_BackFaceSelection(CSelectList *List)
{
	List->StartParseList();
	D3DVERTEXEX	Vertex;
	DXTriangle	*Node, *New;
	
	while(Node=(DXTriangle*)(List->GetItemFromList()).Node){
		
		switch(Node->Type){
			case	DOT	:		break;

			case	LINE:		break;

			case	TRIANGLE:	New=new DXTriangle();
								*New=*Node;
								New->TreeItem=0;
								ReverseVertex(&New->Vertex[0]);
								ReverseVertex(&New->Vertex[1]);
								ReverseVertex(&New->Vertex[2]);
								Vertex=New->Vertex[1];
								New->Vertex[1]=New->Vertex[2];
								New->Vertex[2]=Vertex;
								New->SelectID=0;
								New->SelectV=0;
								New->Surface.dwFlags.b.Frame=0;
								if(Node->Next) Node->Next->Prev=New;
								New->Next=Node->Next;
								New->Prev=Node;
								Node->Next=New;
								break;
		}

	}
	
	List->ClearSelection();
	TheTreeViews->UpdateTree();
}


void	E_TranslateSelection(CSelectList *List, float	MoveX, float MoveY, float MoveZ)
{
	List->StartParseList();
	DXTriangle	*Node;
	
	while(Node=(DXTriangle*)(List->GetItemFromList()).Node){
		
		switch(Node->Type){
			case	DOT	:		((DXVertex*)Node)->Vertex.vx+=MoveX;
								((DXVertex*)Node)->Vertex.vy+=MoveY;
								((DXVertex*)Node)->Vertex.vz+=MoveZ;
								break;

			case	LINE:		((DXLine*)Node)->Vertex[0].vx+=MoveX;
								((DXLine*)Node)->Vertex[0].vy+=MoveY;
								((DXLine*)Node)->Vertex[0].vz+=MoveZ;
								((DXLine*)Node)->Vertex[1].vx+=MoveX;
								((DXLine*)Node)->Vertex[1].vy+=MoveY;
								((DXLine*)Node)->Vertex[1].vz+=MoveZ;
								break;

			case	TRIANGLE:	((DXLine*)Node)->Vertex[0].vx+=MoveX;
								((DXLine*)Node)->Vertex[0].vy+=MoveY;
								((DXLine*)Node)->Vertex[0].vz+=MoveZ;
								((DXLine*)Node)->Vertex[1].vx+=MoveX;
								((DXLine*)Node)->Vertex[1].vy+=MoveY;
								((DXLine*)Node)->Vertex[1].vz+=MoveZ;
								((DXLine*)Node)->Vertex[2].vx+=MoveX;
								((DXLine*)Node)->Vertex[2].vy+=MoveY;
								((DXLine*)Node)->Vertex[2].vz+=MoveZ;
								break;
		}

	}
	List->RecalcValues();
}



void	E_PlaneSelection(CSelectList *List, float	*X, float *Y, float *Z)
{
	List->StartParseList();
	DXTriangle	*Node;
	
	while(Node=(DXTriangle*)(List->GetItemFromList()).Node){
		if(IS_SURFACE(Node)){
			for(DWORD i=0; i<Node->Surface.dwVCount; i++){
				if(X)((DXTriangle*)Node)->Vertex[i].vx=*X;
				if(Y)((DXTriangle*)Node)->Vertex[i].vy=*Y;
				if(Z)((DXTriangle*)Node)->Vertex[i].vz=*Z;
			}
		}
	}
	List->RecalcValues();
}



void	E_RotateSelection(CSelectList *List, float	AngleX, float AngleY, float AngleZ, D3DXVECTOR3 Center)
{
	D3DXMATRIX	M;
	D3DXMatrixIdentity(&M);
	D3DXMatrixRotationYawPitchRoll(&M, AngleX, AngleY, AngleZ);
	D3DXVECTOR3 V,R;
	List->StartParseList();
	DXTriangle	*Node;
	
	while(Node=(DXTriangle*)(List->GetItemFromList()).Node){
		if(IS_SURFACE(Node)){
			for(DWORD i=0; i<Node->Surface.dwVCount; i++){
				V.x=((DXLine*)Node)->Vertex[i].vx-Center.x;
				V.y=((DXLine*)Node)->Vertex[i].vy-Center.y;
				V.z=((DXLine*)Node)->Vertex[i].vz-Center.z;
				D3DXVec3TransformCoord(&R, &V, &M);
				((DXLine*)Node)->Vertex[i].vx=R.x+Center.x;
				((DXLine*)Node)->Vertex[i].vy=R.y+Center.y;
				((DXLine*)Node)->Vertex[i].vz=R.z+Center.z;
			}
		}
	}
	List->RecalcValues();
}



void	E_MakeModelCopy(CSelectList *List, E_Object *Source, E_Object *Backup)
{

	DXNode	*Node, *New, *Last=NULL;
	
	if(!Source->Nodes) return;

	List->ClearSelection();

	// if a Bacup already present, kill it
	if(Backup->Nodes){
		Node=(DXNode*)Backup->Nodes;
		while(Node){
			New=Node;
			Node=Node->Next;
			delete New;
		}
		Backup->Nodes=NULL;
	}

	// if a backup Light List present, kill it
	if(Backup->LightList){
		E_Light	*ptr=Backup->LightList, *NL;
		while(ptr){
			NL=ptr;
			ptr=ptr->Next;
			delete NL;
		}
	}


	// Copy the Model Header
	*Backup=*Source;

	// Copy light list
	if(Source->LightList){
		E_Light	*ptr=Source->LightList, *LNew, *LLast=NULL;
		while(ptr){
			LNew=new E_Light();
			*LNew=*ptr;
			if(!LLast) Backup->LightList=LNew;
			else LLast->Next=LNew;
			LNew->Prev=LLast;
			LNew->Next=NULL;
			LLast=LNew;
			ptr=ptr->Next;
		}
	}


	// Backup the Lights


	// and get the start of source nodes
	Node=(DXNode*)Source->Nodes;


	// Find last node in cache
	while(Node){ 
		switch(Node->Type){

			case	DOT:		New=new DXVertex();
								*(DXVertex*)New=*(DXVertex*)Node;
								((DXVertex*)New)->Surface.dwFlags.b.Frame=false;
								((DXVertex*)New)->Surface.dwFlags.b.Disable=false;
								break;
		
			case	LINE:		New=new DXLine();
								*(DXLine*)New=*(DXLine*)Node;
								((DXLine*)New)->Surface.dwFlags.b.Frame=false;
								((DXLine*)New)->Surface.dwFlags.b.Disable=false;
								break;

			case	TRIANGLE:	New=new DXTriangle();
								*(DXTriangle*)New=*(DXTriangle*)Node;
								((DXTriangle*)New)->Surface.dwFlags.b.Frame=false;
								((DXTriangle*)New)->Surface.dwFlags.b.Disable=false;
								break;

			case	DOF:		New=new DXDof();
								*(DXDof*)New=*(DXDof*)Node;
								break;

			case	SLOT:		New=new DXSlot();
								*(DXSlot*)New=*(DXSlot*)Node;
								break;

			case	CLOSEDOF:	New=new DXEndDof();
								*(DXEndDof*)New=*(DXEndDof*)Node;
								break;

			case	DX_MODELEND:New=new DXNode(MODELEND);
								*New=*Node;
								break;

		}

		if(Last) Last->Next=New;
		else Backup->Nodes=New;
		New->Prev=Last;
		New->Next=NULL;
		New->TreeItem=NULL;
		New->SelectID=NULL;
		New->SelectV=NULL;
		Last=New;
		New=Node;
		Node=Node->Next;
	}

	TheTreeViews->UpdateTree();
	List->UpdateTexUsedList();
	TheTextureEdit->UpdateValues();
}




void	E_SetMinMaxSI(CSelectList *List, float *Min, float *Max, float *Value)
{
	// Initialize the list parsing
	List->StartParseList();
	DWORD	DofLevel=0;
	DXNode	*Node;
	
	while(Node=(List->GetItemFromList()).Node){
		if(IS_VALID_SURFACE(Node)){
			if(Min && SURFACE(Node).SpecularIndex<*Min) SURFACE(Node).SpecularIndex=List->m_MinSI=*Min;
			if(Max && SURFACE(Node).SpecularIndex>*Max) SURFACE(Node).SpecularIndex=List->m_MaxSI=*Max;
			if(Value) {
				SURFACE(Node).SpecularIndex=*Value;
				if(*Value>List->m_MaxSI) List->m_MaxSI=*Value;
				if(*Value<List->m_MinSI) List->m_MinSI=*Value;
			}
		}
	}

}


void	E_SetMinMaxZB(CSelectList *List, DWORD *Min, DWORD *Max, DWORD *Value)
{
	// Initialize the list parsing
	List->StartParseList();
	DWORD	DofLevel=0;
	DXNode	*Node;
	
	while(Node=(List->GetItemFromList()).Node){
		if(IS_VALID_SURFACE(Node) && SURFACE(Node).dwFlags.b.zBias ){
			if(Min && SURFACE(Node).dwzBias<*Min) SURFACE(Node).dwzBias=List->m_MinZBias=*Min;
			if(Max && SURFACE(Node).dwzBias>*Max) SURFACE(Node).dwzBias=List->m_MaxZBias=*Max;
			if(Value) {
				SURFACE(Node).dwzBias=*Value;
				if(*Value>List->m_MaxZBias) List->m_MaxZBias=*Value;
				if(*Value<List->m_MinZBias) List->m_MinZBias=*Value;
			}
		}
	}

}




void	E_TranslateVertices(CSelectList *List, float	MoveX, float MoveY, float MoveZ)
{
	D3DVERTEXEX	*Vertex;
	List->StartParseVertex();
	// Traverse all vertices
	while(Vertex=List->GetVertexFromList()){
		Vertex->vx+=MoveX;
		Vertex->vy+=MoveY;
		Vertex->vz+=MoveZ;
	}

	List->RecalcValues();
}


void	E_TranslateUV(CSelectList *List, float	MoveU, float MoveV)
{
	D3DVERTEXEX	*Vertex;
	List->StartParseVertex();
	// Traverse all vertices
	while(Vertex=List->GetVertexFromList()){
		Vertex->tu+=MoveU;
		Vertex->tv+=MoveV;
	}

	List->RecalcValues();
}


void	E_SetVertices(CSelectList *List, bool OnlySelection, DWORD *Diffuse, DWORD *Specular, DWORD *DAlpha, DWORD *SAlpha)
{
	D3DVERTEXEX	*Vertex;
	DXNode	*Node;

	List->StartParseVertex();
	List->StartParseList();

	// Traverse all vertices
	if(!OnlySelection){
		while(Vertex=List->GetVertexFromList()){
			DWORD	Spc=Vertex->dwSpecular;
			DWORD	Dfs=Vertex->dwColour;

			if(Specular) Spc=(Spc&0xff000000)|(*Specular&0x00ffffff);
			if(Diffuse) Dfs=(Dfs&0xff000000)|(*Diffuse&0x00ffffff);
			if(SAlpha) Spc=(Spc&0x00ffffff)|((*SAlpha)<<24);
			if(DAlpha) Dfs=(Dfs&0x00ffffff)|((*DAlpha)<<24);
			

			Vertex->dwSpecular=Spc;
			Vertex->dwColour=Dfs;
		}
	}
	
	// Selection by surfaces
	if(OnlySelection){
		while(Node=List->GetItemFromList().Node){
			
			if(IS_SURFACE(Node)){
				for(DWORD v=0; v<SURFACE(Node).dwVCount; v++){
					Vertex=&((DXTriangle*)Node)->Vertex[v];
					DWORD	Spc=Vertex->dwSpecular;
					DWORD	Dfs=Vertex->dwColour;

					if(Specular) Spc=(Spc&0xff000000)|(*Specular&0x00ffffff);
					if(Diffuse) Dfs=(Dfs&0xff000000)|(*Diffuse&0x00ffffff);
					if(SAlpha) Spc=(Spc&0x00ffffff)|((*SAlpha)<<24);
					if(DAlpha) Dfs=(Dfs&0x00ffffff)|((*DAlpha)<<24);
					

					Vertex->dwSpecular=Spc;
					Vertex->dwColour=Dfs;
				}
			}
		}
	}

}


D3DVERTEXEX	*E_SelectVertexInSurfaces(E_Object *Obj, CSelectList *List, bool Selection, bool SingleVertex)
{
	// Get The Model start
	DXNode	*Node;
	D3DVERTEXEX	*Vertex=NULL;
	
	List->StartParseList();

	// Traverse all the model
	while(Node=List->GetItemFromList().Node){
		if(IS_VALID_SURFACE(Node)){
			Vertex=&(((DXTriangle*)Node)->Vertex[0]);
			for(DWORD a=0; a<SURFACE(Node).dwVCount; a++){
				if(!SingleVertex) E_SelectCommonVertex(Selection, Obj, List, &Vertex[a]);
				else (Selection)?List->AddVertex(Node, a):List->RemoveVertex(Node,a);
			}
		}
	}
	return (Selection?Vertex:NULL);
}


void	E_ScaleVertices(CSelectList *List,  D3DXVECTOR3 *Center, float ScaleX, float ScaleY, float ScaleZ)
{
	D3DVERTEXEX	*Vertex;
	List->StartParseVertex();


	// Traverse all vertices
	while(Vertex=List->GetVertexFromList()){
		Vertex->vx+=(Vertex->vx-Center->x)*ScaleX;
		Vertex->vy+=(Vertex->vy-Center->y)*ScaleY;
		Vertex->vz+=(Vertex->vz-Center->z)*ScaleZ;
	}

	List->RecalcValues();
}


void	E_OffsetTexture(CSelectList *List, float U, float V)
{
	D3DVERTEXEX	*Vertex;
	List->StartParseList();
	DXNode	*Node;
	
	while(Node=(List->GetItemFromList()).Node){
		if(IS_VALID_SURFACE(Node)){
			Vertex=&(((DXTriangle*)Node)->Vertex[0]);
			for(DWORD a=0; a<SURFACE(Node).dwVCount; a++){
				Vertex[a].tu+=U;
				Vertex[a].tv+=V;
			}
		}
	}
}


void	E_ScaleTexture(CSelectList *List, float sU, float sV)
{
	D3DVERTEXEX	*Vertex;
	List->StartParseList();
	DXNode	*Node;
	
	while(Node=(List->GetItemFromList()).Node){
		if(IS_VALID_SURFACE(Node)){
			Vertex=&(((DXTriangle*)Node)->Vertex[0]);
			for(DWORD a=0; a<SURFACE(Node).dwVCount; a++){
				Vertex[a].tu*=sU;
				Vertex[a].tv*=sV;
			}
		}
	}
}



void	E_RotateTexture(CSelectList *List, float Angle, float RefV, float RefU)
{
	D3DVERTEXEX	*Vertex;
	List->StartParseList();
	DXNode	*Node;
	
	while(Node=(List->GetItemFromList()).Node){
		if(IS_VALID_SURFACE(Node)){
			Vertex=&(((DXTriangle*)Node)->Vertex[0]);
			for(DWORD a=0; a<SURFACE(Node).dwVCount; a++){
				double v=Vertex[a].tv-RefV;
				double u=Vertex[a].tu-RefU;
				double r=sqrt(u*u+v*v);
				double n=atan2(u,v);
				n+=Angle;
				Vertex[a].tu=(float)(r*sin(n)+RefU);
				Vertex[a].tv=(float)(r*cos(n)+RefV);
/*				if(Vertex[a].tv>1.0f) Vertex[a].tv-=1.0f;
				if(Vertex[a].tv<-0.0f) Vertex[a].tv+=1.0f;
				if(Vertex[a].tu>1.0f) Vertex[a].tu-=1.0f;
				if(Vertex[a].tu<-1.0f) Vertex[a].tu+=2.0f;*/
			}
		}
	}
}




void	E_BlendTexture(CSelectList *List, D3DVERTEXEX	*VRef)
{


	D3DVERTEXEX	*Vertex, *ValidVertex, *BadVertex;
	List->StartParseList();
	DXNode	*Node;
	while(Node=(List->GetItemFromList()).Node){
		if(IS_VALID_SURFACE(Node)){
			Vertex=&(((DXTriangle*)Node)->Vertex[0]);
			for(DWORD a=0; a<SURFACE(Node).dwVCount; a++){
				Vertex[a].tv=Vertex[a].tu=99999.0f;
			}
		}
	}

	float	dx=(List->m_MaxX-List->m_MinX);
	float	dy=(List->m_MaxY-List->m_MinY);
	float	dz=(List->m_MaxZ-List->m_MinZ);
	double	Rate=1/(sqrt(dx*dx+dy*dy+dz*dz));
	bool	NewItem=true;

	VRef->tv=VRef->tu=0.0f;

	Vertex=VRef;

	while(NewItem){
		NewItem=false;

		// Look for common vertices to assign same TU/TV coords
		List->StartParseList();
		while(Node=(List->GetItemFromList()).Node){
			if(IS_VALID_SURFACE(Node)){
				for(DWORD a=0; a< SURFACE(Node).dwVCount; a++){
					if(!memcmp(&(((DXTriangle*)Node)->Vertex[a].vx), &Vertex->vx, sizeof(D3DVECTOR)) &&
						((DXTriangle*)Node)->Vertex[a].tu==99999.0f && ((DXTriangle*)Node)->Vertex[a].tv==99999.0f){
						((DXTriangle*)Node)->Vertex[a].tu=Vertex->tu;
						((DXTriangle*)Node)->Vertex[a].tv=Vertex->tv;
						NewItem=true;
					}
				}
			}
		}
		
		// Look for Unassigned TU/TV ( = 99999.0f )
		List->StartParseList();
		while(Node=(List->GetItemFromList()).Node){
			bool	InvalidT=false, ValidT=false;
			if(IS_VALID_SURFACE(Node)){
				Vertex=((DXTriangle*)Node)->Vertex;
				// check for vertices with unassigned and assigned T Coords
				for(DWORD a=0; a< SURFACE(Node).dwVCount; a++){
					if( Vertex[a].tu==99999.0f && Vertex[a].tv==99999.0f ) { BadVertex=&Vertex[a]; InvalidT=true; }
					else { ValidVertex=&Vertex[a]; ValidT=true; }
				}
				
				// if an invalid vertex with something valid in same surface calculate new T Coords
				if(InvalidT && ValidT){
					dx=ValidVertex->vx-BadVertex->vx;
					dy=ValidVertex->vy-BadVertex->vy;
					dz=ValidVertex->vz-BadVertex->vz;
					
					float	r=sqrtf(dx*dx+dy*dy+dz*dz);
					float	alpha=atan2f(dx, dy);
					float	alphz=atan2f(dx, dz);

					float	nx=r*sin(alpha);
					float	ny=r*cos(alpha);


					BadVertex->tu=(float)(ValidVertex->tu+nx*Rate);
					BadVertex->tv=(float)(ValidVertex->tv+ny*Rate);

					Vertex=BadVertex;
					NewItem=true;
					goto restart;
				}
			}
		}
restart:;		
	}



}


void	E_ClearTexture(E_Object *Obj, DWORD TexNr)
{
	DXNode	*Node;
	E_TraverseModelSurfaces(Obj, true);
	

	while(Node=E_TraverseModelSurfaces()){
		if(IS_VALID_SURFACE(Node) && SURFACE(Node).dwFlags.b.Texture){
			if(SURFACE(Node).TexID[0]==TexNr){
				SURFACE(Node).dwFlags.b.Texture=false;
				SURFACE(Node).TexID[0]=-1;
				continue;
			}
			if(SURFACE(Node).TexID[0]>TexNr){
				SURFACE(Node).TexID[0]--;
				continue;
			}

		}
	}
}


bool	E_CheckUsedTexture(E_Object *Obj, DWORD TexNr)
{
	DXNode	*Node;
	E_GetModelSurfaces(Obj, true);

	while(Node=E_GetModelSurfaces()){
		if(IS_VALID_SURFACE(Node) && SURFACE(Node).dwFlags.b.Texture && SURFACE(Node).TexID[0]==TexNr) return true;
	}
	return false;
}


void	E_AssignTexture(CSelectList *List, DWORD TexNr)
{
	List->StartParseList();
	DXNode	*Node;
	
	while(Node=(List->GetItemFromList()).Node){
		if(IS_VALID_SURFACE(Node)){
			SURFACE(Node).TexID[0]=TexNr;
			SURFACE(Node).dwFlags.b.Texture=true;
		}
	}
}


void	E_SelectByTexture(E_Object *Obj, CSelectList *List, DWORD TexNr, D3DCOLORVALUE *Color)
{
	DXNode	*Node;
	CEditing	EditNode;
	E_TraverseModelSurfaces(Obj, true);

	while(Node=E_TraverseModelSurfaces()){
		if(IS_VALID_SURFACE(Node) && SURFACE(Node).dwFlags.b.Texture){
			if(SURFACE(Node).TexID[0]==TexNr){
				EditNode.Node=Node;
				EditNode.Node->FrameColour=*Color;
				EditNode.Node->SelectID=List->AddSelection(&EditNode);
				((DXTriangle*)Node)->Surface.dwFlags.b.Frame=true;
			}
		}
	}
}

void	E_FlipTexture(CSelectList *List, bool U, bool V)
{
	D3DVERTEXEX	*Vertex;
	float	TU,TV;
	List->RecalcValues();
	TU=(List->m_MaxTU-List->m_MinTU)/2.0f;
	TV=(List->m_MaxTV-List->m_MinTV)/2.0f;

	List->StartParseList();
	DXNode	*Node;
	
	while(Node=(List->GetItemFromList()).Node){
		if(IS_VALID_SURFACE(Node) && SURFACE(Node).dwFlags.b.Texture && SURFACE(Node).TexID[0]!=-1){
			Vertex=((DXTriangle*)Node)->Vertex;
			for(DWORD i=0; i<SURFACE(Node).dwVCount; i++){
				if(U) Vertex[i].tu=TU-(Vertex[i].tu-TU);
				if(V) Vertex[i].tv=TV-(Vertex[i].tv-TV);
			}
		}
	}
}



void	E_MakeAllSpcularity(CSelectList *List, DWORD Color)
{
	List->StartParseList();
	DXNode	*Node;

	
	while(Node=(List->GetItemFromList()).Node){
		if(IS_VALID_SURFACE(Node)){
			SURFACE(Node).DefaultSpecularity=Color;
		}
	}
}


void	E_ClearAllStaticLights(E_Object *Obj)
{
	// Get the start of the model
	DXNode			*Node=(DXNode*)Obj->Nodes;

	// Traverse all the model
	while(Node){
		
		switch(Node->Type){
			
			case	DOT:
			case	LINE:		// do not touch emissive surfces..
			case	TRIANGLE:	//if(!SURFACE(Node).dwFlags.b.Lite) break;
								if(SURFACE(Node).dwFlags.b.SwEmissive && SURFACE(Node).dwFlags.b.SWLightOwner!=0xff){
									SURFACE(Node).SwitchNumber=0;
									SURFACE(Node).SwitchMask=0;
									for(DWORD i=0; i<SURFACE(Node).dwVCount; i++) ((DXTriangle*)Node)->Vertex[i].dwSpecular=0;
									SURFACE(Node).dwFlags.b.SwEmissive=false;
									SURFACE(Node).dwFlags.b.SWLightOwner=0xff;
								}
			case	DOF:		break;
			case	CLOSEDOF:	
			default	:			break;
		}
		Node=Node->Next;
	}

}

void	E_AddStaticLight(E_Object *Obj, DXLightType *Light, DWORD LightID, bool Clear)
{
	DxSurfaceType	*Current=NULL;
	D3DXMATRIX		AppliedState;
	bool			SkipSwitch=false;
	DWORD			DofLevel;

	D3DXMatrixIdentity(&AppliedState);

	float	aR=Light->Light.dcvAmbient.r;
	float	aG=Light->Light.dcvAmbient.g;
	float	aB=Light->Light.dcvAmbient.b;
	float	dR=Light->Light.dcvDiffuse.r;
	float	dG=Light->Light.dcvDiffuse.g;
	float	dB=Light->Light.dcvDiffuse.b;
	float	sR=Light->Light.dcvSpecular.r;
	float	sG=Light->Light.dcvSpecular.g;
	float	sB=Light->Light.dcvSpecular.b;
	float	D=Light->Light.dvRange;
	bool	lited=false;


	// Get the start of the model
	DXNode			*Node=(DXNode*)Obj->Nodes;
	NodeScannerType	NODE;

	// Traverse all the model
	while(Node){
		
		switch(Node->Type){
			
			case	DOT:
			case	LINE:		// do not touch emissive surfces..
			case	TRIANGLE:	//if(!SURFACE(Node).dwFlags.b.Lite) break;
								D3DVERTEXEX	Vertex;
								lited=false;
								if(SURFACE(Node).dwFlags.b.SwEmissive && SURFACE(Node).dwFlags.b.SWLightOwner==LightID
									&& SURFACE(Node).SwitchNumber==Light->Switch && SURFACE(Node).SwitchMask==Light->SwitchMask){
									for(DWORD i=0; i<SURFACE(Node).dwVCount; i++) ((DXTriangle*)Node)->Vertex[i].dwSpecular=0;
									SURFACE(Node).dwFlags.b.SwEmissive=false;
									SURFACE(Node).dwFlags.b.SWLightOwner=0xff;
								}
								if(Clear) break;
								ReCheckLight:
								for(DWORD i=0; i<SURFACE(Node).dwVCount; i++){
									// do not apply color to emissive vertices
									if(((DXTriangle*)Node)->Vertex[i].dwSpecular & 0x00ffffff) continue;
									D3DXVec3TransformCoord((D3DXVECTOR3*)&Vertex.vx, (D3DXVECTOR3*)&((DXTriangle*)Node)->Vertex[i].vx, &AppliedState);
									D3DXVec3TransformNormal((D3DXVECTOR3*)&Vertex.nx, (D3DXVECTOR3*)&((DXTriangle*)Node)->Vertex[i].nx, &AppliedState);
									// calculate distance btw light and vertex
									float x=Vertex.vx-Light->Light.dvPosition.x;
									float y=Vertex.vy-Light->Light.dvPosition.y;
									float z=Vertex.vz-Light->Light.dvPosition.z;
									float d=sqrtf(x*x+y*y+z*z);
									// normalize the Light->Point vector
									x/=d; y/=d; z/=d;
									// normalize the vertex nomal
									/*float k=sqrtf(Vertex.nx*Vertex.nx+Vertex.ny*Vertex.ny+Vertex.nz*Vertex.nz);
									Vertex.nx/=k;
									Vertex.ny/=k;
									Vertex.nz/=k;*/
									// get the Dot product for facing test
									float Dir=Vertex.nx*x+Vertex.ny*y+Vertex.nz*z;
									// if vertex in light range
									if(d<=Light->Light.dvRange){
										if(Dir>=0) Dir=0;
										if(Dir==0 && (aR+aG+aB)==0) break;
										if(!lited) { lited=true; goto ReCheckLight; }
										SURFACE(Node).dwFlags.b.SWLightOwner=LightID;
										SURFACE(Node).SwitchNumber=Light->Switch;
										SURFACE(Node).SwitchMask=Light->SwitchMask;
										SURFACE(Node).dwFlags.b.SwEmissive=true;
										Dir=-Dir;
										// get the Dot product for facing test
										//Dir=Vertex.nx*x+Vertex.ny*y+Vertex.nz*z;
										float	vR=(((float)(((DXTriangle*)Node)->Vertex[i].dwColour&0xff0000))/65536.0f)/255.0f;
										float	vG=(((float)(((DXTriangle*)Node)->Vertex[i].dwColour&0xff00))/256.0f)/255.0f;
										float	vB=(((float)(((DXTriangle*)Node)->Vertex[i].dwColour&0xff)))/255.0f;
										float	A=Light->Light.dvAttenuation0+d*Light->Light.dvAttenuation1;
										float	lR=min(((aR+Dir*vR*dR)/A)*255, 255);
										float	lG=min(((aG+Dir*vG*dG)/A)*255, 255);
										float	lB=min(((aB+Dir*vB*dB)/A)*255, 255);

										
										((DXTriangle*)Node)->Vertex[i].dwSpecular=(((int)lR)<<16)|(((int)lG)<<8)|((int)lB);
									} else {
										if(lited){
											((DXTriangle*)Node)->Vertex[i].dwSpecular=0x0000;
										}
									}


								}

								break;

			case	DOF:		if(SkipSwitch) { DofLevel++; break; }
								NODE.BYTE=(BYTE*)&((DXDof*)Node)->dof;
								if(NODE.DOF->Type<SWITCH){
									TheDXEngine.PushMatrixEx(&AppliedState);
									TheDXEngine.DofManageEx(&NODE, Obj->Obj, &AppliedState);
								} else {
									if(SkipSwitch=TheDXEngine.SwitchManageEx(&NODE, Obj->Obj, &AppliedState))
										DofLevel=1;

								}
								break;

			case	CLOSEDOF:	if(SkipSwitch){
									if(DofLevel) DofLevel--;
									if(!DofLevel) SkipSwitch=false;
									break;
								}
								
								TheDXEngine.PopMatrixEx(&AppliedState);

			default	:			break;
		}
		Node=Node->Next;
	}
}



void	E_AddStaticLight(CSelectList *List, PlaneType PlaneMode)
{
	List->StartParseList();
	DXNode	*Node;

	// recalc all the List min & Max
	List->RecalcValues();
	
	while(Node=(List->GetItemFromList()).Node){
		if(IS_VALID_SURFACE(Node)){
//			SURFACE(Node).DefaultSpecularity=Color;
		}
	}
}



DXNode	*E_GetModelEnd(E_Object *Source)
{

	DXNode	*Node;
	
	if(!Source->Nodes) return NULL;

	// and get the start of source nodes
	Node=(DXNode*)Source->Nodes;


	// Find last node in cache
	while(Node->Type!=MODELEND)	Node=Node->Next;
	return(Node);
}



void	E_MirrorOnAxis(CSelectList *List, D3DXVECTOR3 Center, bool X, bool Y, bool Z)
{
	List->StartParseList();
	DXNode	*Node;
	
	while(Node=(List->GetItemFromList()).Node){
		if(IS_VALID_SURFACE(Node)){
			for(DWORD a=0; a<SURFACE(Node).dwVCount; a++){
				if(X) ((DXTriangle*)Node)->Vertex[a].vx=2*Center.x-((DXTriangle*)Node)->Vertex[a].vx;
				if(Y) ((DXTriangle*)Node)->Vertex[a].vy=2*Center.y-((DXTriangle*)Node)->Vertex[a].vy;
				if(Z) ((DXTriangle*)Node)->Vertex[a].vz=2*Center.z-((DXTriangle*)Node)->Vertex[a].vz;
			}
		}
	}
}



void	E_ScaleSelection(CSelectList *List, float	ScaleX, float ScaleY, float ScaleZ)
{
	List->StartParseList();
	DXTriangle	*Node;
	
	while(Node=(DXTriangle*)(List->GetItemFromList()).Node){
		
		switch(Node->Type){
			case	DOT	:		((DXVertex*)Node)->Vertex.vx*=ScaleX;
								((DXVertex*)Node)->Vertex.vy*=ScaleY;
								((DXVertex*)Node)->Vertex.vz*=ScaleZ;
								break;

			case	LINE:		((DXLine*)Node)->Vertex[0].vx*=ScaleX;
								((DXLine*)Node)->Vertex[0].vy*=ScaleY;
								((DXLine*)Node)->Vertex[0].vz*=ScaleZ;
								((DXLine*)Node)->Vertex[1].vx*=ScaleX;
								((DXLine*)Node)->Vertex[1].vy*=ScaleY;
								((DXLine*)Node)->Vertex[1].vz*=ScaleZ;
								break;

			case	TRIANGLE:	((DXLine*)Node)->Vertex[0].vx*=ScaleX;
								((DXLine*)Node)->Vertex[0].vy*=ScaleY;
								((DXLine*)Node)->Vertex[0].vz*=ScaleZ;
								((DXLine*)Node)->Vertex[1].vx*=ScaleX;
								((DXLine*)Node)->Vertex[1].vy*=ScaleY;
								((DXLine*)Node)->Vertex[1].vz*=ScaleZ;
								((DXLine*)Node)->Vertex[2].vx*=ScaleX;
								((DXLine*)Node)->Vertex[2].vy*=ScaleY;
								((DXLine*)Node)->Vertex[2].vz*=ScaleZ;
								break;
		}

	}
	List->RecalcValues();
}
