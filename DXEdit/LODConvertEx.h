// LODConvertEx.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "BSPNodes.h"
#include "resource.h"       // main symbols
#include "context.h"
#include "statestack.h"
//#include "LodInfo.h"
#include "Converter.h"


// CLODConvertExApp:
// See LODConvertEx.cpp for the implementation of this class
//

class CLODConvertExApp : public CWinApp
{
public:
	CLODConvertExApp();

	void *m_pLODBuffer;
	
	DWORD m_dwLODSize;
	DWORD m_dwTagCount;
	char  *m_NodeTreeData;
	BRoot *m_Root;
	BNodeType *m_TagList;
	StateStackClass		m_TheStateStack;

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CLODConvertExApp theApp;
extern Converter *CV;
extern Pcolor	*pPalette;
