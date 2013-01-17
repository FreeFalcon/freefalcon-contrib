#pragma once
#include "afxcmn.h"
#include "afxwin.h"




//===========================================
//===========================================
#define MAX_COLOR						4000 //1568
#define MAX_PALETTE					2000
#define MAX_TEXTURE					10000 //1316 1278 1290
#define MAX_PARENT					10000 //1361 1328 1342
#define	MAX_NODES					20000// Need to update
#define	MAX_LOD						16384
// °°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°° typedefs
				int		LODUsedByCT(int, int*, int*, int* );
				int		LoadCT();
				void	SaveHDR(HANDLE);
				short int LoadHDR(HANDLE);

// °°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°° typedefs

// °°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°° typedefs

typedef struct	taghdr// HDR header structure
{
	unsigned int version;
	unsigned int nColor;
	unsigned int nDarkColor;
	unsigned int nPalette;
	unsigned int nTexture;
	unsigned int unk[2];
	unsigned int nLOD;
	unsigned int nParent;
}	HDR;


typedef struct	taghdrcolor// HDR color
{
	float f[4];
} HDRCOLOR;

typedef struct	taghdrpalette// HDR palettes
{
	unsigned char r[256];
	unsigned char g[256];
	unsigned char b[256];
	unsigned char a[256];
	unsigned int uint[2];
}	HDRPALETTE;

typedef struct	taghdrlod// HDR LODs
{
	unsigned int uint[5];
}	HDRLOD;

typedef struct	taghdrtexture// HDR Textures
{
	unsigned int uint[10];
}	HDRTEXTURE;

typedef struct	taghdrparent// HDR parents
{
	float f[7];
	unsigned int uint[2];
	unsigned short int ushort1[2];
	unsigned char uchar[4];
	unsigned short int ushort2[2];
}	HDRPARENT;

typedef struct	taghdrlodres// HDR LOD data (found at end of HDR file)
{
	int lodind[10];
	float distance[10];
	float slot1[100];
	float slot2[100];
	float slot3[100];
	float dyn1[10];
	float dyn2[10];
	float dyn3[10];
}	HDRLODRES;

typedef struct tagct
{
	unsigned char domain;
	unsigned char cclass;
	unsigned char type;
	unsigned char subtype;
	unsigned char specific;
	unsigned char mode;
	unsigned short int parent[8];
	unsigned char ftype;
	unsigned int foffset;
	char name[22];
}	CT;


// CDBConverter dialog

class CDBConverter : public CDHtmlDialog
{
	DECLARE_DYNCREATE(CDBConverter)

public:
	CDBConverter(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDBConverter();
// Overrides
	BOOL Convert(CString);
	bool	Abort;

// Dialog Data
	enum { IDD = IDD_DBCONVERT, IDH = IDR_HTML_DBCONVERTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()

	int			LoadCT();
	void		SaveHDR(CFile);
	short int	LoadHDR(CFile);
	void		ExampleFunction();
	int			LODUsedByCT(int, int*, int*, int*, int*);

	HDR		hdr;
	HDRCOLOR	hdrColor[MAX_COLOR];
	HDRPALETTE	hdrPalette[MAX_PALETTE+2];
	HDRLOD		hdrLOD[MAX_LOD];
	HDRTEXTURE	hdrTexture[MAX_TEXTURE];
	HDRPARENT	hdrParent[MAX_PARENT+2];
	HDRLODRES	hdrLODRes[MAX_PARENT+2];
	CT	*ct;
	char				KoreaObjFolder[256];  // path to current <F4>\terrdata\Objects folder with "\" at end
	short				ctNumber;							// number of CT records

	int			maxNodes;
	DWORD		LastLOD;
	bool		NextLOD,PrevLOD,NewLOD, DBConvert, ReBoot, FragIt, AssembleIt;
	HANDLE		m_hInstance;


public:
	CProgressCtrl m_ConvProgress;
	CStatic m_ConvText;
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedNextkey();
	CButton m_NextKey;
	afx_msg void OnBnClickedPrevkey();
	afx_msg void OnStnClickedConvtext();
	CEdit M_gotovALUE;
	afx_msg void OnBnClickedGoto();
	afx_msg void OnBnClickedDball();
	afx_msg void OnBnClickedExit();
	CButton m_ExitKey;
	CButton m_GotoKey;
	CButton m_PrevKey;
	afx_msg void OnBnClickedFragdb();
	afx_msg void OnBnClickedMakedb();
};


