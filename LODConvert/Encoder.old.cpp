#include	"stdafx.h"
#include	"BSPNodes.h"
#include	"DXnodes.h"
#include	"Encoder.h"
#include	"d3d9types.h"

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
	m_dwTotalSurfaceVertexes = 0;
	m_dwVertexesPoolSize = 0;
	m_dwVertexesNumber   = 0;
	m_dwDOF              = 0;
}

Encoder::~Encoder()
{
	if(m_pBuffer)
		delete[] m_pBuffer;

	if(m_pVertexesPool)
		delete[] m_pVertexesPool;
}

void Encoder::WriteBuffer(const void *pSrc, size_t count)
{
	memcpy((void*)(m_pBuffer+m_dwBufferOffset), pSrc, count);
	m_dwBufferOffset += count;
}

BOOL Encoder::Initialize(Converter *pConverter)
{
	BOOL bRet = TRUE;

	// Allocate buffer
	m_pBuffer = new BYTE[BUFFER_SIZE];

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
			WriteBuffer((void*)&pConverter->NTextures, sizeof(pConverter->NTextures));
			WriteBuffer((void*)&pConverter->TextureList, pConverter->NTextures*sizeof(DWORD));
			// Save header pointer
			m_pHeader = m_pBuffer + m_dwBufferOffset;
			// Write header data
			DWORD dwNodesNr = 0;
			WriteBuffer((void*)&dwNodesNr, sizeof(DWORD));						// Number of nodes
			//memset((void*)(m_pBuffer+m_dwBufferOffset), -1, 8*sizeof(DWORD));	// Set other flags to -1
			//m_dwBufferOffset += 8*sizeof(DWORD);
			memset((void*)(m_pBuffer+m_dwBufferOffset), 0, 3*sizeof(DWORD));	// Set other flags to -1
			m_dwBufferOffset += 3*sizeof(DWORD);
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

	while(pCurrentNode->Next != NULL)
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

void Encoder::Encode()
{
	BOOL bInsertSurfaceNode = FALSE;
	BOOL bInsertTextureNode = FALSE;
	m_pCurrentNode = m_pRootNode;

	while(m_pCurrentNode != NULL)
	{
		m_pPreviousNode = m_pCurrentNode->Prev;
		//m_pNextNode     = m_pCurrentNode->Next;

		if(m_pCurrentNode == m_pRootNode)
		{
			// Root node, insert Texture and Surface node
			if(m_pCurrentNode->Flags.b.Texture)
				WriteTextureNode(m_pCurrentNode);
			WriteSurfaceNode(m_pCurrentNode);
		}
		else
		{
			// Check for node property changes
			if(m_pCurrentNode->Type != DOF && m_pCurrentNode->Type != CLOSEDOF)
			{
				if(m_pPreviousNode->Type == DOF || m_pPreviousNode->Type == CLOSEDOF)
				{
					if(m_pCurrentNode->Flags.b.Texture)
						WriteTextureNode(m_pCurrentNode);
					WriteSurfaceNode(m_pCurrentNode);
				}
				else
				{
					if(m_pCurrentNode->Flags.b.Texture)
					{
						if(m_pCurrentNode->Texture != m_pPreviousNode->Texture)
						{
							// Texture of current node differs from previous node, we've to insert a TEXTURE node before new SURFACE node
							WriteTextureNode(m_pCurrentNode);
							WriteSurfaceNode(m_pCurrentNode);
						}
						else
						{
							// Check if other fields are changed (in this case we've to insert a new SURFACE node
							if( m_pCurrentNode->Flags.b.ALpha   != m_pPreviousNode->Flags.b.ALpha   ||
								m_pCurrentNode->Flags.b.Gouraud != m_pPreviousNode->Flags.b.Gouraud ||
								m_pCurrentNode->Flags.b.Lite    != m_pPreviousNode->Flags.b.Lite    ||
								m_pCurrentNode->Type            != m_pPreviousNode->Type )
								WriteSurfaceNode(m_pCurrentNode);
						}
					}
					else if(m_pCurrentNode->Flags.b.Solid || m_pCurrentNode->Flags.b.VColor)
					{
						// Colour node, check only for Alpha changes
						if( m_pCurrentNode->Flags.b.ALpha != m_pPreviousNode->Flags.b.ALpha ||
							m_pCurrentNode->Type != m_pPreviousNode->Type )
							WriteSurfaceNode(m_pCurrentNode);
					}
				}
			}
		}

		switch(m_pCurrentNode->Type)
		{
			case DOT:
			{
				DXVertex *pVertex = (DXVertex*)m_pCurrentNode;
				D3DVERTEXEX dxVertex;
				InitializeVertex(&dxVertex);

				memcpy((void*)&dxVertex, (const void*)&pVertex->Vertex, sizeof(pVertex->Vertex));
				memcpy((void*)(&dxVertex + sizeof(pVertex->Vertex)), (const void*)&pVertex->VColor, sizeof(pVertex->VColor));

				Int16 iIndex = AddVertexToPool(&dxVertex);
				WriteBuffer((const void*)&iIndex, sizeof(iIndex));

				//if(m_pCurrentNode->Flags.b.Texture)
				//{
				//	// Write all D3DVertex information
				//	WriteBuffer((void*)&pVertex->Vertex, sizeof(pVertex->Vertex));
				//}
				//else
				//{
				//	// In this case, write only x,y,z,nx,ny,nz (24 bytes total) and VColour
				//	WriteBuffer((void*)&pVertex->Vertex, 24);
				//	WriteBuffer((void*)&pVertex->VColor, sizeof(pVertex->VColor));
				//}
				//m_dwTotalSurfaceVertexes++;
			}
			break;

			case LINE:
			{
				DXLine *pLine = (DXLine*)m_pCurrentNode;

				for(int i=0; i<2; i++)
				{
					D3DVERTEXEX dxVertex;
					InitializeVertex(&dxVertex);

					memcpy((void*)&dxVertex, (const void*)&pLine->Vertex[i], sizeof(pLine->Vertex[i]));
					memcpy((void*)(&dxVertex + sizeof(pLine->Vertex[i])), (const void*)&pLine->VColor[i], sizeof(pLine->VColor[0]));

					Int16 iIndex = AddVertexToPool(&dxVertex);
					WriteBuffer((const void*)&iIndex, sizeof(iIndex));
				}

				//if(m_pCurrentNode->Flags.b.Texture)
				//{
				//	// Write all D3DVertex information
				//	WriteBuffer((void*)&pLine->Vertex, sizeof(pLine->Vertex));
				//}
				//else
				//{
				//	// In this case, write only x,y,z,nx,ny,nz (24 bytes total) and VColour
				//	for(int i=0;i<2;i++)
				//	{
				//		WriteBuffer((void*)&pLine->Vertex[i], 24);
				//		WriteBuffer((void*)&pLine->VColor[i], sizeof(pLine->VColor[0]));
				//	}
				//}
				//m_dwTotalSurfaceVertexes += 2;
			}
			break;

			case TRIANGLE:
			{
				DXTriangle *pTriangle = (DXTriangle*)m_pCurrentNode;

				for(int i=0; i<3; i++)
				{
					D3DVERTEXEX dxVertex;
					InitializeVertex(&dxVertex);
					BYTE *pVertex = (BYTE*)&dxVertex;

					memcpy((void*)&dxVertex, (const void*)&pTriangle->Vertex[i], sizeof(pTriangle->Vertex[i]));
					memcpy((void*)(pVertex + sizeof(pTriangle->Vertex[i])), (const void*)&pTriangle->VColor[i], sizeof(pTriangle->VColor[0]));

					Int16 iIndex = AddVertexToPool(&dxVertex);
					WriteBuffer((const void*)&iIndex, sizeof(iIndex));
				}

				//if(m_pCurrentNode->Flags.b.Texture)
				//{
				//	// Write all D3DVertex information
				//	WriteBuffer((void*)&pTriangle->Vertex, sizeof(pTriangle->Vertex));
				//}
				//else
				//{
				//	// In this case, write only x,y,z,nx,ny,nz (24 bytes total) and VColour
				//	for(int i=0;i<3;i++)
				//	{
				//		WriteBuffer((void*)&pTriangle->Vertex[i], 24);
				//		WriteBuffer((void*)&pTriangle->VColor[i], sizeof(pTriangle->VColor[0]));
				//	}
				//}
				//m_dwTotalSurfaceVertexes += 3;
			}
			break;

			case DOF:
			{
				// We've to insert a new DOF node
				DXDof *pDof = (DXDof*)m_pCurrentNode;
				WriteDofNode(pDof);
			}
			break;

			case CLOSEDOF:
			{
				// We've to insert a new END DOF node
				WriteDofEndNode();
			}
			break;
		}
		// Update pointers
		//m_pPreviousNode = m_pCurrentNode;
		m_pCurrentNode  = m_pCurrentNode->Next;
		//m_pNextNode     = m_pCurrentNode != NULL ? m_pCurrentNode->Next : NULL;
	}

	// Set last node total size (Node + Data (total indexes size))
	DWORD dwTotalNodeSize = (m_dwTotalSurfaceVertexes*sizeof(Int16) + sizeof(DxSurfaceType));
	memcpy((void*)m_pLastSurfaceNode, (const void*)&dwTotalNodeSize, sizeof(dwTotalNodeSize));
	// Add last node Vertex Counter to last surface node
	memcpy(m_pLastSurfaceNode + 12, (const void*)&m_dwTotalSurfaceVertexes, sizeof(m_dwTotalSurfaceVertexes));
	// Write ENDMODEL node
	DxEndModelType dxEndModelNode;
	dxEndModelNode.h.dwNodeSize = sizeof(DxEndModelType);
	dxEndModelNode.h.Type = DX_MODELEND;
 	WriteBuffer((void*)&dxEndModelNode, sizeof(dxEndModelNode));
	// Write total number of nodes
	memcpy((void*)m_pHeader, (const void*)&m_dwTotalNodeCount, sizeof(m_dwTotalNodeCount));
	// Write other header fields
	//DWORD dwVertexesPoolStart = (DWORD)(m_pBuffer + m_dwBufferOffset);
	memcpy((void*)(m_pHeader+4), (const void*)&m_dwBufferOffset, sizeof(DWORD));
	memcpy((void*)(m_pHeader+8), (const void*)&m_dwVertexesPoolSize, sizeof(DWORD));
	memcpy((void*)(m_pHeader+12), (const void*)&m_dwVertexesNumber, sizeof(DWORD));
	// Copy vertexes pool after nodes section
	WriteBuffer((const void*)m_pVertexesPool, m_dwVertexesPoolSize);
	// Write file
	WriteFile();
}

void Encoder::WriteTextureNode(DXNode *pNode)
{
	// Define new node to insert
	DxTextureType dxTextureNode;
	memset((void*)&dxTextureNode, 0, sizeof(dxTextureNode));
	dxTextureNode.h.dwNodeSize = sizeof(DxTextureType);
	dxTextureNode.h.Type       = DX_TEXTURE;
	dxTextureNode.Tex[0] = pNode->Texture;
	for(int i=1;i<4;i++)
		dxTextureNode.Tex[i] = -1;
	WriteBuffer((void*)&dxTextureNode, sizeof(dxTextureNode));
	// Increment total node counter
	m_dwTotalNodeCount++;
}

void Encoder::WriteSurfaceNode(DXNode *pNode)
{
	if(m_pLastSurfaceNode)
	{
		// Set node total size (Node + Data (total indexes size))
		DWORD dwTotalNodeSize = (m_dwTotalSurfaceVertexes*sizeof(Int16) + sizeof(DxSurfaceType));
		memcpy((void*)m_pLastSurfaceNode, (const void*)&dwTotalNodeSize, sizeof(dwTotalNodeSize));
		// Set previous SURFACE node vertex counter and reset it (12 is the offset of dwVCount field)
		memcpy((void*)(m_pLastSurfaceNode + 12), (const void*)&m_dwTotalSurfaceVertexes, sizeof(m_dwTotalSurfaceVertexes));
	}
	m_dwTotalSurfaceVertexes = 0;
	// Define new node to insert
	DxSurfaceType dxSurfaceNode;
	// Reset structure
	memset((void*)&dxSurfaceNode, 0, sizeof(dxSurfaceNode));
	// Set common data
	dxSurfaceNode.h.dwNodeSize = sizeof(DxSurfaceType);
	dxSurfaceNode.h.Type       = DX_SURFACE;
	// Set Flags
	dxSurfaceNode.dwFlags = pNode->Flags;
	// Based on node type, set DX primitive type
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
	}
	
	// Set vertex format using DX Flexible Vertex Format flags
	dxSurfaceNode.dwVFormat = D3DFVF_XYZ|D3DFVF_NORMAL;

	// Set dimension of each vertex item
	if(pNode->Flags.b.Texture)
	{
		dxSurfaceNode.dwVFormat |= D3DFVF_TEX1;
		//dxSurfaceNode.dwStride = 32;   // 32 bytes if Texture Node (tu and tv texture coordinate)
	}
	else if(pNode->Flags.b.Solid || pNode->Flags.b.VColor)
	{
		dxSurfaceNode.dwVFormat |= D3DFVF_DIFFUSE;
		//dxSurfaceNode.dwStride = 28;            // else 28 bytes (only VColour component)
	}

	// Set vertex dimension
	dxSurfaceNode.dwStride = sizeof(D3DVERTEXEX);
	// Save pointer to this SURFACE node
	m_pLastSurfaceNode = m_pBuffer + m_dwBufferOffset;
	WriteBuffer((void*)&dxSurfaceNode, sizeof(dxSurfaceNode));
	// Increment total node counter
	m_dwTotalNodeCount++;
}

