// LODConvert.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include <io.h>
#include <fcntl.h>
#include "BSPNodes.h"
#include "LODConvert.h"
#include "MainFrm.h"
#include "PalBank.h"
#include "TexBank.h"
#include "ObjectLOD.h"
#include "ObjectParent.h"
#include "Tools.h"
#include "DXLlist.h"
#include "VBList.h"
#include "AskBox.h"

#include "LODConvertDoc.h"
#include "LODConvertView.h"
#include "converter.h"
#include "DXEngine.h"
#include "DBConverter.h"
#include ".\lodconvert.h"
void CALLBACK EXPORT TimedRendering(  HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime);


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

PaletteBankClass	ThePaletteBank;
TextureBankClass	TheTextureBank;
ObjectParent		TheObjectParent;
CWnd				*MainWin;
bool				CullEnable,DofMoveEnable,DrawDisable, IHRSaved;
CTools				*ToolsWindow;
DOFvalue			DOFs[128];
UInt32				SWITCHes[128];
CVbList				*VBList;

D3DVERTEX vBuffer[MAX_VERTICES];
int		VertCount;
DWORD	Textures[1024];
int		TexCount;
Pcolor	*pPalette;
float	Fps;
DWORD	NItems;
bool	EnableZBias;
float	GlobalBias;
CDXLList	*DXLLister;
DWORD	gClass, gDomain, gType, gPLevel;
Encoder	*ECD;
CString	TheModelName;
// ************** Global Variables for the App ***************
char	FalconDataDirectory[512];								// Do Ya read the name...?
char	FalconTerrainDataDir[512];
char	FalconObjectDataDir[512];
char	FalconMiscTexDataDir[512];

Converter	*CV;												// the BSP->Intermediate Converter
DXDevice	*DX;												// the DX Device
CDBConverter	*DBCV;

float	ObjectRoll, ObjectPitch, ObjectYaw;
float	MousePitch, MouseRoll, MouseYaw;
float	MouseMoveX, MouseMoveY, MouseMoveZ;
float	ObjectX,ObjectY,ObjectZ;

// CLODConvertApp

BEGIN_MESSAGE_MAP(CLODConvertApp, CWinApp)
	ON_COMMAND(ID_DXKEY, OnDXKey)
	ON_COMMAND(ID_VBKEY, OnVbKey)
	ON_COMMAND(IDD_COLORKEY, OnColorKey)
	ON_COMMAND(IDD_ZBIAS, OnZBiasKey)
	ON_COMMAND(ID_INFO, OnInfoKey)
	ON_COMMAND(IDD_MULTI, OnMultiKey)
	ON_COMMAND(IDC_CULLKEY, OnCullKey)
	ON_COMMAND(ID_DOFMOVE, OnDofMoveKey)
	ON_COMMAND_RANGE(IDD_IN, IDD_OUT, OnDistance)
	ON_COMMAND_RANGE(IDD_LEFT, IDD_RIGHT, OnDirection)
	ON_COMMAND(ID_APP_PLANE, OnAppPlane)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_FILESAVE, OnFileSave)
END_MESSAGE_MAP()





// CLODConvertApp construction

CLODConvertApp::CLODConvertApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	TagList=(BNodeType*)malloc(MAX_TAGS*sizeof(BNodeType));
	LODBuffer=malloc(LOD_BUFFER_SIZE);
	CullEnable=true;
	DofMoveEnable=false;
	NItems=1;
	EnableZBias=true;
	DBCV=NULL;
	DrawDisable=false;
	
	ObjectRoll=ObjectPitch=ObjectYaw=0.0f;
	MousePitch=MouseRoll=MouseYaw=0.0f;
	MouseMoveX=MouseMoveY=MouseMoveZ=0.0f;
	ObjectX=ObjectY=0; ObjectZ=50.0f;

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
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
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


