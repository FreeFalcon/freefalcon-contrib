// LODConvert.h : main header file for the LODConvert application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "BSPNodes.h"
#include "resource.h"       // main symbols
#include "context.h"
#include "statestack.h"
#include "LodInfo.h"
#include "Converter.h"
#include	"TexBank.h"
#include	"PalBank.h"
#include	"Device.h"
#include	"DXEngine.h"
#include	"DXLList.h"
#include	"ColorTool.h"
#include	"VbList.h"
#include "DBConverter.h"
#include "VBAskClass.h"
// CLODConvertApp:
// See LODConvert.cpp for the implementation of this class
//

#define FALCON_REGISTRY_KEY       "Software\\MicroProse\\Falcon\\4.0"


class CLODConvertApp : public CWinApp
{
public:
	CLODConvertApp();
	void *LODBuffer;
	
	DWORD LODSize;
	DWORD TagCount;
	char *NodeTreeData;
	BRoot *Root;
	BNodeType *TagList;
	CLodInfo *LodInfo;
	CString	FileTitle;
	float	m_Rot;
	float	m_Pitch;
	float	m_Dist;
	float	m_Roll;
	bool	m_EnableZBias;
	void	OnFileSave();


// Overrides
public:
	virtual BOOL InitInstance();
	void OnFileOpen();
	StateStackClass	TheStateStack;
	void	Timerfunct(void);
	void VerifyVersion( int file );
	void OnAppPlane();
	void OnDirection( UINT ID);
	void	OnDistance( UINT ID);
	void	OnCullKey();
	void	OnDofMoveKey();
	void	OnMultiKey();
	void	OnInfoKey();
	void	OnZBiasKey();
	void	OnColorKey();
	void	OnVbKey();
	void	OnDXKey();
	void	RunIHRFile( CString FileName);
	void	SetupDBConvert(void);





// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CLODConvertApp theApp;
extern Converter *CV;
extern Pcolor	*pPalette;
extern	DWORD	NItems;
extern	float	Fps;
extern	DXDevice *DX;


extern	CDXEngine			TheDXEngine;
extern	PaletteBankClass	ThePaletteBank;
extern	TextureBankClass	TheTextureBank;
extern	char	FalconDataDirectory[512];								// Do Ya read the name...?
extern	char	FalconTerrainDataDir[512];
extern	char	FalconObjectDataDir[512];
extern	char	FalconMiscTexDataDir[512];
extern	DOFvalue			DOFs[128];
extern	UInt32				SWITCHes[128];
extern	float	GlobalBias;
extern	CDXLList	*DXLLister;
extern	CVbList		*VBList;
extern	bool	DrawDisable;
extern	DWORD	gClass, gDomain, gType, gPLevel;
extern	CDBConverter	*DBCV;
extern	float	ObjectRoll, ObjectPitch, ObjectYaw;
extern	float	MousePitch, MouseRoll, MouseYaw;
extern	float	MouseMoveX, MouseMoveY, MouseMoveZ;
extern	Encoder	*ECD;
extern	CString	TheModelName;

void IdleMode(void);
void Delay(DWORD mSec);
