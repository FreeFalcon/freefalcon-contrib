#include "stdafx.h"
#include "BSPNodes.h"
#include "LODConvert.h"
#include "MainFrm.h"
#include "Materials.h"
#include "LODConvertDoc.h"
#include "LODConvertView.h"
#include "context.h"
#include "PolyLib.h"
#include "Converter.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



void	ClearList(DXNode *node)
{	
	DXDofData *dof=NULL;
	DXNode	*t;
	if(!node) return;
	do{
		t=node->Next;
		
		switch(node->Type){
			case DOT	:	delete (DXVertex*)node;										// adds a NEW triangle to list
							break;
			case LINE	:	delete (DXLine*)node;										// adds a NEW triangle to list
							break;
			case TRIANGLE:	delete (DXTriangle*)node;										// adds a NEW triangle to list
							break;
			case DOF:		delete (DXDof*)node;											// adds a NEW triangle to list
							break;
			case SLOT:		delete (DXSlot*)node;										// adds a NEW triangle to list
							break;
		}
		node=t;
	} while(node);
}

void Converter::Init(void)
{
	memset(TextureList,-1,sizeof(TextureList));
	TexturesCount=0;
	TrianglesCount=0;
	LODVerticesCount=DXVerticesCount=0;
	LastTexID=-1;
	LODDofsCount=DXDofsCount=0;
	UsedTexCount=0;
	LODDraws=DXDraws=0;
	ShaderNormals=0;
	DofNr=-1;
	VRoot=NULL;
	DXSwitches=LODSwitches=0;
	ColourIt=LiteIt=BackCullIt=false;
	LastColorIdx=-1;
	LastTexID=-1;
	DXV=NULL;
	memset(zBiases, 0x00, sizeof(zBiases));
	zBiasLevel=0;
	BillBoard=false;
	ScriptNumber=0;
}

Converter::Converter(void)
{
	Init();
}

Converter::~Converter()
{
	ClearList(VRoot);
}


void	Converter::SetupTextures(int nTex, const int *pTex, int nTex_x_Set)
{
	int	a=0;
	NTextures=nTex;
	UsedTextures=nTex_x_Set;
	while(nTex--) TextureList[a++]=*pTex++;

}




// this function adds a texture change to the nodes list
void	Converter::AddUsedTexture(int	nTex)
{
	if(nTex==LastTexID) return;
	LastTexID=nTex;
/*	DXActual->Next=new DXTexture();
	DXActual=DXActual->Next;
	((DXTexture *)DXActual)->TexID=nTex;*/
	for(int a=0; a<UsedTexCount; a++) if(UsedTex[a]==nTex) return;		// if texture already marked as used exit
	UsedTex[UsedTexCount++]=nTex;										// lse new used Texture
}


void	Converter::AddUsedColor(int	ColorIdx)
{
	LastColorIdx=ColorIdx;
	ColourIt=true;
}


void	Converter::SetLite(void)
{
	LiteIt=true;
}

void	Converter::BackCull(bool Status)
{
	ForceBackCull=Status;
}


void	Converter::AddShaderNormals(DWORD nNormals)
{
	ShaderNormals+=nNormals;
}



DXVertex	*Converter::NewVertex(DXNode* &Root, DXVertex *PT, NodeType T)
{
	DXNode	*k;

	switch(T){
		case DOT	:	k=new DXVertex();										// adds a NEW triangle to list
						break;
		case LINE	:	k=new DXLine();										// adds a NEW triangle to list
						break;
		case TRIANGLE:	k=new DXTriangle();										// adds a NEW triangle to list
						break;
		case DOF:		k=new DXDof();											// adds a NEW triangle to list
						break;
		case SLOT:		k=new DXSlot();											// adds a NEW triangle to list
						break;
	}
	if(!Root){														// if 1st triangle of a list
		Root=k;
		PT=(DXVertex*)Root;																// and setup pointer
		PT->Prev=NULL;//(DXVertex*)&Root;
	} else {
		PT->Next=k;
		PT->Next->Prev=PT;
		PT=(DXVertex*)PT->Next;
	}
	PT->NodeNumber=NodeCount;
	return(PT);
}


void	AssignPmatrixToD3DXMATRIX(D3DXMATRIX *d, Pmatrix *s)
{
	d->m00=s->M11;	d->m01=s->M21;	d->m02=s->M31;	d->m03=0.0f;
	d->m10=s->M12;	d->m11=s->M22;	d->m12=s->M32;	d->m13=0.0f;
	d->m20=s->M13;	d->m21=s->M23;	d->m22=s->M33;	d->m23=0.0f;
	d->m30=0.0f;	d->m31=0.0f;	d->m32=0.0f;	d->m33=1.0f;


}


void	RotateVertex(Ppoint *d, Ppoint *s, Pmatrix *m)
{
	D3DXMATRIX	k;
	
	AssignPmatrixToD3DXMATRIX(&k, m);

	d->x=k.m00*s->x+k.m10*s->y+k.m20*s->z;
	d->y=k.m01*s->x+k.m11*s->y+k.m21*s->z;
	d->z=k.m02*s->x+k.m12*s->y+k.m22*s->z;
}