/*	#ifdef USE_SH_POOLS
	if ( gBSPLibMemPool == NULL )
	{
		gBSPLibMemPool = MemPoolInit( 0 );
	}
	#endif*/

	ZeroMemory(DOFs, sizeof(DOFs));
	ZeroMemory(SWITCHes, sizeof(SWITCHes));

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
   


	char	filename[512];

	// COBRA - RED - BSP Palette LOAD //
	ZeroMemory(filename, sizeof(filename));
	strcpy( filename, FalconObjectDataDir );
	strcat( filename, "\\KoreaObj.HDR" );
	CFile	File;
	if(!File.Open(filename, CFile::modeRead, NULL)){
		MessageBox(NULL, "File 'KoreaObj.hdr' missing!", "File Error", 0);
		return(false);
	}
	DWORD FileSize=(DWORD)File.GetLength();
	void *FileBuffer=malloc(FileSize);
	File.Read(FileBuffer, FileSize);
	File.Close();
	DWORD Palettesize=*(((DWORD*)FileBuffer)+1);
	pPalette=(Pcolor*)malloc(Palettesize*sizeof(Pcolor));
	memcpy((void*)pPalette,(((DWORD*)FileBuffer)+3),Palettesize*sizeof(Pcolor));
	free(FileBuffer);

	ZeroMemory(filename, sizeof(filename));
	strcpy( filename, FalconObjectDataDir );
	strcat( filename, "\\KoreaObj" );
	TheObjectParent.SetupTable(filename);

	ToolsWindow=new CTools();


	// Here ceates the DX Device
	DX=new DXDevice(m_pMainWnd);

	m_Rot=0.0f;
	m_Pitch=0.0f;
	m_Dist=50.0f;

	gClass=gDomain=gType=gPLevel=0;
	TheModelName="";

	SetTimer(NULL, 0, 40, TimedRendering);
	return TRUE;
}


void	CLODConvertApp::SetupDBConvert(void)
{
	if(!DBCV) {
		CString path=FalconObjectDataDir;
		path+="\\";
		// DBConverter, but no window
		DBCV=new CDBConverter(m_pMainWnd, false);
	}
	
	// Compile the HDR File
	char	szFile[512];
	CFile	fp;
	sprintf(szFile,"%s\\KoreaObj.hdr", FalconObjectDataDir); 
	fp.Open(szFile, CFile::modeRead);
	DBCV->LoadHDR(fp);
	fp.Close();
	// Compile the Names
	DBCV->CompileCTNames(FalconObjectDataDir);
	sprintf(DBCV->KoreaObjFolder,"%s\\", FalconObjectDataDir); 
	DBCV->LoadCT();

}


