#include	"stdafx.h"
#include	"afxtempl.h"
#include	"LODConvert.h"
#include	"DXnodes.h"
#include	"Encoder.h"
#include	"d3d9types.h"
#include	"ObjectInstance.h"
#include	"DXEngine.h"

void IdleMode(void);

Encoder::Encoder(CString strFileName)
{
	m_strFileName   = strFileName;
	m_pBuffer       = NULL;
	m_pPreviousNode = NULL;
	m_pCurrentNode  = NULL;
	m_pNextNode     = NULL;
	m_pRootNode     = NULL;
	m_pLastSurfaceNode = NULL;
	m_pHeader          = NULL;
	m_dwBufferOffset   = 0;
	m_dwTotalNodeCount = 0;
//	m_dwTotalSurfaceVertexes = 0;
	m_dwVertexesPoolSize = 0;
	m_dwVertexesNumber   = 0;
	m_dwDOF              = 0;
	m_pdwSurfaceNodeSizeField = NULL;
	m_pdwSurfaceTotalVertexesField = NULL;
	m_dwNodeID=0;
}

Encoder::~Encoder()
{
	if(m_pBuffer)
		delete[] m_pBuffer;

	m_pBuffer=NULL;

	if(m_pVertexesPool)
		delete[] m_pVertexesPool;

	m_pVertexesPool=NULL;
}

void Encoder::WriteBuffer(const void *pSrc, size_t count)
{
	memcpy((void*)(m_pBuffer+m_dwBufferOffset), pSrc, count);
	m_dwBufferOffset += (DWORD)count;
}




BOOL Encoder::Initialize(E_Object *pConverter)
{
	BOOL bRet = TRUE;

	m_pBuffer       = NULL;
	m_pPreviousNode = NULL;
	m_pCurrentNode  = NULL;
	m_pNextNode     = NULL;
	m_pRootNode     = NULL;
	m_pLastSurfaceNode = NULL;
	m_pHeader          = NULL;
	m_dwBufferOffset   = 0;
	m_dwTotalNodeCount = 0;
//	m_dwTotalSurfaceVertexes = 0;
	m_dwVertexesPoolSize = 0;
	m_dwVertexesNumber   = 0;
	m_dwDOF              = 0;
	m_pdwSurfaceNodeSizeField = NULL;
	m_pdwSurfaceTotalVertexesField = NULL;
	m_dwNodeID=0;

	// Allocate buffer
	m_pBuffer = new BYTE[BUFFER_SIZE];
	DofStackLevel=0;
	m_dwNodeID=0;

	// Allocate vertexes pool
	int iTotalVertexes = GetTotalVertexesNumber((DXNode*)pConverter->Nodes);
	m_pVertexesPool = new BYTE[iTotalVertexes*sizeof(D3DVERTEXEX)];

	if(m_pBuffer == NULL || m_pVertexesPool == NULL)
		bRet = FALSE;
	else
	{
		memset((void*)m_pVertexesPool, 0, iTotalVertexes*sizeof(D3DVERTEXEX));
		memset((void*)m_pBuffer, 0, BUFFER_SIZE*sizeof(BYTE));
		
		if(pConverter->Nodes != NULL)
		{
			

			// Assign root node pointer
			m_pRootNode = (DXNode*)pConverter->Nodes;
			// Write textures section
			((DxDbHeader*)m_pBuffer)->dwTexNr=pConverter->TexNr;

			// assign the script number
			*((DxDbHeader*)m_pBuffer)->Scripts=*pConverter->Scripts;


			// Texture Buffer Pointer AFTER the Header
			m_dwBufferOffset+=sizeof(DxDbHeader);
			WriteBuffer((void*)&pConverter->Textures, pConverter->TexNr*sizeof(DWORD));

			LightList=pConverter->LightList;

			// Save header pointer
			m_pHeader = m_pBuffer;
		}
		else
			bRet = FALSE;
	}
	return bRet;
}

int Encoder::GetTotalVertexesNumber(DXNode *pRootNode)
{
	DXNode *pCurrentNode = pRootNode;
	int iNumVertexes = 0;

	while(pCurrentNode != NULL)
	{
		switch(pCurrentNode->Type)
		{
			case DOT:
				iNumVertexes++;
				break;

			case LINE:
				iNumVertexes += 2;
				break;

			case TRIANGLE:
				iNumVertexes += 3;
				break;
		}
		pCurrentNode = pCurrentNode->Next;
	}

	return iNumVertexes;
}


