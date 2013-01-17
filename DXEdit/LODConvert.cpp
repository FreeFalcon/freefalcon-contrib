// LODConvert.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include <io.h>
#include <fcntl.h>
#include "LODConvert.h"
#include "MainFrm.h"
#include "PalBank.h"
#include "TexBank.h"
#include "ObjectLOD.h"
#include "ObjectParent.h"
#include "InputBox.h"

#include "LODConvertDoc.h"
#include "LODConvertView.h"
#include "DXEngine.h"
#include "DXEditing.h"
#include ".\lodconvert.h"
#include "vertexedit.h"
#include "DofEditing.h"
#include "SunEditing.h"
#include "Encoder.h"
#include "3DSImport.h"
#include "DXFiles.h"

void CALLBACK EXPORT TimedRendering(  HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime);


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

PaletteBankClass	ThePaletteBank;
TextureBankClass	TheTextureBank;
ObjectParent		TheObjectParent[MAX_EDITABLE_OBJECTS];
CWnd				*MainWin;
bool				CullEnable,DofMoveEnable,DrawDisable;
DOFvalue			DOFs[MAX_EDITABLE_OBJECTS][128];
UInt32				SWITCHes[MAX_EDITABLE_OBJECTS][MAX_SWITCHES];
CLODConvertView		*m_DisplayArea;
CLODConvertApp		*MainApp;
UINT				MouseKeyFlags;
CToolBar			*ToolBar;
CSelModeWindow		TheSelector;
E_Object			*OSelected;
CTreePanel			*TheTreeViews;
CSwitchPanel		*SwitchesPanel;
CDofEditing			*TheDofEditor;
CTextureEdit		*TheTextureEdit;
CLightEdit			*TheLightEdit;

Pcolor	*pPalette;
DWORD	gClass, gDomain, gType;
float	GridSize,LastGridDone;
PlaneType	PlaneMode;
D3DVERTEXEX		*gVSelected;
DxSurfaceType	*gSSelected;

CPoint			MouseLeftClick, MouseRightClick;
bool				MouseLClick, MouseRClick;
bool			RenderPhase;

// ************** Global Variables for the App ***************
char	FalconDataDirectory[512];								// Do Ya read the name...?
char	FalconTerrainDataDir[512];
char	FalconObjectDataDir[512];
char	FalconMiscTexDataDir[512];

DXDevice	*DX;												// the DX Device

float	ObjectRoll, ObjectPitch, ObjectYaw;
float	MousePitch, MouseRoll, MouseYaw;
float	MouseMoveX, MouseMoveY, MouseMoveZ, CameraMoveZ, SelectionAlpha;
float	ObjectX,ObjectY,ObjectZ;

void	(*MoveFunction)(D3DXVECTOR3, D3DXVECTOR3, bool);

D3DVERTEXEX	ObjectFrame[24];
D3DVERTEXEX	VertexCursor[6];

E_Object	TheOmniLight, TheSpotLight;


// CLODConvertApp

BEGIN_MESSAGE_MAP(CLODConvertApp, CWinApp)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILESAVE, OnFileSave)
	ON_COMMAND_RANGE(ID_M1, ID_M8, OnModelSelector)
	ON_COMMAND_RANGE(ID_V1, ID_V8, OnViewSelector)
	// Standard print setup command
	//ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_RESETANGLES, OnModelCenterAngles)
	ON_COMMAND(ID_SETREFERENCE, OnSetReference)
	ON_COMMAND(ID_XPLANE, OnXPlane)
	ON_COMMAND(ID_YPLANE, OnYPlane)
	ON_COMMAND(ID_ZPLANE, OnZPlane)
	ON_COMMAND(ID_3DTOGGLE, On3DToggle)
	ON_COMMAND(ID_GRIDTOGGLE, OnGridToggle)
	ON_COMMAND(ID_SETGRIDSIZE, OnSetgridsize)
	ON_COMMAND(ID_FRAMEMODE, OnFrameToggle)

	ON_COMMAND(ID_VERTEXMODE, OnVertexMode)
	ON_COMMAND(ID_FACEMODE, OnFaceMode)
	ON_COMMAND(ID_JOINTFACEMODE, OnJointFaceMode)

	ON_COMMAND(ID_NVGBUTTON, OnNVG)

	ON_COMMAND(ID_OPTIONS_ENABLESCRIPT, OnOptionsEnablescript)
	ON_COMMAND(ID_OPTIONS_SELECTIONMODE, OnOptionsSelectionmode)
	ON_COMMAND(ID_OPTIONS_BACKCULLING, OnOptionsBackculling)
	ON_COMMAND(ID_EDIT_HIDESELECTED, OnEditHideselected)
	ON_COMMAND(ID_EDIT_DISABLEHIDE, OnEditDisablehide)
	ON_COMMAND(ID_EDIT_CLEARSELECTION, OnEditClearselection)
	ON_COMMAND(ID_EDITSUN, OnEditSun)
	ON_COMMAND(ID_EDIT_MODELTREE, OnEditModeltree)
	ON_COMMAND(ID_EDIT_EDITVERTEX, OnEditEditvertex)
	ON_COMMAND(ID_EDIT_EDITSURFACE, OnEditEditsurface)
	ON_COMMAND(ID_EDIT_MAKEABACKUP, OnEditMakeabackup)
	ON_COMMAND(ID_EDIT_RESTOREABACKUP, OnEditRestoreabackup)
	ON_COMMAND(ID_EDITSWITCHES, OnEditswitches)
	ON_COMMAND(ID_OPTIONS_SNAPTOGRID, OnOptionsSnaptogrid)
	ON_COMMAND(ID_OPTIONS_SINGLEVERTEXMODE, OnOptionsSinglevertexmode)
	ON_COMMAND(ID_EDIT_EDITTEXTURE, OnEditEdittexture)
	ON_COMMAND(ID_FILE_3DSTOOL, OnFile3dstool)
	ON_COMMAND(ID_OPTIONS_LOCKSELECTION, OnOptionsLockselection)
	ON_COMMAND(ID_TOOLS_ASSIGNSPECULARITY, OnToolsAssignspecularity)
	ON_COMMAND(ID_EDIT_EDITLIGHTS, OnEditEditlights)
	ON_COMMAND(ID_FILE_TEXTURESFOLDER, OnFileTexturesfolder)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_KILL, OnEditKill)
	ON_COMMAND(ID_EDIT_INVERSELECTION, OnEditInverselection)
	ON_COMMAND(ID_FACESELECTION_REMOVEEMISSIVECOLOR, OnFaceselectionRemoveemissivecolor)
	ON_COMMAND(ID_FACESELECTION_MAKEEMISSIVE, OnFaceselectionMakeemissive)
	ON_COMMAND(ID_FACESELECTION_REMOVEZBIAS, OnFaceselectionRemovezbias)