void	CLODConvertApp::OnFileOpen()
{
	CString	FileName;
#ifdef __LIMITED__
	CFileDialog F(true, NULL, NULL, OFN_OVERWRITEPROMPT, "LOD Files|*.Lod|");
#else
	CFileDialog F(true, NULL, NULL, OFN_OVERWRITEPROMPT, "LOD Files|*.Lod|IHR Files|*.ihr|");
#endif
	F.DoModal();
	if((FileName=F.GetPathName())!=""){
		FileName.MakeUpper();
		if(FileName.Right(3)=="IHR") RunIHRFile(FileName);
		else {
			CString sParent, sLodLevel;
			CAskBox *Ask=new CAskBox("Type PARENT#", &sParent);
			Ask->DoModal();
			delete	Ask;
			Ask=new CAskBox("Type LOD LEVEL(1-6)#", &sLodLevel);
			Ask->DoModal();
			delete	Ask;

			SetupDBConvert();

			DWORD Parent=atoi(sParent);
			DWORD LL=atoi(sLodLevel);

			CFile File;
			if(Parent && LL>=1 && LL<=6 && File.Open(FileName, CFile::modeRead, &CFileException(CFileException::generic, NULL))){
				
				IHRSaved=false;
				LL-=1;

				DWORD LodID=DBCV->GetLod(Parent, LL);
				
				int			FileType;
				int			Domain, Class, Type, CTNumber, CTIndex, LodLevel, ParentLevel;
				DWORD		MType;
				CString		LODName;
				// Get Domain, Class and File Type, if LOD has a CT record
				// if no CT record, then Domain = Class = FileType = 0
				FileType = DBCV->LODUsedByCT((int) LodID, (int*) &Domain, (int*) &Class, (int*)&Type, (int*)&CTNumber, (int*)&CTIndex, (int*)&LodLevel, (int*)&ParentLevel, &LODName, &MType);

				// Assign to Global Variables				
				gClass=Class;
				gDomain=Domain;
				gType=Type;
				gPLevel=ParentLevel;

				DWORD VBClass;
				VBClass=VB_CLASS_FEATURES;
				if(Class==7) VBClass=VB_CLASS_DOMAIN_GROUND;
				if(Class==7 && Domain==2) VBClass=VB_CLASS_DOMAIN_AIR;

				LODSize=(DWORD)File.GetLength();
				void *FileBuffer=malloc(LODSize);
				File.Read(FileBuffer, LODSize);
				FileTitle=File.GetFileTitle();
				File.Close();

				TagCount=*(int*)FileBuffer;
				memcpy(TagList,(int*)FileBuffer+1,TagCount*sizeof(*TagList));
				LODSize=LODSize-sizeof(int)-TagCount*sizeof(*TagList);
				memcpy(LODBuffer,(char*)FileBuffer+sizeof(int)+TagCount*sizeof(*TagList),LODSize);
				
				LodInfo=new CLodInfo(MainWin, LODName, new Converter(), (BRoot*) BNode::RestorePointers((BYTE*)LODBuffer, 0, &TagList), TagList, TagCount, Class, CTNumber, MType, VBClass);
				free(FileBuffer);
				while(!IHRSaved) IdleMode();
				delete LodInfo;
			}
			delete DBCV;
			DBCV=NULL;
			File.Close();

			/*
			CFile File;
			File.Open(FileName, CFile::modeRead, &CFileException(CFileException::generic, NULL));
			LODSize=(DWORD)File.GetLength();
			void *FileBuffer=malloc(LODSize);
			File.Read(FileBuffer, LODSize);
			FileTitle=File.GetFileTitle();
			File.Close();


			TagCount=*(int*)FileBuffer;
			memcpy(TagList,(int*)FileBuffer+1,TagCount*sizeof(*TagList));
			LODSize=LODSize-sizeof(int)-TagCount*sizeof(*TagList);
			memcpy(LODBuffer,(char*)FileBuffer+sizeof(int)+TagCount*sizeof(*TagList),LODSize);
			
			LodInfo=new CLodInfo(NULL, FileTitle, new Converter(), (BRoot*) BNode::RestorePointers((BYTE*)LODBuffer, 0, &TagList), TagList, TagCount);
			free(FileBuffer);*/

		}
	}
}