bool	Converter::CheckCCW(Ppoint *Vtx, Poly *poly)
{
	float	 nx, ny, nz, r;
	Ppoint	V0, V1, V2, U, V, P, N;


	// Apply Eventual DOF Transformation
	RotateVertex( &V0, &Vtx[0], &TheStateStack.Rotation);
	RotateVertex( &V1, &Vtx[1], &TheStateStack.Rotation);
	RotateVertex( &V2, &Vtx[2], &TheStateStack.Rotation);
	
	// Transformate plane normal too
	P.x=poly->A;
	P.y=poly->B;
	P.z=poly->C;
	RotateVertex( &N, &P, &TheStateStack.Rotation);

	// Calculate Vectors V0-V2 and V0-V1
	V.x=V2.x-V0.x;
	V.y=V2.y-V0.y;
	V.z=V2.z-V0.z;

	U.x=V1.x-V0.x;
	U.y=V1.y-V0.y;
	U.z=V1.z-V0.z;

	// Cross product to get the Plane Normal
	nx=U.y*V.z-U.z*V.y;
	ny=U.z*V.x-U.x*V.z;
	nz=U.x*V.y-U.y*V.x;

	// Dot product with original plane normal
	r=sqrtf(nx*nx + ny*ny + nz*nz);
	nx/=r;
	ny/=r;
	nz/=r;

	r=N.x*nx+N.y*ny+N.z*nz;

	// if Dot Product > 0 then CCW else CW
	if(r>0) return true;
	return false;
}



