// 3DSImport.cpp : implementation file
//

#include "stdafx.h"
#include "LodConvert.h"
#include "3dsFileFormat.h"
#include "3dsFileLoader.h"
#include "3DSImport.h"
#include "DXediting.h"
#include "DXFiles.h"
#include ".\3dsimport.h"



#undef	sqrt

void C3DSLoader::Initialize(void)
{
	m_WarnNr=m_ErrorNr=0;
	m_Overrun=false;
	m_MaterialNr=-1;
	m_VertexXYZ=m_VertexUV=m_VertexDIFF=m_VertexSPEC=0;
	m_FaceNr=m_FaceMatNr=0;
	m_BaseVertex=0;
	m_ChunkData=0;

}


void C3DSLoader::Log(CString LogText)
{
	m_LogOut->AddString(LogText);
	m_LogOut->SetTopIndex(m_LogOut->GetCount()-10);

	LogText.Format("%d",m_MaterialNr+1);
	m_MaterialNrValue->SetWindowText(LogText);
	LogText.Format("%d",m_VertexXYZ);
	m_VerticesNrValue->SetWindowText(LogText);
	LogText.Format("%d",m_FaceNr);
	m_FacesNrValue->SetWindowText(LogText);
	LogText.Format("%d",m_WarnNr);
	m_WarnsNrValue->SetWindowText(LogText);
	LogText.Format("%d",m_ErrorNr);
	m_ErrorNrValue->SetWindowText(LogText);

}

void C3DSLoader::MessageOut(string text)
{
	CString Text=text.c_str();
	Log(Text);
}

void C3DSLoader::ErrorUnimplemented(short Chunk)
{
	m_WarnNr++;
	CString	Text;
	Text.Format("W%04d Unimplemented -> %04x",m_WarnNr,Chunk&0xffff);
	Log(Text);
}
void C3DSLoader::ErrorUnknown(short Chunk)
{
	m_ErrorNr++;
	CString	Text;
	Text.Format("E%04d UNKNOWN -> %04x",m_ErrorNr,Chunk&0xffff);
	Log(Text);
}
/////////////////////////////////////////// IMPORTING CLASS FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void C3DSLoader::User3dVert(float x, float y, float z)
{
	if(m_VertexXYZ>=MAX_3DS_VERTICES) { m_Overrun=true; return;}

	m_Vertices[m_VertexXYZ].vx=x;
	m_Vertices[m_VertexXYZ].vy=y;
	m_Vertices[m_VertexXYZ++].vz=z;
	m_Report.Format("V%04d Vertex %#6.2f, %#6.2f, %#6.2f",m_VertexXYZ,x,y,z);
	Log(m_Report);
}


void C3DSLoader::User3dFace(unsigned short A, unsigned short B, unsigned short C, unsigned short Flags)
{
	m_Faces[m_FaceNr].VertexIndex[0]=A+m_BaseVertex;
	m_Faces[m_FaceNr].VertexIndex[1]=B+m_BaseVertex;
	m_Faces[m_FaceNr].VertexIndex[2]=C+m_BaseVertex;
	m_Faces[m_FaceNr++].Flags=Flags;
	m_Report.Format("F%04d Face %#04d %#04d %#04d %#04d" ,m_FaceNr,A,B,C,Flags);
	Log(m_Report);
}


void	C3DSLoader::UserFaceMaterial(string Name, int Number)
{
	Number+=m_BaseFace;
	m_Report.Format("F%04d Material : %#s" ,Number+1 ,Name.c_str());
	Log(m_Report);
	DWORD a=0;
	while(m_Materials[a].MatName!=Name.c_str() && a<MAX_3DS_MATERIALS) a++;
	if(a==MAX_3DS_MATERIALS) a = 0;
	if((unsigned int)Number>=m_FaceNr) return;
	m_Faces[Number].Material = a;
}


void	C3DSLoader::UserMapVertex(float U, float V)
{
	if(m_VertexUV>=MAX_3DS_VERTICES) { m_Overrun=true; return;}
	m_Vertices[m_VertexUV].tu=U;
	m_Vertices[m_VertexUV++].tv=V;
	m_Report.Format("T%04d Tex Coords U:%#4.2f V:%#4.2f " ,m_VertexUV ,U, V);
	Log(m_Report);
}


void	C3DSLoader::UserTransformMatrix(const Transform3dsMatrix &Transform, const Translate3dsMatrix &Translate)
{
	m_Report.Format("Transform Matrix");
	Log(m_Report);
}