END_MESSAGE_MAP()



void CLODConvertApp::ZeroOrigin()
{
	float RefX=EditList[ReferenceModel].Pos.x;
	float RefY=EditList[ReferenceModel].Pos.y;
	float RefZ=EditList[ReferenceModel].Pos.z;

	for(int a=0; a<MAX_EDITABLE_OBJECTS; a++){
		EditList[a].Pos.x-=RefX;
		EditList[a].Pos.y-=RefY;
		EditList[a].Pos.z-=RefZ;
	}
	
}

void CLODConvertApp::OnFrameToggle()
{
	FrameEnable^=true;
}


void CLODConvertApp::OnXPlane()
{
	// Orient with front X Axis
	CameraPitch=0.0f;
	CameraYaw=0.0f;
	CameraRoll=PI/2.0f;
	ZeroOrigin();
	PlaneMode=PLANE_X;
}

void CLODConvertApp::OnYPlane()
{
	// Orient with front Y Axis
	CameraPitch=0.0f;
	CameraYaw=0.0f;
	CameraRoll=0.0f;
	ZeroOrigin();
	PlaneMode=PLANE_Y;
}

void CLODConvertApp::OnZPlane()
{
	// Orient with front Z Axis
	CameraPitch=PI/2.0f;
	CameraYaw=0.0f;
	CameraRoll=0.0f;
	ZeroOrigin();
	PlaneMode=PLANE_Z;

}


void CLODConvertApp::OnSetReference()
{
	ReferenceModel=ModelSelected;
}

void CLODConvertApp::OnModelCenterAngles()
{
	EditList[ModelSelected].Yaw=0.0f;
	EditList[ModelSelected].Pitch=0.0f;
	EditList[ModelSelected].Roll=0.0f;
}


void CLODConvertApp::BuildSelectionFrame(DWORD ID)
{
	D3DVECTOR	UL=EditList[ID].UpLeft;
	D3DVECTOR	DR=EditList[ID].DnRight;

	SelectionFrameTime=50;

	// Create the Frame for the selected object
	ZeroMemory(ObjectFrame, sizeof(ObjectFrame));
	D3DVERTEXEX	Point[8];
	for(int a=0; a<8; a++) Point[a].dwColour=0xffff0000;

	Point[0].vx=UL.x; Point[0].vy=UL.y ; Point[0].vz=UL.z ;
	Point[1].vx=UL.x; Point[1].vy=DR.y ; Point[1].vz=UL.z ;
	Point[2].vx=UL.x; Point[2].vy=DR.y ; Point[2].vz=DR.z ;
	Point[3].vx=UL.x; Point[3].vy=UL.y ; Point[3].vz=DR.z ;
	Point[4].vx=DR.x; Point[4].vy=DR.y ; Point[4].vz=DR.z ;
	Point[5].vx=DR.x; Point[5].vy=UL.y ; Point[5].vz=DR.z ;
	Point[6].vx=DR.x; Point[6].vy=UL.y ; Point[6].vz=UL.z ;
	Point[7].vx=DR.x; Point[7].vy=DR.y ; Point[7].vz=UL.z ;

	ObjectFrame[0]=Point[0]; ObjectFrame[1]=Point[1];
	ObjectFrame[2]=Point[1]; ObjectFrame[3]=Point[2];
	ObjectFrame[4]=Point[2]; ObjectFrame[5]=Point[3];
	ObjectFrame[6]=Point[3]; ObjectFrame[7]=Point[0];
	ObjectFrame[8]=Point[4]; ObjectFrame[9]=Point[5];
	ObjectFrame[10]=Point[5]; ObjectFrame[11]=Point[6];
	ObjectFrame[12]=Point[6]; ObjectFrame[13]=Point[7];
	ObjectFrame[14]=Point[7]; ObjectFrame[15]=Point[4];
	ObjectFrame[16]=Point[0]; ObjectFrame[17]=Point[6];
	ObjectFrame[18]=Point[3]; ObjectFrame[19]=Point[5];
	ObjectFrame[20]=Point[2]; ObjectFrame[21]=Point[4];
	ObjectFrame[22]=Point[1]; ObjectFrame[23]=Point[7];
}

void CLODConvertApp::OnModelSelector( UINT ID)
{
	SelectList.ClearSelection();
	ModelSelected=ID-ID_M1;

	BuildSelectionFrame(ModelSelected);
	OSelected=&EditList[ModelSelected];
	TheTreeViews->SelectModel(ModelSelected);


}

void CLODConvertApp::On3DToggle()
{
	Mode3D^=true;
}

void CLODConvertApp::OnGridToggle()
{
	GridEnable^=true;
}


void CLODConvertApp::OnViewSelector( UINT ID)
{
	DWORD View=ID-ID_V1;
	ViewEnable[View]^=true;
}


/*E_Object::E_Object()
{
	memset(this, 0, sizeof(this));
}
*/


// CLODConvertApp construction