void	Converter::AddPoly(DWORD opFlag, Poly *poly, int *xyzIdxPtr, int *rgbaIdxPtr, int *IIdxPtr, Ptexcoord *uv, bool bUseFGColor)
{	
	int	VNum=poly->nVerts,a;
	Ppoint	TriangleVtx[3];
	Pnormal TriangleNmx[3];
	DWORD	VColor[3];
	float	Tu[3],Tv[3];
	DXFlagsType	Flags;
	DWORD ColorID=-1,TextureID=-1;
	Flags.w=0;
	bool	Solid=false;


	LODVerticesCount+=poly->nVerts;

	switch(poly->type){										//	 Settles lags based on poly Type
		case	PointF:	Flags.b.Point=1;					// * 0 *
						ColorID=LastColorIdx;
						Solid=1;
						// Points & Lines Defaults to LITE... have to made UNLITE by SWITCH Selection
						Flags.b.Lite=1;
						break;

		case	LineF:	Flags.b.Line=1;						// * 1 *
						ColorID=LastColorIdx;
						Solid=1;
						// Points & Lines Defaults to LITE... have to made UNLITE by SWITCH Selection
						Flags.b.Lite=1;
						break;
		
		case	F	:	Flags.b.Poly=1;						// * 2 *
						ColorID=LastColorIdx;
						break;
		
		case	FL	:	Flags.b.Poly=1;						// * 3 *
						ColorID=LastColorIdx;
						break;
		
		case	G	:	Flags.b.Poly=1;						// * 4 *
						Flags.b.Gouraud=1;						// * GOURAUD *
						ColorID=LastColorIdx;
						break;

		case	GL	:	Flags.b.Poly=1;						// * 5 *
						Flags.b.Gouraud=1;						// * GOURAUD LITE *
						ColorID=LastColorIdx;
						break;

		case	CTex:	Flags.b.ChromaKey=1;									// * 6 *
		case	Tex	:	Flags.b.Poly=1;						// * 7 *
						TextureID=LastTexID;
						break;

		case	CTexL:	Flags.b.ChromaKey=1;									// * 8 *
		case	TexL:	Flags.b.Poly=1;						// * 9 *
						TextureID=LastTexID;
						break;
						
		case	CTexG:	Flags.b.ChromaKey=1;									// * 10 *
		case	TexG:	Flags.b.Poly=1;						// * 11 *
						Flags.b.Gouraud=1;						// * TEXTURED GOURAUD *
						TextureID=LastTexID;
						break;

		case	CTexGL:	Flags.b.ChromaKey=1;									
		case	TexGL:	Flags.b.Poly=1;
						Flags.b.Gouraud=1;						// * TEXTURED GOURAUD LITE *
						TextureID=LastTexID;
						break;

		case	AF	:	Flags.b.Poly=1;
						Flags.b.Alpha=1;
						ColorID=LastColorIdx;
						break;
		
		case	AFL	:	Flags.b.Poly=1;
						Flags.b.Lite=1;							// * FLAT LIT *
						Flags.b.Alpha=1;
						ColorID=LastColorIdx;
						break;
		
		case	AG	:	Flags.b.Poly=1;
						Flags.b.Gouraud=1;						// * GOURAUD *
						Flags.b.Alpha=1;
						ColorID=LastColorIdx;
						break;

		case	AGL	:	Flags.b.Poly=1;
						Flags.b.Gouraud=1;						// * GOURAUD LITE *
						Flags.b.Alpha=1;
						ColorID=LastColorIdx;
						break;

		case	CATex:	Flags.b.ChromaKey=1;
		case	ATex :	Flags.b.Poly=1;
						Flags.b.Alpha=1;
						TextureID=LastTexID;
						ColorID=LastColorIdx;
						break;

		case	CATexL:	Flags.b.ChromaKey=1;
		case	ATexL:	Flags.b.Poly=1;
						Flags.b.Alpha=1;
						TextureID=LastTexID;
						ColorID=LastColorIdx;
						break;
						
		case	CATexG:	Flags.b.ChromaKey=1;
		case	ATexG:	Flags.b.Poly=1;
						Flags.b.Alpha=1;
						TextureID=LastTexID;
						ColorID=LastColorIdx;
						break;

		case	CATexGL:Flags.b.ChromaKey=1;
		case	ATexGL:	Flags.b.Poly=1;
						Flags.b.Gouraud=1;						// * TEXTURED GOURAUD LITE *
						Flags.b.Alpha=1;
						TextureID=LastTexID;
						ColorID=LastColorIdx;
						break;
						
		case	BAptTex:Flags.b.Poly=1;
						Flags.b.Gouraud=1;						// * TEXTURED GOURAUD LITE *
						Flags.b.Alpha=1;
						TextureID=LastTexID;
						break;

		

	}

	// * DO NOT ADD ALPHA BACKCULLED SURFACES - THEY R BACKCULLED BY THE RENDERING ENGINE !!!
	if( ForceBackCull && Flags.b.Alpha) return;


	if(opFlag&PRIM_COLOP_TEXTURE) Flags.b.Texture=1;				// if Vertex colour requested assign it
	if(opFlag&PRIM_COLOP_COLOR) Flags.b.VColor=1;					// if Vertex colour requested assign it
	if((bUseFGColor)&&(ColourIt)) Solid=1;							// if external colour requested, then single colour Poly
	
//  if external Light request lite it...
	if(opFlag&PRIM_COLOP_INTENSITY) Flags.b.Lite=1;				// if Light requested assign it
	if((bUseFGColor)&&(LiteIt)) Flags.b.Lite=1;

	Flags.b.BillBoard=BillBoard;

//	Here starts with 1st TRIANGLE, so a=0 as 3 vertices to fetch and process
 	a=0;

	// COBRA - RED - Sometimes Lines are passed as Poly - TRASH Them
	if(Flags.b.Poly && poly->nVerts<3) goto CloseHere;

	//	Here fetch 'a' vertices COORDS and NORMALS
	// *** START OF TRIANGLE SECTION ***

	Flags.b.Gouraud=1;

	// HACK **** HACK **** HACK
	// This is to solve various problems as shining nite buildings
	
	// Feature Class
	if(gClass==2){
		// Taxi Signs
		if(gType==24) goto NoFeatureChange;
		// Landing Lite
		if(gType==0) goto NoFeatureChange;
		// Normal Features are NOT EMISSIVE 
		if(gType==25) goto NoFeatureChange;
		// VASIN
		if(gType==47) goto NoFeatureChange;
		Flags.b.Lite=1;
	}
NoFeatureChange:

	bool	first=true;
	if(Flags.b.Poly){
TriangleLoop:
		while(a<3){
			TriangleVtx[a]=TheStateStack.pCoords[*xyzIdxPtr++];					// asssign vertices
			if(IIdxPtr) {
				TriangleNmx[a]=TheStateStack.pNormals[*IIdxPtr++];				// asssign normals if present
			}else {																// else assigns Plane normal
				TriangleNmx[a].i=poly->A;
				TriangleNmx[a].j=poly->B;
				TriangleNmx[a].k=poly->C;
			}


			// OK - ZBIAS - Negative Vlues are not Possible, so, translate a little back the surface
			if(GlobalBias<0){
				TriangleVtx[a].x+=GlobalBias*TriangleNmx[a].i/2;
				TriangleVtx[a].y+=GlobalBias*TriangleNmx[a].j/2;
				TriangleVtx[a].z+=GlobalBias*TriangleNmx[a].k/2;
			}

			DWORD ColorIdx;
			if(Flags.b.VColor) {
				ColorIdx=*rgbaIdxPtr++;
				VColor[a]=D3DRGBA(pPalette[ColorIdx].r, pPalette[ColorIdx].g, pPalette[ColorIdx].b, pPalette[ColorIdx].a);
			}

			if(Solid) {
				ColorIdx=ColorID;
				VColor[a]=D3DRGBA(pPalette[ColorIdx].r, pPalette[ColorIdx].g, pPalette[ColorIdx].b, pPalette[ColorIdx].a);
			}

			if(Flags.b.Texture){
				Tu[a]=uv->u;
				Tv[a]=uv->v;
				uv++;
			}

			VNum--;															// one vertex less to manage
			a++;
		}
		
		// check for CCW Winding
		// This function checks for polygons backculling and eventually reverses triangle winding
		BackCullIt=false;
		if( ForceBackCull ) {
			BackCullIt=true;
		} else {
			if (!CheckCCW(TriangleVtx, poly)) BackCullIt=true;
			else BackCullIt=false;
		}


		DXV=NewVertex(VRoot, DXV, TRIANGLE);		// Creates the Triangle
		DXTriangle *t=(DXTriangle*)DXV;

		// FLAGS
		t->Flags=Flags;
		if(Solid) t->Flags.b.VColor=1;
		
		// zBias Feature 
		t->zBias=GlobalBias;

		// **** DISABLED Z-SURFACES MESSED UP - FUCKED MODELLERS...!!!!
		// CHroma Key Addon, add a little ZBias
		//if(t->Flags.b.ChromaKey && (t->zBias==0.0f)) t->zBias=0.01f;

		// look if this zBias already stored ONLY IF >0, look before in same function to know reasons why
		if(t->zBias>=0){
			int	z;
			for(z=0; z<zBiasLevel; z++){
				// if found exit here
				if(t->zBias==zBiases[z]) break;
			}
			// if not found add it to the list
			if(z==zBiasLevel) zBiases[zBiasLevel++]=t->zBias;
		}


		// * Back Culling Feature *
		if((BackCullIt)){
			Ppoint	temp;
			
			temp=TriangleVtx[1];
			TriangleVtx[1]=TriangleVtx[2];
			TriangleVtx[2]=temp;

			Pnormal nemp=TriangleNmx[1];
			TriangleNmx[1]=TriangleNmx[2];
			TriangleNmx[2]=nemp;

			// * Adding tickness to BLitPrimitives surfaces *
			if(ForceBackCull){
				for(a=0; a<3; a++){
					TriangleVtx[a].x+=TriangleNmx[a].i/1000;
					TriangleVtx[a].y+=TriangleNmx[a].j/1000;
					TriangleVtx[a].z+=TriangleNmx[a].k/1000;
				}
			}

			float	tt;
			tt=Tu[1];
			Tu[1]=Tu[2];
			Tu[2]=tt;

			tt=Tv[1];
			Tv[1]=Tv[2];
			Tv[2]=tt;

			DWORD	tc;
			tc=VColor[1];
			VColor[1]=VColor[2];
			VColor[2]=tc;
		}

	
	// Here assigns data to the triangle
		for(a=0; a<3; a++){													// Copies triangle data in list
			// COORDS		
			t->Vertex[a].dvX=TriangleVtx[a].x;									
			t->Vertex[a].dvY=TriangleVtx[a].y;									
			t->Vertex[a].dvZ=TriangleVtx[a].z;									
			// NORMALS
			t->Vertex[a].dvNX=TriangleNmx[a].i;									
			t->Vertex[a].dvNY=TriangleNmx[a].j;									
			t->Vertex[a].dvNZ=TriangleNmx[a].k;									
			if(Flags.b.Texture){
				t->Vertex[a].dvTU=Tu[a];
				t->Vertex[a].dvTV=Tv[a];
				t->Texture=TextureID;
				t->VColor[a]=DEFAULT_MAT_DIFFUSE;	// Default to give light to textured items
			}										// The colour is replaced after if needed ( 'Solid' or 'VColor' )
			
			if(Solid || Flags.b.VColor){
				t->VColor[a]=VColor[a];
			}
			if(Solid) t->SolidColor=VColor[a];

			//if(Flags.b.Solid) { t->Flags.b.VColor=1; t->Flags.b.SOlid=0; }

		}
		if(VNum){											// * If other vertices ( poly was more that a 3 vertex *
			//  Swap only if not BackCulling
			if(!BackCullIt){
				TriangleVtx[1]=TriangleVtx[2];
				TriangleNmx[1]=TriangleNmx[2];
				Tv[1]=Tv[2];
				Tu[1]=Tu[2];
				VColor[1]=VColor[2];
			}
			a=2;
			goto TriangleLoop;								// and go to process one more
		}
	}
	// *** END OF TRIANGLE SECTION ***


	// *** START OF POINTS SECTION ***
	if(Flags.b.Point){
		while(VNum--){
			DXV=NewVertex(VRoot, DXV, DOT);
			// FLAGS
			DXV->Flags=Flags;				

			Ppoint k=TheStateStack.pCoords[*xyzIdxPtr++];					// asssign vertices
			DXV->Vertex.dvX=k.x;
			DXV->Vertex.dvY=k.y;
			DXV->Vertex.dvZ=k.z;
			DWORD ColorIdx=ColorID;
			DXV->VColor=D3DRGBA(pPalette[ColorIdx].r, pPalette[ColorIdx].g, pPalette[ColorIdx].b, pPalette[ColorIdx].a);
//			DXV->VColor=ColorID;
		}

	}
	// *** END OF POINTS SECTION ***


	// *** START OF LINE SECTION ***
	if(Flags.b.Line){
		// No Lines with Odd Vertices
		if(VNum&0x01) goto CloseHere;
LineLoop:
		DXV=NewVertex(VRoot, DXV, LINE);
		DXLine *t=(DXLine*)DXV;
		t->Flags=Flags;				
		for(a=0; a<2; a++){
			// FLAGS
			Ppoint k=TheStateStack.pCoords[*xyzIdxPtr++];					// asssign vertices
			t->Vertex[a].dvX=k.x;
			t->Vertex[a].dvY=k.y;
			t->Vertex[a].dvZ=k.z;
			DWORD ColorIdx=ColorID;
			t->VColor[a]=D3DRGBA(pPalette[ColorIdx].r, pPalette[ColorIdx].g, pPalette[ColorIdx].b, pPalette[ColorIdx].a);
//			DXV->VColor=ColorID;
			VNum--;
			if(Solid) t->SolidColor=t->VColor[a];
		}
		if(VNum) goto LineLoop;

	}
	// *** END OF LINE SECTION ***

	if(!Flags.b.Point && !Flags.b.Line && !Flags.b.Poly ) MessageBox(NULL, "BAD SURFACE","ERROR", NULL);

CloseHere:
	ColourIt=LiteIt=BackCullIt=ForceBackCull=false;

}