// This function pages to DWORD the Nodes
void Encoder::PatchDWORD(void)
{
	// If already 4 Bytes Paged exits here
	if(!(m_dwBufferOffset&0x03)) return;
	// if Not a Node Written exits here
	if(!m_pdwSurfaceNodeSizeField) return;

	// Get the bytes to add for Patching
	DWORD	Count=0x00000004-(m_dwBufferOffset&0x03);
	// and updates all
	m_dwBufferOffset+=Count;
	*m_pdwSurfaceNodeSizeField+=Count;
}


void Encoder::Encode(DWORD ID, DWORD Class)
{
	BOOL bInsertSurfaceNode = FALSE;
	BOOL bInsertTextureNode = FALSE;
	m_pCurrentNode = m_pRootNode;
	
	// COBRA - RED - Missing Last Texture Initialization to INVALID
	DWORD			dwCurrentTexture=-1;
	DWORD			dwCurrentSpecularity=0;
	DWORD			dwCurrentFaceSwitch=-1;
	DWORD		dwCurrentzBias=0;
	NodeType	LastType=ROOT;
	DXFlagsType		LastFlags;
	DxSurfaceType	*Surface;
	LastType=(NodeType)-1;

	LastFlags.w=0x00;
	bool		ChangeSurface=false;

	while(m_pCurrentNode != NULL)
	{
		IdleMode();
		NodeType	Type=m_pCurrentNode->Type;
		DXFlagsType	Flags;
		Flags.w=0;
	
		// Make all checks here
		ChangeSurface=false;
		if(Type!=LastType || Type==DOF || Type==CLOSEDOF || Type==SLOT ) ChangeSurface = true;
		
		if(Type==DOT || Type==LINE || Type==TRIANGLE){
			Surface=&((DXTriangle*)m_pCurrentNode)->Surface;
			Surface->dwFlags.b.Frame=0; Surface->dwFlags.b.Disable=0;
			Flags.w=Surface->dwFlags.w;
			if(Flags.b.Texture && Surface->TexID[0]!=dwCurrentTexture) ChangeSurface = true;
			if(Flags.b.Texture != LastFlags.b.Texture) ChangeSurface=true;
			if(Flags.b.Alpha != LastFlags.b.Alpha) ChangeSurface=true;
			if(Flags.b.Gouraud != LastFlags.b.Gouraud ) ChangeSurface=true;
			if(Flags.b.Lite != LastFlags.b.Lite ) ChangeSurface=true;
			if((Flags.b.VColor) != (LastFlags.b.VColor)) ChangeSurface=true;
			if(Flags.b.zBias != LastFlags.b.zBias) ChangeSurface=true;
			if(Surface->dwzBias!=dwCurrentzBias) ChangeSurface=true;
			if(Flags.b.ChromaKey != LastFlags.b.ChromaKey) ChangeSurface=true;
			if(Flags.b.ChromaKey != LastFlags.b.ChromaKey) ChangeSurface=true;
			if(Flags.b.SwEmissive != LastFlags.b.SwEmissive) ChangeSurface=true;
			if(Flags.b.SwEmissive && Surface->SwitchNumber!=dwCurrentFaceSwitch) ChangeSurface=true;
			if(*((DWORD*)&Surface->DefaultSpecularity) !=  dwCurrentSpecularity) ChangeSurface=true;
		}

		// if any change
		if(ChangeSurface){
			
			// Patch the segments
			PatchDWORD();

			switch (Type){

				case	DOT:
				case	LINE:
				case	TRIANGLE:	// Check if a Textured new surace and last texture still valid
									if(Flags.b.Texture && Surface->TexID[0]!=dwCurrentTexture){
										// No moer used Texture node
										//WriteTextureNode(m_pCurrentNode);
										dwCurrentTexture=Surface->TexID[0];
									}
									
									dwCurrentSpecularity=Surface->DefaultSpecularity;

									dwCurrentFaceSwitch=Surface->SwitchNumber;
										
									dwCurrentzBias=Surface->dwzBias;
									// Write the new Surface
									WriteSurfaceNode((DXTriangle*)m_pCurrentNode, dwCurrentTexture);
									// New ID
									m_dwNodeID++;
									break;

				case	DOF:		// We've to insert a new DOF node
									{DXDof *pDof = (DXDof*)m_pCurrentNode;	WriteDofNode(pDof);}
									// and reset surfaces properties
									Flags.w=0;
									dwCurrentTexture=-1;
									// New ID
									m_dwNodeID++;
									break;

				case	SLOT:		// We've to insert a new SLOT node
									WriteSlotNode((DXSlot*)m_pCurrentNode);
									// New ID
									m_dwNodeID++;
									break;

		
				case CLOSEDOF:		// We've to insert a new END DOF node
									WriteDofEndNode();
									// and reset surfaces properties
									Flags.w=0;
									dwCurrentTexture=-1;
									// New ID
									m_dwNodeID++;
									break;

				case	MODELEND:	break;
				default	:			MessageBox(NULL, "Unknown SURFACE TYPE!!", "Encoder", 0);
			}

			ChangeSurface=false;
			LastFlags=Flags;
			LastType=Type;

		}


		switch(Type)
		{
			case DOT:
			{
				DXVertex *pVertex = (DXVertex*)m_pCurrentNode;
				D3DVERTEXEX dxVertex;
				InitializeVertex(&dxVertex);

				CopyVertex(&(pVertex->Vertex),&dxVertex);

				// Add vertex in NON INDEXED MODE
				UInt16 iIndex = AddVertexToPool(&dxVertex, false);
				WriteBuffer((const void*)&iIndex, sizeof(iIndex));
			}
			break;

			case LINE:
			{
				DXLine *pLine = (DXLine*)m_pCurrentNode;

				for(int i=0; i<2; i++)
				{
					D3DVERTEXEX dxVertex;
					InitializeVertex(&dxVertex);

					CopyVertex(&(pLine->Vertex[i]), &dxVertex);

					UInt16 iIndex = AddVertexToPool(&dxVertex);
					WriteBuffer((const void*)&iIndex, sizeof(iIndex));
				}
			}
			break;

			case TRIANGLE:
			{
				DXTriangle *pTriangle = (DXTriangle*)m_pCurrentNode;

				for(int i=0; i<3; i++)
				{
					D3DVERTEXEX dxVertex;
					InitializeVertex(&dxVertex);
					
					CopyVertex(&(pTriangle->Vertex[i]), &dxVertex);
					
					UInt16 iIndex = AddVertexToPool(&dxVertex);
					WriteBuffer((const void*)&iIndex, sizeof(iIndex));
				}
			}
			break;

		}
		// Update pointers
		//m_pPreviousNode = m_pCurrentNode;
		m_pCurrentNode  = m_pCurrentNode->Next;
		//m_pNextNode     = m_pCurrentNode != NULL ? m_pCurrentNode->Next : NULL;
	}

	//// Set last node total size (Node + Data (total indexes size))
	//DWORD dwTotalNodeSize = (m_dwTotalSurfaceVertexes*sizeof(Int16) + sizeof(DxSurfaceType));
	//memcpy((void*)m_pLastSurfaceNode, (const void*)&dwTotalNodeSize, sizeof(dwTotalNodeSize));
	//// Add last node Vertex Counter to last surface node
	//memcpy(m_pLastSurfaceNode + 12, (const void*)&m_dwTotalSurfaceVertexes, sizeof(m_dwTotalSurfaceVertexes));

	// Patch the segments
	PatchDWORD();

	// Write ENDMODEL node
	DxEndModelType dxEndModelNode;
	dxEndModelNode.h.dwNodeSize = sizeof(DxEndModelType);
	dxEndModelNode.h.Type = DX_MODELEND;
	dxEndModelNode.h.dwNodeID=m_dwNodeID++;
 	WriteBuffer((void*)&dxEndModelNode, sizeof(dxEndModelNode));

	// Write total number of nodes
	((DxDbHeader*)m_pHeader)->dwNodesNr=m_dwTotalNodeCount;

	// Add the lights
	((DxDbHeader*)m_pBuffer)->dwLightsNr=0;
	((DxDbHeader*)m_pBuffer)->pLightsPool=m_dwBufferOffset;
	E_Light *LPtr=LightList;
	while(LPtr){
		((DxDbHeader*)m_pBuffer)->dwLightsNr++;
		WriteBuffer((void*)&LPtr->Light, sizeof(DXLightType));
		LPtr=LPtr->Next;
	}

	// Write other header fields
	//DWORD dwVertexesPoolStart = (DWORD)(m_pBuffer + m_dwBufferOffset);
	((DxDbHeader*)m_pHeader)->pVPool=m_dwBufferOffset;
	((DxDbHeader*)m_pHeader)->dwPoolSize=m_dwVertexesPoolSize;
	((DxDbHeader*)m_pHeader)->dwNVertices=m_dwVertexesNumber;

	// Copy vertexes pool after nodes section
	WriteBuffer((const void*)m_pVertexesPool, m_dwVertexesPoolSize);

	((DxDbHeader*)m_pHeader)->Id=ID;
	((DxDbHeader*)m_pHeader)->ModelSize= m_dwBufferOffset;
	((DxDbHeader*)m_pHeader)->VBClass=Class;
	((DxDbHeader*)m_pHeader)->Version=MODEL_VERSION|((MODEL_VERSION^0xffff)<<16);

#ifdef	CRYPTED_MODELS
	TheVbManager.Encrypt((DWORD*)m_pBuffer);
#endif
	// Write file
	//WriteFile();
}

