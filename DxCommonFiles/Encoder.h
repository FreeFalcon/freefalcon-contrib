#pragma once

#include	"Lodconvert.h"
// Define buffer size (2Mb)
#define BUFFER_SIZE 16000000

class	Encoder
{
public:
	Encoder(CString strFileName);
	virtual ~Encoder();

	BOOL Initialize(E_Object *pConverter);
	void Encode(DWORD ID, DWORD Class);
	BYTE *GetModelBuffer()					{ return m_pBuffer; }
	DWORD	GetModelSize()					{ return m_dwBufferOffset; }


private:
	CString	m_strFileName;
	BYTE	*m_pBuffer;							// Data buffer
	BYTE	*m_pVertexesPool;				// Vertexes pool
	BYTE	*m_pLastSurfaceNode;				// Pointer to last SURFACE node inserted (used to update dwVCount member)
	BYTE	*m_pHeader;							// Pointer to header of database structure
	DXNode	*m_pPreviousNode, *m_pCurrentNode,
			*m_pNextNode, *m_pRootNode;			// Pointers to nodes we're analyzing
	DWORD	*m_pdwSurfaceNodeSizeField;
	DWORD	*m_pdwSurfaceTotalVertexesField;
	DWORD	 m_dwBufferOffset;					// Indicate position (in bytes) in the buffer m_pBuffer
	DWORD	 m_dwTotalNodeCount;				// Total number of nodes inserted in model
	//DWORD	 m_dwTotalSurfaceVertexes;			// Total number of vertexes per SURFACE node
	DWORD	 m_dwVertexesPoolSize;
	DWORD	 m_dwVertexesNumber;
	DWORD	 m_dwDOF;
	DWORD	 m_dwNodeID;						// Node Identifier
	E_Light	*LightList;
	CArray<DxDofType*,DxDofType*> m_arDOFPointers;

private:
	void InitializeVertex(D3DVERTEXEX *pVertex);
	int  GetTotalVertexesNumber(DXNode *pRootNode);
	UInt16  AddVertexToPool(D3DVERTEXEX *pVertex, bool Indexed = true);
	void CopyVertex(D3DVERTEXEX *pSourceVertex, D3DVERTEXEX *pDestinationVertex);
	BOOL IsSameVertexes(D3DVERTEXEX *pVertex1, D3DVERTEXEX *pVertex2);
	void WriteBuffer(const void *pSrc, size_t count);
	void WriteTextureNode(DXNode *pNode);
	void WriteSurfaceNode(DXTriangle *pNode, DWORD Texture);
	void WriteDofNode(DXDof *pNode);
	void WriteDofEndNode();
	void WriteSlotNode(DXSlot *pNode);
	void WriteFile();
	void PatchDWORD(void);

	DxDofType	*DOFsStack[512];
	DWORD		DofStackLevel;
};


