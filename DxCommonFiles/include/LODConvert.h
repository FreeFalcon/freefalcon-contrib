// LODConvert.h : main header file for the LODConvert application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include	"TexBank.h"
#include	"PalBank.h"
#include	"Device.h"
#include	"DXEngine.h"
#include	"Editing.h"
#ifdef	GFX_EDITING
	#include "SelModeWindow.h"
	#include "VertexEdit.h"
	#include "SurfaceEdit.h"
	#include "SwitchPanel.h"
	#include "TextureEdit.h"
#endif
// CLODConvertApp:
// See LODConvert.cpp for the implementation of this class
//

typedef	enum {PLANE_Y, PLANE_X, PLANE_Z} PlaneType;

#define	WINTOD3DCOLOUR(x)	((x&0xff000000) | ((x&0x00ff0000)>>16) | (x&0x0000ff00) | ((x&0x000000ff)<<16))
#define	D3DTOWINCOLOUR(x)	((x&0xff000000) | ((x&0x00ff0000)>>16) | (x&0x0000ff00) | ((x&0x000000ff)<<16))


#define FALCON_REGISTRY_KEY       "Software\\MicroProse\\Falcon\\4.0"
#define	MAX_EDITABLE_OBJECTS	4
#define	MAX_SWITCHES			256
#define	MAX_TEXTURES_X_SET		32

class E_Light
{
public:
	E_Light	*Prev, *Next;
	bool	Selected;
	DXLightType		Light;
};

#ifdef	GFX_EDITING
	#include "LightEdit.h"
#endif
class E_Object
{

public:
	E_Object() { 	memset(this, 0, sizeof(this)); };
	DWORD			NodeNr;
	D3DXMATRIX		State;
	D3DVECTOR		Pos;
	DxDbHeader		Header;
	E_Light			*LightList;
	void			*Nodes;
	DWORD			Textures[256];
	DWORD			TexSetNr;
	DWORD			TexPerSet;
	DWORD			TexNr;
	DXScriptVariableType	Scripts[MAX_SCRIPTS_X_MODEL];
	float			Yaw,Pitch,Roll;
	ObjectInstance	*Obj;
	D3DVECTOR		UpLeft,DnRight;
	CString			Name;
	float			Radius;
	DWORD			MaxTexNr;
};





#ifdef	GFX_EDITING
	#include "DXTree.h"
	#include "Dofediting.h"

class CLODConvertApp : public CWinApp
{
public:
	CLODConvertApp();
	void		Draw();

	CString	FileTitle;
	float		m_Rot;
	float		m_Pitch;
	float		m_Dist;
	float		m_Roll;
	bool		m_EnableZBias;
	E_Object	EditList[MAX_EDITABLE_OBJECTS];
	E_Object	BackList[MAX_EDITABLE_OBJECTS];
	CToolBar	*m_MainToolBar;


// Overrides
public:
	virtual BOOL InitInstance();
	void OnFileOpen();
	void OnFileSave();
	void	Timerfunct(void);
	void VerifyVersion( int file );
	void ModelMove(D3DXVECTOR3	Trans, D3DXVECTOR3	Move);
	void	FileOpen(CString FileName);
	CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	CString		TheModelPath;
	


	void	SetupModel();

private:
	DWORD	ModelSelected, ReferenceModel;
	bool	ViewEnable[MAX_EDITABLE_OBJECTS],Mode3D,GridEnable,FrameEnable;
	float	CameraYaw, CameraRoll, CameraPitch, CameraRange;
	float	CameraX, CameraY, CameraZ;
	void	ZeroOrigin();
	void	BuildSelectionFrame(DWORD ID);
	DWORD	SelectionFrameTime;
	void			EditClick();
	CSelectList		SelectList, ClipBoard;
	bool		VCursorOn;
	D3DVECTOR	VCursorPos;
	D3DVERTEXEX		*VSelected;
	CVertexEdit		VertexEditing;
	CSurfaceEdit	SurfaceEditing;
	DxSurfaceType	*SSelected;
	DXNode			*m_NSelected;
	DWORD			m_VSelected;
	bool			m_Snap;
	D3DVECTOR		m_SnapPos;
	bool			m_SingleVertexMode;