void C3DSLoader::UserMapFile(const string FileName)
{
	m_Report.Format("Texture : %s", FileName.c_str());
	Log(m_Report);
	CString Texture=FileName.c_str();
	Texture.MakeUpper();
	if(Texture.Right(4)==".DDS"){
		DWORD	Tex=atoi(Texture.Left(Texture.GetLength()-4));

//		Tex=3998;

		m_Materials[m_MaterialNr].Flags.b.Texture=1;
		m_Materials[m_MaterialNr].Flags.b.VColor=0;
		for(DWORD t=0; t<m_Obj->TexNr && t<256; t++){
			if(m_Obj->Textures[t]==Tex) {
				m_Materials[m_MaterialNr].TexID=t;
				return;
			}
		}
		if(t<256){
			m_Obj->Textures[t]=Tex;
			m_Materials[m_MaterialNr].TexID=t;
			m_Obj->TexNr++;
			m_Obj->Header.dwTexNr=m_Obj->TexNr;
			m_Obj->TexPerSet=m_Obj->TexNr;
		}

	}

}


void C3DSLoader::UserObjName(string Name)
{
	Log("");
	m_Report.Format("---- Name : %s -----", Name.c_str());
	Log(m_Report);
	m_ChunkData=0;
	m_BaseVertex=m_VertexXYZ;
	m_BaseFace=m_FaceNr;

}


void C3DSLoader::UserMatName(const string Name)
{
	m_Materials[++m_MaterialNr].MatName=Name.c_str();
	m_Materials[m_MaterialNr].SpecPower=2.0f;
	m_Materials[m_MaterialNr].Flags.w=0;
	m_Materials[m_MaterialNr].Flags.b.Gouraud=1;
	m_Materials[m_MaterialNr].Flags.b.Poly=1;
	m_Materials[m_MaterialNr].Flags.b.VColor=1;
	m_Materials[m_MaterialNr].Flags.b.Lite=1;
	m_Materials[m_MaterialNr].TexID=-1;
	m_Materials[m_MaterialNr].MatUScale=m_Materials[m_MaterialNr].MatVScale=1.0f;
	m_Materials[m_MaterialNr].MatUOffset=m_Materials[m_MaterialNr].MatVOffset=1.0f;
	m_Materials[m_MaterialNr].MatRotation=0.0f;

	Log("");
	m_Report.Format("M%04d Material : %s --------", m_MaterialNr, Name.c_str());
	Log(m_Report);

	m_ChunkData=0;
}


void C3DSLoader::SetColorDestination(unsigned short ChunkFlag)
{
	m_ChunkData=ChunkFlag;

}

void	C3DSLoader::UserChunkObj()
{
	m_Overrun=false;
	m_MaterialNr=-1;
	m_VertexXYZ=m_VertexUV=m_VertexDIFF=m_VertexSPEC=0;
	m_FaceNr=m_FaceMatNr=0;
	m_BaseVertex=m_BaseFace=0;
}


void ScaleRGB(DWORD Scale, DWORD ScaleMask, DWORD *Dest)
{
	DWORD	Mask=0xff;
	DWORD	Offset=0;

	while(Mask){
		if(*Dest&Mask&ScaleMask){
			DWORD	Value=(*Dest&Mask)>>Offset;
			Value*=Scale;
			Value/=100;
			*Dest=*Dest&(~(Mask));
			if(Value>0xff) Value=0xff;
			*Dest|=(Value<<Offset);
		}
		Offset+=8;
		Mask<<=8;
	}
}



void C3DSLoader::SetRGB(unsigned int RGB)
{
	if(!m_ChunkData) return;

	switch(m_ChunkData){
		
		case	CHUNK_AMBIENT:	m_Materials[m_MaterialNr].Ambient=RGB;
								break;
		
		case	CHUNK_DIFFUSE:	m_Materials[m_MaterialNr].Diffuse=RGB;
								break;
		
		case	CHUNK_SPECULAR:	m_Materials[m_MaterialNr].Specular=RGB;
								break;

		case CHUNK_U_SCALE:
		case CHUNK_V_SCALE:
		case CHUNK_U_OFFSET:
		case CHUNK_V_OFFSET:
		case CHUNK_M_ROTATE:
		case CHUNK_SHINE:
		case CHUNK_ALPHA:
		case CHUNK_SHINE_STR:	break;

	}
	m_ChunkData=0;
}


void C3DSLoader::SetFloat(float Float)
{
	if(!m_ChunkData) return;

	CString	Text("");

	switch(m_ChunkData){

		case CHUNK_U_SCALE:		m_Materials[m_MaterialNr].MatUScale=Float;
								break;

		case CHUNK_V_SCALE:		m_Materials[m_MaterialNr].MatVScale=Float;
								break;


		case CHUNK_U_OFFSET:	m_Materials[m_MaterialNr].MatUOffset=Float;
								break;

		case CHUNK_V_OFFSET:	m_Materials[m_MaterialNr].MatVOffset=Float;
								break;

		
		case CHUNK_M_ROTATE:	m_Materials[m_MaterialNr].MatRotation=Float;
								break;

	}

	if(Text!="") Log(Text);
}