CLODConvertApp::CLODConvertApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	
	ObjectRoll=ObjectPitch=ObjectYaw=0.0f;
	MousePitch=MouseRoll=MouseYaw=0.0f;
	MouseMoveX=MouseMoveY=MouseMoveZ=CameraMoveZ=0.0f;
	SelectionAlpha=0.5;
	ObjectX=ObjectY=0; ObjectZ=50.0f;
	ModelSelected=ReferenceModel=0;
	MouseLClick=MouseRClick=false;

	//memset(EditList, 0, sizeof(EditList));
	ZeroMemory(DOFs, sizeof(DOFs));
	ZeroMemory(SWITCHes, sizeof(SWITCHes));

	gVSelected=NULL;
	gSSelected=NULL;

	for(int a=0; a<MAX_EDITABLE_OBJECTS; a++){
		EditList[a].Obj=new ObjectInstance(a);
		EditList[a].Obj->DOFValues=DOFs[a];
		EditList[a].Obj->SwitchValues=SWITCHes[a];
		EditList[a].Obj->ParentObject=&TheObjectParent[a];
		EditList[a].Obj->ParentObject->nDOFs=40;
		EditList[a].Obj->ParentObject->nSwitches=40;
		EditList[a].Obj->ParentObject->nTextureSets=1;

		EditList[a].Obj->SwitchValues[5]=1;
	}

	TheOmniLight.Obj=new ObjectInstance(a);
	TheOmniLight.Obj->DOFValues=DOFs[a];
	TheOmniLight.Obj->SwitchValues=SWITCHes[a];
	TheOmniLight.Obj->ParentObject=&TheObjectParent[a];
	TheOmniLight.Obj->ParentObject->nDOFs=40;
	TheOmniLight.Obj->ParentObject->nSwitches=40;
	TheOmniLight.Obj->ParentObject->nTextureSets=1;

	TheSpotLight.Obj=new ObjectInstance(a);
	TheSpotLight.Obj->DOFValues=DOFs[a];
	TheSpotLight.Obj->SwitchValues=SWITCHes[a];
	TheSpotLight.Obj->ParentObject=&TheObjectParent[a];
	TheSpotLight.Obj->ParentObject->nDOFs=40;
	TheSpotLight.Obj->ParentObject->nSwitches=40;
	TheSpotLight.Obj->ParentObject->nTextureSets=1;



	CameraPitch=CameraRoll=CameraYaw=0.0f;
	CameraX=CameraY=0.0f;CameraZ=50.0f;
	CameraRange=50.0f;
	Mode3D=GridEnable=FrameEnable=m_LockSelection=false;
	GridSize=1.0f;
	LastGridDone=GridSize;
	SelectionFrameTime=0;
	EditMode=JOINTFACE;
	Mode3D=true;
	VSelected=NULL;
	OSelected=NULL;
	MoveFunction=NULL;
	m_SingleVertexMode=m_NVG=false;

	// Build up the Vertex Cursor
	memset(VertexCursor, 0, sizeof(VertexCursor));
	for(a=0; a<6; a++){
		VertexCursor[a].dwColour=0xc0ffff00;
		VertexCursor[a].dwSpecular=0xc0ffff00;
	}
	VertexCursor[0].nx=-1.0f; VertexCursor[1].nx=1.0f; 
	VertexCursor[2].ny=-1.0f; VertexCursor[3].ny=1.0f; 
	VertexCursor[4].nz=-1.0f; VertexCursor[5].nz=1.0f; 

}


// The one and only CLODConvertApp object

CLODConvertApp theApp;
// CLODConvertApp initialization

// Update this each time the object file formats change
//static const UInt32	FORMAT_VERSION = 0x03087000;


void CLODConvertApp::VerifyVersion( int file )
{
	int				result;
	UInt32			fileVersion;
	char			message[80];

	// Read the magic number at the head of the file
	result = read( file, &fileVersion, sizeof(fileVersion) );

	// If the version doesn't match our expectations, report an error
	if (fileVersion != FORMAT_VERSION) {
		Beep( 2000, 500 );
		Beep( 2000, 500 );
		sprintf( message, "Got object format version 0x%08X, want 0x%08X", fileVersion, FORMAT_VERSION );
		ShiError( message );
	}
}