void	Converter::AddSlot(DWORD Slot, Pmatrix *rotation, Ppoint *pos)
{		
		// Add the Slot
		DXV=NewVertex(VRoot, DXV, SLOT);
		
		DXSlot *s=(DXSlot*)DXV;
		
		s->SlotNr=Slot;
		AssignPmatrixToD3DXMATRIX(&s->Transform, rotation);
		s->Transform.m30=pos->x;
		s->Transform.m31=pos->y;
		s->Transform.m32=pos->z;
}




void	Converter::OpenDof(DofType	T, int dN, float mn, float mx, float mult, float fut,
					int flg, Ppoint sc, Ppoint tr )
{
	DXDof *DXF=(DXDof*)NewVertex(VRoot, DXV, DOF);
	DXV=(DXVertex*)DXF;
	DofNr=dN;
	DXF->dof.Type=T;
	DXF->dof.dofNumber=dN;
	DXF->dof.min=mn;
	DXF->dof.max=mx;
	DXF->dof.multiplier=mult;
	DXF->dof.future=fut;
	DXF->dof.flags=flg;
	DXF->dof.scale=sc;
	DXF->dof.translation=tr;

	LastDof.dof.Type=DXF->dof.Type=T;
}

void	Converter::OpenDof(DofType	T, int dN, Pmatrix rot, Ppoint tr )
{
	DXDof *DXF=(DXDof*)NewVertex(VRoot, DXV, DOF);
	DXV=(DXVertex*)DXF;
	DofNr=dN;
	DXF->dof.Type=T;
	DXF->dof.dofNumber=dN;
	//DXF->dof.rotation=rot;
	AssignPmatrixToD3DXMATRIX(&DXF->dof.rotation, &rot);
	DXF->dof.translation=tr;
}


