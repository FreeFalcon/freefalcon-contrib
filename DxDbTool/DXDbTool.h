// DXDbTool.h : file di intestazione principale per l'applicazione PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error: inclusione di "stdafx.h" prima dell'inclusione di questo file per PCH
#endif

#include "resource.h"		// simboli principali
#define FALCON_REGISTRY_KEY       "Software\\MicroProse\\Falcon\\4.0"


// CDXDbToolApp:
// Per l'implementazione di questa classe, vedere DXDbTool.cpp.
//

class CDXDbToolApp : public CWinApp
{
public:
	CDXDbToolApp();

// Sostituzioni
	public:
	virtual BOOL InitInstance();

// Implementazione

	DECLARE_MESSAGE_MAP()
};

extern	CDXDbToolApp DxDbApp;
extern	CString	DBPath,FalconDir;