void Encoder::WriteDofNode(DXDof *pNode)
{
	DxDofType dxDOFNode;
	// Reset structure
	memset((void*)&dxDOFNode, 0, sizeof(dxDOFNode));
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
	// Write data to buffer
	WriteBuffer((void*)&dxDOFNode, sizeof(dxDOFNode));
	// Increment total node counter
	m_dwTotalNodeCount++;
}

void Encoder::WriteDofEndNode()
{
	DxDofEndType dxDofEndNode;
	// Reset structure
	memset((void*)&dxDofEndNode, 0, sizeof(dxDofEndNode));
	// Set common data
	dxDofEndNode.h.dwNodeSize = sizeof(DxDofEndType);
	dxDofEndNode.h.Type       = DX_ENDDOF;
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
	pVertex->tu = -1;
	pVertex->tv = -1;
	pVertex->dwColour = -1;
}
BOOL Encoder::IsSameVertexes(D3DVERTEXEX *pVertex1, D3DVERTEXEX *pVertex2)
{
	return (
			pVertex1->nx == pVertex2->nx &&
			pVertex1->ny == pVertex2->ny &&
			pVertex1->nz == pVertex2->nz &&
			pVertex1->vx == pVertex2->vx &&
			pVertex1->vy == pVertex2->vy &&
			pVertex1->vz == pVertex2->vz &&
			pVertex1->tu == pVertex2->tu &&
			pVertex1->tv == pVertex2->tv &&
			pVertex1->dwColour == pVertex2->dwColour
			);
}

