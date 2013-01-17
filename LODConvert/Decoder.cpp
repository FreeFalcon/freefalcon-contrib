#include "StdAfx.h"
#include ".\decoder.h"
#include "Encoder.h"

Decoder::Decoder()
{
	m_pBuffer = NULL;
	m_pVertexesPool = NULL;
	m_pDXLTree = NULL;
	m_uiBufferSize = 0;
	m_uiOffset     = 0;
	m_dwTotalDOFs  = 0;
	m_dwTotalSurface = 0;
	m_dwTotalTexture = 0;
	m_dwTotalVertexes = 0;
	m_dwFileSize      = 0;
}

Decoder::~Decoder(void)
{
	if(m_pBuffer != NULL)
		delete[] m_pBuffer;
}

BOOL Decoder::Initialize(CString strFileName, CTreeCtrlEx *pDXLTree)
{
	m_pDXLTree = pDXLTree;
	m_strFileName = strFileName;

	// Try to load file
	CFile cFile(m_strFileName, CFile::modeRead);
	
	if(cFile.m_hFile != CFile::hFileNull)
	{
		m_uiBufferSize = cFile.GetLength();
		m_pBuffer = new BYTE[m_uiBufferSize];
		
		if(m_pBuffer == NULL)
			return FALSE;
		else
			cFile.Read((void*)m_pBuffer, m_uiBufferSize);
	}
	else
	{
		AfxMessageBox(_T("Error opening file"));
		return FALSE;
	}
}

void Decoder::ReadBuffer(UINT uiBytesToRead, void *pBuffer)
{
	memcpy(pBuffer, (const void*)(m_pBuffer+m_uiOffset), uiBytesToRead);
	m_uiOffset += uiBytesToRead;
}