void Encoder::WriteTextureNode(DXNode *pNode)
{
}

void Encoder::WriteSurfaceNode(DXTriangle *pNode, DWORD Texture)
{
	//if(m_pLastSurfaceNode)
	//{
	//	// Set node total size (Node + Data (total indexes size))
	//	DWORD dwTotalNodeSize = (m_dwTotalSurfaceVertexes*sizeof(Int16) + sizeof(DxSurfaceType));
	//	memcpy((void*)m_pLastSurfaceNode, (const void*)&dwTotalNodeSize, sizeof(dwTotalNodeSize));
	//	// Set previous SURFACE node vertex counter and reset it (12 is the offset of dwVCount field)
	//	memcpy((void*)(m_pLastSurfaceNode + 12), (const void*)&m_dwTotalSurfaceVertexes, sizeof(m_dwTotalSurfaceVertexes));
	//}
	//m_dwTotalSurfaceVertexes = 0;

	// Define new node to insert
	DxSurfaceType dxSurfaceNode;
	// Reset structure
	memset((void*)&dxSurfaceNode, 0, sizeof(dxSurfaceNode));

	//Set the ID
	dxSurfaceNode.h.dwNodeID=m_dwNodeID;
	// Set common data
	dxSurfaceNode.h.dwNodeSize = sizeof(DxSurfaceType);
	dxSurfaceNode.h.Type       = DX_SURFACE;
	// Set Flags
	dxSurfaceNode.dwFlags = pNode->Surface.dwFlags;
	// Based on node type, set DX primitive type

	//if(!pNode->Surface.dwFlags.b.Point && !pNode->Surface.dwFlags.b.Line && !pNode->Surface.dwFlags.b.Poly ) MessageBox(NULL, "BAD SURFACE","ENCODER", NULL);


	switch(pNode->Type)
	{
		case TRIANGLE:
			dxSurfaceNode.dwPrimType = D3DPT_TRIANGLELIST;
			pNode->Surface.dwFlags.b.Point=false;
			pNode->Surface.dwFlags.b.Line=false;
			pNode->Surface.dwFlags.b.Poly=true;
		break;

		case LINE:
			dxSurfaceNode.dwPrimType = D3DPT_LINELIST;
			pNode->Surface.dwFlags.b.Point=false;
			pNode->Surface.dwFlags.b.Line=true;
			pNode->Surface.dwFlags.b.Poly=false;
		break;

		case DOT:
			dxSurfaceNode.dwPrimType = D3DPT_POINTLIST;
			pNode->Surface.dwFlags.b.Point=true;
			pNode->Surface.dwFlags.b.Line=false;
			pNode->Surface.dwFlags.b.Poly=false;
		break;

		default	:MessageBox(NULL, "UNRECOGNIZED PRIMITIVE TYPE!!!", "ENCODER", 0);
	}
	
	// Set vertex format using DX Flexible Vertex Format flags
//	dxSurfaceNode.dwVFormat = D3DFVF_XYZ|D3DFVF_NORMAL;

	// Set dimension of each vertex item
	if(pNode->Surface.dwFlags.b.Texture)
	{
//		dxSurfaceNode.dwVFormat |= D3DFVF_TEX1;
		//dxSurfaceNode.dwStride = 32;   // 32 bytes if Texture Node (tu and tv texture coordinate)
	}
	else if(pNode->Surface.dwFlags.b.VColor)
	{
//		dxSurfaceNode.dwVFormat |= D3DFVF_DIFFUSE;
		//dxSurfaceNode.dwStride = 28;            // else 28 bytes (only VColour component)
	}

	// Assign new zBias
	dxSurfaceNode.dwzBias=pNode->Surface.dwzBias;

	// Set vertex dimension
	dxSurfaceNode.dwStride = sizeof(D3DVERTEXEX);
	// Save pointer to this SURFACE node
	//m_pLastSurfaceNode = m_pBuffer + m_dwBufferOffset;

	// Set default material
	dxSurfaceNode.SpecularIndex=pNode->Surface.SpecularIndex;

	// Set the Texture
	if(pNode->Surface.dwFlags.b.Texture) dxSurfaceNode.TexID[0]=pNode->Surface.TexID[0];
	else dxSurfaceNode.TexID[0]=-1;


	// Save pointer to NodeSize field and to TotalVertexes number
	m_pdwSurfaceNodeSizeField      = (DWORD*)(m_pBuffer + m_dwBufferOffset);
	m_pdwSurfaceTotalVertexesField = (DWORD*)(m_pBuffer + m_dwBufferOffset + 16);

	// Set default specularity for emissive surfaces
	dxSurfaceNode.DefaultSpecularity=pNode->Surface.DefaultSpecularity;
	dxSurfaceNode.SwitchNumber=pNode->Surface.SwitchNumber;
	dxSurfaceNode.SwitchMask=pNode->Surface.SwitchMask;


	// Write new SURFACE node to buffer
	WriteBuffer((void*)&dxSurfaceNode, sizeof(dxSurfaceNode));
	// Increment total node counter
	m_dwTotalNodeCount++;
}