BOOL CLODConvertApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(8);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CLODConvertDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CLODConvertView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	CString	ModelToLoad="";
	if(cmdInfo.m_nShellCommand==CCommandLineInfo::FileOpen){
		cmdInfo.m_nShellCommand=CCommandLineInfo::FileNew;
		ModelToLoad=cmdInfo.m_strFileName;
	}
	
	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	MainWin=m_pMainWnd;
	MainWin->SetWindowText("DX Edit");
	MainApp=this;

	char path[512];
	CString	AppPath;
	if(GetModuleFileName(NULL, path, sizeof(path))){	
		AppPath=path;
		AppPath=AppPath.Left(AppPath.ReverseFind('\\'));
	} else 
		AppPath="";


	DWORD	 size = sizeof (FalconDataDirectory);
	HKEY	theKey;
	long	retval = RegOpenKeyEx(HKEY_LOCAL_MACHINE, FALCON_REGISTRY_KEY,
      0, KEY_ALL_ACCESS, &theKey);
	DWORD type;
	retval = RegQueryValueEx(theKey, "baseDir", 0, &type, (LPBYTE)&FalconDataDirectory, &size);
	if (retval != ERROR_SUCCESS){
	   MessageBox(NULL, "Where is Falcon...???", "Boh...!!!!", 0);
	   exit(1);
	}
   size = sizeof (FalconTerrainDataDir);
   RegQueryValueEx(theKey, "theaterDir", 0, &type, (LPBYTE)FalconTerrainDataDir, &size);
   size = sizeof (FalconObjectDataDir);
   RegQueryValueEx(theKey, "objectDir", 0, &type, (LPBYTE)FalconObjectDataDir, &size);
   size = sizeof (FalconMiscTexDataDir);
   RegQueryValueEx(theKey, "misctexDir", 0, &type, (LPBYTE)FalconMiscTexDataDir, &size);
   

	CFile	File;
	char	filename[512];
	ZeroMemory(filename, sizeof(filename));
	strcpy(filename, AppPath+"\\OmniLight.dxm");
	if(!File.Open(filename, CFile::modeRead, NULL)){
		MessageBox(NULL, "File OmniLight.dxm missing!", "File Error", 0);
		return(false);
	}
	DWORD FileSize=(DWORD)File.GetLength();
	void *FileBuffer=malloc(FileSize);
	File.Read(FileBuffer, FileSize);
	File.Close();
	DeIndexModel(FileBuffer, &TheOmniLight);
	free(FileBuffer);

	ZeroMemory(filename, sizeof(filename));
	strcpy(filename, AppPath+"\\SpotLight.dxm");
	if(!File.Open(filename, CFile::modeRead, NULL)){
		MessageBox(NULL, "File SpotLight.dxm missing!", "File Error", 0);
		return(false);
	}
	FileSize=(DWORD)File.GetLength();
	FileBuffer=malloc(FileSize);
	File.Read(FileBuffer, FileSize);
	File.Close();
	DeIndexModel(FileBuffer, &TheSpotLight);
	free(FileBuffer);


	
	// COBRA - RED - BSP Palette LOAD //
	ZeroMemory(filename, sizeof(filename));
	strcpy( filename, FalconObjectDataDir );
	strcat( filename, "\\KoreaObj.DXH" );
	if(!File.Open(filename, CFile::modeRead, NULL)){
		MessageBox(NULL, "File 'KoreaObj.DXH' missing!", "File Error", 0);
		return(false);
	}
	FileSize=(DWORD)File.GetLength();
	FileBuffer=malloc(FileSize);
	File.Read(FileBuffer, FileSize);
	File.Close();
	DWORD Palettesize=*(((DWORD*)FileBuffer)+1);
	pPalette=(Pcolor*)malloc(Palettesize*sizeof(Pcolor));
	memcpy((void*)pPalette,(((DWORD*)FileBuffer)+3),Palettesize*sizeof(Pcolor));
	free(FileBuffer);

	ZeroMemory(filename, sizeof(filename));
	strcpy( filename, FalconObjectDataDir );
	strcat( filename, "\\KoreaObj" );
	for(int p=0; p<MAX_EDITABLE_OBJECTS; p++) TheObjectParent[p].SetupTable(filename);

	//ToolsWindow=new CTools();


	// Here ceates the DX Device
	DX=new DXDevice(m_DisplayArea);
	DX->CreateGrid(GridSize, 1000, 0x40808080);

	m_Rot=0.0f;
	m_Pitch=0.0f;
	m_Dist=50.0f;

	gClass=gDomain=gType=0;
	ToolBar->GetToolBarCtrl().SetState(ID_JOINTFACEMODE, TBSTATE_CHECKED);
	ToolBar->GetToolBarCtrl().SetState(ID_YPLANE, TBSTATE_CHECKED);
	ToolBar->GetToolBarCtrl().SetState(ID_3DTOGGLE, TBSTATE_CHECKED);
	OSelected=&EditList[0];
	PlaneMode=PLANE_Y;

	SwitchesPanel=new CSwitchPanel();
	TheTreeViews=new CTreePanel(m_pMainWnd);	
	TheDofEditor=new CDofEditing(m_pMainWnd);
	TheTextureEdit=new CTextureEdit(m_pMainWnd);
	TheLightEdit=new CLightEdit(m_pMainWnd);
	TheModelPath="";
	SetTimer(NULL, 0, 40, TimedRendering);


	if(ModelToLoad!="") FileOpen(ModelToLoad);

	return TRUE;
}



void	CLODConvertApp::SetupModel()
{
		SelectList.ClearSelection();
		gSSelected=NULL;
		VSelected=NULL;
		// and no transformations
		D3DXMatrixIdentity(&EditList[ModelSelected].State);
		// Zero Position
		EditList[ModelSelected].Pos.x=0;
		EditList[ModelSelected].Pos.y=0;
		EditList[ModelSelected].Pos.z=0;
		EditList[ModelSelected].Yaw=0;
		EditList[ModelSelected].Pitch=0;
		EditList[ModelSelected].TexSetNr=0;

		ViewEnable[ModelSelected]=true;
		ToolBar->GetToolBarCtrl().SetState(ID_V1+ModelSelected, TBSTATE_CHECKED);
		ToolBar->GetToolBarCtrl().SetState(ID_M1+ModelSelected, TBSTATE_CHECKED);

		// Load Textures
		DX->LoadTextures(EditList[ModelSelected].Header.dwTexNr, EditList[ModelSelected].Textures);
		BuildSelectionFrame(ModelSelected);
		CameraZ=EditList[ModelSelected].UpLeft.x - EditList[ModelSelected].DnRight.x + EditList[ModelSelected].UpLeft.y - EditList[ModelSelected].DnRight.y;

		OSelected=&EditList[ModelSelected];
		TheTreeViews->UpdateModel(&EditList[ModelSelected], ModelSelected, &SelectList);
		TheTreeViews->SelectModel(ModelSelected);

		E_MakeModelCopy(&SelectList, &EditList[ModelSelected], &BackList[ModelSelected]);

}


void	CLODConvertApp::FileOpen(CString FileName)
{

		EditList[ModelSelected].Name="";
		// Ok... Load the model
		CFile File;
		File.Open(FileName, CFile::modeRead, &CFileException(CFileException::generic, NULL));
		DWORD	FileSize=(DWORD)File.GetLength();
		void *FileBuffer=malloc(FileSize);
		File.Read(FileBuffer, FileSize);
		FileTitle=File.GetFileTitle();
		File.Close();
		TheModelPath=FileName;
		
		while(RenderPhase) Delay(5);
		// Eventually clear the model
		ClearModel(&EditList[ModelSelected]);
		// Linearize the model and assign to selected item
		DeIndexModel(FileBuffer, &EditList[ModelSelected]);
		E_FixModelFeatures(&EditList[ModelSelected]);
		EditList[ModelSelected].Name=FileTitle;
	
		SetupModel();
		AddToRecentFileList(FileName);

}

