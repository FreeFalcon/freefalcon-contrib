#pragma once
#include "3dsFileFormat.h"
#include "3dsFileLoader.h"
#include "afxwin.h"
#include "afxcmn.h"

// C3DSImport dialog

#define	MAX_3DS_VERTICES	(65536*2)
#define	MAX_3DS_MATERIALS	65536
#define	MAX_3DS_FACES		(65536*2)


typedef	struct	{
					CString		MatName;
					bool		Alpha;
					DWORD		Diffuse, Ambient, Specular;
					float		SpecPower;
					DXFlagsType	Flags;
					DWORD		TexID;

					float		MatRotation;
					float		MatVScale, MatUScale, MatVOffset, MatUOffset;
					DWORD		MapFlags;
} MaterialType;

typedef struct	{
					DWORD	VertexIndex[3], Flags;
					DWORD	Material;
} FaceType;
					

class	C3DSLoader: public C3dsFileLoader
{
public:
	CListBox	*m_LogOut;
	E_Object	*m_Obj;
	bool		m_Overrun;
	DWORD			m_MaterialNr;
	CProgressCtrl	*LoadProgress;
	void		Initialize(void);

	CStatic *m_MaterialNrValue;
	CStatic *m_VerticesNrValue;
	CStatic *m_FacesNrValue;
	CStatic *m_WarnsNrValue;
	CStatic *m_ErrorNrValue;

protected:
	CString		m_Report;
	void		User3dVert(float x, float y, float z);
	void		User3dFace(unsigned short A, unsigned short B, unsigned short C, unsigned short Flags);
	void		UserFaceMaterial(string Name, int Number);
	void		UserMapVertex(float U, float V);
	void		UserTransformMatrix(const Transform3dsMatrix &Transform, const Translate3dsMatrix &Translate);
	void		UserMapFile(const string FileName);
	void		UserObjName(string Name);
	void		UserMatName(const string Name);

	void		SetColorDestination(unsigned short ChunkFlag);
	void		SetRGB(unsigned int RGB);
	void		SetFloat(float Float);
	void		SetPercW(unsigned int Perc);
	void		ComposeObject(void);
	void		UserChunkObj();
	void		SetInt(int Int);
	void		Log(CString LogText);
	void		MessageOut(string text);

	void		SetProgressRange(int Min, int Max);
	void		SetProgressPos(int Value);
	void		ErrorUnimplemented(short Chunk);
	void		ErrorUnknown(short Chunk);

	DWORD		m_WarnNr,m_ErrorNr;


	D3DVERTEXEX		m_Vertices[MAX_3DS_VERTICES];
	DWORD			m_VertexXYZ, m_VertexUV, m_VertexDIFF, m_VertexSPEC, m_BaseFace;
	MaterialType	m_Materials[MAX_3DS_MATERIALS];
	unsigned short	m_ChunkData;
	FaceType		m_Faces[MAX_3DS_FACES];
	DWORD			m_FaceNr, m_FaceMatNr;
	DXNode			*m_LastNode;

	DWORD			m_BaseVertex;



/*	virtual void		UserCamera(float px, float py, float pz,
								   float tx, float ty, float tz,
								   float Bank, float Lens);
	virtual void		UserSpotLight(float x, float y, float z, float HotSpot, float FallOff);
	virtual void		UserFrames(long Start, long End);
	virtual void		UserTrackObjectName(const string Name, int Key1, int Key2, int Key3);
	virtual void		UserTrackPos(int Frame, long Flags, float x, float y, float z);
	virtual void		UserTrackRot(int Frame, long Flags, float DegX, float DegY, float DegZ);
	virtual void		UserTrackScale(int Frame, long Flags, float ScaleX, float ScaleY, float ScaleZ);
	virtual void		UserObjNumber(int ObjNumber);
	virtual void		UserOneUnit(float Unit);
	virtual void		UserViewPortLayout(ViewPortLayout &Layout);
	virtual void		UserViewPortSize(int PosX, int PosY, int Width, int Height);
	virtual void		UserViewPortData(ViewPortData &Data);
	virtual	void		UserViewUser(float x, float y, float z, float Width, float XYangle, float YZangle, float BackAngle);
	virtual void		UserViewCamera(string CameraName);
	virtual void		UserEndOfFile();*/


};


class C3DSImport : public CDialog
{
	DECLARE_DYNAMIC(C3DSImport)

public:
	C3DSImport(E_Object *Obj, CWnd* pParent = NULL);   // standard constructor
	virtual ~C3DSImport();

// Dialog Data
	enum { IDD = IDD_3DSIMPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	E_Object	*m_Obj;
	C3DSLoader	m_Loader;
	DECLARE_MESSAGE_MAP()
	CListBox m_3DSLogList;
public:
	afx_msg void OnBnClicked3dsimport();
protected:
	CProgressCtrl m_LoadProgress;


public:
	afx_msg void OnBnClickedButton1();
	CStatic m_MaterialNrValue;
	CStatic m_VerticesNrValue;
	CStatic m_FacesNrValue;
	CStatic m_WarnsNrValue;
	CStatic m_ErrorNrValue;
};
