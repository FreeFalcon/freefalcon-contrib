//*******************************************************************************************************
// DX Format Encoding functions
// 2005 [R]ed
//*******************************************************************************************************

#pragma once

#include "Converter.h"
#include "DXVbManager.h"
// File Header Structure
typedef	struct { 
					DWORD	dwModelSize;			// Model Size in Bytes
					DWORD	dwNodesNr;				// Total number of nodes in the Model
				} DxHeaderType;

// Structure defining a Vector item
typedef	struct	{ D3DVALUE	px, py, pz; } VectorsType;	

// Structure defining a Normal item
typedef	struct	{ D3DVALUE	nx, ny, nz; } NormalType;	

// Structure defining a Texture Coord Item
typedef	struct	{ D3DVALUE	tu, tv; } TextureType;	

// Structure defining a Diffuse Type
typedef	struct	{ union { DWORD dwDiffuse; char da, dr, dg, db; }; } ColourType;	

// Structure defining a Specular Type
typedef	struct	{ union { DWORD dwSpecular; char sa, sr, sg, sb; }; } SpecularType;	

// Type of Items available for drawing
typedef enum {	
				DX_ROOT=0,					// ROOT OF another model, not used usually
				DX_SURFACE,					// SURFACE Type
				DX_MATERIAL,				// MATERIAL Type
				DX_TEXTURE,					// TEXTURE Type
				DX_DOF,						// DOF Type
				DX_ENDDOF,					// DOF END
				DX_SWITCH,					// SWITCH Type
				DX_LIGHT,					// LIGHT Type
				DX_MODELEND
				} ItemType;

// The header of each node
typedef struct {	
					DWORD			dwNodeSize;
					ItemType		Type;
				} DXNodeHeadType;


//****************************** DX NODES STRUCTURES *********************************

// * SURFACE ITEM *
typedef struct {	
					DXNodeHeadType		h;						// NODE HEAD
					DXFlagsType			dwFlags;				// Node Flags from DXNodes
					DWORD				dwVCount;				// Vertex counting
					DWORD				dwStride;				// Vertex stride, size of each vertex in bytes
					DWORD				dwVFormat;				// DX FLEXIBE VERTEX FORMAT Flags
					D3DPRIMITIVETYPE	dwPrimType;				// DX Primitive Type
					D3DRENDERSTATETYPE	dwRenderState;			// DX renderer state
				} DxSurfaceType;


//************************************************************************************

// * MATERIAL ITEM *
typedef	struct	{
					DXNodeHeadType		h;						// NODE HEAD
					D3DMATERIAL			m;						// DX MATERIAL STRUCTURE
				} DxMaterialType;


//************************************************************************************

// * TEXTURE ITEM *
typedef	struct	{
					DXNodeHeadType		h;						// NODE HEAD
					DWORD				Tex[4];					// TEXTURES
				}	DxTextureType;



//************************************************************************************

// * DOF ITEM *
typedef	struct	{
					DXNodeHeadType		h;						// NODE HEAD
					DWORD				dwDOFVertices;
					DofType				Type;
					union {				int	dofNumber;
										int SwitchNumber;
					};
					float				min,max,multiplier,future;
					union {				int  flags;
										int SwitchBranch;
					};
					Ppoint				scale;
					Pmatrix				rotation;
					Ppoint				translation; 
				} DxDofType;
					
//************************************************************************************

// * DOF END ITEM *
typedef	struct	{
					DXNodeHeadType		h;						// NODE HEAD
				}DxDofEndType;

//************************************************************************************

// * MODEL END ITEM *
typedef	struct	{
	DXNodeHeadType		h;						// NODE HEAD
}DxEndModelType;


// * DX Database Textures section *
typedef struct DxDbHeader{
		DWORD dwNodesNr;	// This field is the count of all Nodes forming the Model
		DWORD pVVectors;	// This field is the offset from the start of model of Vertices Vector Data
		DWORD pVNormals;	// This field is the offset from the start of model of Vertices Normals Data
		DWORD pVDiffuse;	// This field is the offset from the start of model of Vertices Diffuse Data
		DWORD pVSpecular;	// This field is the offset from the start of model of Vertices Specular Data
		DWORD pVTexture1;	// This field is the offset from the start of model of Vertices Texture Coords
		DWORD pVTexture2;	// This field is the offset from the start of model of Vertices Texture Coords
		DWORD pVTexture3;	// This field is the offset from the start of model of Vertices Texture Coords
		DWORD pVTexture4;	// This field is the offset from the start of model of Vertices Texture Coords
}DxDbHeader;
//************************************************************************************


// Define buffer size (2Mb)
#define BUFFER_SIZE 2000000

class	Encoder
{
public:
	Encoder(CString strFileName);
	virtual ~Encoder();

	BOOL Initialize(Converter *pConverter);
	void Encode();
	BYTE *GetModelBuffer()					{ return m_pBuffer; }


private:
	CString	m_strFileName;
	BYTE	*m_pBuffer;							// Data buffer
	BYTE	*m_pVertexesPool;				// Vertexes pool
	BYTE	*m_pLastSurfaceNode;				// Pointer to last SURFACE node inserted (used to update dwVCount member)
	BYTE	*m_pHeader;							// Pointer to header of database structure
	DXNode	*m_pPreviousNode, *m_pCurrentNode,
			*m_pNextNode, *m_pRootNode;			// Pointers to nodes we're analyzing
	DWORD	 m_dwBufferOffset;					// Indicate position (in bytes) in the buffer m_pBuffer
	DWORD	 m_dwTotalNodeCount;				// Total number of nodes inserted in model
	DWORD	 m_dwTotalSurfaceVertexes;			// Total number of vertexes per SURFACE node
	DWORD	 m_dwVertexesPoolSize;
	DWORD	 m_dwVertexesNumber;
	DWORD	 m_dwDOF;
	CArray<DxDofType*,DxDofType*> m_arDOFPointers;

private:
	void InitializeVertex(D3DVERTEXEX *pVertex);
	int  GetTotalVertexesNumber(DXNode *pRootNode);
	Int16  AddVertexToPool(D3DVERTEXEX *pVertex);
	BOOL IsSameVertexes(D3DVERTEXEX *pVertex1, D3DVERTEXEX *pVertex2);
	void WriteBuffer(const void *pSrc, size_t count);
	void WriteTextureNode(DXNode *pNode);
	void WriteSurfaceNode(DXNode *pNode);
	void WriteDofNode(DXDof *pNode);
	void WriteDofEndNode();
	void WriteFile();
};