void	CLODConvertApp::OnFileOpen()
{
	CString	FileName="*.dxm";

	if(CWinApp::DoPromptFileName(FileName,IDS_LOD_OPEN,0,true,NULL)){
		FileOpen(FileName);	

	}
}




// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void CLODConvertApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}



void CLODConvertApp::EditClick()
{
	D3DXMATRIX	CameraState;
	D3DVECTOR	ClickPoint;
	// Get the coords transformations
	D3DXMatrixRotationYawPitchRoll(&CameraState, CameraYaw, CameraPitch, CameraRoll);
	D3DVECTOR	RefVect(EditList[ReferenceModel].Pos.x, EditList[ReferenceModel].Pos.y, EditList[ReferenceModel].Pos.z);
	D3DXVec3TransformCoord((D3DXVECTOR3*)&RefVect, (D3DXVECTOR3*)&RefVect, &CameraState);

	// Get the screen coordinates
	if(MouseLClick)
		DX->GetWorldCoordinates(&ClickPoint, MouseLeftClick.x, MouseLeftClick.y, CameraZ, RefVect.x, RefVect.z);
	if(MouseRClick)
		DX->GetWorldCoordinates(&ClickPoint, MouseRightClick.x, MouseRightClick.y, CameraZ, RefVect.x, RefVect.z);
		
	// in F4 normal View, Z is Y
	ClickPoint.z=ClickPoint.y;	ClickPoint.y=0.0f;
		
	// Transform  in the reverse of camera view to get real coordinates
	D3DXMatrixRotationYawPitchRoll(&CameraState, -CameraYaw, -CameraPitch, -CameraRoll);
	D3DXVec3TransformCoord((D3DXVECTOR3*)&ClickPoint, (D3DXVECTOR3*)&ClickPoint, &CameraState);
		
		// Create the Mask to select which vector to use for calculations
	D3DVECTOR	Mask(1,0,1);
	D3DXVec3TransformCoord((D3DXVECTOR3*)&Mask, (D3DXVECTOR3*)&Mask, &CameraState);
	if(fabs(Mask.x)<0.0000001) Mask.x=0.0f;
	if(fabs(Mask.y)<0.0000001) Mask.y=0.0f;
	if(fabs(Mask.z)<0.0000001) Mask.z=0.0f;

	// Now find the nearest vertex to that point on screen and it's surface
	CEditing	EditNode;
	D3DCOLORVALUE	Color;
	Color.a=Color.r=1.0f; Color.b=Color.g=0.0f;
	DWORD	VertexNr=E_FindVertex(&EditList[ModelSelected], &ClickPoint, &EditNode, &Mask);
	OSelected=&EditList[ModelSelected];

	m_VSelected=VertexNr;
	m_NSelected=EditNode.Node;
	gVSelected=&(((DXTriangle*)m_NSelected)->Vertex[m_VSelected]);
	gSSelected=(EditNode.Node)?&SURFACE(EditNode.Node):NULL;

	// ok, switch the editing mode
	switch(EditMode){
									// if something found
			case	VERTEX		:	if(EditNode.Node){
										if(MouseLClick)	{
											VSelected=&(((DXTriangle*)EditNode.Node)->Vertex[VertexNr]);
											if(m_SingleVertexMode) SelectList.AddVertex(EditNode.Node, VertexNr);
											else E_SelectCommonVertex(true, &EditList[ModelSelected], &SelectList, VSelected); 
											VertexEditing.UpdateEditValues(&SelectList, (void**)&OSelected, &VSelected);
										}
										if(MouseRClick){
											VSelected=&(((DXTriangle*)EditNode.Node)->Vertex[VertexNr]);
											if(m_SingleVertexMode) SelectList.RemoveVertex(EditNode.Node, VertexNr);
											else E_SelectCommonVertex(false, &EditList[ModelSelected], &SelectList, VSelected); 
											VSelected=NULL;
										}
									}
									break;

			case	FACE		:	if(m_LockSelection) break;
									if(EditNode.Node){
										if(MouseLClick){
											SelectList.ClearVertexList();
											SelectList.AddVertex(EditNode.Node,VertexNr);
											// if alreadys elected brek here
											if(!EditNode.Node->SelectID){
												((DXTriangle*)EditNode.Node)->Surface.dwFlags.b.Frame=true;
												// else add to the selec list
												EditNode.Node->SelectID=SelectList.AddSelection(&EditNode);
												EditNode.Node->FrameColour=Color;
												SSelected=&(((DXTriangle*)EditNode.Node)->Surface);
											}
										}
										if(MouseRClick){
											((DXTriangle*)EditNode.Node)->Surface.dwFlags.b.Frame=false;
											// if it's a selected node, remove it from selection list
											if(EditNode.Node->SelectID) SelectList.RemoveSelection(EditNode.Node->SelectID);
											EditNode.Node->SelectID=0;
											SSelected=NULL;
										}
										SurfaceEditing.UpdateEditValues(&SelectList, m_NSelected, m_VSelected);
									}
									SelectList.UpdateTexUsedList();
									TheTextureEdit->UpdateValues();
									break;

			case	JOINTFACE	:	if(m_LockSelection) break;
									if(EditNode.Node){
										if(MouseLClick){
											SelectList.ClearVertexList();
											SSelected=&(((DXTriangle*)EditNode.Node)->Surface);
											SelectList.AddVertex(EditNode.Node,VertexNr);
										}	else SSelected=NULL;
										E_ExtendedSelect(&EditList[ModelSelected], &SelectList, EditNode.Node, MouseLClick);
										SurfaceEditing.UpdateEditValues(&SelectList, m_NSelected, m_VSelected);

									}
									SelectList.UpdateTexUsedList();
									TheTextureEdit->UpdateValues();
									
									break;

	}
		//
	MouseLClick=MouseRClick=false;
}

