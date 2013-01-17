// DXDbToolDlg.cpp : file di implementazione
//

#include "stdafx.h"
#include "DXDbTool.h"
#include "DXDbToolDlg.h"
#include ".\dxdbtooldlg.h"
//#include "lodconvert.h"
#include "DXDefines.h"
#include "DXFiles.h"
#include "encoder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

E_Object	TheModel,SpareModel;
CStdioFile	fLog, fLogU, fErrors,fWarnings, fTex;
DWORD		TexStatistics[MAX_TEXTURE];
bool		TexMissing[MAX_TEXTURE];
bool		ModelErrorOut, ModelWarningOut;
HDRPARENT	ModelParent;
CT			ModelCT;

// finestra di dialogo CAboutDlg utilizzata per visualizzare le informazioni sull'applicazione.



class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dati della finestra di dialogo
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // supporto DDX/DDV

// Implementazione
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



void *CDXDbToolDlg::OpenModel(CString FileName)
{
	CFile	File;
	void *ModelPtr=NULL;
	if(File.Open(FileName, CFile::modeRead, NULL)){
		DWORD FileSize=(DWORD)File.GetLength();
		ModelPtr=malloc(FileSize);
		File.Read(ModelPtr, FileSize);
		File.Close();
		TheModel.Name=FileName;
	}
	return ModelPtr;

}


// finestra di dialogo CDXDbToolDlg

void CDXDbToolDlg::ReplaceModel(CString FileName, DWORD ParentNr, DWORD Lod)
{
	CFile	File;
	void *ModelPtr;
//	if(!(ModelPtr=OpenModel(FileName))) { MessageBox("DXM not Found", 0); return; }
	LoadHDR(FalconDir+"\\KoreaObj.DXH",  true);
	if(LoadCT(FalconDir+"\\")<=(int)ParentNr) { MessageBox("Bad Parent", 0); return; }
	if(Lod>=7) { MessageBox("Bad LOD Level", 0); return; }

	DWORD	Domain=GetDomain(ParentNr);
	DWORD	Class=GetClass(ParentNr);
	DWORD	Type=GetType(ParentNr);
	DWORD	ID=GetParentLodID(ParentNr, Lod);
	if(ID==-1) { MessageBox("Bad LOD Level", 0); return; };
	// Import the Model
	ImportModel(&TheModel, ModelPtr, Class, ID);
	// Get its offset in the DB File
	DWORD	EntryOffset=GetFileOffset(ID);

	CFile	DBFile, OutFile;
	void	*Buffer=malloc(64*1024);
    if(!DBFile.Open(FalconDir+"\\KoreaObj.DXL", CFile::modeRead)) { MessageBox("DXL File not found", 0); return; }
	OutFile.Open("Temp.DXL", CFile::modeCreate|CFile::modeWrite);

}

void CDXDbToolDlg::ExecuteCommand(CString Command, CString Arg1, CString Arg2, CString Arg3)
{	
	// Evaluate command
	if(Command=="REPLACE") ReplaceModel(Arg1, atoi(Arg2), atoi(Arg3));

}

CDXDbToolDlg::CDXDbToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDXDbToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDXDbToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DBFOLDER, m_DbFolderText);

	CString Title;
	Title.Format("DxDbTool - Db Models Rev %d", MODEL_VERSION);

	SetWindowText(Title);
	m_DbFolderText.SetWindowText(DBPath);
	// Load the HDR	
	LoadHDR(DBPath);

	DDX_Control(pDX, IDC_PROGRESSBAR, m_ProgressBar);
	DDX_Control(pDX, IDC_STATUSTEXT, m_StatusText);
	DDX_Control(pDX, IDC_BACKUPCHECK, m_BackupCheck);

	m_BackupCheck.SetCheck(1);
	DDX_Control(pDX, IDC_FORCECHECK, m_ForceCheck);
	DDX_Control(pDX, IDC_SIGNATURESCALC, m_Signatures);
}

BEGIN_MESSAGE_MAP(CDXDbToolDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_DXHBUTTON, OnBnClickedDxhbutton)
	ON_BN_CLICKED(IDC_DXMBUTTON, OnBnClickedDxmbutton)
	ON_BN_CLICKED(IDC_UPDATEBUTTON, OnBnClickedUpdatebutton)
