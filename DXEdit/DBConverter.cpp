// DBConverter.cpp : implementation file
//


#include "stdafx.h"
#include "LODConvert.h"
#include "DBConverter.h"
#include ".\dbconverter.h"
#include "DXDefines.h"
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>



// CDBConverter dialog

IMPLEMENT_DYNCREATE(CDBConverter, CDHtmlDialog)

CDBConverter::CDBConverter(CWnd* pParent )
	: CDHtmlDialog(CDBConverter::IDD, CDBConverter::IDH, pParent)
{
//	strcpy(KoreaObjFolder, path);
	Create(IDD_DBCONVERT);
	ShowWindow(SW_SHOW);
	m_hInstance=CreateMutex(NULL, false, "DXDBConverter");
	
}

CDBConverter::~CDBConverter()
{
}

void CDBConverter::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONVPROGRESS, m_ConvProgress);
	DDX_Control(pDX, IDC_CONVTEXT, m_ConvText);

	DDX_Control(pDX, IDC_NEXTKEY, m_NextKey);
	DDX_Control(pDX, IDC_GOTOVAL, M_gotovALUE);
	DDX_Control(pDX, IDC_EXIT, m_ExitKey);
	DDX_Control(pDX, IDC_GOTO, m_GotoKey);
	DDX_Control(pDX, IDC_PREVKEY, m_PrevKey);
}

BOOL CDBConverter::OnInitDialog()
{
	CDialog::OnInitDialog();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CDBConverter, CDHtmlDialog)
	ON_BN_CLICKED(IDC_NEXTKEY, OnBnClickedNextkey)
	ON_BN_CLICKED(IDC_PREVKEY, OnBnClickedPrevkey)
	ON_STN_CLICKED(IDC_CONVTEXT, OnStnClickedConvtext)
	ON_BN_CLICKED(IDC_GOTO, OnBnClickedGoto)
	ON_BN_CLICKED(IDC_DBALL, OnBnClickedDball)
	ON_BN_CLICKED(IDC_EXIT, OnBnClickedExit)
	ON_BN_CLICKED(IDC_FRAGDB, OnBnClickedFragdb)
	ON_BN_CLICKED(IDC_MAKEDB, OnBnClickedMakedb)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CDBConverter)
END_DHTML_EVENT_MAP()