void CLODConvertApp::ModelMove(D3DXVECTOR3	Trans, D3DXVECTOR3	Move)
{
	EditList[ModelSelected].Yaw+=Trans.x;//*LastGridDone;
	EditList[ModelSelected].Pitch+=Trans.y;//*LastGridDone;
	EditList[ModelSelected].Roll+=Trans.z;//*LastGridDone;

	if(ModelSelected!=ReferenceModel){
		EditList[ModelSelected].Pos.x+=Move.x;
		EditList[ModelSelected].Pos.y+=Move.y;
		EditList[ModelSelected].Pos.z+=Move.z;
	} else {
		for(int a=0; a<MAX_EDITABLE_OBJECTS; a++){
			EditList[a].Pos.x+=Move.x;
			EditList[a].Pos.y+=Move.y;
			EditList[a].Pos.z+=Move.z;
		}
	}

}




void CLODConvertApp::Draw()
{
	if(DrawDisable) return;

	// 1st, calculate Camera Space transformation
	D3DXMATRIX	CameraState;

	if(MouseLClick || MouseRClick){
		EditClick();
	}

	if((LastGridDone/fabs(CameraZ))>0.05f && LastGridDone>GridSize){
		LastGridDone*=0.1f;
		if(LastGridDone<GridSize) LastGridDone=GridSize;
		DX->CreateGrid(LastGridDone, 1000, 0x40808080);
		CameraMoveZ=0.0f;
	}
	if((LastGridDone/fabs(CameraZ))<=0.005f){
		LastGridDone*=10;
		DX->CreateGrid(LastGridDone, 1000, 0x40808080);
		CameraMoveZ=0.0f;
	}

	CameraZ-=CameraMoveZ;//*LastGridDone;
	//if(CameraZ<0.0f) CameraZ=0.0f;
	CameraRange=CameraZ;
	E_SetScale(CameraZ);

	D3DXMatrixRotationYawPitchRoll(&CameraState, CameraPitch, CameraRoll, CameraYaw);
	D3DXVECTOR3	Trans(MouseYaw, MousePitch, 0.0f);
	D3DXVec3TransformCoord(&Trans, &Trans, &CameraState);

	D3DXMatrixRotationYawPitchRoll(&CameraState, -CameraYaw, -CameraPitch, -CameraRoll);
	D3DXVECTOR3	Move(MouseMoveX, MouseMoveY, MouseMoveZ);
	D3DXVec3TransformCoord(&Move, &Move, &CameraState);
	Move.x*=LastGridDone;
	Move.y*=LastGridDone;
	Move.z*=LastGridDone;

	if(m_Snap){
		m_SnapPos.x+=Move.x;
		m_SnapPos.y+=Move.y;
		m_SnapPos.z+=Move.z;
		Move.x=Move.y=Move.z=0.0f;
		if(fabs(m_SnapPos.x)>GridSize) {Move.x=(float)_copysign(GridSize, m_SnapPos.x); m_SnapPos.x-=(float)_copysign(GridSize, m_SnapPos.x); }
		if(fabs(m_SnapPos.y)>GridSize) {Move.y=(float)_copysign(GridSize, m_SnapPos.y); m_SnapPos.y-=(float)_copysign(GridSize, m_SnapPos.y); }
		if(fabs(m_SnapPos.z)>GridSize) {Move.z=(float)_copysign(GridSize, m_SnapPos.z); m_SnapPos.z-=(float)_copysign(GridSize, m_SnapPos.z); }
	} else 
		m_SnapPos.x=m_SnapPos.y=m_SnapPos.z=0.0f;



	if(!MoveFunction) ModelMove(Trans, Move);
	else 
		MoveFunction(Trans, Move, false);


	D3DXMatrixRotationYawPitchRoll(&CameraState, CameraYaw, CameraPitch, CameraRoll);
	MousePitch=MouseYaw=MouseRoll=0.0f;
	MouseMoveX=MouseMoveY=MouseMoveZ=CameraMoveZ=0;
	D3DXMatrixRotationYawPitchRoll(&EditList[ModelSelected].State, EditList[ModelSelected].Roll, EditList[ModelSelected].Pitch, -EditList[ModelSelected].Yaw);
	
	RenderPhase=true;

	D3DXVECTOR3 CamPos(CameraX, CameraY, CameraZ);
	DX->BeginScene(&CameraState, &CamPos, Mode3D);
	DX->SetupLights();



	// Draw all models
	for(int a=0; a<MAX_EDITABLE_OBJECTS; a++){
		if(EditList[a].LightList && ViewEnable[a]) E_SetupLights(&EditList[a],a, &EditList[a].State, &EditList[a].Pos);
	}


	for(int a=0; a<MAX_EDITABLE_OBJECTS; a++){ 
		if(EditList[a].NodeNr && ViewEnable[a]) {
			E_DrawModel(&EditList[a], a, &EditList[a].State, &EditList[a].Pos, FrameEnable);
			E_Light	*l=EditList[a].LightList;
			while(l && TheLightEdit->Active){
				if((!l->Selected)||((l->Selected)&&(GetTickCount()&0x80)))
					E_DrawLight(&l->Light, &EditList[a].State, &EditList[a].Pos);
				l=l->Next;
			}

			if(a==ModelSelected && SelectionFrameTime){
				D3DXMATRIX	State=EditList[a].State;
				State.m30=EditList[a].Pos.x;
				State.m31=EditList[a].Pos.y;
				State.m32=EditList[a].Pos.z;
				DX->DrawFrame(ObjectFrame, 24, &State);
			}
		}

	}

	DX->EndDraws();

	if(SelectionFrameTime) SelectionFrameTime--;

	D3DXMatrixRotationYawPitchRoll(&CameraState, CameraYaw, CameraPitch, CameraRoll);
	CameraState.m30=EditList[ReferenceModel].Pos.x;
	CameraState.m31=EditList[ReferenceModel].Pos.y;
	CameraState.m32=EditList[ReferenceModel].Pos.z;
	if(GridEnable) DX->DrawGrid(&CameraState);

	DX->EndScene();

	RenderPhase=false;

}




