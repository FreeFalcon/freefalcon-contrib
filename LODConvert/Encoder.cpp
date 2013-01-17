#include	"stdafx.h"
#include	"BSPNodes.h"
#include	"DXnodes.h"
#include	"Encoder.h"
#include	"d3d9types.h"
#include	"ObjectInstance.h"
#include	"DXEngine.h"
#include	"Materials.h"

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
	m_dwBufferOffset += count;
}




BOOL Encoder::Initialize(Converter *pConverter)
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
	int iTotalVertexes = GetTotalVertexesNumber(pConverter->VRoot);
	m_pVertexesPool = new BYTE[iTotalVertexes*sizeof(D3DVERTEXEX)];

	if(m_pBuffer == NULL || m_pVertexesPool == NULL)
		bRet = FALSE;
	else
	{
		memset((void*)m_pVertexesPool, 0, iTotalVertexes*sizeof(D3DVERTEXEX));
		memset((void*)m_pBuffer, 0, BUFFER_SIZE*sizeof(BYTE));
		
		if(pConverter->VRoot != NULL)
		{
			

			// Assign root node pointer
			m_pRootNode = pConverter->VRoot;
			// Write textures section
			((DxDbHeader*)m_pBuffer)->dwTexNr=pConverter->NTextures;

			// assign the script number
			((DxDbHeader*)m_pBuffer)->Scripts[0].Script=(ScriptType)pConverter->ScriptNumber;


			// Buffer Pointer AFTER the Header
			m_dwBufferOffset+=sizeof(DxDbHeader);
			WriteBuffer((void*)&pConverter->TextureList, pConverter->NTextures*sizeof(DWORD));
			
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


void Encoder::Encode(DWORD ID, DWORD Class, DWORD CTNumber, DWORD MType)
{
	BOOL bInsertSurfaceNode = FALSE;
	BOOL bInsertTextureNode = FALSE;
	m_pCurrentNode = m_pRootNode;
	
	// COBRA - RED - Missing Last Texture Initialization to INVALID
	DWORD		dwCurrentTexture=-1;
	DWORD		dwCurrentzBias=0;
	NodeType	LastType=ROOT;
	DXFlagsType	LastFlags;

	LastFlags.w=0x00;
	bool		ChangeSurface=false;

	while(m_pCurrentNode != NULL)
	{
		IdleMode();
		NodeType	Type=m_pCurrentNode->Type;
		DXFlagsType	Flags;
		Flags.w=m_pCurrentNode->Flags.w;
	
		// Make all checks here
		ChangeSurface=false;
		if(Type!=LastType || Type==DOF || Type==CLOSEDOF || Type==SLOT ) ChangeSurface = true;
		if(Flags.b.Texture && m_pCurrentNode->Texture!=dwCurrentTexture) ChangeSurface = true;
		if(Flags.b.Texture != LastFlags.b.Texture) ChangeSurface=true;
		if(Flags.b.Alpha != LastFlags.b.Alpha) ChangeSurface=true;
		if(Flags.b.Gouraud != LastFlags.b.Gouraud ) ChangeSurface=true;
		if(Flags.b.Lite != LastFlags.b.Lite ) ChangeSurface=true;
		if((Flags.b.VColor) != (LastFlags.b.VColor)) ChangeSurface=true;
		if(Flags.b.zBias != LastFlags.b.zBias) ChangeSurface=true;
		if(Flags.b.zBias && m_pCurrentNode->dwzBias!=dwCurrentzBias) ChangeSurface=true;
		if(Flags.b.ChromaKey != LastFlags.b.ChromaKey) ChangeSurface=true;

		// if any change
		if(ChangeSurface){
			
			// Patch the segments
			PatchDWORD();

			switch (Type){

				case	DOT:
				case	LINE:
				case	TRIANGLE:	// Check if a Textured new surace and last texture still valid
									if(Flags.b.Texture && m_pCurrentNode->Texture!=dwCurrentTexture){
										// No moer used Texture node
										//WriteTextureNode(m_pCurrentNode);
										dwCurrentTexture=m_pCurrentNode->Texture;
									}
									dwCurrentzBias=m_pCurrentNode->dwzBias;
									// Write the new Surface
									WriteSurfaceNode(m_pCurrentNode, dwCurrentTexture);
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

				CopyVertex(&(pVertex->Vertex), pVertex->VColor, &dxVertex);
				/*memcpy((void*)&dxVertex, (const void*)&pVertex->Vertex, sizeof(pVertex->Vertex));
				memcpy((void*)(&dxVertex + sizeof(pVertex->Vertex)), (const void*)&pVertex->VColor, sizeof(pVertex->VColor));*/

				// Add vertex in NON INDEXED MODE
				Int16 iIndex = AddVertexToPool(&dxVertex, false);
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

					CopyVertex(&(pLine->Vertex[i]), pLine->VColor[i], &dxVertex);
					/*memcpy((void*)&dxVertex, (const void*)&pLine->Vertex[i], sizeof(pLine->Vertex[i]));
					memcpy((void*)(&dxVertex + sizeof(pLine->Vertex[i])), (const void*)&pLine->VColor[i], sizeof(pLine->VColor[0]));*/

					Int16 iIndex = AddVertexToPool(&dxVertex);
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
					
					CopyVertex(&(pTriangle->Vertex[i]), pTriangle->VColor[i], &dxVertex);
					
					/*BYTE *pVertex = (BYTE*)&dxVertex;
					memcpy((void*)&dxVertex, (const void*)&pTriangle->Vertex[i], sizeof(pTriangle->Vertex[i]));
					memcpy((void*)(pVertex + sizeof(pTriangle->Vertex[i])), (const void*)&pTriangle->VColor[i], sizeof(pTriangle->VColor[0]));*/

					Int16 iIndex = AddVertexToPool(&dxVertex);
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
 	WriteBuffer((void*)&dxEndModelNode, sizeof(dxEndModelNode));

	// Write total number of nodes
	((DxDbHeader*)m_pHeader)->dwNodesNr=m_dwTotalNodeCount;

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

	// Assign Materials Features
	m_dwBufferOffset=AssignSurfaceFeatures(m_pBuffer, CTNumber, MType);

#ifdef	CRYPTED_MODELS
	TheVbManager.Encrypt((DWORD*)m_pBuffer);
#endif
	// Write file
	//WriteFile();
}

void Encoder::WriteTextureNode(DXNode *pNode)
{
	// Define new node to insert
	DxTextureType dxTextureNode;
	memset((void*)&dxTextureNode, 0, sizeof(dxTextureNode));
	dxTextureNode.h.dwNodeID=m_dwNodeID;
	dxTextureNode.h.dwNodeSize = sizeof(DxTextureType);
	dxTextureNode.h.Type       = DX_TEXTURE;
	dxTextureNode.Tex[0] = pNode->Texture;
	for(int i=1;i<4;i++)
		dxTextureNode.Tex[i] = -1;
	WriteBuffer((void*)&dxTextureNode, sizeof(dxTextureNode));
	// Increment total node counter
	m_dwTotalNodeCount++;
}

void Encoder::WriteSurfaceNode(DXNode *pNode, DWORD Texture)
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
	dxSurfaceNode.dwFlags = pNode->Flags;
	// Based on node type, set DX primitive type

	if(!pNode->Flags.b.Point && !pNode->Flags.b.Line && !pNode->Flags.b.Poly ) MessageBox(NULL, "BAD SURFACE","ENCODER", NULL);

	switch(pNode->Type)
	{
		case TRIANGLE:
			dxSurfaceNode.dwPrimType = D3DPT_TRIANGLELIST;
		break;

		case LINE:
			dxSurfaceNode.dwPrimType = D3DPT_LINELIST;
		break;

		case DOT:
			dxSurfaceNode.dwPrimType = D3DPT_POINTLIST;
		break;

		default	:MessageBox(NULL, "UNRECOGNIZED PRIMITIVE TYPE!!!", "ENCODER", 0);
	}
	
	// Set vertex format using DX Flexible Vertex Format flags
//	dxSurfaceNode.dwVFormat = D3DFVF_XYZ|D3DFVF_NORMAL;

	// Set dimension of each vertex item
	if(pNode->Flags.b.Texture)
	{
//		dxSurfaceNode.dwVFormat |= D3DFVF_TEX1;
		//dxSurfaceNode.dwStride = 32;   // 32 bytes if Texture Node (tu and tv texture coordinate)
	}
	else if(pNode->Flags.b.VColor)
	{
//		dxSurfaceNode.dwVFormat |= D3DFVF_DIFFUSE;
		//dxSurfaceNode.dwStride = 28;            // else 28 bytes (only VColour component)
	}

	// Assign new zBias
	dxSurfaceNode.dwzBias=pNode->dwzBias;

	// Set vertex dimension
	dxSurfaceNode.dwStride = sizeof(D3DVERTEXEX);
	// Save pointer to this SURFACE node
	//m_pLastSurfaceNode = m_pBuffer + m_dwBufferOffset;

	// Set default material
	dxSurfaceNode.SpecularIndex=DEFAULT_MAT_DVPOWER;

	// Set the Texture
	if(pNode->Flags.b.Texture) dxSurfaceNode.TexID[0]=Texture;
	else dxSurfaceNode.TexID[0]=-1;


	// Save pointer to NodeSize field and to TotalVertexes number
	m_pdwSurfaceNodeSizeField      = (DWORD*)(m_pBuffer + m_dwBufferOffset);
	m_pdwSurfaceTotalVertexesField = (DWORD*)(m_pBuffer + m_dwBufferOffset + 16);




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
	dxSlotNode.SlotNr=pNode->SlotNr;
	dxSlotNode.rotation=pNode->Transform;
	

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

void Encoder::CopyVertex(D3DVERTEX *pSourceVertex, DWORD dwSourceColor, D3DVERTEXEX *pDestinationVertex)
{
	ASSERT(pSourceVertex != NULL);
	ASSERT(pDestinationVertex != NULL);

	pDestinationVertex->vx = pSourceVertex->x;
	pDestinationVertex->vy = pSourceVertex->y;
	pDestinationVertex->vz = pSourceVertex->z;
	pDestinationVertex->nx = pSourceVertex->nx;
	pDestinationVertex->ny = pSourceVertex->ny;
	pDestinationVertex->nz = pSourceVertex->nz;
	pDestinationVertex->dwColour = dwSourceColor;
	pDestinationVertex->dwSpecular = 0x000000;
	pDestinationVertex->tu = pSourceVertex->tu;
	pDestinationVertex->tv = pSourceVertex->tv;

	// NO SPECULARITY Vertex Feature 
	if(!m_pCurrentNode->Flags.b.Gouraud  ){
			pDestinationVertex->dwSpecular=0x00000000;
	}

	// Glowing vertex feature - This corrects glowing surfaces with no shading assigned
	// SUSPENDED...!!!! HAS TO BE SOLVED USING CLASS - the SPECULARITY is assigned only to ALPHA SURFACES
	if(!m_pCurrentNode->Flags.b.Lite /*&& m_pCurrentNode->Flags.b.Alpha*/){
		if(m_pCurrentNode->Flags.b.Texture){
			// if Textured surface assign Max Light
			pDestinationVertex->dwSpecular=0xfffffff;
		}
		if(m_pCurrentNode->Flags.b.VColor){
			// if Solid surface keep only Alpha value
			pDestinationVertex->dwSpecular=dwSourceColor;
			pDestinationVertex->dwColour &= 0xFF000000;
		}
	}


	// HACK to avoid lines & points shimmering at nite
	//if((m_pCurrentNode->Flags.b.Line || m_pCurrentNode->Flags.b.Point) && m_pCurrentNode->Flags.b.Lite )
	//		pDestinationVertex->dwSpecular=DEFAULT_MAT_LOSPECULAR;
}

Int16 Encoder::AddVertexToPool(D3DVERTEXEX *pVertex, bool Indexed)
{
	BYTE *pCurrentVertex = m_pVertexesPool;
	Int16 iVertexIndex = -1;

	// COBRA - RED - Only for Indexed vertices ( e.g not for point lists )
	if(Indexed){
		for(int i=0; i<m_dwVertexesNumber; i++)
		{
			if(IsSameVertexes((D3DVERTEXEX*)pCurrentVertex, pVertex))
			{
				iVertexIndex = i;
				break;
			}
			pCurrentVertex += sizeof(D3DVERTEXEX);
		}
	}

	if(iVertexIndex == -1)
	{
		iVertexIndex = m_dwVertexesNumber;
		memcpy((void*)(m_pVertexesPool+iVertexIndex*sizeof(D3DVERTEXEX)), (const void*)pVertex, sizeof(D3DVERTEXEX));
		m_dwVertexesNumber++;
		m_dwVertexesPoolSize += sizeof(D3DVERTEXEX);
	}

	// Increment current DOF vertexes counter
	/*if(m_dwDOF > 0)
		m_arDOFPointers[m_dwDOF-1]->dwDOFVertices++;*/



	// Incrementent total vertexes and indexes counter
	(*m_pdwSurfaceNodeSizeField) += sizeof(Int16);
	(*m_pdwSurfaceTotalVertexesField)++;
	//m_dwTotalSurfaceVertexes++;

	return iVertexIndex;
}