BOOL CDBConverter::Convert(CString path)
	{
		CFile		fp,fOut,fModel;
		CStdioFile	fLog;
		int			c, d, FileType;
		int			Domain, Class, Type, CTNumber;
		char		szFile[512];
		int			DXL_Offset = 0, DXL_size = 0, DXL_Processing_Type = 0;
		DWORD		FileOffset,FileSize;

		strcpy(KoreaObjFolder, path);

		sprintf(szFile, "%sKoreaObj.hdr", KoreaObjFolder); 
		fp.Open(szFile, CFile::modeRead);
		LoadHDR(fp);
		fp.Close();
		LoadCT();



		DWORD LODSize;
		DWORD TagCount;
		char *NodeTreeData;
		BRoot *Root;
		BNodeType *TagList,*TagRoot;
		void		*LODBuffer;
		Converter	*cvt;
		CString s,k;
		CString DirName;
		BYTE	*Buffer;
		Encoder *ecd;


		DrawDisable=true;
		DBConvert=false;
		ReBoot=false;
		Abort=false;
		FragIt=false;
		AssembleIt=false;

		TagRoot=TagList=(BNodeType*)malloc(MAX_TAGS*sizeof(BNodeType));
		LODBuffer=malloc(LOD_BUFFER_SIZE);
		ObjectInstance *TestObj=new ObjectInstance(1);

		m_ConvProgress.SetRange(0, hdr.nLOD);

		//...


		// Open KO.lod
		sprintf(szFile, "%sKoreaObj.lod", KoreaObjFolder); 
		fp.Open(szFile, CFile::modeRead);
		void *FileBuffer=malloc(16384*1024);

Restart:
		// Open the Saving File
		if(DBConvert){
			m_ExitKey.EnableWindow(false);
			m_GotoKey.EnableWindow(false);
			m_NextKey.EnableWindow(false);
			m_PrevKey.EnableWindow(false);
			sprintf(szFile, "%s"OBJECTS_DATABASE_NAME, KoreaObjFolder); 
			DirName.Format("%s\\Models", KoreaObjFolder); 
			DirName+='\\';
			if(!FragIt){
				fOut.Open(szFile, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
			} else {
				CFileFind File;
				int OnList=File.FindFile(DirName);
				
				while(OnList){
					OnList=File.FindNextFile();
					if(File.IsDirectory())goto ok;
				}	
				File.Close();
				char Buffer[128];
				strcpy(Buffer,DirName);
				_mkdir(Buffer);
			}
			ok:	_asm nop

			sprintf(szFile, "%sDXL.Log", KoreaObjFolder); 
			fLog.Open(szFile, CFile::modeCreate|CFile::modeWrite|CFile::typeText);

		}
		FileOffset=0;
		FileSize=0;
		
		c=0;

		while(c < hdr.nLOD)
		{
			m_ConvProgress.SetPos(c);
		
			TagList=TagRoot;
			//...

			// Get Domain, Class and File Type, if LOD has a CT record
			// if no CT record, then Domain = Class = FileType = 0
			FileType = LODUsedByCT((int) c, (int*) &Domain, (int*) &Class, (int*)&Type, (int*)&CTNumber);

			if (Domain < 0)
			{
				// a shadow BSP model
				//...
				Domain = -Domain;
			}

			//...

			// hdrLOD[c].uint[3] = offset into KO.lod
			// hdrLOD[c].uint[4] = size of lod (bytes)
			if(!hdrLOD[c].uint[4])
				_asm	 nop;

			// ONLY IF IT HAS A SIZE...
			if(hdrLOD[c].uint[4] && (c<4804 || c>4805)){


		///////////////// ONLY DO IF NOT REASSEMBLING FROM SEPARATED MODELS ///////////////////////
				if(!AssembleIt){
					// Seek to start of this LOD in KO.lod
					fp.Seek(hdrLOD[c].uint[3], CFile::begin);
					// Read LOD
					fp.Read(FileBuffer, hdrLOD[c].uint[4]);

					gClass=Class;
					gDomain=Domain;
					gType=Type;

					s.Format("LOD %d  Class %d  Type %d \n Parent %d",c, Class, Type, CTNumber);
					m_ConvText.SetWindowText(s);


					// TRANSFORMATION OF THE OBJECT 
					ecd =new Encoder("temp.DXL");
					IdleMode();
					cvt=CV=new Converter();
					IdleMode();
						
					// Upadtes BSP Objects
					TagCount=*(int*)FileBuffer;
					memcpy((void*)TagList,(int*)FileBuffer+1,TagCount*sizeof(*TagList));
					IdleMode();
					LODSize=hdrLOD[c].uint[4]-sizeof(int)-TagCount*sizeof(*TagList);
					memcpy(LODBuffer,(char*)FileBuffer+sizeof(int)+TagCount*sizeof(*TagList),LODSize);
					IdleMode();
					//Root=(BRoot*)BNode::RestorePointers((BYTE*)LODBuffer, 0, &TagList);

					DWORD VBClass;
					VBClass=VB_CLASS_FEATURES;
					if(Class==7) VBClass=VB_CLASS_DOMAIN_GROUND;
					if(Class==7 && Domain==2) VBClass=VB_CLASS_DOMAIN_AIR;

					GlobalBias=0.0f;
					cvt->Init();
					TheStateStack.pvtDrawObject(Root);
					cvt->Statistics();
					ecd->Initialize(cvt);
					ecd->Encode(c, VBClass, CTNumber);

					Buffer=ecd->GetModelBuffer();
					FileSize=ecd->GetModelSize();
				}

				if(AssembleIt){
					CString ModelName;
					ModelName.Format("%s\\%05d.DXM",DirName,c);
					CFileStatus Stat;
					FileSize=0;
					if(fModel.GetStatus(ModelName, Stat)){
						if(fModel.Open(ModelName, CFile::modeRead|CFile::typeBinary)){
							FileSize=(DWORD)Stat.m_size;
							Buffer=(BYTE*)malloc(FileSize);
							fModel.Read((void*)(Buffer), FileSize);
							fModel.Close();
						}
					}
				}

				// Write out the Model
				if(DBConvert){
					// Assign the Model Class
					k.Format("LOD %04d, Offset: %08x, Size %06x\n", c, FileOffset, FileSize);
					fLog.WriteString(k);
					// Single models saving
					if(FragIt){
						CString ModelName;
						ModelName.Format("%s\\%05d.DXM",DirName,c);
						fModel.Open(ModelName, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
						fModel.Write((void*)(Buffer), FileSize);
						fModel.Close();
					} else {
						fOut.Write((void*)(Buffer), FileSize);
					}
				}

				NextLOD=PrevLOD=NewLOD=false;
				m_NextKey.EnableWindow(false);
				if(!DBConvert){
						
					TestObj->id=c;
					TestObj->DOFValues=DOFs;
					TestObj->SwitchValues=SWITCHes;
					TestObj->ParentObject->nTextureSets=1;

					DxDbHeader *Head=(DxDbHeader*)ecd->GetModelBuffer();
					TheVbManager.SetupModel(TestObj->id, (BYTE*)Head, Head->VBClass);
					TheDXEngine.LoadTextures(Head->Id);
					TheVbManager.AssertValidModel(TestObj->id);

					DX->Draw(TestObj);

					DrawDisable=false;
					m_NextKey.EnableWindow(true);
					while((!NextLOD) && (!ReBoot)){
						if(PrevLOD){
							c-=2;
							NextLOD=true;
						}
						if(NewLOD){
							char x[10];
							M_gotovALUE.GetWindowText(x,5);
							c=atoi(x);
							c--;
							NextLOD=true;
						}
						if(Abort) return true;
						IdleMode();
					}
					m_NextKey.EnableWindow(false);
					DrawDisable=true;
					Delay(100);
					DX->UnDraw(TestObj);
					TheDXEngine.UnLoadTextures(TestObj->id);
					TheVbManager.ReleaseModel(TestObj->id);
				}

				if(!AssembleIt){
					delete ecd;
					delete cvt;
				}

				if(ReBoot && (!DBConvert)){
					DBConvert=true;
					ReBoot=false;
					goto Restart;
				}
				// Set Up New Pointers in the HDR File
				if(DBConvert){
					hdrLOD[c].uint[3] = FileOffset;
					hdrLOD[c].uint[4] = FileSize;
					FileOffset+=FileSize;
				}

			} else {
				// MODELS WITH NO SIZE !!!!
				CString z;
				z.Format("LOD %04d  -------------------------\n", c);
				fLog.WriteString(z);
			}


Skip:
			//Delay(10);
			IdleMode();
			c++;


		}
		fLog.Close();
		fp.Close();
		free(FileBuffer);
		free(TagRoot);

		//...
		if(DBConvert){
			if(!FragIt) fOut.Close();
			m_ConvText.SetWindowText("Writing HDR File.....");
			sprintf(szFile, "%s"OBJECTS_HEADER_NAME, KoreaObjFolder); 
			fp.Open(szFile, CFile::modeCreate|CFile::modeWrite);
			SaveHDR(fp);
			fp.Close();
		}
		// release File Buffer

		DrawDisable=false;
		return true;
	}

//==============================================================
//  Code segment for assigning DXL model offset and size of 
//    model (bytes)
//==============================================================
//		hdrLOD[LOD_Number].uint[0] = DXL_Offset;
//		hdrLOD[LOD_Number].uint[1] = DXL_size;
//==============================================================
//==============================================================



//==============================================================
//==============================================================
//	doLoadHDR() - This is the routine to load in the HDR file.
//==============================================================
//==============================================================

float	dogetfloat(CFile f)
{
	float	x;
	f.Read(&x, sizeof(x));
	return x;
}


DWORD	dogetlong(CFile f)
{
	DWORD	x;
	f.Read(&x, sizeof(x));
	return x;
}

char	dogetchar(CFile f)
{
	char	x;
	f.Read(&x, sizeof(x));
	return x;
}


unsigned short int	dogetshort(CFile f)
{
	unsigned short int	x;
	f.Read(&x, sizeof(x));
	return x;
}



void	doputlong(long x, CFile f)
{
	f.Write(&x, sizeof(x));
}

void	doputfloat(float x, CFile f)
{
	f.Write(&x, sizeof(x));
}

void	doputchar(char x, CFile f)
{
	f.Write(&x, sizeof(x));
}


void	doputshort(unsigned short int x, CFile f)
{
	f.Write(&x, sizeof(x));
}


short int CDBConverter::LoadHDR(CFile fp)// send to the function the FILE pointer of your open file stream (Koreaobj.hdr)
{
	int						c,d;
	int						cc;

	// Zero out HDR records
	ZeroMemory(&hdrLOD,sizeof(hdrLOD));

	cc = 0;
	hdr.version = dogetlong(fp);// dogetlong simply reads in a 4 byte integer
															// since the Mac reads bytes differently, I use a function to read in the bytes and reverse the order.
	hdr.nColor = dogetlong(fp);
	if((hdr.nColor > MAX_COLOR) | (hdr.nColor < 1))
		return (1);
	hdr.nDarkColor = dogetlong(fp);
	for(c=0;c<hdr.nColor;c++)
	{
		for(d=0;d<4;d++)
			hdrColor[c].f[d] = dogetfloat(fp);// dogetfloat reads in a 4 byte float
		
		if (c == 774)	// Formation strip-lights
		{
			hdrColor[c].f[0] = 0.6f;
			hdrColor[c].f[1] = 0.6f;
			hdrColor[c].f[2] = 0.0f;
		}
	}
	hdr.nPalette = dogetlong(fp);
	if((hdr.nPalette > MAX_PALETTE) | (hdr.nPalette < 1))
//		return (2);
		cc = 2;
	for(c=0;c<hdr.nPalette;c++)
	{
		for(d=0;d<256;d++)
		{
			hdrPalette[c].r[d] = dogetchar(fp);  
			hdrPalette[c].g[d] = dogetchar(fp);
			hdrPalette[c].b[d] = dogetchar(fp);
			hdrPalette[c].a[d] = dogetchar(fp);
		}
		hdrPalette[c].uint[0] = dogetlong(fp); 
		hdrPalette[c].uint[1] = dogetlong(fp); 
	}
	hdr.nTexture = dogetlong(fp);
	if((hdr.nTexture > MAX_TEXTURE) | (hdr.nTexture < 1))
		return (3);
	
	hdr.unk[0] = dogetlong(fp); // Largest texture (compressed size)
	for(c=0;c<hdr.nTexture;c++)
	{
		for(d=0;d<10;d++)
			hdrTexture[c].uint[d] = dogetlong(fp);
	}
	hdr.unk[1] = dogetlong(fp); // Max number of nodes
	maxNodes = hdr.unk[1];
	if (hdr.unk[1] < MAX_NODES)
		maxNodes = hdr.unk[1] = MAX_NODES;
	hdr.nLOD = dogetlong(fp);
	LastLOD = hdr.nLOD;
	if((hdr.nLOD > MAX_LOD) | (hdr.nLOD < 1))
		return (4);
	for(c=0;c<hdr.nLOD;c++)
	{
		for(d=0;d<5;d++)										 // [0], [1], [2] not used 
			hdrLOD[c].uint[d] = dogetlong(fp); // [3] KO.lod file offset
	 																			 // [4] LOD length
	}
	hdr.nParent = dogetlong(fp);
	if((hdr.nParent > MAX_PARENT) | (hdr.nParent < 1))
		return (5);
	for(c=0;c<hdr.nParent;c++)
	{
		for(d=0;d<7;d++)
			hdrParent[c].f[d] = dogetfloat(fp);	// Hitbox + "radius" (shadow size???)
		for(d=0;d<2;d++)
			hdrParent[c].uint[d] = dogetlong(fp);	// file offset [0] & size [1]
		for(d=0;d<2;d++)
			hdrParent[c].ushort1[d] = dogetshort(fp);// nTexures [0] & nDynamics[1]
		for(d=0;d<4;d++)
			hdrParent[c].uchar[d] = dogetchar(fp); // nLODs [0], nSwitches [1], nDOFs [2], nSlots [3]
		for(d=0;d<2;d++)
			hdrParent[c].ushort2[d] = dogetshort(fp);	// Unk1 [0] & Unk2 [1]
	}
	for(c=0;c<hdr.nParent;c++)
	{
		if(hdrParent[c].uchar[3] > 100)
			return (6);
		if(hdrParent[c].uchar[3] > 0)// Load Slots
		{
			for(d=0;d<hdrParent[c].uchar[3];d++)
			{
				hdrLODRes[c].slot1[d] = dogetfloat(fp);
				hdrLODRes[c].slot2[d] = dogetfloat(fp);
				hdrLODRes[c].slot3[d] = dogetfloat(fp);
			}
		}
		if(hdrParent[c].ushort1[1] > 10)
			return (7);
		if(hdrParent[c].ushort1[1] > 0)// Load Dynamic
		{
			for(d=0;d<hdrParent[c].ushort1[1];d++)
			{
				hdrLODRes[c].dyn1[d] = dogetfloat(fp);
				hdrLODRes[c].dyn2[d] = dogetfloat(fp);
				hdrLODRes[c].dyn3[d] = dogetfloat(fp);
			}
		}
		if(hdrParent[c].uchar[0] > 10)
			return (8);

		if(hdrParent[c].uchar[0] > 0)// Load LOD indexes
		{
			for(d=0;d<hdrParent[c].uchar[0];d++)
			{
				hdrLODRes[c].lodind[d] = dogetlong(fp);
				hdrLODRes[c].distance[d] = dogetfloat(fp);
			}
		}
	}
	return (cc);
} // end LoadHDR()

//====================================================================================
//====================================================================================
//	SaveHDR() - saves the HDR file (you wont need this unless you allow editing of the HDR data)
//====================================================================================
//====================================================================================

void	CDBConverter::SaveHDR(CFile fp)
{
				int						c,d;

	doputlong((long)hdr.version,fp);
	if(hdr.nColor > MAX_COLOR)
		hdr.nColor = MAX_COLOR;
	doputlong((long)hdr.nColor,fp);
	doputlong((long)hdr.nDarkColor,fp);
	for(c=0;c<hdr.nColor;c++)
	{
		for(d=0;d<4;d++)
			doputfloat(hdrColor[c].f[d],fp);
	}
	if(hdr.nPalette > MAX_PALETTE)
		hdr.nPalette = MAX_PALETTE;
	doputlong(hdr.nPalette,fp);
	for(c=0;c<hdr.nPalette;c++)
	{
		for(d=0;d<256;d++)
		{
			doputchar(hdrPalette[c].r[d],fp);
			doputchar(hdrPalette[c].g[d],fp);
			doputchar(hdrPalette[c].b[d],fp);
			doputchar(hdrPalette[c].a[d],fp);
		}
		doputlong(hdrPalette[c].uint[0],fp);
		doputlong(hdrPalette[c].uint[1],fp);
	}
	if(hdr.nTexture > MAX_TEXTURE)
		hdr.nTexture = MAX_TEXTURE;
	doputlong(hdr.nTexture,fp);// 21
	doputlong(hdr.unk[0],fp);
	for(c=0;c<hdr.nTexture;c++)
	{
		for(d=0;d<10;d++)
			doputlong(hdrTexture[c].uint[d],fp);
	}
	// Make sure max number of nodes is big enough
	if (hdr.unk[1] < 10000)
		hdr.unk[1] = 10000;
	doputlong(hdr.unk[1],fp);
	if(hdr.nLOD > MAX_LOD)
		hdr.nLOD = MAX_LOD;
	doputlong(hdr.nLOD,fp);// 54
	for(c=0;c<hdr.nLOD;c++)
	{
		if(c==1629)
				_asm nop;

		for(d=0;d<5;d++)
			doputlong(hdrLOD[c].uint[d],fp);		// <<====  use hdrLOD[c].uint[0] - hdrLOD[c].uint[2] for dxl offset and size
	}
	if(hdr.nParent > MAX_PARENT)
		hdr.nParent = MAX_PARENT;
	doputlong(hdr.nParent,fp);// 16
	for(c=0;c<hdr.nParent;c++)
	{
		if(hdrParent[c].uchar[3] > 100)
			hdrParent[c].uchar[3] = 100;
		if(hdrParent[c].ushort1[1] > 10)
			hdrParent[c].ushort1[1] = 10;
		if(hdrParent[c].uchar[0] > 10)
			hdrParent[c].uchar[0] = 10;
		for(d=0;d<7;d++)
			doputfloat(hdrParent[c].f[d],fp);
		for(d=0;d<2;d++)
			doputlong(hdrParent[c].uint[d],fp);
		for(d=0;d<2;d++)
			doputshort(hdrParent[c].ushort1[d],fp);
		for(d=0;d<4;d++)
			doputchar(hdrParent[c].uchar[d],fp);
		for(d=0;d<2;d++)
			doputshort(hdrParent[c].ushort2[d],fp);
	}
	for(c=0;c<hdr.nParent;c++)
	{
		if(hdrParent[c].uchar[3] > 0)// Save Slots
		{
			for(d=0;d<hdrParent[c].uchar[3];d++)
			{
				doputfloat(hdrLODRes[c].slot1[d],fp);
				doputfloat(hdrLODRes[c].slot2[d],fp);
				doputfloat(hdrLODRes[c].slot3[d],fp);
			}
		}
		if(hdrParent[c].ushort1[1] > 0)// Save Dynamic
		{
			for(d=0;d<hdrParent[c].ushort1[1];d++)
			{
				doputfloat(hdrLODRes[c].dyn1[d],fp);
				doputfloat(hdrLODRes[c].dyn2[d],fp);
				doputfloat(hdrLODRes[c].dyn3[d],fp);
			}
		}
		if(hdrParent[c].uchar[0] > 0)// Save LOD indexes
		{
			for(d=0;d<hdrParent[c].uchar[0];d++)
			{
				if ((int)hdrLODRes[c].lodind[d] == -1)
					hdrLODRes[c].lodind[d] = 0;
				doputlong(hdrLODRes[c].lodind[d],fp);
				doputfloat(hdrLODRes[c].distance[d],fp);
			}
		}
	}

//	fp.Close();
	return;
} // end SaveHDR()


//====================================================================================
/*
		DATA DESCRIPTIONS:

	DOMAIN:
		0. Unknown
		1. Abstract
		2. Air
		3. Land
		4. Sea
		5. Space
		6. Underground
		7. Undersea
	CLASS:
		0. Abstract
		1. Animal
		2. Feature
		3. Manager
		4. Objective
		5. SFX
		6. Unit
		7. Vehicle
		8. Weapon
		9. Weather
		10. Dialog
		11. Game
		12. Group
		13. Session
	TYPE:
		Number to indicate type of Domain-Class
	SUBTYPE:
		Further sequential differentiation
	SPECIFIC:
		Further sequential differentiation
	MODE:
		0. Normal
		1. Crimson
		2. Shark
		3. Viper
		4. Tiger
	FTYPE:
		0. Unknown
		1. Feature
		2. None
		3. Objective
		4. Unit
		5. Vehicle
		6. Weapon
	PARENT:
		0. Normal
		1. Repaired/Shadow (for air)
		2. Damaged
		3. Destroyed
		4. Left Destroyed
		5. Right Destroyed
		6. Both Destroyed
	FOFFSET:
		Record index into FTYPE file

*/
//====================================================================================

//====================================================================================
// Read in Falcon.ct
//====================================================================================
int	CDBConverter::LoadCT()
{
				int						c,d;
				float					BubbleDist;
				CFile				fp;
				char					nctr[6];
				char				szMsg[512];

	sprintf(szMsg, "%sFalcon4.ct", KoreaObjFolder); 
	fp.Open(szMsg,CFile::modeRead);
	ctNumber = dogetshort(fp);
	if (ct)
	{
		free(ct);
		ct = NULL;
	}
	ct = (CT*)malloc(ctNumber*sizeof(CT));
	memset(ct, 0, (ctNumber*sizeof(CT)));
	for(c=0;c<ctNumber;c++)
	{
		fp.Seek(c*81+10,CFile::begin);
		ct[c].domain = dogetchar(fp);
		ct[c].cclass = dogetchar(fp);
		ct[c].type = dogetchar(fp);
		ct[c].subtype = dogetchar(fp);
		ct[c].specific = dogetchar(fp);
		ct[c].mode = dogetchar(fp);
		fp.Seek(10,CFile::current);
		BubbleDist = dogetfloat(fp);
		fp.Seek(32, CFile::current);
		for(d=0;d<8;d++)
			ct[c].parent[d] = dogetshort(fp);
//		fseek(fp,2,SEEK_CUR);
		ct[c].ftype = dogetchar(fp);
		ct[c].foffset = dogetlong(fp);
			
	}
	fp.Close();
	return(ctNumber);
} // end LoadCT

//====================================================================================
//====================================================================================

//=================================================================
//  LODUsedBy() - Find LOD that is used by CT
//=================================================================
int	CDBConverter::LODUsedByCT(int eLOD, int* Domain, int* Class, int* Type, int* CTNumnber)
{
			int					c, d, e;
			int					FileType = 0;
      int					grDat, nL;
	*CTNumnber=0;

	*Domain = 0;
	*Class = 0;
  for(c=0;c<ctNumber;c++)
  {
    for(d=0;d<7;d++)
    {
			 grDat = ct[c].parent[d];

       if(grDat < 0 || grDat >= hdr.nParent)
         continue;
       nL = hdrParent[grDat].uchar[0];
       for(e=0;e<nL;e++)
       {
          if((hdrLODRes[grDat].lodind[e]-1)/2 == eLOD)
          {

						FileType = (int)ct[c].ftype;
						if (ct[c].domain == 2 && ct[c].cclass == 7 &&  e == 1)  // aircraft shadow
							*Domain = - ct[c].domain;
						else
							*Domain = ct[c].domain;
						*Class = ct[c].cclass;
						*Type = ct[c].type;
						*CTNumnber=grDat;
						return (FileType);
			}
		}
		}
	}  
	// *** This is the scanning for Parents having no CTs 
	for(c=0;c<hdr.nParent;c++){
						if(eLOD==15)
							_asm	 nop;
       nL = hdrParent[c].uchar[0];
       for(e=0;e<nL;e++)
       {
          if((hdrLODRes[c].lodind[e]-1)/2 == eLOD)
          {
						FileType = 2;
						*Domain = 1;
						*Class = (int)2;
						*Type = 0;
						*CTNumnber=c;
						return (FileType);
          }
       }
	}

  
  
  return (FileType);
}	// end LODUsedByCT()


//====================================================================================
//====================================================================================

void CDBConverter::OnBnClickedCheck1()
{
	// TODO: Add your control notification handler code here
}

void CDBConverter::OnBnClickedNextkey()
{
	NextLOD=true;
}

void CDBConverter::OnBnClickedPrevkey()
{
	// TODO: Add your control notification handler code here
	PrevLOD=true;
}

void CDBConverter::OnStnClickedConvtext()
{
	// TODO: Add your control notification handler code here
}

void CDBConverter::OnBnClickedGoto()
{
	// TODO: Add your control notification handler code here
	NewLOD=true;
}

void CDBConverter::OnBnClickedDball()
{
	// TODO: Add your control notification handler code here
	ReBoot=true;
}

void CDBConverter::OnBnClickedExit()
{
	// TODO: Add your control notification handler code here
	Abort=true;
}

void CDBConverter::OnBnClickedFragdb()
{
	// TODO: Add your control notification handler code here
	// ok...restart from root
	ReBoot=true;
	// and fragment DB
	FragIt=true; 
}

void CDBConverter::OnBnClickedMakedb()
{
	// TODO: Add your control notification handler code here
	// ok...restart from root
	ReBoot=true;
	// and ASSEMBLE DB
	AssembleIt=true; 
}
