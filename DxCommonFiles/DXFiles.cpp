#include "stdafx.h"
#include <math.h>

#define	 ShiAssert(x)

#include "d3dtypes.h"
#include "d3dxmath.h"
#include "ObjectInstance.h"
#include "dxdefines.h"
#include "DXnodes.h"
#include "DXFiles.h"
/////////////////////////////////////////// EDITING TOOL FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

#define	LOD_INDEX(x)		((x-1)/2)
#define	MAKE_LODINDEX(x)	(x * 2 +1)	

#undef sqrt
#undef sqrtf


DWORD	Version;
char			CTNames[MAX_LOD][HDR_LODNAMELEN];
CString			LODNames[MAX_LOD];
int				ctNumber;
CT				*ct=NULL;
HDR				hdr;
HDRCOLOR		hdrColor[MAX_COLOR];
HDRPALETTE		hdrPalette[MAX_PALETTE+2];
HDRLOD			hdrLOD[MAX_LOD];
HDRTEXTURE		hdrTexture[MAX_TEXTURE];
HDRPARENT		hdrParent[MAX_PARENT+2];
HDRLODRES		hdrLODRes[MAX_PARENT+2];

CFile			DBFileIn, DBFileOut;
DWORD			InPosition, OutPosition, InSize;
bool			NotRadiusSwitch;
DWORD			NotRadiusLevel;
DWORD			DofLevel;


float	GetRadarSignature(D3DXVECTOR3 *UpLeft, D3DXVECTOR3 *DnRight)
{
	float	X = UpLeft->x - DnRight->x;
	float	Y = UpLeft->y - DnRight->y;
	float	Z = UpLeft->z - DnRight->z;

	
	if( Z <1.0f ) return 300.0f;
	return sqrtf( X * Y * pow(Z, 1.25f));

}

float	GetRadarSignature(E_Object *Obj)
{
	return GetRadarSignature((D3DXVECTOR3*)&Obj->UpLeft, (D3DXVECTOR3*)&Obj->DnRight);
}




void	AssignVertex(E_Object *Obj, D3DVERTEXEX *Dest, D3DVERTEXEX *Source, unsigned short *VIdx)
{
	// calculate radius if not inside a NON RADIUS SWITCH
	*Dest=Source[*VIdx];
	
	if(!DofLevel){
		if(Source[*VIdx].vx<Obj->DnRight.x) Obj->DnRight.x=Source[*VIdx].vx;
		if(Source[*VIdx].vy<Obj->DnRight.y) Obj->DnRight.y=Source[*VIdx].vy;
		if(Source[*VIdx].vz<Obj->DnRight.z) Obj->DnRight.z=Source[*VIdx].vz;

		if(Source[*VIdx].vx>Obj->UpLeft.x) Obj->UpLeft.x=Source[*VIdx].vx;
		if(Source[*VIdx].vy>Obj->UpLeft.y) Obj->UpLeft.y=Source[*VIdx].vy;
		if(Source[*VIdx].vz>Obj->UpLeft.z) Obj->UpLeft.z=Source[*VIdx].vz;

		float	r=sqrtf(Source[*VIdx].vx*Source[*VIdx].vx + Source[*VIdx].vy*Source[*VIdx].vy + Source[*VIdx].vz * Source[*VIdx].vz);
		if(Obj->Radius<r) Obj->Radius=r;
	}
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


	Obj->NodeNr=0;
	Obj->Nodes=NULL;
	Obj->LightList=NULL;

	Obj->DnRight.x=Obj->DnRight.y=Obj->DnRight.z=0;
	Obj->UpLeft.x=Obj->UpLeft.y=Obj->UpLeft.z=0;
	Obj->TexNr=0, Obj->MaxTexNr = 0;

	Obj->Name="";

}






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


DWORD	GetModelVersion(DxDbHeader *S)
{

	DWORD	Version;
	// Version Check, version has to be reversed in upper 16 bits
	Version=((DxDbHeader*)S)->Version;
	Version^=0xffff0000;
	// eventually OLD NO MORE USED VERSION
	if((Version>>16)!=(Version&0xffff)) Version=-1;
	else Version&=0xffff;
	return Version;
}


// Get the Model Texture Number
DWORD	GetModelTexture(DxDbHeader *Model, DWORD Index)
{
	// Consisency Check
	if(Index >= Model->dwTexNr) return -1;
	return 	((DWORD*)((char*)Model + sizeof(DxDbHeader)))[Index];
}