END_MESSAGE_MAP()


// gestori di messaggi di CDXDbToolDlg

BOOL CDXDbToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Aggiungere la voce di menu "Informazioni su..." al menu di sistema.

	// IDM_ABOUTBOX deve trovarsi tra i comandi di sistema.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Impostare l'icona per questa finestra di dialogo. Il framework non esegue questa operazione automaticamente
	//  se la finestra principale dell'applicazione non è una finestra di dialogo.
	SetIcon(m_hIcon, TRUE);			// Impostare icona grande.
	SetIcon(m_hIcon, FALSE);		// Impostare icona piccola.

	// TODO: aggiungere qui inizializzazione aggiuntiva.
	
	return TRUE;  // restituisce TRUE a meno che non venga impostato lo stato attivo su un controllo.
}

void CDXDbToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// Se si aggiunge alla finestra di dialogo un pulsante di riduzione a icona, per trascinare l'icona sarà necessario
//  il codice sottostante. Per le applicazioni MFC che utilizzano il modello documento/vista,
//  questa operazione viene eseguita automaticamente dal framework.

void CDXDbToolDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // contesto di periferica per il disegno

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Centrare l'icona nel rettangolo client.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Disegnare l'icona
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// Il sistema chiama questa funzione per ottenere la visualizzazione del cursore durante il trascinamento
//  della finestra ridotta a icona.
HCURSOR CDXDbToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDXDbToolDlg::OnBnClickedDxhbutton()
{
	CString	FileName=DBPath;
	CFileDialog	F(true, "*.dxh", FileName, 0, "DXH Files|*.dxh");
	DWORD	rs=(DWORD)F.DoModal();
	if(rs=IDOK) DBPath=F.GetPathName();
	m_DbFolderText.SetWindowText(DBPath);
	m_StatusText.SetWindowText("Loading File....");
	Delay(500);
	// Load the HDR	
	LoadHDR(DBPath);
	m_StatusText.SetWindowText("Done...");
}



void CDXDbToolDlg::WriteModel(void *ptr, float *Radius, bool CalcSign)
{
	DWORD	nSwitches, nSlots, nDOFs;
	// Get the Model features
	GetModelFeatures(ptr, &nSwitches, &nDOFs, &nSlots);

	UpdateParentRecord(((DxDbHeader*)ptr)->Id, &nSwitches, &nDOFs, &nSlots, Radius);
	// if Signatures to rebuild
	if(CalcSign){
		// Get the parent of this model
		DWORD	Parent = GetParentUsingLOD(0, ((DxDbHeader*)ptr)->Id);
		DWORD	Class = GetClass(Parent);
		float	RadarSign = GetRadarSignature(&SpareModel);
		float	IRSign = (Class==CLASS_VEHICLE) ? RadarSign : 0.0f;
		UpdateParentSignature(((DxDbHeader*)ptr)->Id, &RadarSign, &IRSign, true);
	}
	// write the model out
	WriteNextModel(ptr);
}