void Decoder::Decode(DWORD& dwTotalSurfaceNodes, DWORD& dwTotalDOFNodes, DWORD& dwTotalTextureNodes)
{
	HTREEITEM hRootItem = m_pDXLTree->GetRootItem();
	HTREEITEM hItem = NULL;

	UINT uiOffset = 0;
	CString strLog, strFormat;
	strLog += _T("Starting decode file ") + m_strFileName + _T("\r\n");
	strLog += _T("--------------------------------");
	
	// Read texture information
	DWORD dwTextureNr = 0, dwItem;
	ReadBuffer(4, (void*)&dwTextureNr);
	
	hItem = m_pDXLTree->InsertItem(_T("Header Texture Section"), 1, 1, hRootItem);
	m_pDXLTree->SetItemFgColor(hItem, RGB(75, 0, 130));
	m_pDXLTree->SetItemBold(hItem, FW_BOLD);

	strLog += _T("\r\n***** TEXTURE SECTION *****\r\n");
	strFormat.Format(_T("Number of Texture: %d\r\n"), dwTextureNr);
	strLog += strFormat;

	m_pDXLTree->InsertItem(strFormat, 2, 2, hItem);
	
	strLog += _T("Texture list:\r\n");
	hItem = m_pDXLTree->InsertItem(_T("Texture List"), 6, 6, hItem);
	
	for(DWORD dw=0; dw<dwTextureNr; dw++)
	{
		ReadBuffer(4, (void*)&dwItem);
		strFormat.Format(_T("Texture[%d] -> %d\r\n"), dw, dwItem);
		strLog += strFormat;
		m_pDXLTree->InsertItem(strFormat, 8, 8, hItem);
	}

	hItem = m_pDXLTree->InsertItem(_T("Header Section"), 1, 1, hRootItem);
	m_pDXLTree->SetItemFgColor(hItem, RGB(75, 0, 130));
	m_pDXLTree->SetItemBold(hItem, FW_BOLD);

	strLog += _T("\r\n***** HEADER SECTION *****\r\n");
	DWORD dwNodesNr = 0;
	ReadBuffer(4, (void*)&dwNodesNr);
	strFormat.Format(_T("Total number of nodes: %d\r\n"), dwNodesNr);
	strLog += strFormat;
	
	m_pDXLTree->InsertItem(strFormat, 6, 6, hItem);
	
	/*strLog += _T("Other fields value: ");
	hItem = m_pDXLTree->InsertItem(_T("Other fields value: "), 0, 0, hItem);*/

	//strLog += _T("Offset of vertexes pool: ");
	//m_pDXLTree->InsertItem(_T("Offset of vertexes pool: "), 0, 0, hItem);
	ReadBuffer(4, (void*)&dwItem);
	strFormat.Format(_T("Offset of vertexes pool: 0x%x\r\n"), dwItem);
	strLog += strFormat;
	m_pDXLTree->InsertItem(strFormat, 6, 6, hItem);

	// Initialize vertexes pool pointer
	m_pVertexesPool = m_pBuffer + dwItem;

	//strLog += _T("Size of vertexes pool: ");
	//m_pDXLTree->InsertItem(_T("Size of vertexes pool: "), 0, 0, hItem);
	ReadBuffer(4, (void*)&dwItem);
	strFormat.Format(_T("Size of vertexes pool: %d bytes\r\n"), dwItem);
	strLog += strFormat;
	m_pDXLTree->InsertItem(strFormat, 6, 6, hItem);

	//strLog += _T("Number of vertexes present in pool: ");
	//m_pDXLTree->InsertItem(_T("Number of vertexes present in pool: \r\n"), 0, 0, hItem);
	ReadBuffer(4, (void*)&dwItem);
	strFormat.Format(_T("Number of vertexes present in pool: %d"), dwItem);
	strLog += strFormat;
	m_pDXLTree->InsertItem(strFormat, 6, 6, hItem);

	/*for(dw = 0; dw<3; dw++)
	{
		ReadBuffer(4, (void*)&dwItem);
		strFormat.Format(_T(" %d"), dwItem);
		strLog += strFormat;
		m_pDXLTree->InsertItem(strFormat, 0, 0, hItem);
	}*/

	strLog += _T("\r\n");
	strLog += _T("\r\n***** NODES SECTION *****\r\n");

	HTREEITEM hNodeRootItem = m_pDXLTree->InsertItem(_T("Nodes Section"), 7, 7, hRootItem);
	m_pDXLTree->SetItemFgColor(hNodeRootItem, RGB(75, 0, 130));
	m_pDXLTree->SetItemBold(hNodeRootItem, FW_BOLD);

	for(dw = 0; dw < dwNodesNr; dw++)
	{
		DWORD dwNodeType;
		memcpy((void*)&dwNodeType, (const void*)(m_pBuffer+m_uiOffset+4), 4);
		switch(dwNodeType)
		{
			case DX_TEXTURE:
			{
				HTREEITEM hParentItem = m_arDOFItems.GetSize() > 0 ? m_arDOFItems.GetAt(m_arDOFItems.GetSize()-1) : hNodeRootItem;

				hItem = m_pDXLTree->InsertItem(_T("Texture Node"), 2, 2, hParentItem);
				m_pDXLTree->SetItemFgColor(hItem, RGB(139, 0, 0));
				m_pDXLTree->SetItemBold(hItem, FW_BOLD);

				strLog += _T("\r\nTexture Node\r\n");
				
				DxTextureType dxTexture;
				ReadBuffer(sizeof(DxTextureType), (void*)&dxTexture);
				
				for(int iTex=0;iTex<(sizeof(dxTexture.Tex)/sizeof(dxTexture.Tex[0]));iTex++)
				{
					strFormat.Format(_T("Texture %d: %d\r\n"), iTex+1, dxTexture.Tex[iTex]);
					strLog += strFormat;
					m_pDXLTree->InsertItem(strFormat, 8, 8, hItem);
				}

				m_dwTotalTexture++;
			}
			break;

			case DX_SURFACE:
			{
				HTREEITEM hParentItem = m_arDOFItems.GetSize() > 0 ? m_arDOFItems.GetAt(m_arDOFItems.GetSize()-1) : hNodeRootItem;
				
				hItem = m_pDXLTree->InsertItem(_T("Surface Node"), 3, 3, /*hNodeRootItem*/ hParentItem);
				m_pDXLTree->SetItemFgColor(hItem, RGB(139, 0, 0));
				m_pDXLTree->SetItemBold(hItem, FW_BOLD);

				strFormat.Format(_T("\r\nSurface Node\r\n"), dw);
				strLog += strFormat;
				DxSurfaceType dxSurface;
				memset((void*)&dxSurface, 0, sizeof(dxSurface));
				ReadBuffer(sizeof(DxSurfaceType), (void*)&dxSurface);
				strFormat.Format(_T("Number of vertices of Surface: %d\r\n"), dxSurface.dwVCount);
				strLog += strFormat;

				m_pDXLTree->InsertItem(strFormat, 0, 0, hItem);
				
				switch(dxSurface.dwPrimType)
				{
					case D3DPT_TRIANGLELIST:
						strLog += _T("\tVertexes type: Triangle\r\n");
						m_pDXLTree->InsertItem(_T("Vertexes type: Triangle"), 6, 6, hItem);
					break;

					case D3DPT_LINELIST:
						strLog += _T("\tVertexes type: Line\r\n");
						m_pDXLTree->InsertItem(_T("Vertexes type: Line"), 6, 6, hItem);
					break;

					case D3DPT_POINTLIST:
						strLog += _T("\tVertexes type: Point\r\n");
						m_pDXLTree->InsertItem(_T("Vertexes type: Point"), 6, 6, hItem);
					break;
				}
				
				HTREEITEM hFlags = m_pDXLTree->InsertItem(_T("Flags"), 6, 6, hItem);
				strFormat.Format(_T("Alpha: %d"), dxSurface.dwFlags.b.ALpha);
				m_pDXLTree->InsertItem(strFormat, 8, 8, hFlags);
				strFormat.Format(_T("Gouraud: %d"), dxSurface.dwFlags.b.Gouraud);
				m_pDXLTree->InsertItem(strFormat, 8, 8, hFlags);
				strFormat.Format(_T("Line: %d"), dxSurface.dwFlags.b.Line);
				m_pDXLTree->InsertItem(strFormat, 8, 8, hFlags);
				strFormat.Format(_T("Lite: %d"), dxSurface.dwFlags.b.Lite);
				m_pDXLTree->InsertItem(strFormat, 8, 8, hFlags);
				strFormat.Format(_T("Point: %d"), dxSurface.dwFlags.b.Point);
				m_pDXLTree->InsertItem(strFormat, 8, 8, hFlags);
				strFormat.Format(_T("Poly: %d"), dxSurface.dwFlags.b.Poly);
				m_pDXLTree->InsertItem(strFormat, 8, 8, hFlags);
				strFormat.Format(_T("Solid: %d"), dxSurface.dwFlags.b.Solid);
				m_pDXLTree->InsertItem(strFormat, 8, 8, hFlags);
				strFormat.Format(_T("Texture: %d"), dxSurface.dwFlags.b.Texture);
				m_pDXLTree->InsertItem(strFormat, 8, 8, hFlags);
				strFormat.Format(_T("VColor: %d"), dxSurface.dwFlags.b.VColor);
				m_pDXLTree->InsertItem(strFormat, 8, 8, hFlags);

				strFormat.Format(_T("Vertex Format: %s"), (dxSurface.dwVFormat & D3DFVF_TEX1) ? _T("D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1") : _T("D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE"));
				strLog += _T("\tOther datas follow...\r\n");
				strLog += _T("\tVertex list:\r\n");
				
				HTREEITEM hRootVertexItem = hItem = m_pDXLTree->InsertItem(_T("Vertex list"), 6, 6, hItem);
				
				for(int iVertexIdx=0;iVertexIdx < dxSurface.dwVCount;iVertexIdx++)
				{
					CString strTemp, strFormat2;
					D3DVERTEXEX dxVertex;
					Int16 iVertexPoolIndex = -1;
					memset((void*)&dxVertex, 0, sizeof(dxVertex));
					ReadBuffer(sizeof(Int16), (void*)&iVertexPoolIndex);
					memcpy((void*)&dxVertex, (const void*)(m_pVertexesPool+iVertexPoolIndex*sizeof(D3DVERTEXEX)), dxSurface.dwStride);
					
					strFormat.Format(_T("Vertex %d: (x=%f, y=%f, z=%f), (nx=%f, ny=%f, nz=%f)"), iVertexIdx + 1, dxVertex.vx, dxVertex.vy, dxVertex.vz, dxVertex.nx, dxVertex.ny, dxVertex.nz);
					strTemp += strFormat;
					
					strFormat.Format(_T("Vertex %d"), iVertexIdx+1);
					hItem = m_pDXLTree->InsertItem(strFormat, 8, 8, hRootVertexItem);
					strFormat.Format(_T("x = %.3f"), dxVertex.vx);
					m_pDXLTree->InsertItem(strFormat, 8, 8, hItem);
					strFormat.Format(_T("y = %.3f"), dxVertex.vy);
					m_pDXLTree->InsertItem(strFormat, 8, 8, hItem);
					strFormat.Format(_T("z = %.3f"), dxVertex.vz);
					m_pDXLTree->InsertItem(strFormat, 8, 8, hItem);
					strFormat.Format(_T("nx = %.3f"), dxVertex.nx);
					m_pDXLTree->InsertItem(strFormat, 8, 8, hItem);
					strFormat.Format(_T("ny = %.3f"), dxVertex.ny);
					m_pDXLTree->InsertItem(strFormat, 8, 8, hItem);
					strFormat.Format(_T("nz = %.3f"), dxVertex.nz);
					m_pDXLTree->InsertItem(strFormat, 8, 8, hItem);
					strFormat.Format(_T("nz = %.3f"), dxVertex.nz);
					m_pDXLTree->InsertItem(strFormat, 8, 8, hItem);
					strFormat.Format(_T("tu = %.3f"), dxVertex.tu);
					m_pDXLTree->InsertItem(strFormat, 8, 8, hItem);
					strFormat.Format(_T("tv = %.3f"), dxVertex.tv);
					m_pDXLTree->InsertItem(strFormat, 8, 8, hItem);
					strFormat.Format(_T("VColour = %d (RGB = (%d, %d, %d))"), dxVertex.dwColour, GetRValue(dxVertex.dwColour), GetGValue(dxVertex.dwColour), GetBValue(dxVertex.dwColour));
					m_pDXLTree->InsertItem(strFormat, 8, 8, hItem);

					/*if(dxSurface.dwStride == 28)
					{
						strFormat.Format(_T(", (VColour = %d)\r\n"), ((BYTE*)&dxVertex)+24);
						strFormat2.Format(_T("VColour = %d"), ((BYTE*)&dxVertex)+24);
						m_pDXLTree->InsertItem(strFormat2, 0, 0, hItem);
					}
					else
					{
						strFormat.Format(_T(", (tu=%f, tv=%f)\r\n"), dxVertex.tu, dxVertex.tv);
						strFormat2.Format(_T("tu = %.3f"), dxVertex.tu);
						m_pDXLTree->InsertItem(strFormat2, 0, 0, hItem);
						strFormat2.Format(_T("tv = %.3f"), dxVertex.tv);
						m_pDXLTree->InsertItem(strFormat2, 0, 0, hItem);
					}*/
					strLog += strTemp + strFormat;
					
					//m_pDXLTree->InsertItem(strTemp + strFormat, 0, 0, hItem);
				}

				m_dwTotalVertexes += dxSurface.dwVCount;
				m_dwTotalSurface++;
			}
			break;

			case DX_DOF:
			{
				HTREEITEM hParentItem = m_arDOFItems.GetSize() > 0 ? m_arDOFItems.GetAt(m_arDOFItems.GetSize()-1) : hNodeRootItem;

				DxDofType dxDOF;
				ReadBuffer(sizeof(DxDofType), (void*)&dxDOF);

				strFormat.Format(_T("\r\nDOF Node\r\n"), dw);
				strLog += strFormat;
				hItem = m_pDXLTree->InsertItem(_T("DOF Node"), 4, 4, hParentItem);
				m_pDXLTree->SetItemFgColor(hItem, RGB(139, 0, 0));
				m_pDXLTree->SetItemBold(hItem, FW_BOLD);
				
				m_arDOFItems.Add(hItem);

				strFormat.Format(_T("Number of vertexes of this DOF: %d\r\n"), dxDOF.dwDOFVertices);
				strLog += strFormat;

				m_pDXLTree->InsertItem(strFormat, 6, 6, hItem);
				m_dwTotalDOFs++;
				//m_uiOffset += sizeof(DxDofType);
			}
			break;

			case DX_ENDDOF:
			{
				HTREEITEM hParentItem = m_arDOFItems.GetSize() > 0 ? m_pDXLTree->GetParentItem(m_arDOFItems.GetAt(m_arDOFItems.GetSize()-1)) : hNodeRootItem;
				strFormat.Format(_T("\r\nEndDOF Node\r\n"), dw);
				strLog += strFormat;
				hItem = m_pDXLTree->InsertItem(_T("ENDDOF Node"), 5, 5, hParentItem);
				m_pDXLTree->SetItemFgColor(hItem, RGB(139, 0, 0));
				m_pDXLTree->SetItemBold(hItem, FW_BOLD);
				
				m_arDOFItems.RemoveAt(m_arDOFItems.GetSize()-1);
				m_uiOffset += sizeof(DxDofEndType);
			}
			break;
		}
	}
	CFile cFile;
	if(!cFile.Open(_T("c:\\decoder_log.txt"), CFile::modeCreate|CFile::modeWrite))
		AfxMessageBox(_T("Error creating log file"));
	else
	{
		cFile.Write((const void*)strLog.GetBuffer(strLog.GetLength()), strLog.GetLength());
		cFile.Close();
	}

	dwTotalSurfaceNodes = m_dwTotalSurface;
	dwTotalDOFNodes     = m_dwTotalDOFs;
	dwTotalTextureNodes = m_dwTotalTexture;
}