// This function convert a DXM model from Indexed to Linear
void	DeIndexModel(void *Source, E_Object *Obj, bool KillStaticLights)
{
	// *********** Types used in translations - will be eventually changed by Version ****************
	DWORD			DxDbHeaderSize=sizeof(DxDbHeader);
	void			*DxDbHeaderTarget=(void*)&Obj->Header;
	DWORD			SurfaceSize=sizeof(DxSurfaceType), DXLightSize=sizeof(DXLightType);
	DxSurfaceType	Surface;

	bool	TexUsed[256];

	memset(TexUsed, false, sizeof(TexUsed));

	DofLevel = 0;

	// Version Check, version has to be reversed in upper 16 bits
	Version=GetModelVersion((DxDbHeader*)Source);


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

	// Default Max Texture Used to none
	Obj->MaxTexNr = 0;
	//and calculate them
	for(DWORD t=0; t<Obj->Header.dwTexNr; t++){
		// Update max textures used
		if(Obj->MaxTexNr < (Obj->Textures[t]+1))
			Obj->MaxTexNr = Obj->Textures[t]+1;
	}

	// Build up the lights list
	E_Light	*DxL=NULL, *DxLp;
	DWORD	LightsNr=0;
	Obj->LightList=NULL;
	for(DWORD l=0; l<Obj->Header.dwLightsNr; l++){
		DxL=new E_Light();
		CopyLight(&DxL->Light, ((char*)Source+Obj->Header.pLightsPool + l * DXLightSize));
		// Staic light kill feature... if it's staic delete last created object, can not be done before not being sure of light version
		if(KillStaticLights && DxL->Light.Flags.Static){
			delete DxL;
		} else {
			if( !Obj->LightList ) { Obj->LightList=DxL; DxLp=NULL; }
			else DxLp->Next=DxL;
			
			DxL->Prev=DxLp;
			DxL->Next=NULL;
			DxL->Selected=false;
			DxLp=DxL;
			LightsNr++;
		}
	}
	Obj->Header.dwLightsNr=LightsNr;

	// copy the scripts
	*Obj->Scripts=*Obj->Header.Scripts;

	// and assign the number of textures
	Obj->TexNr=Obj->Header.dwTexNr;

	// clear radius
	Obj->Radius=0.0f;

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
										unsigned short	*VIdx=(unsigned short*)(N.BYTE+SurfaceSize);
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
										unsigned short	*VIdx=(unsigned short*)(N.BYTE+SurfaceSize);
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
										unsigned short	*VIdx=(unsigned short*)(N.BYTE+SurfaceSize);
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

									

			
			case	DX_DOF:			DofLevel++;
									New=new DXDof();
									New->NodeNumber=Obj->NodeNr++;
									if(!DxN) { Obj->Nodes=New; DxN=NULL; }
									else DxN->Next=New;
									New->Prev=DxN;
									DxN=New;
									((DXDof*)New)->dof=*N.DOF;
									break;

			case	DX_ENDDOF:		DofLevel--;
									New=new DXEndDof();
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


#define	MAX_NODE_ROOTS	4096


void	JoinSwitches(DXNode *Node)
{
	struct	{ DXNode *Node; DWORD SwitchNr, Branch, Level; } Switches[MAX_NODE_ROOTS];
	DWORD	Level=0, SwNr=0;
	NodeScannerType N;

	memset(Switches, -1, sizeof(Switches));
	// get the start of nodes
	N.BYTE=(BYTE*)Node;

	// Till end of Model
	while(N.HEAD->Type!=DX_MODELEND){
		
		// Selects actions for each node
		switch(N.HEAD->Type){

	
			case	DX_SWITCH:		
			case	DX_LIGHT:	
			case	DX_TEXTURE:		
			case	DX_MATERIAL:	
			case	DX_ROOT:		break;
			case	DX_SURFACE:		break;
			case	DX_SLOT:		break;
									
			case	DX_DOF:			if(N.DOF->Type==SWITCH || N.DOF->Type==XSWITCH){
										Switches[SwNr].Node=(DXNode*)N.BYTE;
										Switches[SwNr].SwitchNr=N.DOF->SwitchNumber;
										Switches[SwNr].Branch=N.DOF->SwitchBranch;
										Switches[SwNr].Level=Level;
										SwNr++;
									}
									break;


			case	DX_ENDDOF:		break;

			default			:		char s[128]; 
									printf(s, "Corrupted Model !!!");
									MessageBox(NULL, s, "DX DB Tool", NULL);
			
		}
		// Traverse the model
		N.BYTE+=N.HEAD->dwNodeSize;
	}
	


}


DXNode	*LookForSameNode(DXNode **Roots, DXNode *Node)
{
	DWORD	a=0;
	while(a<MAX_NODE_ROOTS && Roots[a]){

	}
	return Node;
}










// This function convert a DXM model from Indexed to Linear
void	GetModelFeatures(void *Source, DWORD *nSwitches, DWORD *nDOFs, DWORD *nSlots, D3DXVECTOR3 *SlotsArray)
{

	DxDbHeader *Header=(DxDbHeader*)Source;

	*nSwitches=0;
	*nDOFs=0;
	*nSlots=0;

	// Build up the lights list
	DXLightType	*DxL=(DXLightType*)((char*)Source+Header->pLightsPool);
	for(DWORD l=0; l<Header->dwLightsNr; l++){
		if((DxL[l].Switch!=-1) && ((DxL[l].Switch+1)>*nSwitches)) *nSwitches=DxL[l].Switch+1;
	}

	NodeScannerType N;
	// get the start of nodes
	N.BYTE=(BYTE*)((char*)Source + sizeof(DxDbHeader) + Header->dwTexNr * sizeof(DWORD));

	// the start of the list
	DXNode *DxN=NULL;

	// Till end of Model
	while(N.HEAD->Type!=DX_MODELEND){
		
		// Selects actions for each node
		switch(N.HEAD->Type){

			case	DX_SWITCH:		
			case	DX_LIGHT:	
			case	DX_TEXTURE:		
			case	DX_MATERIAL:	
			case	DX_ROOT:		break;

			case	DX_SURFACE:		if(N.SURFACE->dwFlags.b.SwEmissive && (N.SURFACE->SwitchNumber!=-1) && (N.SURFACE->SwitchNumber+1)>*nSwitches)
										*nSwitches=N.SURFACE->SwitchNumber+1;
										for(DWORD a=0; a<N.SURFACE->dwVCount; a++){

										}

									break;

									

			
			case	DX_DOF:			switch(N.DOF->Type){

										case	NO_DOF:		break;

										// * POSITIONAL DOF MANAGEMENT *
										case	ROTATE:		
										case	XROTATE:
										case	TRANSLATE:
										case	SCALE:		if(((DWORD)N.DOF->dofNumber+1)>*nDOFs) *nDOFs=N.DOF->dofNumber+1;
															break;
										
										case	SWITCH:
										case	XSWITCH:	if(((DWORD)N.DOF->SwitchNumber+1)>*nSwitches) *nSwitches=N.DOF->SwitchNumber+1;
															break;

									}
									break;

			case	DX_ENDDOF:		break;
									
									
			case	DX_SLOT:		if((N.SLOT->SlotNr+1)>*nSlots) *nSlots=(N.SLOT->SlotNr)+1;
									if(SlotsArray){
										SlotsArray[N.SLOT->SlotNr].x = N.SLOT->rotation.m30;
										SlotsArray[N.SLOT->SlotNr].y = N.SLOT->rotation.m31;
										SlotsArray[N.SLOT->SlotNr].z = N.SLOT->rotation.m32;
									}

									break;
									

			default			:		char s[128]; 
									printf(s, "Corrupted Model !!!");
									MessageBox(NULL, s, "DX DB Tool", NULL);
			
		}
		// Traverse the model
		N.BYTE+=N.HEAD->dwNodeSize;
	}

}



// This function convert a DXM model from Indexed to Linear
float	GetModelRadius(void *Source)
{

	DxDbHeader *Header=(DxDbHeader*)Source;

	D3DVERTEXEX* Vertices=(D3DVERTEXEX*)((char*)Source + Header->pVPool);

	NodeScannerType N;
	// get the start of nodes
	N.BYTE=(BYTE*)((char*)Source + sizeof(DxDbHeader) + Header->dwTexNr * sizeof(DWORD));

	// the start of the list
	DXNode *DxN=NULL;
	
	float	Radius = 0.0f;
	DofLevel = 0;

	// Till end of Model
	while(N.HEAD->Type!=DX_MODELEND){
		
		// Selects actions for each node
		switch(N.HEAD->Type){

			case	DX_SWITCH:		
			case	DX_LIGHT:	
			case	DX_TEXTURE:		
			case	DX_MATERIAL:	
			case	DX_SLOT:		
			case	DX_ROOT:		break;

			case	DX_SURFACE:		if(!DofLevel){
										unsigned short	*VIdx=(unsigned short*)(N.BYTE+sizeof(DxSurfaceType));
										for(DWORD a=0; a<N.SURFACE->dwVCount; a++){
											float	x = Vertices[*VIdx].vx;
											float	y = Vertices[*VIdx].vy;
											float	z = Vertices[*VIdx].vz;
											float	r=sqrtf(x*x + y*y + z*z);
											if(r > Radius ) Radius = r;
											VIdx++;
										}
									}
									break;

			case	DX_DOF:			DofLevel++;
									break;
			case	DX_ENDDOF:		if(DofLevel) DofLevel--;
									break;
									

			default			:		char s[128]; 
									printf(s, "Corrupted Model !!!");
									MessageBox(NULL, s, "DX DB Tool", NULL);
			
		}
		// Traverse the model
		N.BYTE+=N.HEAD->dwNodeSize;
	}

	return Radius;
}



// This function convert a DXM model from Indexed to Linear
void	GetModelHitBox(void *Source, D3DXVECTOR3 *UpLeft, D3DXVECTOR3 *DnRight)
{

	DxDbHeader *Header=(DxDbHeader*)Source;

	D3DVERTEXEX* Vertices=(D3DVERTEXEX*)((char*)Source + Header->pVPool);

	NodeScannerType N;
	// get the start of nodes
	N.BYTE=(BYTE*)((char*)Source + sizeof(DxDbHeader) + Header->dwTexNr * sizeof(DWORD));

	// the start of the list
	DXNode *DxN=NULL;
	
	memset(DnRight, 0, sizeof(D3DXVECTOR3));
	memset(UpLeft, 0, sizeof(D3DXVECTOR3));
	DofLevel = 0;

	// Till end of Model
	while(N.HEAD->Type!=DX_MODELEND){
		
		// Selects actions for each node
		switch(N.HEAD->Type){

			case	DX_SWITCH:		
			case	DX_LIGHT:	
			case	DX_TEXTURE:		
			case	DX_MATERIAL:	
			case	DX_SLOT:		
			case	DX_ROOT:		break;

			case	DX_SURFACE:		if(!DofLevel){
										unsigned short	*VIdx=(unsigned short*)(N.BYTE+sizeof(DxSurfaceType));
										for(DWORD a=0; a<N.SURFACE->dwVCount; a++){
											float	x = Vertices[*VIdx].vx;
											float	y = Vertices[*VIdx].vy;
											float	z = Vertices[*VIdx].vz;
											if(x<DnRight->x) DnRight->x=x;
											if(y<DnRight->y) DnRight->y=y;
											if(z<DnRight->z) DnRight->z=z;

											if(x>UpLeft->x) UpLeft->x=x;
											if(y>UpLeft->y) UpLeft->y=y;
											if(z>UpLeft->z) UpLeft->z=z;
											VIdx++;
										}
									}
									break;

			case	DX_DOF:			DofLevel++;
									break;
			case	DX_ENDDOF:		if(DofLevel) DofLevel--;
									break;
									

			default			:		char s[128]; 
									printf(s, "Corrupted Model !!!");
									MessageBox(NULL, s, "DX DB Tool", NULL);
			
		}
		// Traverse the model
		N.BYTE+=N.HEAD->dwNodeSize;
	}

}




// Old/New hdr files management

void	CompileNewName	(char *Source)
{
	DWORD	a,NameCount=0,len=(DWORD)strlen(Source);
	for(a=0; a<len; a++){
		switch(Source[a]){

			case	'|'		:
			case	'?'		:
			case	'"'		:
			case	'*'		:
			case	'.'		:
			case	'/'		:
			case	'\\'	:	Source[a]='_';
							break;

							break;
		}
	}

	for(a=0; a<MAX_LOD; a++){
		if(!memcmp(CTNames[a], Source, len)) NameCount++;
	}

	sprintf(Source,"%s_%02d",Source,NameCount);

}


void	CompileCTNames(CString Path)
{
	CFile			F;
	DWORD		UnkName=0;

	for(DWORD a=0; a<MAX_LOD; a++) {
		sprintf(CTNames[a],"CT_%04d", a);
		LODNames[a]="";
	}
}


float	dogetfloat(CFile f)
{
	float	x;
	f.Read(&x, sizeof(x));
	return x;
}


DWORD	dogetlong(CFile f)
{
	DWORD	x;
	f.Read(&x, sizeof(x));
	return x;
}

char	dogetchar(CFile f)
{
	char	x;
	f.Read(&x, sizeof(x));
	return x;
}


unsigned short int	dogetshort(CFile f)
{
	unsigned short int	x;
	f.Read(&x, sizeof(x));
	return x;
}



void	doputlong(long x, CFile f)
{
	f.Write(&x, sizeof(x));
}

void	doputfloat(float x, CFile f)
{
	f.Write(&x, sizeof(x));
}

void	doputchar(char x, CFile f)
{
	f.Write(&x, sizeof(x));
}


void	doputshort(unsigned short int x, CFile f)
{
	f.Write(&x, sizeof(x));
}

short int LoadHDR(CString FileName,  bool DXMode)// send to the function the FILE pointer of your open file stream (Koreaobj.hdr)
{
	DWORD						c,d;
	int						cc, maxNodes;
	CFile fp;

	if(!fp.Open(FileName, CFile::modeRead, NULL)) return 0;


	// Zero out HDR records
	ZeroMemory(&hdrLOD,sizeof(HDRLOD)*MAX_LOD);
	ZeroMemory(&hdrLODRes,sizeof(HDRLODRES)*MAX_PARENT+2);

	cc = 1;
	hdr.version = dogetlong(fp);// dogetlong simply reads in a 4 byte integer
															// since the Mac reads bytes differently, I use a function to read in the bytes and reverse the order.
	hdr.nColor = dogetlong(fp);
	if((hdr.nColor > MAX_COLOR) | (hdr.nColor < 1))
		return (1);
	hdr.nDarkColor = dogetlong(fp);
	for(c=0;c<hdr.nColor;c++)
	{
		for(d=0;d<4;d++)
			hdrColor[c].f[d] = dogetfloat(fp);// dogetfloat reads in a 4 byte float
		
		if (c == 774)	// Formation strip-lights
		{
			hdrColor[c].f[0] = 0.6f;
			hdrColor[c].f[1] = 0.6f;
			hdrColor[c].f[2] = 0.0f;
		}
	}
	hdr.nPalette = dogetlong(fp);
	if((hdr.nPalette > MAX_PALETTE) | (hdr.nPalette < 1))
//		return (2);
		cc = 2;
	for(c=0;c<hdr.nPalette;c++)
	{
		for(d=0;d<256;d++)
		{
			hdrPalette[c].r[d] = dogetchar(fp);  
			hdrPalette[c].g[d] = dogetchar(fp);
			hdrPalette[c].b[d] = dogetchar(fp);
			hdrPalette[c].a[d] = dogetchar(fp);
		}
		hdrPalette[c].uint[0] = dogetlong(fp); 
		hdrPalette[c].uint[1] = dogetlong(fp); 
	}
	hdr.nTexture = dogetlong(fp);
	if((hdr.nTexture > MAX_TEXTURE) | (hdr.nTexture < 1))
		return (3);
	
	hdr.unk[0] = dogetlong(fp); // Largest texture (compressed size)
	for(c=0;c<hdr.nTexture;c++)
	{
		for(d=0;d<10;d++)
			hdrTexture[c].uint[d] = dogetlong(fp);
	}
	hdr.unk[1] = dogetlong(fp); // Max number of nodes
	maxNodes = hdr.unk[1];
	if (hdr.unk[1] < MAX_NODES)
		maxNodes = hdr.unk[1] = MAX_NODES;
	hdr.nLOD = dogetlong(fp);
	if((hdr.nLOD > MAX_LOD) | (hdr.nLOD < 1))
		return (4);
	for(c=0;c<hdr.nLOD;c++)
	{
		for(d=0;d<5;d++)										 // [0], [1], [2] not used 
			hdrLOD[c].uint[d] = dogetlong(fp); // [3] KO.lod file offset
	 																			 // [4] LOD length
	}
	hdr.nParent = dogetlong(fp);
	if((hdr.nParent > MAX_PARENT) || (hdr.nParent < 1))
		return (5);
	for(c=0;c<hdr.nParent;c++)
	{
		if(c==2455)
			_asm nop;
		for(d=0;d<7;d++)
			hdrParent[c].f[d] = dogetfloat(fp);	// Hitbox +"radius" (shadow size???)
		hdrParent[c].RadarSignature = dogetfloat(fp);	// file offset [0] & size [1]
		hdrParent[c].IRSignature = dogetfloat(fp);	// file offset [0] & size [1]
		for(d=0;d<2;d++)
			hdrParent[c].ushort1[d] = dogetshort(fp);// nTexures [0] & nDynamics[1]
		for(d=0;d<4;d++)
			hdrParent[c].uchar[d] = dogetchar(fp); // nLODs [0], nSwitches [1], nDOFs [2], nSlots [3]
		for(d=0;d<2;d++)
			hdrParent[c].ushort2[d] = dogetshort(fp);	// Unk1 [0] & Unk2 [1]
	}
	for(c=0;c<hdr.nParent;c++)
	{
		if(hdrParent[c].uchar[3] > 100)
			return (6);
		if(hdrParent[c].uchar[3] > 0)// Load Slots
		{
			for(d=0;d<hdrParent[c].uchar[3];d++)
			{
				hdrLODRes[c].slotX[d] = dogetfloat(fp);
				hdrLODRes[c].slotY[d] = dogetfloat(fp);
				hdrLODRes[c].slotZ[d] = dogetfloat(fp);
			}
		}
		if(hdrParent[c].ushort1[1] > 10)
			return (7);
		if(hdrParent[c].ushort1[1] > 0)// Load Dynamic
		{
			for(d=0;d<hdrParent[c].ushort1[1];d++)
			{
				hdrLODRes[c].dyn1[d] = dogetfloat(fp);
				hdrLODRes[c].dyn2[d] = dogetfloat(fp);
				hdrLODRes[c].dyn3[d] = dogetfloat(fp);
			}
		}
		if(hdrParent[c].uchar[0] > 10)
			return (8);

		if(hdrParent[c].uchar[0] > 0)// Load LOD indexes
		{
			for(d=0;d<hdrParent[c].uchar[0];d++)
			{
				if(DXMode)
					fp.Read(hdrLODRes[c].LODName[d],32);
				hdrLODRes[c].lodind[d] = dogetlong(fp);
				hdrLODRes[c].distance[d] = dogetfloat(fp);
			}
		}
	}
	fp.Close();
	return (cc);
} // end LoadHDR()


void	ResetParentRecords()
{
	for(DWORD c=0;c<hdr.nParent;c++){
		if(hdrParent[c].nLODs > 0){// Save LOD indexes
//			for(DWORD d=0;d<hdrParent[c].nLODs;d++){
				hdrParent[c].nSwitches=0; 
				hdrParent[c].nDOFs=0; 
				hdrParent[c].nSlots=0;
				//hdrParent[c].RadarSignature = 0.0f;
				//hdrParent[c].IRSignature = 0.0f;
//			}
		}
	}
}




void	UpdateParentRecord(DWORD Id, DWORD *nSwitches, DWORD *nDOFs, DWORD *nSlots, float *Radius, bool Force, D3DXVECTOR3 *SlotsArray, D3DXVECTOR3 *HitBox)
{
	for(DWORD c=0;c<hdr.nParent;c++){
		if(hdrParent[c].nLODs > 0){// Save LOD indexes
			for(DWORD d=0;d<hdrParent[c].nLODs;d++){
				if(LOD_INDEX(hdrLODRes[c].lodind[d])==Id){
					if(nSwitches && (hdrParent[c].nSwitches<*nSwitches || Force )) hdrParent[c].nSwitches=(char)*nSwitches; 
					if(nDOFs && (hdrParent[c].nDOFs<*nDOFs || Force )) hdrParent[c].nDOFs=(char)*nDOFs; 
					if(nSlots && (hdrParent[c].nSlots<*nSlots || Force )) hdrParent[c].nSlots=(char)*nSlots;
					if(Radius && ((hdrParent[c].Radius<*Radius && d==0) || Force )) 
							hdrParent[c].Radius=*Radius;
					if(SlotsArray){
						for(DWORD a=0; a<hdrParent[c].nSlots; a++){
							hdrLODRes[c].slotX[a] = SlotsArray[a].x;
							hdrLODRes[c].slotY[a] = SlotsArray[a].y;
							hdrLODRes[c].slotZ[a] = SlotsArray[a].z;
						}
					}
					if(HitBox) SetParentBox(c, HitBox[0], HitBox[1]);
				}
			}
		}
	}
}


void	UpdateSinglePRecord(DWORD Parent, DWORD *nSwitches, DWORD *nDOFs, DWORD *nSlots, float *Radius, D3DXVECTOR3 *SlotsArray, D3DXVECTOR3 *HitBox)
{
	if(nSwitches) hdrParent[Parent].nSwitches=(char)*nSwitches; 
	if(nDOFs) hdrParent[Parent].nDOFs=(char)*nDOFs; 
	if(nSlots) hdrParent[Parent].nSlots=(char)*nSlots;
	if(Radius) hdrParent[Parent].Radius=*Radius;
	if(SlotsArray){
		for(DWORD a=0; a<hdrParent[Parent].nSlots; a++){
			hdrLODRes[Parent].slotX[a] = SlotsArray[a].x;
			hdrLODRes[Parent].slotY[a] = SlotsArray[a].y;
			hdrLODRes[Parent].slotZ[a] = SlotsArray[a].z;
		}
	}
	if(HitBox) SetParentBox(Parent, HitBox[0], HitBox[1]);
}


D3DXVECTOR3	GetParentSlots(DWORD Parent, DWORD SlotNr)
{
	D3DXVECTOR3	Coords(hdrLODRes[Parent].slotX[SlotNr], hdrLODRes[Parent].slotY[SlotNr], hdrLODRes[Parent].slotZ[SlotNr]);
	return Coords;
	
}



void	SetParentLod(DWORD Parent, DWORD LodLevel, DWORD LodId, CString Name)
{
	// Make sure there are enough Lods in this parent
	if(hdrParent[Parent].nLODs < (unsigned char)(LodLevel + 1)) hdrParent[Parent].nLODs = (unsigned char)(LodLevel + 1);
	// assign the lod Index, and name
	hdrLODRes[Parent].lodind[LodLevel] = MAKE_LODINDEX(LodId);
	memset(hdrLODRes[Parent].LODName[LodLevel], 0, HDR_LODNAMELEN);
	memcpy(hdrLODRes[Parent].LODName[LodLevel], Name, min(strlen(Name), HDR_LODNAMELEN));

}

void	SetParentBox(DWORD Parent, D3DVECTOR *UpLeft, D3DVECTOR *DnRight)
{
	hdrParent[Parent].XMax = UpLeft->x;
	hdrParent[Parent].YMax = UpLeft->y;
	hdrParent[Parent].ZMax = UpLeft->z;
	hdrParent[Parent].XMin = DnRight->x;
	hdrParent[Parent].YMin = DnRight->y;
	hdrParent[Parent].ZMin = DnRight->z;
}


void	SetParentTexSets(DWORD Parent, DWORD TexSets)
{
	hdrParent[Parent].TextureSets = (unsigned char)TexSets;
}

DWORD	GetParentTexSets(DWORD Parent)
{
	return(hdrParent[Parent].TextureSets);
}

void	GetParentData(DWORD Parent, HDRPARENT *ptr)
{
	*ptr = hdrParent[Parent];
}


void	SetParentSignatures(DWORD Parent, float *Radar, float *IR)
{
	if(IR) hdrParent[Parent].IRSignature = *IR;
	if(Radar) hdrParent[Parent].RadarSignature = *Radar;
}


void	SetParentDistance(DWORD Parent, DWORD LodLevel, float Distance)
{
	if(hdrParent[Parent].nLODs < LodLevel + 1) return;
	hdrLODRes[Parent].distance[LodLevel] = Distance;
}

void	SetLodDistance(DWORD Id, float Distance)
{
	for(DWORD c=0;c<hdr.nParent;c++){
		if(hdrParent[c].nLODs > 0){// Save LOD indexes
			for(DWORD d=0;d<hdrParent[c].nLODs;d++){
				if(LOD_INDEX(hdrLODRes[c].lodind[d])==Id){
					hdrLODRes[c].distance[d] = Distance;
				}
			}
		}
	}
}



void	UpdateParentSignature(DWORD Id, float *RadarSignature, float *IRSignature, bool Force)
{
	for(DWORD c=0;c<hdr.nParent;c++){
		if(hdrParent[c].nLODs > 0){// Save LOD indexes
			if(LOD_INDEX(hdrLODRes[c].lodind[0])==Id){
				if(RadarSignature && (hdrParent[c].RadarSignature<*RadarSignature || Force )) hdrParent[c].RadarSignature=*RadarSignature; 
				if(IRSignature && (hdrParent[c].IRSignature<*IRSignature || Force )) hdrParent[c].IRSignature=*IRSignature; 
			}
		}
	}
}



bool	SaveHDR(CString FileName, bool DXMode)
{
	unsigned int						c,d;
	CFile	fp;

	if(!fp.Open(FileName, CFile::modeWrite|CFile::modeCreate, NULL)) return false;
	char	LODName[HDR_LODNAMELEN];
	doputlong((long)hdr.version,fp);
	if(hdr.nColor > MAX_COLOR)
		hdr.nColor = MAX_COLOR;
	doputlong((long)hdr.nColor,fp);
	doputlong((long)hdr.nDarkColor,fp);
	for(c=0;c<hdr.nColor;c++)
	{
		for(d=0;d<4;d++)
			doputfloat(hdrColor[c].f[d],fp);
	}
	if(hdr.nPalette > MAX_PALETTE)
		hdr.nPalette = MAX_PALETTE;
	doputlong(hdr.nPalette,fp);
	for(c=0;c<hdr.nPalette;c++)
	{
		for(d=0;d<256;d++)
		{
			doputchar(hdrPalette[c].r[d],fp);
			doputchar(hdrPalette[c].g[d],fp);
			doputchar(hdrPalette[c].b[d],fp);
			doputchar(hdrPalette[c].a[d],fp);
		}
		doputlong(hdrPalette[c].uint[0],fp);
		doputlong(hdrPalette[c].uint[1],fp);
	}
	if(hdr.nTexture > MAX_TEXTURE)
		hdr.nTexture = MAX_TEXTURE;
	doputlong(hdr.nTexture,fp);// 21
	doputlong(hdr.unk[0],fp);
	for(c=0;c<hdr.nTexture;c++)
	{
		for(d=0;d<10;d++)
			doputlong(hdrTexture[c].uint[d],fp);
	}
	// Make sure max number of nodes is big enough
	if (hdr.unk[1] < 10000)
		hdr.unk[1] = 10000;
	doputlong(hdr.unk[1],fp);
	if(hdr.nLOD > MAX_LOD)
		hdr.nLOD = MAX_LOD;
	doputlong(hdr.nLOD,fp);// 54
	for(c=0;c<hdr.nLOD;c++)
	{
		if(c==1629)
				_asm nop;

		for(d=0;d<5;d++)
			doputlong(hdrLOD[c].uint[d],fp);		// <<====  use hdrLOD[c].uint[0] - hdrLOD[c].uint[2] for dxl offset and size
	}
	if(hdr.nParent > MAX_PARENT)
		hdr.nParent = MAX_PARENT;
	doputlong(hdr.nParent,fp);// 16
	for(c=0;c<hdr.nParent;c++)
	{
		if(hdrParent[c].uchar[3] > 100)
			hdrParent[c].uchar[3] = 100;
		if(hdrParent[c].ushort1[1] > 10)
			hdrParent[c].ushort1[1] = 10;
		if(hdrParent[c].uchar[0] > 10)
			hdrParent[c].uchar[0] = 10;
		for(d=0;d<7;d++)
			doputfloat(hdrParent[c].f[d],fp);
		doputfloat(hdrParent[c].RadarSignature,fp);
		doputfloat(hdrParent[c].IRSignature,fp);
		for(d=0;d<2;d++)
			doputshort(hdrParent[c].ushort1[d],fp);
		for(d=0;d<4;d++)
			doputchar(hdrParent[c].uchar[d],fp);
		for(d=0;d<2;d++)
			doputshort(hdrParent[c].ushort2[d],fp);
	}
	for(c=0;c<hdr.nParent;c++)
	{
		if(hdrParent[c].uchar[3] > 0)// Save Slots
		{
			for(d=0;d<hdrParent[c].uchar[3];d++)
			{
				doputfloat(hdrLODRes[c].slotX[d],fp);
				doputfloat(hdrLODRes[c].slotY[d],fp);
				doputfloat(hdrLODRes[c].slotZ[d],fp);
			}
		}
		if(hdrParent[c].ushort1[1] > 0)// Save Dynamic
		{
			for(d=0;d<hdrParent[c].ushort1[1];d++)
			{
				doputfloat(hdrLODRes[c].dyn1[d],fp);
				doputfloat(hdrLODRes[c].dyn2[d],fp);
				doputfloat(hdrLODRes[c].dyn3[d],fp);
			}
		}
		if(hdrParent[c].uchar[0] > 0)// Save LOD indexes
		{
			for(d=0;d<hdrParent[c].uchar[0];d++)
			{
				if ((int)hdrLODRes[c].lodind[d] == -1)
					hdrLODRes[c].lodind[d] = 0;
				if(!DXMode){
					memset(LODName,0,sizeof(LODName));
					strcpy(LODName,LODNames[LODNUMBER(hdrLODRes[c].lodind[d])]);
					fp.Write(LODName,HDR_LODNAMELEN);
				} else {
					fp.Write(hdrLODRes[c].LODName[d],HDR_LODNAMELEN);
				}
				doputlong(hdrLODRes[c].lodind[d],fp);
				doputfloat(hdrLODRes[c].distance[d],fp);
			}
		}
	}

	fp.Close();
	return true;
} // end SaveHDR()


//====================================================================================
/*
		DATA DESCRIPTIONS:

	DOMAIN:
		0. Unknown
		1. Abstract
		2. Air
		3. Land
		4. Sea
		5. Space
		6. Underground
		7. Undersea
	CLASS:
		0. Abstract
		1. Animal
		2. Feature
		3. Manager
		4. Objective
		5. SFX
		6. Unit
		7. Vehicle
		8. Weapon
		9. Weather
		10. Dialog
		11. Game
		12. Group
		13. Session
	TYPE:
		Number to indicate type of Domain-Class
	SUBTYPE:
		Further sequential differentiation
	SPECIFIC:
		Further sequential differentiation
	MODE:
		0. Normal
		1. Crimson
		2. Shark
		3. Viper
		4. Tiger
	FTYPE:
		0. Unknown
		1. Feature
		2. None
		3. Objective
		4. Unit
		5. Vehicle
		6. Weapon
	PARENT:
		0. Normal
		1. Repaired/Shadow (for air)
		2. Damaged
		3. Destroyed
		4. Left Destroyed
		5. Right Destroyed
		6. Both Destroyed
	FOFFSET:
		Record index into FTYPE file

*/
//====================================================================================

//====================================================================================
// Read in Falcon.ct
//====================================================================================

int	LoadCT(CString KoreaObjFolder)
{
	int					c,d;
	CFile				fp;
	char				nctr[6];
	float				BubbleDist;
	char				szMsg[512];
	char				Names[128];


	sprintf(szMsg, "%sFalcon4.ct", KoreaObjFolder); 
	fp.Open(szMsg,CFile::modeRead);
	ctNumber = dogetshort(fp);
	if (ct)
	{
		free(ct);
		ct = NULL;
	}
	ct = (CT*)malloc(ctNumber*sizeof(CT));
	memset(ct, 0, (ctNumber*sizeof(CT)));
	for(c=0;c<ctNumber;c++)
	{
		fp.Seek(c*81+10,CFile::begin);
		ct[c].domain = dogetchar(fp);
		ct[c].cclass = dogetchar(fp);
		ct[c].type = dogetchar(fp);
		ct[c].subtype = dogetchar(fp);
		ct[c].specific = dogetchar(fp);
		ct[c].mode = dogetchar(fp);
		fp.Seek(10,CFile::current);
		BubbleDist = dogetfloat(fp);
		fp.Seek(32,CFile::current);
		for(d=0;d<8;d++)
			ct[c].parent[d] = dogetshort(fp);
//		fseek(fp,2,SEEK_CUR);
		ct[c].ftype = dogetchar(fp);
		ct[c].foffset = dogetlong(fp);
			
	}
	fp.Close();
	sprintf(szMsg, "%sFalcon4.fcd", KoreaObjFolder); // Load Features
	fp.Open(szMsg,CFile::modeRead);
	for(c=0;c<ctNumber;c++)
	{
		if(ct[c].ftype == 1)
		{

			fp.Seek(ct[c].foffset*60+10,CFile::begin);
			fp.Read(ct[c].name,20);
			if(strlen(ct[c].name)){
				strcpy(Names, ct[c].name);
				CompileNewName(Names);
				strcpy(CTNames[c], Names);
			}
		}
	}
	fp.Close();
	sprintf(szMsg, "%sFalcon4.ocd", KoreaObjFolder); // Load Objects 
	fp.Open(szMsg,CFile::modeRead);
	for(c=0;c<ctNumber;c++)
	{
		if(ct[c].ftype == 3)
		{
			fp.Seek(ct[c].foffset*54+4,CFile::begin);
			fp.Read(ct[c].name,20);
			if(strlen(ct[c].name)){
				strcpy(Names, ct[c].name);
				CompileNewName(Names);
				strcpy(CTNames[c], Names);
			}
		}
	}
	fp.Close();
	sprintf(szMsg, "%sFalcon4.ucd", KoreaObjFolder); // Load Units
	fp.Open(szMsg,CFile::modeRead);
	for(c=0;c<ctNumber;c++)
	{
		if(ct[c].ftype == 4)
		{
			fp.Seek((ct[c].foffset)*336+232,CFile::begin);
			fp.Read(ct[c].name,22);
			if(strlen(ct[c].name)){
				strcpy(Names, ct[c].name);
				CompileNewName(Names);
				strcpy(CTNames[c], Names);
			}
		}
	}
	fp.Close();
	sprintf(szMsg, "%sFalcon4.vcd", KoreaObjFolder); // Load Vehicles
	fp.Open(szMsg,CFile::modeRead);
	for(c=0;c<ctNumber;c++)
	{
		if(ct[c].ftype == 5)
		{
//			if (bLP)
//				fp.Seek(ct[c].foffset*176+16,CFile::begin);  //LP
//			else
				fp.Seek(ct[c].foffset*160+10,CFile::begin);	//FF
			fp.Read(ct[c].name,15);
			fp.Read(nctr,5);
			if(strlen(ct[c].name)){
				strcpy(Names, ct[c].name);
				CompileNewName(Names);
				strcpy(CTNames[c], Names);
			}
		}
	}
	fp.Close();
	sprintf(szMsg, "%sFalcon4.wcd", KoreaObjFolder); // Load Weapons
	fp.Open(szMsg,CFile::modeRead);
	for(c=0;c<ctNumber;c++)
	{
		if(ct[c].ftype == 6)
		{
			fp.Seek(ct[c].foffset*60+14,CFile::begin);
			fp.Read(ct[c].name,16);
			if(strlen(ct[c].name)){
				strcpy(Names, ct[c].name);
				CompileNewName(Names);
				strcpy(CTNames[c], Names);
			}
		}
	}
	fp.Close();
	return(ctNumber);
} 


//====================================================================================
//====================================================================================

//=================================================================
//  LODUsedBy() - Find LOD that is used by CT
//=================================================================

const	CString	StatusTab[]={

	"Nm","Rp","Dm","Ds","Lt","Rt","Bt"
};



const	VisType	Visable1[]={PARENT_CONSTANTS1};
const	VisType	Visable2[]={PARENT_CONSTANTS2};
const	VisType	Visable3[]={PARENT_CONSTANTS3};
const	VisType	Visable4[]={PARENT_CONSTANTS4};

int	LODUsedByCT(int eLOD, int* Domain, int* Class, int* Type, int* CTNumnber, int *CTIndex, int *LodLevel, int *ParentLevel, CString *Name)
{
	int					c, d, e;
	int					FileType = 0;
    int					grDat, nL;

	*CTNumnber=0;
	*CTIndex=-1;
	*Domain = 0;
	*Class = 0;
	*Name="";
	*ParentLevel=-1;
  for(c=0;c<ctNumber;c++)
  {
    for(d=0;d<7;d++)
    {
			 grDat = ct[c].parent[d];

       if(grDat < 0 || grDat >= (int)hdr.nParent)
         continue;
       nL = hdrParent[grDat].uchar[0];
       for(e=0;e<nL;e++)
       {
          if((hdrLODRes[grDat].lodind[e]-1)/2 == eLOD)
          {

						FileType = (int)ct[c].ftype;
						if (ct[c].domain == 2 && ct[c].cclass == 7 &&  e == 1)  // aircraft shadow
							*Domain = - ct[c].domain;
						else
							*Domain = ct[c].domain;
						*Class = ct[c].cclass;
						*Type = ct[c].type;
						*CTNumnber=grDat;
						*CTIndex=c;
						*LodLevel=e+1;
						*ParentLevel=d;
						Name->Format("%s_%s_L%1d", CTNames[c], StatusTab[d],	e+1);


						return (FileType);
			}
		}
		}
	}  
	// *** This is the scanning for Parents having no CTs 
	for(c=0;(unsigned int)c<hdr.nParent;c++){
						if(eLOD==2138)
							_asm	 nop;
       nL = hdrParent[c].uchar[0];
       for(e=0;e<nL;e++)
       {
          if((hdrLODRes[c].lodind[e]-1)/2 == eLOD)
          {
						FileType = 2;
						*Domain = 1;
						*Class = (int)2;
						*Type = 0;
						*CTNumnber=c;
						*LodLevel=e+1;
						Name->Format("Unknown_P%d_L%d", c, e+1);

						DWORD	a=0;
						while(Visable1[a].Id!=-1){
							if(Visable1[a].Id==c){
								Name->Format("%s_L%d", Visable1[a].Name, e+1);
								return (FileType);
							}
							a++;
						}
						a=0;
						while(Visable2[a].Id!=-1){
							if(Visable2[a].Id==c){
								Name->Format("%s_L%d", Visable2[a].Name, e+1);
								return (FileType);
							}
							a++;
						}
						a=0;
						while(Visable3[a].Id!=-1){
							if(Visable3[a].Id==c){
								Name->Format("%s_L%d", Visable3[a].Name, e+1);
								return (FileType);
							}
							a++;
						}
						a=0;
						while(Visable4[a].Id!=-1){
							if(Visable4[a].Id==c){
								Name->Format("%s_L%d", Visable4[a].Name, e+1);
								return (FileType);
							}
							a++;
						}


						return (FileType);
          }
       }
	}

  

  return (FileType);
}	// end LODUsedByCT()



DWORD	GetDomain(DWORD ParentNr)
{
	for(int c=0;c<ctNumber;c++){
		for(DWORD d=0;d<7;d++){
			DWORD nParent = ct[c].parent[d];
			if(nParent < 0 || nParent >= (int)hdr.nParent) continue;
			if(nParent==ParentNr) return ct[c].domain;
		}
	}
	return -1;
}


DWORD	GetClass(DWORD ParentNr)
{
	for(int c=0;c<ctNumber;c++){
		for(DWORD d=0;d<7;d++){
			DWORD nParent = ct[c].parent[d];
			if(nParent < 0 || nParent >= (int)hdr.nParent) continue;
			if(nParent==ParentNr) return ct[c].cclass;
		}
	}
	return -1;
}


DWORD	GetType(DWORD ParentNr)
{
	for(int c=0;c<ctNumber;c++){
		for(DWORD d=0;d<7;d++){
			DWORD nParent = ct[c].parent[d];
			if(nParent < 0 || nParent >= (int)hdr.nParent) continue;
			if(nParent==ParentNr) return ct[c].type;
		}
	}
	return -1;
}


DWORD	GetParentLodID(DWORD Parent, DWORD LodLevel)
{
	if(LodLevel<hdrParent[Parent].nLODs)
		return (LOD_INDEX(hdrLODRes[Parent].lodind[LodLevel]));
	return -1;
}


float	GetParentLodDistance(DWORD Parent, DWORD LodLevel)
{
	if(LodLevel<hdrParent[Parent].nLODs)
		return (hdrLODRes[Parent].distance[LodLevel]);
	return 0;
}




// Returns ther File Offset in the DX Dbase of model 'Name', -1 if not found
DWORD	GetNameIndex(CString Name, HDRPARENT *Parent, DWORD *ParentNr)
{
	// Look in the Table and return the offset if found
	for(DWORD a=0; a<hdr.nParent; a++){
		if(ParentNr) *ParentNr=a;
		if(Parent) *Parent=hdrParent[a];
		for(DWORD i=0; i<hdrParent[a].nLODs; i++){
			if(hdrLODRes[a].LODName[i]==Name){
				return (hdrLODRes[a].lodind[i]-1)/2;
			}
		}
	}
	return -1;
}

// Returns the File Name from the entered index in the DX Dbase of model, "" if not found
CString	GetIndexName(DWORD Index, HDRPARENT *Parent, DWORD *ParentNr)
{
	// Look in the Table and return the offset if found
	for(DWORD a=0; a<hdr.nParent; a++){
		if(ParentNr) *ParentNr=a;
		if(Parent) *Parent=hdrParent[a];
		for(DWORD i=0; i<hdrParent[a].nLODs; i++){
			if(((hdrLODRes[a].lodind[i]-1)/2)==Index){
				if(strlen(hdrLODRes[a].LODName[i])) return (hdrLODRes[a].LODName[i]);
				else hdrLODRes[a].lodind[i]=0; 
			}
		}
	}
	return "";
}

DWORD	GetParentUsingLOD(DWORD ParentStart, DWORD	LodNr)
{
	// Look in the Table and return the offset if found
	for(DWORD a=ParentStart; a<hdr.nParent; a++){
		for(DWORD i=0; i<hdrParent[a].nLODs; i++){
			if(LOD_INDEX(hdrLODRes[a].lodind[i])==LodNr){
				return	a;
			}
		}
	}
	return -1;
}



// Renames all Lod links using a pased index with the passed name
void	SetIndexName(DWORD Index, CString Name)
{
	// Look in the Table and return the offset if found
	for(DWORD a=0; a<hdr.nParent; a++){
		for(DWORD i=0; i<hdrParent[a].nLODs; i++){
			if(((hdrLODRes[a].lodind[i]-1)/2)==Index){
				memset(hdrLODRes[a].LODName[i], 0, HDR_LODNAMELEN);
				memcpy(hdrLODRes[a].LODName[i], Name, min(strlen(Name), HDR_LODNAMELEN));
			}
		}
	}
}



DWORD	GetLodIDFromName(CString Name)
{
	// Look in the Table and return the offset if found
	Name.MakeUpper();
	for(DWORD a=0; a<hdr.nParent; a++){
		for(DWORD i=0; i<hdrParent[a].nLODs; i++){
			CString	LName=hdrLODRes[a].LODName[i];
			LName.MakeUpper();
			if(LName==Name) return LOD_INDEX(hdrLODRes[a].lodind[i]);
		}
	}
	return -1;


}




DWORD	GetParentOfCT(int CTNumber, DWORD ParentLevel)	{ if(CTNumber<ctNumber) return ct[CTNumber].parent[ParentLevel]; return -1; }


// Returns ther File Offset in the DX Dbase of model 'Name', -1 if not found
DWORD	GetFileOffset(CString Name, HDRPARENT *Parent, DWORD *ParentNr)
{
	// Get the index of the Model
	DWORD a=GetNameIndex(Name, Parent, ParentNr);
	if(a!=-1) return hdrLOD[a].FileOffset;
	return -1;
}


// Returns ther File Offset in the DX Dbase of model Index
DWORD	GetFileOffset(DWORD ID)
{
	return hdrLOD[ID].FileOffset;
}




// Returns ther File Offset in the DX Dbase of model 'Name', -1 if not found
DWORD	GetFileSize(CString Name, HDRPARENT *Parent, DWORD *ParentNr)
{
	// Get the index of the Model
	DWORD a=GetNameIndex(Name, Parent, ParentNr);
	if(a!=-1) return hdrLOD[a].FileSize;
	return -1;
}



// Returns the File Name from the entered index in the DX Dbase of model, "" if not found
DWORD	GetNrTextureSets(DWORD Parent)
{
	return hdrParent[Parent].TextureSets;
}


//////////////////////// DATABASE MANAGEMENT FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

// Just Open the Input DB
bool	InitDbaseIN(CString FileName)
{
	InPosition=0;
	if(!DBFileIn.Open(FileName, CFile::modeRead)) return false;
	InSize=(DWORD)DBFileIn.GetLength();
	return true;
}

// Just Open the Input DB
bool	InitDbaseOUT(CString FileName)
{
	OutPosition=0;
	if(!DBFileOut.Open(FileName, CFile::modeCreate|CFile::modeWrite)) return false;
	return true;
}

DWORD	GetInPosition(void) { return InPosition; }
DWORD	GetOutPosition(void) { return OutPosition; }
void	CloseDBaseIN(void)	{ DBFileIn.Close(); }
void	CloseDBaseOUT(void)	{ DBFileOut.Close(); }
DWORD	GetInSize(void) { return InSize; }


void	*GetModelAtOffset(DWORD Offset)
{
	DxDbHeader	Hdr;
	if(Offset>=InSize) return NULL;
	DBFileIn.Seek(Offset, CFile::begin);
	DBFileIn.Read((void*)&Hdr, sizeof(DxDbHeader));
	DBFileIn.Seek(-((signed int)sizeof(DxDbHeader)), CFile::current);
	void	*ptr=malloc(Hdr.ModelSize);
	DBFileIn.Read(ptr, Hdr.ModelSize);
	return ptr;
}


void	*GetNextModel(void)
{
	DxDbHeader	Hdr;
	InPosition=(DWORD)DBFileIn.GetPosition();
	if(InPosition>=InSize) return NULL;
	DBFileIn.Read((void*)&Hdr, sizeof(DxDbHeader));
	DBFileIn.Seek(-((signed int)sizeof(DxDbHeader)), CFile::current);
	void	*ptr=malloc(Hdr.ModelSize);
	DBFileIn.Read(ptr, Hdr.ModelSize);
	return ptr;
}

void	WriteNextModel(void *Model)
{
	DxDbHeader	*Hdr=(DxDbHeader*)Model;
	DWORD		id=Hdr->Id;

	if(hdrLOD[id].FileOffset!=(DWORD)DBFileOut.GetPosition())
		_asm nop;
	
	if(hdrLOD[id].FileSize!=Hdr->ModelSize)
		_asm nop;


	hdrLOD[id].FileOffset=(DWORD)DBFileOut.GetPosition();
	hdrLOD[id].FileSize=Hdr->ModelSize;
	DBFileOut.Write(Model, Hdr->ModelSize);
	OutPosition=(DWORD)DBFileOut.GetPosition();
}


DWORD	GetCTNumber(void) {	return (DWORD)ctNumber; }
DWORD	GetCTNumber(CString Name) { for(int a=0; a<ctNumber; a++) if(!memcmp(&CTNames[a], Name, strlen(Name))) return a; return -1; }









DWORD	GetLODNumber(void) 
{
	return (DWORD)hdr.nLOD;
}


#define	MAX_SORT_ROOTS	2048

#define	CHECK_SURFACE(x)	if(ok && S.x!=D.x) ok=false;
DWORD	LookForSameSurface(DXNode	**Roots, DXNode *Node)
{
	DWORD	a=0;
	DxSurfaceType &S=((DXTriangle*)Node)->Surface;
	// clear no more used Flags
	S.dwFlags.b.Lite=0;
	S.dwFlags.b.zBias=0;
	// unassign any textuire for not textured surfaces
	if(S.dwFlags.b.Texture==0) S.TexID[0]=S.TexID[1]=-1;
	// reset any switch stuff fror not emissive switchable surface
	if(S.dwFlags.b.SwEmissive==0) S.SwitchMask=S.SwitchNumber=0;

	while(Roots[a] && a<MAX_SORT_ROOTS){

		bool	ok=true;
		
		// if it's a surface
		if(IS_SURFACE(Roots[a]) && Roots[a]->Type==Node->Type){
			DxSurfaceType &D=((DXTriangle*)Roots[a])->Surface;

			// ok... default as good, and test all
			bool	ok=true;
			CHECK_SURFACE(dwFlags.StateFlags);
			CHECK_SURFACE(dwFlags.b.BillBoard);			
			CHECK_SURFACE(dwzBias);
			CHECK_SURFACE(SpecularIndex);
			CHECK_SURFACE(DefaultSpecularity);
			CHECK_SURFACE(dwFlags.b.SwEmissive);			
			CHECK_SURFACE(SwitchMask);			
			CHECK_SURFACE(SwitchNumber);			
			CHECK_SURFACE(TexID[0]);			
			CHECK_SURFACE(TexID[1]);			

			// if all tests passed, we found the right root
			if(ok) return a;
		}		
		// Next Root		
		a++;		
	}

	return	a;
}



#define	CHECK_DOF(x)	if(ok && S.x!=D.x) ok=false;
DWORD	LookForSameDOF(DXNode	**Roots, DXNode *Node)
{
	DWORD	a=0;
	DxDofType &S=((DXDof*)Node)->dof;

	while(Roots[a] && a<MAX_SORT_ROOTS){

		bool	ok=true;
		
		// if it's a surface
		if(Roots[a]->Type==Node->Type){
			DxDofType &D=((DXDof*)Roots[a])->dof;

			// ok... default as good, and test all
			bool	ok=true;
			CHECK_DOF(Type);
			if(ok) switch(S.Type) {

				case	NO_DOF:		break;

				// * POSITIONAL DOF MANAGEMENT *
				case	ROTATE:		
				case	XROTATE:
				case	TRANSLATE:
				case	SCALE:		CHECK_DOF(dofNumber);
									CHECK_DOF(min);
									CHECK_DOF(max);
									CHECK_DOF(multiplier);
									CHECK_DOF(future);
									if(ok && memcmp(&S.scale, &D.scale, sizeof(S.scale))) ok=false;
									if(ok && memcmp(&S.translation, &D.translation, sizeof(S.translation))) ok=false;
									if(ok && memcmp(&S.rotation, &D.rotation, sizeof(S.rotation))) ok=false;
									break;
				
				case	SWITCH:
				case	XSWITCH:	CHECK_DOF(SwitchNumber);
									CHECK_DOF(SwitchBranch);
									break;

			}


			// if all tests passed, we found the right root
			if(ok) return a;
		}		
		// Next Root		
		a++;		
	}

	return	a;
}


DWORD	LookForSameSlot(DXNode	**Roots, DXNode *Node)
{
	DWORD	a=0;

	while(Roots[a] && a<MAX_SORT_ROOTS){
		// if it's a SLOT
		if(Roots[a]->Type==Node->Type) return a;
		// Next Root		
		a++;		
	}
	return a;
}




DXNode	*E_SortModelTree(DXNode *Node, DXNode **ExitLink)
{
	DWORD	Level;
	DWORD	Rn, LastRn=0;
	DXNode	*Roots[MAX_SORT_ROOTS], *Tails[MAX_SORT_ROOTS], *Next;
	DXNode *S, *Last;
	bool	OutOfBranch=false;
	
	*ExitLink=NULL;

	//return Node;

	memset(Roots, NULL, sizeof(Roots));
	memset(Tails, NULL, sizeof(Tails));

	while(Node && !OutOfBranch){
		
		Next=Node->Next;

		switch(Node->Type){

			case	DOT:
			case	LINE:		// look for a similar, or new surface
			case	TRIANGLE:	Rn=LookForSameSurface(Roots, Node);
								// add the new item ( added at top of list )
								Node->Next=Roots[Rn];
								Roots[Rn]=Node;
								// Headr has no Prev node
								Node->Prev=NULL;
								// while the following, has this as prev
								if(Node->Next) Node->Next->Prev=Node;
								// keep track of last item (1st added ) of the list
								if(!Tails[Rn]) { Tails[Rn]=Node; Node->Next=NULL; }
								break;

								// if DOF... look for the whole dof branch, and join if already present
			case	DOF:		Rn=LookForSameDOF(Roots, Node);
								// here look for the corresponding CLOSEDOF
								Level=0;
								S=Node->Next, Last=NULL;
								// repeat till in a branch or not the end of the DOF
								while(S->Type!=CLOSEDOF || Level){
									if(S->Type==DOF) Level++;
									if(S->Type==CLOSEDOF) Level--;
									// track for last surface
									Last=S;
									// and get next
									S=S->Next;
								};
								
								// *** add the whole branch now ***
								
								// Empty Branch.... no last assigned
								if(!Last){
									// previous utem
									Next=S->Next;
									delete	S;
									delete	Node;
									break;
								}


								// if NEW LIST
								if(!Roots[Rn]){
									// Just add the item to the List
									Roots[Rn]=Node;
									Tails[Rn]=S;
									// and go ahead from end of branch
									Next=S->Next;
									// Clear Pointers
									S->Next=NULL;
									Node->Prev=NULL;
								} else {
									// 1st, get the 1st item after the DOF node in the list and link it after last of new branch
									Last->Next=Roots[Rn]->Next;
									if(Last->Next) Last->Next->Prev=Last;
									// Kill the entry DOF Node of the already present branch, will be replaced by the new one
									delete	Roots[Rn];
									// ok... the new DOF becames the top of the list
									Roots[Rn]=Node;
									Node->Prev=NULL;;
									// and go ahead from end of branch
									Next=S->Next;
									// remove last found CLOSEDOF
									delete S;
								}
								break;
								
								// if here, we started in a branch, signal we r going out
			case	CLOSEDOF:	OutOfBranch=true;
								// Signal this item as the Exit of the Branch and do not touch it
								*ExitLink=Node;
								break;

								// look for a similar SLOT
			case	SLOT:		Rn=LookForSameSlot(Roots, Node);
								// add the new item ( added at top of list )
								Node->Next=Roots[Rn];
								Roots[Rn]=Node;
								// Headr has no Prev node
								Node->Prev=NULL;
								// while the following, has this as prev
								if(Node->Next) Node->Next->Prev=Node;
								// keep track of last item (1st added ) of the list
								if(!Tails[Rn]) { Tails[Rn]=Node; Node->Next=NULL; }
								break;


								// Ok, only a Model End exists... add it as last Root
			case	MODELEND:	/*Roots[LastRn+1]=Node;
								Tails[LastRn+1]=Node;
								Node->Next=Node->Prev=NULL;
								Rn++;*/
								// if here, we started in a branch, signal we r going out
								OutOfBranch=true;
								// Signal this item as the Exit of the Branch and do not touch it
								*ExitLink=Node;
								break;

		}
		// keep track for latest used Root list
		if(Rn>LastRn) LastRn=Rn;
		// go for next node
		Node=Next;

	}

	// ok... now put all roots togheter, linking tails to next root
	DWORD	a=0;
	while(Roots[a])	{
		Tails[a]->Next=Roots[a+1];
		// check if next root has a valid node, and fix its PREV
		if(Roots[a+1]) Roots[a+1]->Prev=Tails[a];
		// if not valid, link last present Item to the exit link
		else Tails[a]->Next=*ExitLink;
		a++;
	}


	// Return the 1st Root
	return Roots[0];
}


// Main Model Sorting funtion
DXNode *E_SortModel(DXNode *Model)
{
	DWORD	Level=0, MaxLevel=0, CurrentLevel=0;
	bool	Changed=false;
	bool	KilledBranch;
	DXNode	*Node;
	do{
		
		// Root of the Model
		Node=Model;
		Level=0;
		Changed=false;

		// The front link of the srted stuff
		DXNode	*FrontLink=NULL;

		// check till end of model
		while(Node && Node->Type!=MODELEND){
			// reset killing branch flag
			KilledBranch=false;

			// check if we are in the inner level to be sorted	
			if(Level==CurrentLevel){
				// if so, sort it, end of sorted stuff will be passed back into DXNode
				DXNode *Front=E_SortModelTree(Node, &Node);
				// if we r at 1st node of model
				if(!FrontLink) Model=Front;
				// else, append to last checked node
				else {
					// check ifwe r dealing with an empty Branch
					if(Front){
						FrontLink->Next=Front;
						Front->Prev=FrontLink;
					} else { // in case of empty branch, kill it
						if(FrontLink){
							// connect the item before the Front to the tail of the sorted stuff
							if(FrontLink->Prev) { FrontLink->Prev->Next=Node; Node->Prev=FrontLink->Prev; }
							else { Model=Node; Node->Prev=NULL; }
							// kill the Front link
							delete FrontLink;
							// Now deal with tail, deal only if NOT END OF MODEL 
							// ( should never happen though, as it would mean unbalanced DOF )
							if(Node->Type!=MODELEND){
								if(Node->Prev) Node->Prev->Next=Node->Next;
								if(Node->Next) Node->Next->Prev=Node->Prev;
								// assign to kill it
								Front=Node;
								// go back to last valid node
								if(Node->Prev) Node=Node->Prev;
								else Node=Model;
								// kill the tail
								delete Front;
								// signal that a branch have been killed... so no Type evaluation
								KilledBranch=true;
								// exited from this level
								Level--;
							}
						}

					}
				}
				Changed=true;
			}
									
			// Evaluate Nodes, ONLY IF NOT KILLED A BRANCH, as node would point to an invalid node
			if(!KilledBranch ) switch(Node->Type){
			
				case	SLOT:		
				case	DOT:
				case	LINE:		
				case	TRIANGLE:	break;

									
				case	DOF:		Level++;
									break;
									
									
				case	CLOSEDOF:	Level--;
									break;

									

									
				case	MODELEND:	break;

			}

			FrontLink=Node;
			if(Node) Node=Node->Next;
		
		}
		CurrentLevel++;

	} while(Changed);

	return Model;
}



DWORD	GetMaxDbTexture(void)
{
	return hdr.nTexture;
}


void	SetMaxDbTexture(DWORD TexNr)
{
	hdr.nTexture = TexNr;
}


DWORD	GetMaxDbParent(void)
{
	return hdr.nParent;
}


void	SetMaxDbParent(DWORD ParentNr)
{
	hdr.nParent = ParentNr;
}