void CDXDbToolDlg::OnBnClickedDxmbutton()
{

	CString	FileName="*.dxm", FileTitle;
	CString	DBName=DBPath.Left(DBPath.GetLength()-3)+"dxl";
	CFileDialog	F(true, "*.dxm", FileName, 0, "DXM Files|*.dxm");
	DWORD	Updated=0;
	DWORD	rs=(DWORD)F.DoModal();
	DWORD	ModelOffset,ModelSize;
	if(rs==IDOK) {
		FileName=F.GetPathName();
		FileTitle=F.GetFileTitle();
	}
	else{
		m_StatusText.SetWindowText("Failed....");
		return;
	}

	m_StatusText.SetWindowText("Looking for file in the DB....");
	Delay(500);
	void *ModelPtr;
	if((ModelOffset=GetFileOffset(FileTitle, &ModelParent))==-1){
		MessageBox("File Not Present in the Selected dDatabase");
		m_StatusText.SetWindowText("Failed....");
		return;
	}
	ModelSize=GetFileSize(FileTitle);

	// ok... start
	CFile	File;
	if(File.Open(FileName, CFile::modeRead, NULL)){
		DWORD FileSize=(DWORD)File.GetLength();
		void	*ptr=malloc(FileSize);
		File.Read(ptr, FileSize);
		File.Close();
		TheModel.Name=FileName;
		ModelPtr=ImportModel(&TheModel, ptr, 0, 0);

		if(m_BackupCheck.GetCheck()){
			m_StatusText.SetWindowText("Database Files Backup");
			CopyFile(DBPath, DBPath+".bak", false);
			CopyFile(DBName, DBName+".bak", false);
		}

		m_StatusText.SetWindowText("Adding/Updating....");
		InitDbaseIN(DBName);
		InitDbaseOUT("Temp.dxl");
		DWORD Version;

		m_ProgressBar.SetRange32(0, GetInSize());
		// Start, then clear parent records
		ResetParentRecords();

		// Scan all the DB
		while(ptr=GetNextModel()){

			bool	SignCalc = false;

			DxDbHeader	*h=(DxDbHeader*)ptr;
			// if different version, update it assigning same VBClass + ID
			if((Version=GetModelVersion(h))!=MODEL_VERSION && GetOutPosition()!=ModelOffset){
				ptr=ImportModel(&SpareModel, ptr, h->VBClass, h->Id);
				Updated++;
				// if checked for signaures, do it
				if(m_Signatures.GetCheck()) SignCalc=true;
			}

			// if the substituted model, substitute it and assign VB Class + ID
			if(GetInPosition()==ModelOffset){
				((DxDbHeader*)ModelPtr)->Id=h->Id;
				((DxDbHeader*)ModelPtr)->VBClass=h->VBClass;
				ptr=ModelPtr;
				Updated++;
			}
			
			WriteModel(ptr, NULL, SignCalc);
			free(ptr);
			ClearModel(&SpareModel);
			ClearModel(&TheModel);
			m_ProgressBar.SetPos(GetInPosition());
			IdleMode();

		}

		CloseDBaseIN();
		CloseDBaseOUT();
		m_StatusText.SetWindowText("Saving....");
		SaveHDR(DBPath);
		CopyFile("Temp.dxl", DBName, false);
		DeleteFile("Temp.dxl");
		m_ProgressBar.SetPos(0);

	} else MessageBox("Error in loading file");
		
	CString rep;
	rep.Format("Done... Updated models %d", Updated);
	m_StatusText.SetWindowText(rep);


}



void *CDXDbToolDlg::ImportModel(E_Object *obj, void *ptr, DWORD Class, DWORD Id)
{
	// ok... deindex and eventually update the model, kill any static light
	DeIndexModel(ptr, obj, true);
	obj->Nodes=(void*)E_SortModel((DXNode*)obj->Nodes);
	Encoder	E(obj->Name);
	E.Initialize(obj);
	E.Encode(Id, Class);
	free(ptr);
	ptr=malloc(E.GetModelSize());
	memcpy(ptr, E.GetModelBuffer(), E.GetModelSize());
	return	ptr;
}



void WriteWarning(CString ModelName, CString Error)
{
	if(!ModelWarningOut){
		fWarnings.WriteString(" * " + ModelName + " *\n");		
		ModelWarningOut=true;
	}
	fWarnings.WriteString(Error+"\n");		
}


void WriteError(CString ModelName, CString Error)
{
	if(!ModelErrorOut){
		fErrors.WriteString(" * " + ModelName + " *\n");		
		ModelErrorOut=true;
	}
	fErrors.WriteString(Error+"\n");		
}