Int16 Encoder::AddVertexToPool(D3DVERTEXEX *pVertex)
{
	BYTE *pCurrentVertex = m_pVertexesPool;
	Int16 iVertexIndex = -1;

	// * COBRA - RED - * Vertex Transpositions fo DX Left Handed Engine
	float	temp;
	temp=pVertex->vy;
	pVertex->vy=-pVertex->vz;
	pVertex->vz=-temp;
	temp=pVertex->ny;
	pVertex->ny=-pVertex->nz;
	pVertex->nz=-temp;

	temp=pVertex->tv;
	pVertex->tv=pVertex->tu;
	pVertex->tu=*(float*)&pVertex->dwColour;
	*(float*)&pVertex->dwColour=temp;

	for(int i=0; i<m_dwVertexesNumber; i++)
	{
		if(IsSameVertexes((D3DVERTEXEX*)pCurrentVertex, pVertex))
		{
			iVertexIndex = i;
			break;
		}
		pCurrentVertex += sizeof(D3DVERTEXEX);
	}

	if(iVertexIndex == -1)
	{
		iVertexIndex = i;
		memcpy((void*)(m_pVertexesPool+iVertexIndex*sizeof(D3DVERTEXEX)), (const void*)pVertex, sizeof(D3DVERTEXEX));
		m_dwVertexesNumber++;
		m_dwVertexesPoolSize += sizeof(D3DVERTEXEX);
	}

	// Increment current DOF vertexes counter
	if(m_dwDOF > 0)
		m_arDOFPointers[m_dwDOF-1]->dwDOFVertices++;

	// Incrementent total vertexes and indexes counter
	m_dwTotalSurfaceVertexes++;
	//m_dwTotalSurfaceIndexes++;

	return iVertexIndex;
}