void C3DSLoader::SetInt(int Value)
{
	if(!m_ChunkData) return;

	switch(m_ChunkData){
						
		case CHUNK_M_OPTIONS:	m_Materials[m_MaterialNr].MapFlags=Value;
								break;	
	}

}



void C3DSLoader::SetPercW(unsigned int Perc)
{
	if(!m_ChunkData) return;

	CString	Text("");

	switch(m_ChunkData){
		
		case	CHUNK_AMBIENT:	
		case	CHUNK_DIFFUSE:	
		case	CHUNK_SPECULAR:	break;

		case CHUNK_SHINE:		ScaleRGB(Perc, 0x00ffffff, &m_Materials[m_MaterialNr].Specular);
								Text.Format("Specular : %3d%%", Perc);
								break;
		
		case CHUNK_ALPHA:		if(Perc>100) Perc=100;
								if(Perc){
									m_Materials[m_MaterialNr].Flags.b.Alpha=true;
									ScaleRGB(100-Perc, 0xff000000, &m_Materials[m_MaterialNr].Diffuse);
									Text.Format("Alpha : %3d%%", Perc);
								}
								break;

		case CHUNK_SHINE_STR:	m_Materials[m_MaterialNr].SpecPower=(float)Perc*128.0f/100.0f;
								Text.Format("Specular Power : %3.2f", m_Materials[m_MaterialNr].SpecPower);
								break;


		case CHUNK_SELFLITE:	if(!Perc) m_Materials[m_MaterialNr].Flags.b.Lite=1;
								else{
									m_Materials[m_MaterialNr].Flags.b.Lite=0;
									ScaleRGB(Perc, 0x00ffffff, &m_Materials[m_MaterialNr].Specular);
									Text.Format("Self-Illuminating : %3d%%", Perc);
								}
								break;
								
	}

	if(Text!="") Log(Text);
	m_ChunkData=0;
}




void C3DSLoader::ComposeObject(void)
{
	DXNode *Node;

	m_Obj->DnRight.x=m_Obj->DnRight.y=m_Obj->DnRight.z=0;
	m_Obj->UpLeft.x=m_Obj->UpLeft.y=m_Obj->UpLeft.z=0;

	for(DWORD a=0; a<m_FaceNr; a++){
		Node=(DXNode*)new DXTriangle();
		Node->NodeNumber=m_Obj->NodeNr++;


		// Assign Surface features
		DWORD MaterialNr=m_Faces[a].Material;
		if(MaterialNr==1)
				__asm nop;
		SURFACE(Node).dwFlags=m_Materials[MaterialNr].Flags;
		SURFACE(Node).dwVCount=3;
		SURFACE(Node).SpecularIndex=m_Materials[MaterialNr].SpecPower;
		SURFACE(Node).dwPrimType=D3DPT_TRIANGLELIST;
		SURFACE(Node).DefaultSpecularity=m_Materials[MaterialNr].Specular;
		((DXTriangle*)Node)->Surface.h.Type=DX_SURFACE;
		if(SURFACE(Node).dwFlags.b.Texture) {
			SURFACE(Node).TexID[0]=m_Materials[MaterialNr].TexID;
			m_Materials[MaterialNr].Diffuse=m_Materials[MaterialNr].Diffuse&0xff000000|0x00ffffff;
		}
		else SURFACE(Node).TexID[0]=-1;
		// Assign vertex Colours & Alpha
		for(DWORD b=0; b<3; b++){
			((DXTriangle*)Node)->Vertex[b].dwColour=m_Materials[MaterialNr].Diffuse;
			((DXTriangle*)Node)->Vertex[b].dwSpecular=0x00;
		}
		for(DWORD i=0; i<3; i++){
			((DXTriangle*)Node)->Vertex[i].vx=-m_Vertices[m_Faces[a].VertexIndex[i]].vy;
			((DXTriangle*)Node)->Vertex[i].vy=-m_Vertices[m_Faces[a].VertexIndex[i]].vx;
			((DXTriangle*)Node)->Vertex[i].vz=-m_Vertices[m_Faces[a].VertexIndex[i]].vz;


			if(((DXTriangle*)Node)->Vertex[i].vx>m_Obj->DnRight.x) m_Obj->DnRight.x=((DXTriangle*)Node)->Vertex[i].vx;
			if(((DXTriangle*)Node)->Vertex[i].vy>m_Obj->DnRight.y) m_Obj->DnRight.y=((DXTriangle*)Node)->Vertex[i].vy;
			if(((DXTriangle*)Node)->Vertex[i].vz>m_Obj->DnRight.z) m_Obj->DnRight.z=((DXTriangle*)Node)->Vertex[i].vz;

			if(((DXTriangle*)Node)->Vertex[i].vx<m_Obj->UpLeft.x) m_Obj->UpLeft.x=((DXTriangle*)Node)->Vertex[i].vx;
			if(((DXTriangle*)Node)->Vertex[i].vy<m_Obj->UpLeft.y) m_Obj->UpLeft.y=((DXTriangle*)Node)->Vertex[i].vy;
			if(((DXTriangle*)Node)->Vertex[i].vz<m_Obj->UpLeft.z) m_Obj->UpLeft.z=((DXTriangle*)Node)->Vertex[i].vz;
			

			//((DXTriangle*)Node)->Vertex[i].tu=m_Vertices[m_Faces[a].VertexIndex[i]].tu;
			//((DXTriangle*)Node)->Vertex[i].tv=m_Vertices[m_Faces[a].VertexIndex[i]].tv;

			double v=m_Vertices[m_Faces[a].VertexIndex[i]].tv;
			double u=m_Vertices[m_Faces[a].VertexIndex[i]].tu;
			double r=sqrt(u*u+v*v);
			double n=atan2(u,v);
			n+=m_Materials[MaterialNr].MatRotation*(double)PI/180;
			((DXTriangle*)Node)->Vertex[i].tu=(float)(r*sin(n));
			((DXTriangle*)Node)->Vertex[i].tv=-(float)(r*cos(n));

		}

		E_CalculateFaceNormal((DXTriangle*)Node);
		if(!m_Obj->Nodes){
			m_Obj->Nodes=Node;
			Node->Prev=NULL;
		} else {
			Node->Prev=m_LastNode;
			m_LastNode->Next=Node;
		}
		m_LastNode=Node;
	}

	Node=new DXNode(MODELEND);
	Node->NodeNumber=m_Obj->NodeNr++;
	m_LastNode->Next=Node;
	Node->Prev=m_LastNode;
	Node->Next=NULL;

}