void	Converter::OpenDof(DofType	T, int dN, float mn, float mx, float mult, float fut,
					int flg, Pmatrix rot, Ppoint tr )
{
	DXDof *DXF=(DXDof*)NewVertex(VRoot, DXV, DOF);
	DXV=(DXVertex*)DXF;
	DofNr=dN;
	DXF->dof.Type=T;
	DXF->dof.dofNumber=dN;
	DXF->dof.min=mn;
	DXF->dof.max=mx;
	DXF->dof.multiplier=mult;
	DXF->dof.future=fut;
	DXF->dof.flags=flg;
//	DXF->dof.rotation=rot;
	AssignPmatrixToD3DXMATRIX(&DXF->dof.rotation, &rot);
	DXF->dof.translation=tr;

	LastDof.dof.Type=DXF->dof.Type=T;

}

void	Converter::OpenDof(DofType	T, int dN, float mn, float mx, float mult, float fut, int flg, Ppoint tr )
{
	DXDof *DXF=(DXDof*)NewVertex(VRoot, DXV, DOF);
	DXV=(DXVertex*)DXF;
	DofNr=dN;
	DXF->dof.Type=T;
	DXF->dof.dofNumber=dN;
	DXF->dof.min=mn;
	DXF->dof.max=mx;
	DXF->dof.multiplier=mult;
	DXF->dof.future=fut;
	DXF->dof.flags=flg;
	DXF->dof.translation=tr;

	LastDof.dof.Type=DXF->dof.Type=T;
}


void	Converter::OpenDof(DofType	T, int SwitchNumber, int SwitchBranch)
{
	// Avoid appending sequential switches with same switch values, join them as a single switch
	/*if(DXV){
		if( DXV->Type==CLOSEDOF ){
			if ( LastDof.dof.Type==T && LastDof.dof.SwitchNumber==SwitchNumber && LastDof.dof.SwitchBranch==SwitchBranch){
			// so get back one position to kill the CloseDof
			DXV=(DXVertex*)DXV->Prev;
			// Kill It
			free(DXV->Next);
			// an stay pointing there to append vertices	
			return;
			}
		}
	}*/

	DXDof *DXF=(DXDof*)NewVertex(VRoot, DXV, DOF);
	DXV=(DXVertex*)DXF;

	DofNr=SwitchNumber;
	
	LastDof.dof.Type=DXF->dof.Type=T;
	LastDof.dof.SwitchNumber=DXF->dof.SwitchNumber=SwitchNumber;
	LastDof.dof.SwitchBranch=DXF->dof.SwitchBranch=SwitchBranch;

	
}


void	Converter::CloseDof(void)
{
	DXV=NewVertex(VRoot, DXV, DOF);;
	DofNr=-1;
	DXV->Type=CLOSEDOF;
	
}



DXNode	*Converter::OptimizeSections(DXNode *Start, DXNode *End)
{	
	DXNode	*n, *troot;
	
	troot=new DXNode();
	n=troot;


	// 1st Step - Order Based on Textures
	return troot;
}


void	Converter::SortzBiases(void)
{
	int	a=0,b;
	float	t;
	// look thru the whole list
	while(a<(zBiasLevel-1) && a<MAX_ZBIAS_LEVELS){
		// if pointed value less than successive
		if(zBiases[a]>zBiases[a+1]){
			// swap it
			b=a+1;
			while(b){
				if(zBiases[b]<zBiases[b-1]){
					t=zBiases[b-1];
					zBiases[b-1]=zBiases[b];
					zBiases[b]=t;
				}
				b--;
			}
		}
		a++;
	}

}


// Function remapping zBiases in values btw 0 - 15
void	Converter::AssignzBiases(DXNode *Start)
{
	DXNode *k=Start;
	DWORD	ZeroIndex, Idx;
	DWORD	zBiasStep;

	// if zBias not used, go back
	if(zBiasLevel==1) return;
	
	// Find the Zero index in the zBias List
	ZeroIndex=0;
	while(zBiases[ZeroIndex]!=0.0f) ZeroIndex++;
	
	// The step btweenzBias Values - to keep separate as much as possible zBiases
	zBiasStep=(16/(zBiasLevel-1));
	


	// thru all the list
	while(k){

		// if a zBiasable surface and zBias assigned
		if((k->Type==TRIANGLE || k->Type==LINE || k->Type==DOT) && k->zBias>0.0f){
			// signals zBias
			k->Flags.b.zBias=1;
			// lookor such value in table
			for(Idx=0; Idx<zBiasLevel; Idx++){
				// if found assign the mapped value
				if(k->zBias==zBiases[Idx]){
					k->dwzBias=(Idx-ZeroIndex)*zBiasStep;
					if(k->dwzBias>16) k->dwzBias=16;
					break;
				}
			}
		}
		k=k->Next;
	}
}