void CDXDbToolDlg::OnBnClickedUpdatebutton()
{
	CString	DBName=DBPath.Left(DBPath.GetLength()-3)+"dxl";
	void	*ptr,*nptr;
	DWORD	Updated=0;
	DWORD	BadModels=0, BadTextures=0;
	bool	Errors=false;
	bool	ModelError;
	float	Radius;

	// Ask for textures folder
	GetTexturesFolder();
	// if no texture folder assigned exit here
	if(m_TexturesFolder=="") return;

	if(m_BackupCheck.GetCheck()){
		m_StatusText.SetWindowText("Database Files Backup");
		CopyFile(DBPath, DBPath+".bak", false);
		CopyFile(DBName, DBName+".bak", false);
	}
	

	// Ask for textures folder
	GetCDFilesFolder();
	LoadCT(m_CDFilesFolder+"\\");

	m_StatusText.SetWindowText("Updating....");

	InitDbaseIN(DBName);
	InitDbaseOUT("Temp.dxl");
	DWORD Version;

	memset(TexStatistics, 0, sizeof(TexStatistics));
	memset(TexMissing, false, sizeof(TexMissing));

	m_ProgressBar.SetRange32(0, GetLODNumber());
	// Start, then clear parent records
	ResetParentRecords();
	
	CString	ModelName;
	CFile F;

	CString FileFolder=DBPath;
	DWORD	ModelID=0, ModelOffset;
	FileFolder=FileFolder.Left(FileFolder.ReverseFind('\\'))+"\\";
	CString Log;
	Log.Format("%sActions.Log", FileFolder); 
	fLog.Open(Log, CFile::modeCreate|CFile::modeWrite|CFile::typeText);
	Log.Format("%sImports.Log", FileFolder); 
	fLogU.Open(Log, CFile::modeCreate|CFile::modeWrite|CFile::typeText);
	Log.Format("%sErrors.Log", FileFolder); 
	fErrors.Open(Log, CFile::modeCreate|CFile::modeWrite|CFile::typeText);
	Log.Format("%sWarning.Log", FileFolder); 
	fWarnings.Open(Log, CFile::modeCreate|CFile::modeWrite|CFile::typeText);
	Log.Format("DxDbTool - UPDATING \n%s\n\n\n", FileFolder);
	fLog.WriteString(Log);
	fLogU.WriteString(Log);
	DWORD	ParentNr;
	// Scan all the DB
	while(ModelID<GetLODNumber()){

		bool	SignCalc = false;
		// if checked for signaures, do it
		if(m_Signatures.GetCheck()) SignCalc=true;

		if(ModelID==3473)
			_asm nop;
		// Radius=0 deafuklts to do no update it...
		Radius=0.0f;

		ModelErrorOut=ModelWarningOut=false;
		ModelError=false;
		ModelName=GetIndexName(ModelID, &ModelParent, &ParentNr);
		DWORD	Domain=GetDomain(ParentNr);
		DWORD	Class=GetClass(ParentNr);
		DWORD	Type=GetType(ParentNr);
		Log.Format("#%04d - %-32s ",ModelID, ModelName);
		fLog.WriteString(Log);
		if(strlen(ModelName)){
			ModelOffset=GetFileOffset(ModelName);
			ptr=GetModelAtOffset(ModelOffset);
			DxDbHeader	*h=(DxDbHeader*)ptr;
			if(F.Open(FileFolder+ModelName+".DXM", CFile::modeRead)){
				// if found load new model and ssign same features
				DWORD FileSize=(DWORD)F.GetLength();
				nptr=malloc(FileSize);
				F.Read(nptr, FileSize);
				F.Close();
				((DxDbHeader*)nptr)->Id=h->Id;
				((DxDbHeader*)nptr)->VBClass=h->VBClass;
				ptr=nptr;
				h=(DxDbHeader*)nptr;
				Updated++;
				fLogU.WriteString(Log+"\n");
				Log.Format(" * IMPORTING * ");
				fLog.WriteString(Log);
			} else {
				Log.Format("               ");
				fLog.WriteString(Log);
			}

			Log.Format(" Version %d03d ", GetModelVersion(h)&0xff);
			fLog.WriteString(Log);
			DWORD	a=h->ModelSize;
			// if different version, update it assigning same VBClass + ID
			if((Version=GetModelVersion(h))!=MODEL_VERSION || m_ForceCheck.GetCheck()){
				Log.Format(" - Updating to %03d ", MODEL_VERSION&0xff);
				
				ptr=ImportModel(&SpareModel, ptr, h->VBClass, h->Id);
				
				Radius=SpareModel.Radius;

				CFileFind	ff;
				CString		TName;
				bool		First=true;
				DWORD		TexNr;


				// Number of textuers check
				if(SpareModel.TexNr < ( SpareModel.TexPerSet * ModelParent.TextureSets)){
					TName.Format("ERROR - %d TEXTURES, should be %d by %d SETS\n",SpareModel.TexNr, SpareModel.TexPerSet, ModelParent.TextureSets );
					ModelError=Errors=true;
					WriteError(ModelName, TName);
				}

				// Textures check
				for(DWORD t=0; t<SpareModel.TexNr; t++){
					TexNr=SpareModel.Textures[t];
					TName.Format("%s\\%d.dds", m_TexturesFolder, TexNr);
					
					if(TexNr!=-1) TexStatistics[ TexNr]++;

					if( TexNr==-1 || TexMissing[TexNr] || !ff.FindFile(TName)) {
						First=false;
						if(SpareModel.Textures[t]!=-1){
							TName.Format("ERROR - FILE TEXTURE MISSING %d : %d.dds\n", t,  TexNr);
							BadTextures++;
							TexMissing[TexNr]=true;
							Errors=true;
							WriteError(ModelName, TName);
						} else {
							TName.Format("WARNING - UNASSIGNED TEXTURE %d \n", t);
							WriteWarning(ModelName, TName);
						}
						
					}
				}


				// Lights check
				E_Light *Light=SpareModel.LightList;
				DWORD Dynamics=0, Statics=0, ToOthers=0, Common=0, Own=0, LightNr=0, Spots=0;
				while(Light){
					if(Light->Light.Flags.Static){
						if(	Light->Light.Light.dltType==D3DLIGHT_SPOT){
							TName.Format("ERROR - Light %d STATIC & SPOT LIGHT", LightNr);
							WriteError(ModelName, TName);
							ModelError=Errors=true;
						}
					} else {
						if(!Light->Light.Flags.OwnLight && !Light->Light.Flags.NotSelfLight){
							TName.Format("WARNING - Light %d not OWN and not TO OTHERS", LightNr);
							WriteWarning(ModelName, TName);
							Common++;
						}
						if(	Light->Light.Light.dltType==D3DLIGHT_SPOT && !Light->Light.Flags.NotSelfLight){
							TName.Format("WARNING - Light %d SPOT not TO OTHERS", LightNr);
							WriteWarning(ModelName, TName);
							Common++;
						}
						if(	Light->Light.Light.dltType==D3DLIGHT_POINT && !Light->Light.Flags.OwnLight){
							TName.Format("WARNING - Light %d OMNI not TO OWN", LightNr);
							WriteWarning(ModelName, TName);
							Common++;
						}
						if(Light->Light.Light.dltType==D3DLIGHT_SPOT) Spots++;
						Dynamics++;
						if(Light->Light.Flags.OwnLight) Own++;
						if(Light->Light.Flags.NotSelfLight) ToOthers++;
					}
					Light=Light->Next;
				}
				if(Spots>1) {
					TName.Format("ERROR - MULTIPLE (%d) SPOT LIGHTS PRESENT IN SAME MODEL", Spots);
					WriteError(ModelName, TName);
					ModelError=Errors=true;
				}
				if(Own>3){
					TName.Format("ERROR - TOO MANY (%d) 'OWN' DYNAMIC LIGHTS PRESENT IN SAME MODEL", Own);
					WriteError(ModelName, TName);
					ModelError=Errors=true;
				}
				if(ToOthers>1){
					TName.Format("ERROR - TOO MANY (%d) 'TO OTHERS' DYNAMIC LIGHTS PRESENT IN SAME MODEL", ToOthers);
					WriteError(ModelName, TName);
					ModelError=Errors=true;
				}
				if(Dynamics>4){
					TName.Format("ERROR - TOO MANY (%d) DYNAMIC LIGHTS PRESENT IN SAME MODEL", ToOthers);
					WriteError(ModelName, TName);
					ModelError=Errors=true;
				}

				if(ModelErrorOut) fErrors.WriteString("\n\n");
				if(ModelWarningOut) fWarnings.WriteString("\n\n");
				if(ModelError) BadModels++; 
			}

			h=(DxDbHeader*)ptr;

			// HACK TO MAKE DB BUILT... TO REMOVE
			//Errors=false;

			// write the model out - IF AC DO NOT WRITE CALCULATED RADIUS - OVERSIZED SHADOWS FIX
			if(!Errors) WriteModel(ptr, (Domain==DOMAIN_AIR && Class==CLASS_VEHICLE && Type==1)? NULL : &Radius, SignCalc);
			ClearModel(&SpareModel);
			ClearModel(&TheModel);
			free(ptr);
		}
		m_ProgressBar.SetPos(ModelID);
		Log.Format(" @ %08x x %06x", hdrLOD[ModelID].FileOffset, hdrLOD[ModelID].FileSize); 
		fLog.WriteString(Log);
		fLog.WriteString("\n");
		IdleMode();
		ModelID++;

	}

	// HACK TO MAKE DB BUILT... TO REMOVE
	//Errors=false;

	CloseDBaseIN();
	CloseDBaseOUT();
	m_StatusText.SetWindowText("Saving....");
	
	if(!Errors) {
		SaveHDR(DBPath);
		CopyFile("Temp.dxl", DBName, false);
	}
	DeleteFile("Temp.dxl");
	m_ProgressBar.SetPos(0);

	Log.Format("Failed Models : %d \nFailed Textures : %d \n", BadModels, BadTextures);
	fErrors.WriteString(Log);


	fErrors.Close();
	fLog.Close();
	fLogU.Close();

	Log.Format("%sTextures.Log", FileFolder); 
	fLog.Open(Log, CFile::modeCreate|CFile::modeWrite|CFile::typeText);

	fLog.WriteString("\n\n ******** USED TEXTURES ********* \n\n");

	for( DWORD t=0; t<MAX_TEXTURE; t++){
		if(TexStatistics[t]) {
			Log.Format("%d.dds \t\t %d Hits\n", t, TexStatistics[t]);
			fLog.WriteString(Log);
		}
	}

	fLog.WriteString("\n\n ********* UNUSED TEXTURES ************\n\n");
	CFileFind	ff;
	DWORD TotalSize=0, Count=0;;
	for( DWORD t=0; t<MAX_TEXTURE; t++){
		Log.Format("%s\\%d.dds", m_TexturesFolder, t);
		if(!TexStatistics[t] & ff.FindFile(Log)){
			ff.FindNextFile();
			DWORD Size=(DWORD)ff.GetLength();
			Log.Format("%d.dds - %d bytes\n", t, Size);
			fLog.WriteString(Log);
			TotalSize+=Size;
			Count++;
		}
	}
	Log.Format("\n\nNr : %d    - Total Size : %d\n", Count, TotalSize);
	fLog.WriteString(Log);
	fLog.Close();

	CString rep;
	rep.Format("Done... Updated models %d", Updated);
	m_StatusText.SetWindowText(rep);
}


void CDXDbToolDlg::GetTexturesFolder(void)
{
	m_TexturesFolder="";
	CFileDialog	F(true, "*.dds", "*.dds", 0, "Textures|*.dds");
	DWORD	rs=(DWORD)F.DoModal();
	if(rs=IDOK){
		m_TexturesFolder=F.GetPathName();
		int	ptr=m_TexturesFolder.ReverseFind('\\');
		if(!ptr) return;
		m_TexturesFolder=m_TexturesFolder.Left(ptr);
		m_StatusText.SetWindowText("Texture Folder Assigned....");
	}
}


void CDXDbToolDlg::GetCDFilesFolder(void)
{
	m_CDFilesFolder="";
	CFileDialog	F(true, "*.fcd", "*.fcd", 0, "CD Files|*.fcd");
	DWORD	rs=(DWORD)F.DoModal();
	if(rs=IDOK){
		m_CDFilesFolder=F.GetPathName();
		int	ptr=m_CDFilesFolder.ReverseFind('\\');
		if(!ptr) return;
		m_CDFilesFolder=m_CDFilesFolder.Left(ptr);
		m_StatusText.SetWindowText("CD Files Folder Assigned....");
	}
}