void CALLBACK EXPORT TimedRendering(  HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
{
	 MainApp->Draw();
}




BOOL PumpMessage(MSG *m_msgCur)
{

	if (!::GetMessage(m_msgCur, NULL, NULL, NULL))
	{
		return FALSE;
	}

	::TranslateMessage(m_msgCur);
	::DispatchMessage(m_msgCur);
	return TRUE;
}


void IdleMode(void)
{
	MSG msg;
    while ( ::PeekMessage( &msg, NULL, NULL, NULL, PM_NOREMOVE ) ) 
    { 
        if ( !PumpMessage(&msg) ) 
        { 
            ::PostQuitMessage(WM_DESTROY); 
            break; 
        } 
    } 
}

void Delay(DWORD mSec)
{
	DWORD EndTime=GetTickCount()+mSec;
	while(GetTickCount()<EndTime){
		IdleMode();
	}
}

void CLODConvertApp::OnSetgridsize()
{
	// TODO: Add your command handler code here

	CString	NewGrid;
	CString Mex="Input the new Grid Unit";
	CInputBox	*Box;
	Box=new CInputBox(m_pMainWnd, &NewGrid, &Mex );
	int Result=Box->DoModal();
	delete Box;
	if(Result==IDOK) {
		GridSize=(float)atof(NewGrid);
		DX->CreateGrid(GridSize, 1000, 0x40808080);
		LastGridDone=GridSize;
	}

}


void CLODConvertApp::OnOptionsEnablescript()
{
	CMenu	*Menu=MainWin->GetMenu();
	int	State=Menu->GetMenuState(ID_OPTIONS_ENABLESCRIPT, MF_CHECKED);
	State^=MF_CHECKED;
	Menu->CheckMenuItem(ID_OPTIONS_ENABLESCRIPT, State);
	TheDXEngine.SetScripts((State)?true:false);
	
}

void CLODConvertApp::OnOptionsSelectionmode()
{
	TheSelector.SetSelectMode();
}

void CLODConvertApp::OnOptionsBackculling()
{
	CMenu	*Menu=MainWin->GetMenu();
	int	State=Menu->GetMenuState(ID_OPTIONS_BACKCULLING, MF_CHECKED);
	State^=MF_CHECKED;
	Menu->CheckMenuItem(ID_OPTIONS_BACKCULLING, State);
	TheDXEngine.EnableCull((State)?true:false);
}

void CLODConvertApp::OnEditHideselected()
{
	SelectList.HideSelection();
	SurfaceEditing.UpdateEditValues(&SelectList);
}

void CLODConvertApp::OnEditDisablehide()
{
	// TODO: Add your command handler code here
	SelectList.UnHideSelection();
	SurfaceEditing.UpdateEditValues(&SelectList);
}

void CLODConvertApp::OnEditClearselection()
{
	// TODO: Add your command handler code here
	SelectList.ClearSelection();
	SurfaceEditing.UpdateEditValues(&SelectList);
	SelectList.UpdateTexUsedList();
	TheTextureEdit->UpdateValues();
	
}

void CLODConvertApp::OnEditEdit()
{
	if(EditMode==VERTEX) VertexEditing.EditVertex(&SelectList, (void**)&OSelected, &VSelected);
	else SurfaceEditing.EditSurface((void**)&OSelected, &SSelected, &SelectList);
}

void CLODConvertApp::OnEditSun()
{
	// TODO: Add your command handler code here
	new CSunEditing();
}

void CLODConvertApp::OnVertexMode()
{
	EditMode=VERTEX;
	VSelected=E_SelectVertexInSurfaces(&EditList[ModelSelected], &SelectList, true, m_SingleVertexMode);
}
void CLODConvertApp::OnFaceMode()
{	
	EditMode=FACE;
	VSelected=NULL;
	VSelected=E_SelectVertexInSurfaces(&EditList[ModelSelected], &SelectList, false,  m_SingleVertexMode);
}
void CLODConvertApp::OnJointFaceMode()
{	
	EditMode=JOINTFACE;
	VSelected=NULL;
	VSelected=E_SelectVertexInSurfaces(&EditList[ModelSelected], &SelectList, false,  m_SingleVertexMode);
}

void CLODConvertApp::OnEditModeltree()
{
	TheTreeViews->ShowModelTree(ModelSelected);
}

void CLODConvertApp::OnEditEditvertex()
{

	EditMode=VERTEX;
	if(!m_NSelected) return;
	ToolBar->GetToolBarCtrl().CheckButton(ID_VERTEXMODE, true);
	//VSelected=E_SelectVertexInSurfaces(&EditList[ModelSelected], &SelectList, true,  m_SingleVertexMode);
	VSelected=&((DXTriangle*)m_NSelected)->Vertex[m_VSelected];
	VertexEditing.EditVertex(&SelectList, (void**)&OSelected, &VSelected);
}

void CLODConvertApp::OnEditEditsurface()
{
	// TODO: Add your command handler code here
	EditMode=JOINTFACE;
	VSelected=NULL;
	ToolBar->GetToolBarCtrl().CheckButton(ID_JOINTFACEMODE, true);
	VSelected=E_SelectVertexInSurfaces(&EditList[ModelSelected], &SelectList, false,  m_SingleVertexMode);
	SurfaceEditing.EditSurface((void**)&OSelected, &SSelected, &SelectList);
}

void CLODConvertApp::OnFileSave()
{
	E_Object	*Obj=&EditList[ModelSelected];
	// create the encoder
	Encoder	Encoding(Obj->Name);	

	if(!Encoding.Initialize(Obj)){
		MessageBox(NULL, "Failed to create Model Encoder !!!", "Save Error", MB_ICONERROR);
		return;
	}

	CString	FileName=Obj->Name;
	if(CWinApp::DoPromptFileName(FileName,IDS_DXM_SAVE,0,true,NULL)){
		
		Encoding.Encode(Obj->Header.Id, Obj->Header.VBClass);
	
		DWORD	FileSize=Encoding.GetModelSize();
		void	*FileBuffer=Encoding.GetModelBuffer();

		CFile File;
		File.Open(FileName, CFile::modeCreate|CFile::modeWrite, &CFileException(CFileException::generic, NULL));
		Obj->Name=File.GetFileTitle();
		File.Write(FileBuffer, FileSize);
		File.Close();
		TheModelPath=FileName;
	}
}

void CLODConvertApp::OnEditMakeabackup()
{
	E_MakeModelCopy(&SelectList, &EditList[ModelSelected], &BackList[ModelSelected]);
}

void CLODConvertApp::OnEditRestoreabackup()
{
	SelectList.ClearSelection();
	gSSelected=NULL;
	VSelected=NULL;
	E_MakeModelCopy(&SelectList, &BackList[ModelSelected], &EditList[ModelSelected]);
}

void CLODConvertApp::OnEditswitches()
{
	SwitchesPanel->ActivateWindow();
}

CDocument* CLODConvertApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
	
	FileOpen(lpszFileName);
	return CWinApp::OpenDocumentFile(lpszFileName);

}
void CLODConvertApp::OnOptionsSnaptogrid()
{
	m_Snap^=true;
	CMenu	*Menu=MainWin->GetMenu();
	Menu->CheckMenuItem(ID_OPTIONS_SNAPTOGRID, (m_Snap)?MF_CHECKED:MF_UNCHECKED);
}