void	Converter::Statistics(void)
{
	DXNode *k;
	// go sorting the zBias table
	SortzBiases();
	AssignzBiases(VRoot);

	k=OrderByDOF(VRoot);
	ClearList(VRoot);
//	while(DOFsJoin(k));
/*	MainOptimize(k, true, true, true);
	MainOptimize(k, false, true, true);
	MainOptimize(k, false, false, true);
	MainOptimize(k, false, false, false);
//	AssignDOFsToSameFeatures(k);

*/	DXFlagsType	SortFlags;
	
	// For now sort only by textures	
	SortFlags.w=0x00000000;
	SortFlags.StateFlags=0x1f;
	SortFeatures(k, SortFlags);


	VRoot=k;
	
	TrianglesCount=DotsCount=LinesCount=DXVerticesCount=0;


	// **** Count Draw Surfaces in POLY LIST ***
	DXVertex *t=(DXVertex*)VRoot;
	DXDraws=0;
	DXSwitches=0;	
	DXSWBranches=0;
	VLite=VSolid=VAlpha=VGouraud=VTextured=DofVertices=0;
	DXFlagsType	LastFlags;
	LastFlags.w=0;
	DWORD	LastTex=-1, DofLevel=0;

	while(t){


		if(t->Type==TRIANGLE || t->Type==LINE || t->Type==DOT ){
			int	Add=0;
			if(!t->Flags.b.Point && !t->Flags.b.Line && !t->Flags.b.Poly ) MessageBox(NULL, "BAD SURFACE","STATISTICS", NULL);
			if(t->Type==TRIANGLE) { Add=3; TrianglesCount++; }
			if(t->Type==LINE) { Add=2; LinesCount++; }
			if(t->Type==DOT) { Add=1; DotsCount++; }
			DXVerticesCount+=Add;
			if(DofLevel) DofVertices+=Add;
			if((t->Flags.w!=LastFlags.w)||(LastTex!=t->Texture))
				{ DXDraws++; LastFlags.w=t->Flags.w; LastTex=t->Texture; }
			if(t->Flags.b.Lite) VLite+=Add;
			if(t->Flags.b.Alpha) VAlpha+=Add;
			if(t->Flags.b.VColor) VSolid+=Add;
			if(t->Flags.b.Gouraud) VGouraud+=Add;
			if(t->Flags.b.Texture) VTextured+=Add;
		}
		
		if(t->Type==DOF){
			DofLevel++;
			if(((DXDof*)t)->dof.Type==SWITCH || ((DXDof*)t)->dof.Type==XSWITCH) {
				if(((DXDof*)t)->dof.SwitchBranch==0) { DXSwitches++; DXSWBranches++; DXDraws++; }
				else DXSWBranches++;
			}
			else { DXDofsCount++; DXDraws++; }

		}
		
		if(t->Type==CLOSEDOF){
			DofLevel--;
		}
		
		t=(DXVertex*)t->Next;
	}
}

DXNode *Converter::GetLastVertex(void)
{	DXNode *t;
	t=VRoot;
	while(t->Next) t=t->Next;
	return(t);
}




DXVertex	*Converter::OrderByTexture(DXVertex *Start)
{
	DXVertex	*t;
	t=Start;
	do{
		if(t->Type==DOF){ t=(DXVertex*)t->Next; t=OrderByTexture(t); }			 
		

		

	} while(Start->Next);

	return t;
}






DXNode	*Converter::OrderByDOF(DXNode *Start)
{
	DXNode	*VtPtr=NULL, *VtStart=NULL, *VtLast=NULL;
	DXNode	*t=Start;
	DXNode	*DofSequence[1024];
	DWORD	DofStep=0, DofLevel=0;

	memset(DofSequence, 0, sizeof(DofSequence));

	// 1st of all, creates a list of base vertices, not included in any DOF / SWITCH
	// and stores a list of DOFS/SWITCHES references
	while(t){
		if(t->Type==DOF){							// if we r in a DOF/SWITCH
			DofLevel++;								// one level in
			DofSequence[DofStep++]=t;				// store the step
		}

		if(t->Type==CLOSEDOF){						// if we going out of a DOF/SWITCH
			DofLevel--;								// one level out
			DofSequence[DofStep++]=t;				// store the step
		}
		
		// * Here checks for vertices and adds them to the new list *
		if((t->Type==TRIANGLE || t->Type==LINE || t->Type==DOT || t->Type==SLOT ) && (!DofLevel)){			// if VERTEX and not in a DOF
			VtPtr=(DXNode*)NewVertex(VtStart, (DXVertex*)VtPtr, t->Type);
			memcpy((void*)VtPtr, (void*)t, t->NodeSize);
			//if(!VtLast) VtLast=VtPtr;
			VtPtr->Next=NULL;
			VtPtr->Prev=VtLast;
			if(VtLast) VtLast->Next=VtPtr;
			VtLast=VtPtr;
		}
		t=t->Next;
	};												// Til end of list

	
	// * Here deals with DOFS adding and ordering *
	DofStep=0;
	while(DofSequence[DofStep]){
		
		if(DofSequence[DofStep]->Type==DOF){
			VtPtr=(DXVertex*)new DXDof();				//
			memcpy((void*)VtPtr, (void*)DofSequence[DofStep], sizeof(DXDof)); // Copy the Original DOF Node
			if(!VtStart) VtStart=VtPtr;
			VtPtr->Next=NULL;
			VtPtr->Prev=VtLast;
			if(VtLast)VtLast->Next=VtPtr;
			VtLast=VtPtr;
			t=DofSequence[DofStep]->Next;
			DofLevel=0;

			do{
				if(t->Type==DOF){							// if we r in a DOF/SWITCH
					DofLevel++;								// one level in
				}

				if(t->Type==CLOSEDOF){						// if we going out of a DOF/SWITCH
					DofLevel--;								// one level out
				}

				if((t->Type==TRIANGLE || t->Type==LINE || t->Type==DOT  || t->Type==SLOT ) && (!DofLevel)){			// if VERTEX and not in a DOF
					VtPtr=(DXNode*)NewVertex(VtStart, (DXVertex*)VtPtr, t->Type);
					memcpy((void*)VtPtr, (void*)t, t->NodeSize);
					VtPtr->Next=NULL;
					VtPtr->Prev=VtLast;
					VtLast->Next=VtPtr;
					VtLast=VtPtr;
				}
				t=t->Next;
			} while(DofLevel!=-1);
		}

		if(DofSequence[DofStep]->Type==CLOSEDOF){
			VtPtr->Next=(DXVertex*)new DXDof(CLOSEDOF);	//
			VtPtr=VtPtr->Next;							//
			VtPtr->Prev=VtLast;
			VtLast->Next=VtPtr;
			VtLast=VtPtr;
		}

		DofStep++;
	}

	return(VtStart);
}