	enum	{VERTEX=0, FACE, JOINTFACE} EditMode;
	bool			m_LockSelection, m_NVG;

// Implementation
	afx_msg void OnAppAbout();
	afx_msg	void OnModelSelector( UINT ID);
	afx_msg	void OnViewSelector( UINT ID);
	afx_msg	void OnModelCenterAngles();
	afx_msg	void OnSetReference();
	afx_msg	void OnXPlane();
	afx_msg	void OnYPlane();
	afx_msg	void OnZPlane();
	afx_msg	void On3DToggle();
	afx_msg	void OnGridToggle();
	afx_msg	void OnFrameToggle();

	afx_msg	void OnVertexMode();
	afx_msg	void OnFaceMode();
	afx_msg	void OnJointFaceMode();
	afx_msg	void OnNVG();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSetgridsize();
	afx_msg void OnOptionsEnablescript();
	afx_msg void OnOptionsSelectionmode();
	afx_msg void OnOptionsBackculling();
	afx_msg void OnEditHideselected();
	afx_msg void OnEditDisablehide();
	afx_msg void OnEditClearselection();
	afx_msg void OnEditEdit();
	afx_msg void OnEditSun();
	afx_msg void OnEditModeltree();
	afx_msg void OnEditEditvertex();
	afx_msg void OnEditEditsurface();
	afx_msg void OnEditMakeabackup();
	afx_msg void OnEditRestoreabackup();
	afx_msg void OnEditswitches();
	afx_msg void OnOptionsSnaptogrid();
	afx_msg void OnOptionsSinglevertexmode();
	afx_msg void OnEditEdittexture();
	afx_msg void OnFile3dstool();
	afx_msg void OnOptionsLockselection();
	afx_msg void OnToolsAssignspecularity();
	afx_msg void OnEditEditlights();
	afx_msg void OnFileTexturesfolder();
	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();
	afx_msg void OnEditCut();
	afx_msg void OnEditKill();
	afx_msg void OnEditInverselection();
	afx_msg void OnFaceselectionRemoveemissivecolor();
	afx_msg void OnFaceselectionMakeemissive();
	afx_msg void OnFaceselectionRemovezbias();
};


extern CLODConvertApp theApp;
extern Pcolor	*pPalette;
extern	DXDevice *DX;


extern	CDXEngine			TheDXEngine;
extern	PaletteBankClass	ThePaletteBank;
extern	TextureBankClass	TheTextureBank;
extern	char	FalconDataDirectory[512];								// Do Ya read the name...?
extern	char	FalconTerrainDataDir[512];
extern	char	FalconObjectDataDir[512];
extern	char	FalconMiscTexDataDir[512];
extern	DOFvalue			DOFs[MAX_EDITABLE_OBJECTS][128];
extern	UInt32				SWITCHes[MAX_EDITABLE_OBJECTS][MAX_SWITCHES];
extern	bool	DrawDisable;
extern	DWORD	gClass, gDomain, gType;
extern	float	ObjectRoll, ObjectPitch, ObjectYaw;
extern	float	MousePitch, MouseRoll, MouseYaw;
extern	float	MouseMoveX, MouseMoveY, MouseMoveZ, CameraMoveZ, SelectionAlpha;
extern	CLODConvertApp		*MainApp;
extern	UINT	MouseKeyFlags;
extern	CToolBar		*ToolBar;
extern	CPoint			MouseLeftClick, MouseRightClick;
extern	bool				MouseLClick, MouseRClick;
extern	CSelModeWindow	TheSelector;
extern	E_Object			*OSelected;
extern	bool			RenderPhase;
extern	CTreePanel		*TheTreeViews;
extern	CSwitchPanel		*SwitchesPanel;
extern	CDofEditing			*TheDofEditor;
extern	void	(*MoveFunction)(D3DXVECTOR3, D3DXVECTOR3, bool Remove);
extern	float	GridSize,LastGridDone;
extern	PlaneType	PlaneMode;
extern	CTextureEdit	*TheTextureEdit;
extern	D3DVERTEXEX	*gVSelected;
extern	DxSurfaceType	*gSSelected;
extern	E_Object	TheOmniLight, TheSpotLight;
extern	CLightEdit			*TheLightEdit;

DWORD	GetColour(DWORD Original);
#endif

void IdleMode(void);
void Delay(DWORD mSec);
