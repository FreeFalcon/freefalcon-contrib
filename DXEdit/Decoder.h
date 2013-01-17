#pragma once

#include "TreeCtrlEx.h"

class Decoder
{
	CString m_strFileName;
	BYTE    *m_pBuffer;
	BYTE    *m_pVertexesPool;
	UINT     m_uiOffset;
	CTreeCtrlEx *m_pDXLTree;
	CArray<HTREEITEM, HTREEITEM> m_arDOFItems;

public:
	Decoder();
	~Decoder(void);

	DWORD    m_dwTotalDOFs;
	DWORD    m_dwTotalSurface;
	DWORD    m_dwTotalTexture;
	DWORD    m_dwTotalVertexes;
	DWORD    m_dwFileSize;

	UINT     m_uiBufferSize;
public:
	BOOL Initialize(CString strFileName, CTreeCtrlEx *pDXLTree);
	void Decode(DWORD& dwTotalSurfaceNodes, DWORD& dwTotalDOFNodes, DWORD& dwTotalTextureNodes);

private:
	void ReadBuffer(UINT uiBytesToRead, void *pBuffer);
};