bool	Converter::DOFsJoin(DXNode *Start)
{
	bool DoneSomething=false;
	DXNode	*t=Start;
	DXDof	*DofPtr, *N;
	signed int	DofLevel, DofLevel2;
	DXNode *Prev, *Next, *DofStart;
	
	while(t){
		
		// Check for a DOF that is a SWITCH or XSWITCH
		if(t->Type==DOF){
			DofPtr=(DXDof*)t;
			// if found, start from this branch Level
			DofLevel=0;
			N=(DXDof*)DofPtr->Next;

			if(DofPtr->dof.SwitchNumber==5)
				_asm	nop;

			// Traverse All DOF
			DofLevel2=0;
			while(N && DofLevel2>=0){
				if(N->Type==DOF) DofLevel2++;
				if(N->Type==CLOSEDOF) DofLevel2--;
				N=(DXDof*)N->Next;
			}
				
				// Check till Valid Node or out of this Dof level
			while(N && DofLevel>=0){

					// if found a DOF
				if((!DofLevel) && N->Type==DOF){
						// And same as the Starting Dof and at same Dof Level Join Them
					if((!memcmp(&N->dof, &DofPtr->dof, sizeof(DXDofData))) && DofLevel==0){

						// Store the Address before the DOF and The next after Reference Dof
						Prev=N->Prev;
						Next=DofPtr->Next;
						DofStart=N;
						DofLevel2=0;

						// Traverse All SWITCH
						while(N && DofLevel2>=0){
							N=(DXDof*)N->Next;
							if(N->Type==DOF) DofLevel2++;
							if(N->Type==CLOSEDOF) DofLevel2--;
						}
						
						// iof still a valid pointer ( DB Bug Check )
						if(N){
							if(N->Type==CLOSEDOF){			// Other Check
									
								// Disconnect The SWITCH Surfaces from List
								Prev->Next=N->Next;
								if(N->Next) N->Next->Prev=Prev;

								// modev to 1st Surface of this SWITCH and delete the DOF Node
								DofStart=DofStart->Next;
								free(DofStart->Prev);
									
								// Insert this surfaces at start of original SWITCH
								DofPtr->Next=DofStart;
								DofStart->Prev=DofPtr;

								// Delete the CLOSEDOF Node
								N=(DXDof*)N->Prev;
								free(N->Next);
									
								// and Back Link this list to the Start of already existing surfaces
								N->Next=Next;
								Next->Prev=N;

								// Restart from where we left
								N=(DXDof*)Prev;

								// * SOMETHING HAS CHANGED !!! *
								DoneSomething=true;
							}
						} 
					} else
						// If a DOF but not the right DOF
						DofLevel++;
				} else
					// if end of a DOF, one level Out
					if(N->Type==CLOSEDOF) DofLevel--;
				// Check Next Item;
				N=(DXDof*)N->Next;
			}
		}
		t=t->Next;
	}

	return DoneSomething;

}






void	Converter::AssignDOFsToSameFeatures(DXNode *Start)
{
	DXNode	*VtPtr=Start, *t=Start;
	DXNode	*DofStart, *DofEnd;
	bool	SameFeatures=false;
	DWORD	DofLevel;
	
SortAgain:
	// Traverse all area
	while(t->Next){
		// if entering DOF Areas end
		if(t->Next->Type==DOF || t->Next->Type==CLOSEDOF) return;
		
		// if a Feature Change Stop, T is at end of this segment
		if((t->Flags.w != t->Next->Flags.w) || (t->Texture != t->Next->Texture)) break;

		// or continue
		t=t->Next;
	}
	
	// End of List ?
	if(!t->Next) return;

	// Start from here 
	VtPtr=t;
	while(VtPtr->Next){
		
		// Look for 1st DOF
		while(VtPtr->Next){
			if(VtPtr->Type==DOF) break;
			VtPtr=VtPtr->Next;
		}
		
		if(!VtPtr->Next) break;
		
		// Dof Start
		DofStart=VtPtr;
		SameFeatures=false;
		DofLevel=0;

		// look for same features
		while( DofLevel!=-1 /*&& SameFeatures*/){
			VtPtr=VtPtr->Next;
			if(VtPtr->Type==DOF) {
				DofLevel++;
				continue;
			}
			if(VtPtr->Type==CLOSEDOF) {
				DofLevel--; 
				continue;
			}
			
			if(( VtPtr->Flags.w == t->Flags.w) && (VtPtr->Texture == t->Texture) && DofLevel==0) SameFeatures=true;
		}

		// If a complete DOF with same features of the selected Area
		if(SameFeatures){
			// store the end of this DOF
			DofEnd=VtPtr;

			// Unlink the DOF from the List
			DofStart->Prev->Next=DofEnd->Next;
			if(DofEnd->Next)DofEnd->Next->Prev=DofStart->Prev;

			// Link at End of T Selected Area
			DofStart->Prev=t;
			t->Next->Prev=DofEnd;
			DofEnd->Next=t->Next;
			t->Next=DofStart;
		}

	}

	// Repeat with next Area
	if(t->Next){
		t=t->Next;
		goto	SortAgain;
	}
}


