#pragma	once

#include	"DxNodes.h"
#include	"DxDefines.h"
#include	"lodconvert.h"

#define		MAX_BUFFERIZABLE_VERTEX_COUNT	4096
#define		MAX_DRAWABLE_VERTEX_COUNT		65536
#define		DRAW_POOL_SIZE			(MAX_DRAWABLE_VERTEX_COUNT * 8 * sizeof(D3DVERTEXEX) + sizeof(DxSurfaceType))

// Passed in DXFILES.CPP
/*void	ClearModel(E_Object *Obj);
void	E_CalculateFaceNormal(DXTriangle *Node);
void	DeIndexModel(void *Source, E_Object *Obj);*/
void	E_SetupLights(E_Object *Obj, DWORD ID, D3DXMATRIX *State, D3DVECTOR *Pos);
void	E_DrawModel(E_Object *Obj, DWORD ID, D3DXMATRIX *State, D3DVECTOR *Pos, bool FrameMode=false);
DWORD	E_FindVertex(E_Object *Obj, D3DVECTOR *Search, CEditing *EditNode, D3DVECTOR *CoordMask);
void	E_ExtendedSelect(E_Object *Obj, CSelectList *List, DXNode *Search, bool Select);
void	E_SetScale(float Scale);
void	E_SelectCommonVertex(bool Set, E_Object *Obj, CSelectList *List, D3DVERTEXEX *Vertex);
void	E_SelectCommonSurface(CSelectList *List, DXFlagsType *Flags=NULL , DWORD *ZBias=NULL , float *SIndex=NULL , DWORD *Tex1=NULL , DWORD *Tex2=NULL, DWORD *SwitchNr=NULL,  DWORD *SwitchMask=NULL, DWORD *SpecularDefault=NULL );
D3DVERTEXEX	*	E_SelectVertexInSurfaces(E_Object *Obj, CSelectList *List, bool Selection, bool SingleVertex);
void	E_SelectNode(CSelectList *List, DXNode *Node, bool Select);
void	E_CopySelection(CSelectList *List);
void	E_CutSelection(E_Object *Obj, CSelectList *List);
void	E_PasteSelection(E_Object *Obj, CSelectList *List, DXNode *PastePoint);
bool	E_CheckBalancedSelection(CSelectList *List);
void	E_MirrorSelection(CSelectList *List);
void	E_BackFaceSelection(CSelectList *List);
void	E_TranslateSelection(CSelectList *List, float	MoveX, float MoveY, float MoveZ);
void	E_MakeModelCopy(CSelectList *List, E_Object *Source, E_Object *Backup);
void	E_UpdateFlags(CSelectList *List, DXFlagsType Flags, DXFlagsType FlagMask);
void	E_SetMinMaxSI(CSelectList *List, float *Min, float *Max, float *Value);
void	E_SetMinMaxZB(CSelectList *List, DWORD *Min, DWORD *Max, DWORD *Value);
void	E_TranslateVertices(CSelectList *List, float	MoveX, float MoveY, float MoveZ);
void	E_SetVertices(CSelectList *List, bool OnlySelection=false, DWORD *Diffuse=NULL, DWORD *Specular=NULL, DWORD *DAlpha=NULL, DWORD *SAlpha=NULL);
void	E_ScaleVertices(CSelectList *List,  D3DXVECTOR3 *Center, float ScaleX, float ScaleY, float ScaleZ);
void	E_OffsetTexture(CSelectList *List, float U, float V);
void	E_ScaleTexture(CSelectList *List, float U, float V);
void	E_RotateTexture(CSelectList *List, float Angle, float RefV, float RefU);
void	E_BlendTexture(CSelectList *List, D3DVERTEXEX	*VRef);
void	E_ClearTexture(E_Object *Obj, DWORD TexNr);
void	E_AssignTexture(CSelectList *List, DWORD TexNr);
void	E_CalculateModelVNormals(E_Object *Obj, float Limit, bool SelectList=false, CSelectList *List=NULL);
void	E_FlipTexture(CSelectList *List, bool U=false, bool V=false);
void	E_MakeAllSpcularity(CSelectList *List, DWORD Color);
void	E_DrawLight(DXLightType *Light, D3DXMATRIX *State, D3DVECTOR *Pos);
void	E_AddStaticLight(E_Object *Obj, DXLightType *Light, DWORD LightID, bool Clear=false);
DXNode	*E_GetModelEnd(E_Object *Source);
void	E_RotateSelection(CSelectList *List, float	AngleX, float AngleY, float AngleZ, D3DXVECTOR3 Center);
void	E_SelectCommonVertex(E_Object *Obj, CSelectList *List);

void	E_FixModelFeatures(E_Object *Obj);
void	E_ClearFaceSelection(CSelectList *List, DWORD *Emissive=NULL, DWORD *ZBias=NULL);
void	E_SelectByTexture(E_Object *Obj, CSelectList *List, DWORD TexNr, D3DCOLORVALUE *Color);
bool	E_CheckUsedTexture(E_Object *Obj, DWORD TexNr);
void	E_KillSelection(E_Object *Obj, CSelectList *List);
void	E_MirrorOnAxis(CSelectList *List, D3DXVECTOR3 Center, bool X=false, bool Y=false, bool Z=false);
void	E_ClearAllStaticLights(E_Object *Obj);
void	E_TranslateUV(CSelectList *List, float	MoveU, float MoveV);
void	E_InsertNode(E_Object *Obj, DXNode *PastePoint, DXNode *Node);
void	E_PlaneSelection(CSelectList *List, float	*X, float *Y, float *Z);
void	E_ScaleSelection(CSelectList *List, float	ScaleX, float ScaleY, float ScaleZ);



#define	TEXTURED_DEFAULT_DIFFUSE	0x00ffffff;
#define	MAX_ZBIAS	15
#define	MAX_SINDEX	4

#define	BASE_SI_INDEX		1.0f
#define	OFF_SI		0
#define	MATTE_SI	1
#define	PLASTIC_SI	2
#define	METAL_SI	3
#define	GLASS_SI	4
#define	SI_INDEX_CX	5.0f

#define	SPECULAR_COLOR_OFF		0x00000000
#define	SPECULAR_COLOR_MATTE	0xff404040
#define	SPECULAR_COLOR_PLASTIC	0xff808080
#define	SPECULAR_COLOR_METAL	0xffc0c0c0
#define	SPECULAR_COLOR_GLASS	0xffffffff

#define	SURFACE_SPECULARCOLORS { SPECULAR_COLOR_OFF, SPECULAR_COLOR_MATTE, SPECULAR_COLOR_PLASTIC, SPECULAR_COLOR_METAL, SPECULAR_COLOR_GLASS };