void	CLODConvertApp::RunIHRFile(CString FileName)
{
		// Get the IHR File
		CStdioFile File;
		if(File.Open(FileName, CFile::modeRead, &CFileException(CFileException::generic, NULL))){


			SetupDBConvert();

			CString Data;
			CString Name;
			DWORD	CT;
			DWORD   PType;
			DWORD	Parent;

			File.ReadString(Data); // skip unuseful Data
			File.ReadString(Name); // The Name
			File.ReadString(Data); // CT#
			char data[128];
			strcpy(data, Data);
			char *token=strtok(data," ");
			CT=atoi(token);
			token=strtok(NULL," ");
			PType=atoi(token);
			token=strtok(NULL," ");
			Parent=atoi(token);

			// Till End of File or NLODS label
			while(File.GetPosition()<File.GetLength() && Data.Left(5)!="NDYNX" ) { File.ReadString(Data); Data.Trim(); }
			// if not found the NLODS record, exit here
			if(Data.Left(5)!="NDYNX") goto Close;
			DWORD NDYNX=atoi(Data.Right(Data.GetLength()-6));
			// Till End of File or NLODS label
			while(File.GetPosition()<File.GetLength() && Data.Left(5)!="NLODS" ) { File.ReadString(Data); Data.Trim(); }
			// if not found the NLODS record, exit here
			if(Data.Left(5)!="NLODS") goto Close;
			DWORD NLODS=atoi(Data.Right(Data.GetLength()-6));
			// Till End of File or #LAST label
			// Till End of File or NLODS label
			while(File.GetPosition()<File.GetLength() && Data.Left(5)!="NSLOT" ) { File.ReadString(Data); Data.Trim(); }
			// if not found the NLODS record, exit here
			if(Data.Left(5)!="NSLOT") goto Close;
			DWORD NSLOT=atoi(Data.Right(Data.GetLength()-6));
			while(File.GetPosition()<File.GetLength() && Data.Left(5)!="#LAST" ) { File.ReadString(Data); Data.Trim(); }
			// if not found the #LAST record, exit here
			if(Data.Left(5)!="#LAST") goto Close;

			for(DWORD l=0; l<NDYNX+NSLOT; l++)File.ReadString(Data);
			
			// The LOD LEvel
			DWORD LL=0;
			// ok, from here we r at start of LODS data... order is Distance/LOD#/ lod indication
			for(DWORD l=0; l<NLODS; l++){
				File.ReadString(Data);
				float	Distance=(float)atof(Data);
				File.ReadString(Data);
				Data.Trim();
				// If replacing a LOD ( Lod Number not assigned
				if(atoi(Data)==-1){
					DWORD N=Data.Find(" ");
					Data=Data.Right(Data.GetLength()-N);
					Data.Trim();

					// load each model to be converted
					CString	Path=FileName.Left(FileName.ReverseFind('\\'));
					CFile File;
					if(File.Open(Path+'\\'+Data, CFile::modeRead, &CFileException(CFileException::generic, NULL))){
						
						IHRSaved=false;
						DWORD LodID=DBCV->GetLod(Parent, LL);

						
						int			FileType;
						int			Domain, Class, Type, CTNumber, CTIndex, LodLevel, ParentLevel;
						DWORD		MType;
						CString		LODName;
						// Get Domain, Class and File Type, if LOD has a CT record
						// if no CT record, then Domain = Class = FileType = 0
						FileType = DBCV->LODUsedByCT((int) LodID, (int*) &Domain, (int*) &Class, (int*)&Type, (int*)&CTNumber, (int*)&CTIndex, (int*)&LodLevel, (int*)&ParentLevel, &LODName, &MType);

						// Assign to Global Variables				
						gClass=Class;
						gDomain=Domain;
						gType=Type;
						gPLevel=ParentLevel;

						DWORD VBClass;
						VBClass=VB_CLASS_FEATURES;
						if(Class==7) VBClass=VB_CLASS_DOMAIN_GROUND;
						if(Class==7 && Domain==2) VBClass=VB_CLASS_DOMAIN_AIR;

						LODSize=(DWORD)File.GetLength();
						void *FileBuffer=malloc(LODSize);
						File.Read(FileBuffer, LODSize);
						FileTitle=File.GetFileTitle();
						File.Close();

						TagCount=*(int*)FileBuffer;
						memcpy(TagList,(int*)FileBuffer+1,TagCount*sizeof(*TagList));
						LODSize=LODSize-sizeof(int)-TagCount*sizeof(*TagList);
						memcpy(LODBuffer,(char*)FileBuffer+sizeof(int)+TagCount*sizeof(*TagList),LODSize);
						
						LodInfo=new CLodInfo(MainWin, LODName, new Converter(), (BRoot*) BNode::RestorePointers((BYTE*)LODBuffer, 0, &TagList), TagList, TagCount, Class, CTNumber, MType, VBClass);
						free(FileBuffer);
						while(!IHRSaved) IdleMode();
						delete LodInfo;
					}
				}
				LL++;
			}

	Close:
			delete DBCV;
			DBCV=NULL;
			File.Close();

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

void CLODConvertApp::OnAppPlane()
{
	DX->TogglePlane();
}

void CLODConvertApp::OnDirection( UINT ID)
{

	if(ID==IDD_LEFT) m_Rot+=0.05f;
	if(ID==IDD_RIGHT) m_Rot-=0.05f;


	if(ID==IDD_UP) m_Pitch-=0.05f;
	if(ID==IDD_DN) m_Pitch+=0.05f;
	if(m_Pitch<=-PI) m_Pitch=PI;
	if(m_Pitch>PI) m_Pitch=-PI;
	if(m_Rot<=-PI) m_Rot=PI;
	if(m_Rot>PI) m_Rot=-PI;

	DX->SetVectors(m_Rot, 0, m_Pitch, ObjectX, ObjectY, ObjectZ);
}

void CLODConvertApp::OnDistance( UINT ID)
{
	if(ID==IDD_OUT) m_Dist+=0.5f;
	if(ID==IDD_IN) m_Dist-=0.5f;
	DX->SetVectors(m_Rot, 0, m_Pitch, ObjectX, ObjectY, ObjectZ);
}

void CLODConvertApp::OnCullKey()
{
	CullEnable^=1;
	TheDXEngine.EnableCull(CullEnable);
}

void CLODConvertApp::OnDofMoveKey()
{
	DofMoveEnable^=1;
	TheDXEngine.MoveDof(DofMoveEnable);
}

void CLODConvertApp::OnMultiKey()
{
	NItems++;
	NItems%=9;
	if(NItems==0) NItems++;
}

void CLODConvertApp::OnInfoKey()
{
	DXLLister = new CDXLList();
}


void CLODConvertApp::OnZBiasKey()
{
	EnableZBias^=true;
	TheDXEngine.UseZBias=EnableZBias;
}

void CLODConvertApp::OnColorKey()
{
	new CColorTool();
}

void CLODConvertApp::OnVbKey()
{
	/*if(!VBList)*/ VBList=new CVbList();
}

void CLODConvertApp::OnDXKey()
{
	if(!DBCV) {
		CString path=FalconObjectDataDir;
		path+="\\";
		DBCV=new CDBConverter(m_pMainWnd);
		DBCV->Convert(path);
		delete DBCV;
		DBCV=NULL;
	}
}
void CALLBACK EXPORT TimedRendering(  HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
{
	if(DrawDisable) return;
	ObjectYaw+=MouseYaw;
	ObjectPitch+=MousePitch;
	ObjectX+=MouseMoveX;
	ObjectY+=MouseMoveY;
	ObjectZ+=MouseMoveZ;
	MousePitch=MouseYaw=MouseRoll=0.0f;
	MouseMoveX=MouseMoveY=MouseMoveZ=0;
	DX->SetVectors(ObjectRoll, ObjectYaw, ObjectPitch, ObjectX, ObjectY, -ObjectZ);
	DX->Render();
	CString s;
	s.Format("Obj #%d - ",NItems);
	s+=DX->GetTimings();
	s+=(CullEnable)?"  -  CCW Culling":"  -  NO Culling";
	CString	v;

	v.Format(" - Draws : %d  Vertices : %d  Tex Switch : %d  States : %d", TheDXEngine.DXDrawCalls, TheDXEngine.DXDrawVertices,
		TheDXEngine.DXTexSwitches, TheDXEngine.DXStateChanges);
	s+=v;
	v=(EnableZBias)?" - ZBIAS ON":" - ZBIAS OFF";
	s+=v;
	MainWin->SetWindowText(s);		
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

void CLODConvertApp::OnFileSave()
{
	if(!ECD) return;
	CString	FileName="*.DXM";
	if(TheModelName!="") FileName=TheModelName+".DXM";

	IHRSaved=true;

	if(CWinApp::DoPromptFileName(FileName,IDS_DXM_SAVE,0,true,NULL)){
			
		DWORD	FileSize=ECD->GetModelSize();
		void	*FileBuffer=ECD->GetModelBuffer();

CheckVBClass:
		if(!((DxDbHeader*)FileBuffer)->VBClass){
			CVBAskClass	Ask;
			DWORD	Class=Ask.DoModal();
			if(Class==IDCANCEL) return;
			goto CheckVBClass;
		}

		CFile File;
		File.Open(FileName, CFile::modeCreate|CFile::modeWrite, &CFileException(CFileException::generic, NULL));
		File.Write(FileBuffer, FileSize);
		File.Close();
	}

}