void Encoder::WriteDofNode(DXDof *pNode)
{
	DxDofType dxDOFNode;
	// Reset structure
	memset((void*)&dxDOFNode, 0, sizeof(dxDOFNode));
	//Set the ID
	dxDOFNode.h.dwNodeID=m_dwNodeID;
	// Set common data
	dxDOFNode.h.dwNodeSize = sizeof(DxDofType);
	dxDOFNode.h.Type       = DX_DOF;
	// Set fields
	dxDOFNode.dofNumber = pNode->dof.dofNumber;
	dxDOFNode.flags = pNode->dof.flags;
	dxDOFNode.future = pNode->dof.future;
	dxDOFNode.max = pNode->dof.max;
	dxDOFNode.min = pNode->dof.min;
	dxDOFNode.multiplier = pNode->dof.multiplier;
	dxDOFNode.rotation = pNode->dof.rotation;
	dxDOFNode.scale= pNode->dof.scale;
	dxDOFNode.translation = pNode->dof.translation;
	dxDOFNode.Type = pNode->dof.Type;
	dxDOFNode.SwitchBranch = pNode->dof.SwitchBranch;
	dxDOFNode.SwitchNumber = pNode->dof.SwitchNumber;
	// Save DOF pointer
	m_arDOFPointers.Add((DxDofType*)(m_pBuffer+m_dwBufferOffset));
	m_dwDOF++;
	
	// Saves the DOF Entry in the Stack
	DOFsStack[DofStackLevel++]=(DxDofType*)(m_pBuffer+m_dwBufferOffset);

	// Write data to buffer
	WriteBuffer((void*)&dxDOFNode, sizeof(dxDOFNode));

	// Increment total node counter
	m_dwTotalNodeCount++;
}


