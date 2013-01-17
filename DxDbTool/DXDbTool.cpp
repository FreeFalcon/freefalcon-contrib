// DXDbTool.cpp : definisce i comportamenti delle classi per l'applicazione.
//

#include "stdafx.h"
#include "DXDbTool.h"
#include "DXDbToolDlg.h"
#include "DXDefines.h"
#include "DXFiles.h"
#include "encoder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString	DBPath,FalconDir;
char	FalconDataDirectory[512];								// Do Ya read the name...?
// CDXDbToolApp

extern	E_Object	TheModel,SpareModel;
extern	HDRPARENT	ModelParent;
extern	CT			ModelCT;


BEGIN_MESSAGE_MAP(CDXDbToolApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// costruzione di CDXDbToolApp

CDXDbToolApp::CDXDbToolApp()
{
	// TODO: inserire qui il codice di costruzione.
	// Inserire l'inizializzazione significativa in InitInstance.
}


// L'unico e solo oggetto CDXDbToolApp

CDXDbToolApp	DxDbApp;


// Inizializzazione di CDXDbToolApp

#ifdef	_LINE_COMMAND_





BOOL CDXDbToolApp::InitInstance()
{
	DWORD	 size = sizeof(FalconDataDirectory);
	HKEY	theKey;
	long	retval = RegOpenKeyEx(HKEY_LOCAL_MACHINE, FALCON_REGISTRY_KEY,
	0, KEY_ALL_ACCESS, &theKey);
	DWORD type;
	RegQueryValueEx(theKey, "objectDir", 0, &type, (LPBYTE)FalconDataDirectory, &size);
	FalconDir=FalconDataDirectory;

	if(FalconDir=="") { MessageBox(NULL, "Missing Red Viper Install", "Error", 0); return FALSE; }
	
	// Parsing of Command Line
	if(m_lpCmdLine[0]==0) return FALSE;
	CString	Delimit=", ";
	strtok(m_lpCmdLine,Delimit);
	// Get the Command and the Arguments ( Max 3 )
	CString	Command=_strupr(m_lpCmdLine);
	CString	Argument0=strtok(NULL,Delimit);
	CString	Argument1=strtok(NULL,Delimit);
	CString	Argument2=strtok(NULL,Delimit);

	// Evaluate the Command
	CDXDbToolDlg dlg;
	dlg.ExecuteCommand(Command, Argument0, Argument1, Argument2 );
	



	return	FALSE;
}
#else
BOOL CDXDbToolApp::InitInstance()
{
	// InitCommonControls() è necessario in Windows XP se nel manifesto
	// di un'applicazione è specificato l'utilizzo di ComCtl32.dll versione 6 o successiva per attivare
	// gli stili visuali. In caso contrario, non sarà possibile creare finestre.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Inizializzazione standard
	// Se non si utilizzano queste funzionalità e si desidera ridurre la dimensione
	// dell'eseguibile finale, rimuovere
	// le routine di inizializzazione specifiche non necessarie
	// Modificare la chiave del Registro di sistema in cui sono memorizzate le impostazioni
	// TODO: modificare questa stringa con informazioni appropriate,
	// ad esempio il nome della propria società
	SetRegistryKey(_T("Applicazioni locali generate tramite la Creazione guidata di applicazioni locali"));

	DWORD	 size = sizeof(FalconDataDirectory);
	HKEY	theKey;
	long	retval = RegOpenKeyEx(HKEY_LOCAL_MACHINE, FALCON_REGISTRY_KEY,
	0, KEY_ALL_ACCESS, &theKey);
	DWORD type;
	RegQueryValueEx(theKey, "objectDir", 0, &type, (LPBYTE)FalconDataDirectory, &size);
	FalconDir=FalconDataDirectory;
	DBPath=GetProfileString("Paths","DXH Directory","");
	if(DBPath==""){
		DBPath=FalconDataDirectory;
		DBPath+="\\KoreaObj.dxh";
	}

	CDXDbToolDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: inserire qui il codice per gestire la chiusura della finestra di dialogo 
		//  tramite il pulsante OK.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: inserire qui il codice per gestire la chiusura della finestra di dialogo 
		//  tramite il pulsante Annulla.
	}

	WriteProfileString("Paths","DXH Directory",DBPath);

	// Poiché la finestra di dialogo è stata chiusa, restituisce FALSE in modo che l'applicazione
	//  venga terminata, anziché avviare la message pump dell'applicazione.
	return FALSE;
}
#endif

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

