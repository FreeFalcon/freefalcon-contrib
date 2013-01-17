
//===========================================
//===========================================
#define MAX_COLOR						4000 //1568
#define MAX_PALETTE					2000
#define MAX_TEXTURE					10000 //1316 1278 1290
#define MAX_PARENT					10000 //1361 1328 1342
#define	MAX_NODES						20000// Need to update

// °°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°° typedefs
				int		LODUsedByCT(int, int*, int*);
				int		LoadCT();
				void	SaveHDR(HANDLE);
				short int LoadHDR(HANDLE);

// °°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°° typedefs
				char				KoreaObjFolder[256];  // path to current <F4>\terrdata\Objects folder with "\" at end
				short				ctNumber;							// number of CT records

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

HDR		hdr;

typedef struct	taghdrcolor// HDR color
{
	float f[4];
} HDRCOLOR;
HDRCOLOR	hdrColor[MAX_COLOR];

typedef struct	taghdrpalette// HDR palettes
{
	unsigned char r[256];
	unsigned char g[256];
	unsigned char b[256];
	unsigned char a[256];
	unsigned int uint[2];
}	HDRPALETTE;
HDRPALETTE	hdrPalette[MAX_PALETTE+2];

typedef struct	taghdrlod// HDR LODs
{
	unsigned int uint[5];
}	HDRLOD;
HDRLOD	hdrLOD[MAX_LOD];

typedef struct	taghdrtexture// HDR Textures
{
	unsigned int uint[10];
}	HDRTEXTURE;
HDRTEXTURE	hdrTexture[MAX_TEXTURE];

typedef struct	taghdrparent// HDR parents
{
	float f[7];
	unsigned int uint[2];
	unsigned short int ushort1[2];
	unsigned char uchar[4];
	unsigned short int ushort2[2];
}	HDRPARENT;
HDRPARENT	hdrParent[MAX_PARENT+2];

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
HDRLODRES	hdrLODRes[MAX_PARENT+2];

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
CT	*ct;