void Encoder::WriteSlotNode(DXSlot *pNode)
{
	DxSlotType dxSlotNode;
	// Reset structure
	memset((void*)&dxSlotNode, 0, sizeof(dxSlotNode));
	//Set the ID
	dxSlotNode.h.dwNodeID=m_dwNodeID;
	// Set common data
	dxSlotNode.h.dwNodeSize = sizeof(DxSlotType);
	dxSlotNode.h.Type       = DX_SLOT;
	
	// Set fields
	dxSlotNode.SlotNr=pNode->Slot.SlotNr;
	dxSlotNode.rotation=pNode->Slot.rotation;
	

	// Write data to buffer
	WriteBuffer((void*)&dxSlotNode, sizeof(dxSlotNode));

	// Increment total node counter
	m_dwTotalNodeCount++;
}



void Encoder::WriteDofEndNode()
{
	DxDofEndType dxDofEndNode;
	// Reset structure
	memset((void*)&dxDofEndNode, 0, sizeof(dxDofEndNode));
	//Set the ID
	dxDofEndNode.h.dwNodeID=m_dwNodeID;
	// Set common data
	dxDofEndNode.h.dwNodeSize = sizeof(DxDofEndType);
	dxDofEndNode.h.Type       = DX_ENDDOF;

	// Assign the DOF the size of this before closing it
	if(DofStackLevel){
		DofStackLevel--;
		DOFsStack[DofStackLevel]->dwDOFTotalSize=m_pBuffer+m_dwBufferOffset-(byte*)DOFsStack[DofStackLevel];
	}

	//Write data to buffer
	WriteBuffer((void*)&dxDofEndNode, sizeof(dxDofEndNode));
	// Decrement DOF counter
	m_arDOFPointers.RemoveAt(m_dwDOF-1);
	m_dwDOF--;
	// Increment total node counter
	m_dwTotalNodeCount++;
}