void CLODConvertApp::OnOptionsSinglevertexmode()
{
	m_SingleVertexMode^=true;
	CMenu	*Menu=MainWin->GetMenu();
	Menu->CheckMenuItem(ID_OPTIONS_SNAPTOGRID, (m_SingleVertexMode)?MF_CHECKED:MF_UNCHECKED);
}

void CLODConvertApp::OnEditEdittexture()
{
	// TODO: Add your command handler code here
	TheTextureEdit->EditTex(&SelectList);
}

void CLODConvertApp::OnFile3dstool()
{
	C3DSImport *Import;
	Import=new C3DSImport(&EditList[ModelSelected]);
	E_CalculateModelVNormals(&EditList[ModelSelected],0.45f);
	SetupModel();
}

void CLODConvertApp::OnOptionsLockselection()
{
	m_LockSelection^=true;
	CMenu	*Menu=MainWin->GetMenu();
	Menu->CheckMenuItem(ID_OPTIONS_LOCKSELECTION, (m_LockSelection)?MF_CHECKED:MF_UNCHECKED);
}

void CLODConvertApp::OnToolsAssignspecularity()
{
	DWORD	Original=0xffffffff;

	Original=GetColour(Original);

	if(Original&0xff000000) return;

	E_MakeAllSpcularity(&SelectList, Original);

}

void CLODConvertApp::OnEditEditlights()
{
	TheLightEdit->EditLight(EditList[ModelSelected].LightList);
}

void CLODConvertApp::OnFileTexturesfolder()
{
	if(TheModelPath=="") return;
	
	CString Dir=TheModelPath;
	Dir=Dir.Left(Dir.ReverseFind('.'))+"_DDS";
	CreateDirectory(Dir, NULL);

	DeleteFile(Dir+"\\*.*");
	CString Source=FalconObjectDataDir;

	for(DWORD i=0; i<EditList[ModelSelected].TexNr; i++){
		
		CString SName, DName;
		SName.Format("%s\\KoreaObj\\%d.dds", FalconObjectDataDir, EditList[ModelSelected].Textures[i]);
		DName.Format("%s\\%d.dds", Dir, EditList[ModelSelected].Textures[i]);

		CopyFile(LPCTSTR(SName), LPCTSTR(DName), false);
	}



}

void CLODConvertApp::OnNVG()
{
	m_NVG^=true;
	ToolBar->GetToolBarCtrl().CheckButton(ID_NVGBUTTON, m_NVG);
	//TheDXEngine.SetNVG(m_NVG);
}

void CLODConvertApp::OnEditCopy()
{
	E_CopySelection(&SelectList);
}

void CLODConvertApp::OnEditKill()
{
	E_KillSelection(&EditList[ModelSelected], &SelectList);
}


void CLODConvertApp::OnEditPaste()
{
	DXNode *End=E_GetModelEnd(&EditList[ModelSelected]);
	if(End)	E_PasteSelection(&EditList[ModelSelected], &SelectList, End);
}

void CLODConvertApp::OnEditCut()
{
	E_CutSelection(&EditList[ModelSelected], &SelectList);
}

void CLODConvertApp::OnEditInverselection()
{
	m_VSelected=NULL;
	gVSelected=NULL;
	E_SelectCommonVertex(&EditList[ModelSelected], &SelectList);

}

void CLODConvertApp::OnFaceselectionRemoveemissivecolor()
{
	DWORD Emissive=0;
	E_ClearFaceSelection(&SelectList, &Emissive);
}

void CLODConvertApp::OnFaceselectionMakeemissive()
{
	DWORD Emissive=0xffffff;
	E_ClearFaceSelection(&SelectList, &Emissive);
}

void CLODConvertApp::OnFaceselectionRemovezbias()
{
	DWORD ZBias=0;
	E_ClearFaceSelection(&SelectList, NULL, &ZBias);
}