void	SwapNodes(DXNode *Node1, DXNode *Node2)
{
	Node2->Prev=Node1->Prev;
	if(Node2->Prev) Node2->Prev->Next=Node2;
	Node1->Next=Node2->Next;
	if(Node1->Next) Node1->Next->Prev=Node1;
	Node2->Next=Node1;
	Node1->Prev=Node2;
}


#define	IS_SURFACE(x) (x->Type==TRIANGLE || x->Type==LINE || x->Type==DOT )
#define	FEATURE(x) (((x->Texture&0xff)<<24)|((x->Flags.w&0xffff)<<8)|(x->dwzBias&0xff))

bool	Converter::SortFeatures(DXNode* &Start, DXFlagsType FlagsMask)
{
	DXNode	*Last=Start, *t=Start, *Prev=Start;
	bool	MovedSomething=false;
	bool	SurfaceMoved;

	
	// Check till the end
	while(t){
		
	////////////////// THIS PART SEND SLOTS NODE TO THE END OF A SEGMENT ///////////////////////
		// if found a SLOT and after it a Node
		if(t->Type==SLOT && t->Next){
			// At the end of SLOT sorting, start from the  following position
			Last=t->Next;
			// Signal if moving surfaces
			SurfaceMoved=false;
			// if after is a surface or a Slot swap till end of node
			while(IS_SURFACE(t->Next) || t->Next->Type==SLOT ){
				// if a surface is going to be moved
				if(IS_SURFACE(t->Next)) SurfaceMoved=true;
				// move 2nd node before 1st
				SwapNodes(t, t->Next);
				// if After no node break
				if(!t->Next) break;
			}
			// if any Surface encountered Restart from the position after the moved slot else continue from this position
			if(SurfaceMoved){ t=Last; continue; }
		}
	////////////////////////////////////////////////////////////////////////////////////////////


		// if a surface item
		if(IS_SURFACE(t) && t->Prev){

			signed int	x=FEATURE(t);
			signed int  y=FEATURE(t->Prev);
			// and next a surface too witl 'smaller' order of features
			if(IS_SURFACE(t->Prev) && x<y){
				// Position for next start
				Last=t->Next;	
				// then move up surfaces
				while(IS_SURFACE(t->Prev) && x<y ){
					// swapping with preceding
					SwapNodes(t->Prev, t);
					// if no Previous node, this is then the new Root Node
					if(!t->Prev) {
						Start=t;
						break;
					}
					x=FEATURE(t);
					y=FEATURE(t->Prev);
				}
				t=Last;
				continue;
			}
		}

		t=t->Next;
	}
	return MovedSomething;
}




void	Converter::MainOptimize(DXNode *Start, bool NoTexMode, bool NoTypeMode, bool NoZBiasMode)
{
	DXNode	*VtPtr=Start, *t=Start;
	
	//return;

	while(t && t->Next){
		// following item
		VtPtr=t->Next;
		
		// if following Item same as previous just skip it
		if(VtPtr->Type==SLOT || (VtPtr->Type==t->Type && (NoTypeMode||(VtPtr->Flags.w==t->Flags.w)) && (NoTexMode||(VtPtr->Texture==t->Texture)) && (NoZBiasMode||(VtPtr->dwzBias==t->dwzBias )))) t=VtPtr;
		// if following is different
		else {
			// If not a DRAW Node just skip it & continue
			if(VtPtr->Type==SLOT ||VtPtr->Type==DOF || VtPtr->Type==CLOSEDOF){
				t=t->Next;
				continue;
			}
			
			// else look for surfaces of same type to add
			while(VtPtr && (VtPtr->Type==TRIANGLE || VtPtr->Type==LINE || VtPtr->Type==DOT) ){

				// if found same surface or a SLOT
				if( VtPtr->Type!=SLOT && (VtPtr->Type==t->Type && (NoTypeMode||(VtPtr->Flags.w==t->Flags.w)) && (NoTexMode||(VtPtr->Texture==t->Texture)) && (NoZBiasMode||(VtPtr->dwzBias==t->dwzBias )))){

					DXNode	*Actual;

					// Save Actual location - 1 for skipat end of nodes swap
					Actual=VtPtr->Prev;

					// Previous item linked with next Forward and Backward
					VtPtr->Prev->Next=VtPtr->Next;
					if(VtPtr->Next)VtPtr->Next->Prev=VtPtr->Prev;
					
					// Link the tail of this new item with tail of T Forward and Backward
					VtPtr->Next=t->Next;
					VtPtr->Next->Prev=VtPtr;

					// Link the head of new item to T
					VtPtr->Prev=t;
					t->Next=VtPtr;

					// then move to new item
					t=t->Next;

					// And restart searching from last position
					VtPtr=Actual;
				}

				VtPtr=VtPtr->Next;
				if(!VtPtr) break;
			}
			t=t->Next;
		}
	}
}



void	Converter::LODStat(BNodeType *TL, DWORD TC)
{
	LODDofsCount=0;
	LODSwitches=0;
	LODDraws=0;

	while(TC--){
		switch(*TL){
			case	tagBNode: 
			case	tagBSubTree:
			case	tagBRoot:	
			case	tagBSplitterNode:
			case	tagBSlotNode: break;

			case	tagBDofNode:
			case	tagBTransNode:
			case	tagBScaleNode:
			case	tagBXDofNode:	LODDofsCount++;
									break;

			case	tagBSwitchNode:
			case	tagBXSwitchNode:	LODSwitches++;
										break;

			case	tagBLightStringNode:
			case	tagBSpecialXform:
			case	tagBPrimitiveNode:
			case	tagBLitPrimitiveNode:
			case	tagBCulledPrimitiveNode: LODDraws++;
											 break;
			
			case	tagBRenderControlNode: break;
		}
		TL++;
	}	

}