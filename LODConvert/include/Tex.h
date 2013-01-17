/***************************************************************************\
    Tex.h
    Miro "Jammer" Torrielli
    10Oct03

	- Begin Major Rewrite
\***************************************************************************/
#ifndef _TEX_H_
#define _TEX_H_

#include "grtypes.h"
#include "Context.h"
#include "Palette.h"

struct IDirectDrawSurface7;
struct IDirect3DDevice7;
enum _D3DX_SURFACEFORMAT;
class PaletteHandle;


class Texture
{
public:
	Texture();
	~Texture();

public:
	int dimensions;
	void *imageData;
	Palette *palette;
	DWORD flags;
	DWORD chromaKey;

	#ifdef _DEBUG
	static DWORD m_dwNumHandles;		// Number of instances
	static DWORD m_dwBitmapBytes;		// Bytes allocated for bitmap copies
	static DWORD m_dwTotalBytes;		// Total number of bytes allocated (including bitmap copies and object size)
	#endif

protected:
	TextureHandle *texHandle;

public:
	static void SetupForDevice(DXContext *texRC, char *path);
	static void CleanupForDevice(DXContext *texRC);
	static bool IsSetup();
	BOOL LoadImage(char *filename, DWORD newFlags = 0, BOOL addDefaultPath = TRUE);
	BOOL LoadAndCreate(char *filename, DWORD newFlags = 0);
	bool CreateTexture(char *strName = NULL);
	bool UpdateMPR(char *strName = NULL);
	void FreeAll() { FreeTexture(); FreeImage(); };
	void FreeImage();
	void FreeTexture();
	void FreePalette();
	void RestoreAll();
	DWORD TexHandle() { return (DWORD) texHandle; };
	bool DumpImageToFile(char*,int palID = 0);

protected:
	bool SaveDDS_DXTn(const char *szFileName, BYTE* pDst, int dimensions, DWORD flags);

	#ifdef _DEBUG
public:
	static void MemoryUsageReport();
	#endif
};

#endif // _TEX_H_