void Encoder::WriteFile()
{
	CFile cFile(m_strFileName, CFile::modeCreate|CFile::modeReadWrite);
	if(cFile.m_hFile)
	{
		cFile.Write((const void*)m_pBuffer, m_dwBufferOffset);
		cFile.Close();
	}
	else
		AfxMessageBox(_T("Error writing file to disk"), MB_OK|MB_ICONEXCLAMATION);
}

void Encoder::InitializeVertex(D3DVERTEXEX *pVertex)
{
	pVertex->nx = -1;
	pVertex->ny = -1;
	pVertex->nz = -1;
	pVertex->vx = -1;
	pVertex->vy = -1;
	pVertex->vz = -1;
	pVertex->dwColour = -1;
	pVertex->dwSpecular = -1;
	pVertex->tu = -1;
	pVertex->tv = -1;
}

BOOL Encoder::IsSameVertexes(D3DVERTEXEX *pVertex1, D3DVERTEXEX *pVertex2)
{
#ifdef	INDEXED_MODE_ENGINE	
	return (
			pVertex1->nx == pVertex2->nx &&
			pVertex1->ny == pVertex2->ny &&
			pVertex1->nz == pVertex2->nz &&
			pVertex1->vx == pVertex2->vx &&
			pVertex1->vy == pVertex2->vy &&
			pVertex1->vz == pVertex2->vz &&
			pVertex1->dwColour == pVertex2->dwColour &&
			pVertex1->dwSpecular == pVertex2->dwSpecular &&
			pVertex1->tu == pVertex2->tu &&
			pVertex1->tv == pVertex2->tv
			);
#else
	return false;
#endif
}

void Encoder::CopyVertex(D3DVERTEXEX *pSourceVertex, D3DVERTEXEX *pDestinationVertex)
{
	ASSERT(pSourceVertex != NULL);
	ASSERT(pDestinationVertex != NULL);
	*pDestinationVertex=*pSourceVertex;
}

UInt16 Encoder::AddVertexToPool(D3DVERTEXEX *pVertex, bool Indexed)
{
	BYTE *pCurrentVertex = m_pVertexesPool;
	UInt16 iVertexIndex = 0xffff;

	// COBRA - RED - Only for Indexed vertices ( e.g not for point lists )
	if(Indexed){
		for(unsigned int i=0; i<m_dwVertexesNumber; i++)
		{
			if(IsSameVertexes((D3DVERTEXEX*)pCurrentVertex, pVertex))
			{
				iVertexIndex = i;
				break;
			}
			pCurrentVertex += sizeof(D3DVERTEXEX);
		}
	}

	if(iVertexIndex == 0xffff)

	{
		iVertexIndex = (UInt16)m_dwVertexesNumber;
		memcpy((void*)(m_pVertexesPool+iVertexIndex*sizeof(D3DVERTEXEX)), (const void*)pVertex, sizeof(D3DVERTEXEX));
		m_dwVertexesNumber++;
		m_dwVertexesPoolSize += sizeof(D3DVERTEXEX);
	}

	// Increment current DOF vertexes counter
	/*if(m_dwDOF > 0)
		m_arDOFPointers[m_dwDOF-1]->dwDOFVertices++;*/



	// Incrementent total vertexes and indexes counter
	(*m_pdwSurfaceNodeSizeField) += sizeof(UInt16);
	(*m_pdwSurfaceTotalVertexesField)++;
	//m_dwTotalSurfaceVertexes++;

	return iVertexIndex;
}