// C3DSImport dialog

IMPLEMENT_DYNAMIC(C3DSImport, CDialog)
C3DSImport::C3DSImport(E_Object *Obj, CWnd* pParent /*=NULL*/)
	: CDialog(C3DSImport::IDD, pParent)
{
	m_Obj=m_Loader.m_Obj=Obj;
	m_Loader.m_LogOut=&m_3DSLogList;
	DoModal();
}

C3DSImport::~C3DSImport()
{
	EndDialog(0);
}

void C3DSImport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_3DSLOG, m_3DSLogList);
	DDX_Control(pDX, IDC_LOADPROGRESS, m_LoadProgress);
	DDX_Control(pDX, IDC_MaterialNr, m_MaterialNrValue);
	DDX_Control(pDX, IDC_VERTEXNR, m_VerticesNrValue);
	DDX_Control(pDX, IDC_FACESNR, m_FacesNrValue);
	DDX_Control(pDX, IDC_WARNSNR, m_WarnsNrValue);
	DDX_Control(pDX, IDC_ERRORNR, m_ErrorNrValue);
}


BEGIN_MESSAGE_MAP(C3DSImport, CDialog)
	ON_BN_CLICKED(IDC_3DSIMPORT, OnBnClicked3dsimport)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
END_MESSAGE_MAP()


// C3DSImport message handlers

void C3DSImport::OnBnClicked3dsimport()
{
	CString	FileName;
	CFileDialog	File(true, "*.3DS", NULL, 0, "3DS Files|*.3ds", NULL);
	File.DoModal();
	FileName=File.GetFileExt();
	FileName=File.GetPathName();
	FileName.MakeUpper();
	if(FileName.Right(4)!=".3DS") return;
	
	ClearModel(m_Obj);
	m_Loader.Initialize();
	m_Loader.m_MaterialNrValue=&m_MaterialNrValue;
	m_Loader.m_VerticesNrValue=&m_VerticesNrValue;
	m_Loader.m_FacesNrValue=&m_FacesNrValue;
	m_Loader.m_WarnsNrValue=&m_WarnsNrValue;
	m_Loader.m_ErrorNrValue=&m_ErrorNrValue;

	m_Loader.LoadProgress=&m_LoadProgress;
	m_Loader.ProcessFile(FileName.GetString());
	m_LoadProgress.SetPos(0);
	if(m_Loader.m_Overrun) MessageBox("Import buffer overrun...!!!","3DS Import Error");

}

void C3DSLoader::SetProgressRange(int Min, int Max)
{ 
	LoadProgress->SetRange32(Min, Max);
}


void C3DSLoader::SetProgressPos(int Value)
{ 
	LoadProgress->SetPos(Value);
}

void C3DSImport::OnBnClickedButton1()
{
	EndModalLoop(0);
}
