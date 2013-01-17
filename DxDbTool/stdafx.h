// stdafx.h : file di inclusione per file di inclusione del sistema standard
// o file di inclusione specifici del progetto utilizzati di frequente,
// ma modificati di rado.

#pragma once
#define	DIRECTINPUT_VERSION 0x0700

#define	ShiAssert(x)
#define ShiError(x)
#define ShiWarning(x)
#define	F4IsBadCodePtr(x)	false

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Escludere dalle intestazioni Windows gli elementi utilizzati di rado.
#endif

// Modificare le seguenti definizioni se è necessario creare una piattaforma prima di quelle specificate di seguito.
// Fare riferimento a MSDN per le ultime informazioni sui valori corrispondenti per le differenti piattaforme.
#ifndef WINVER				// È consentito l'utilizzo di funzionalità specifiche per Windows 95 e Windows NT 4 o versioni successive.
#define WINVER 0x0400		// Cambiare il valore immettendo quello corretto per Windows 98, Windows 2000 o versione successiva.
#endif

#ifndef _WIN32_WINNT		// È consentito l'utilizzo di funzionalità specifiche per Windows NT 4 o versioni successive.
#define _WIN32_WINNT 0x0400		// Cambiare il valore immettendo quello corretto per Windows 98, Windows 2000 o versione successiva.
#endif						

#ifndef _WIN32_WINDOWS		// È consentito l'utilizzo di funzionalità specifiche per Windows 98 o versioni successive.
#define _WIN32_WINDOWS 0x0410 // Cambiare il valore immettendo quello corretto per Windows Millennium Edition o versione successiva.
#endif

#ifndef _WIN32_IE			// È consentito l'utilizzo di funzionalità specifiche per IE 4.0 o versioni successive.
#define _WIN32_IE 0x0400	// Cambiare il valore immettendo quello corretto per Internet Explorer 5.0 o versione successiva.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// alcuni costruttori CString saranno espliciti.

// disattiva l'operazione delle classi MFC che consiste nel nascondere alcuni comuni messaggi di avviso che vengono spesso ignorati.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // componenti MFC di base e standard
#include <afxext.h>         // estensioni MFC
#include <afxdisp.h>        // classi di automazione MFC

#include <afxdtctl.h>		// Supporto MFC per controlli comuni di Internet Explorer 4
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// Supporto MFC per controlli comuni di Windows
#endif // _AFX_NO_AFXCMN_SUPPORT

// ATL
#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>

// Smart ptr stuff
#include <comdef.h>

// DirectX
#define D3D_OVERLOADS

#include <ddraw.h>
#include <d3d.h>
#include <d3dxcore.h>
#include <d3dxmath.h>

// STL
#include <vector>
#include <string>
#include <map>

// Misc
#include <io.h>
#include <math.h>
#include <stdio.h>

#include "smart.h"
#include <afxdhtml.h>

#define	MAX_TAGS		65536
#define	LOD_BUFFER_SIZE	16384*1024
#define	MAX_VERTICES	65536


/*extern	D3DVERTEX vBuffer[MAX_VERTICES];
extern	int		VertCount;
extern	DWORD	Textures[1024];
extern	int		TexCount;*/

