/***************************************************************************\
    Context.h
    Miro "Jammer" Torrielli
    10Oct03

	- Begin Major Rewrite
\***************************************************************************/
#include "stdafx.h"
#include <io.h>
#include <fcntl.h>
#include "Utils\lzss.h"
#include "Loader.h"
#include "grinline.h"
#include "StateStack.h"
#include "ObjectLOD.h"
#include "PalBank.h"
#include "TexBank.h"
#include "Image.h"
#include "TerrTex.h"
#include "LodConvert.h"
//#include "FalcLib\include\playerop.h"
//#include "FalcLib\include\dispopts.h"

// Static data members (used to avoid requiring "this" to be passed to every access function)
int					TextureBankClass::nTextures			= 0;
TexBankEntry*		TextureBankClass::TexturePool		= NULL;
TempTexBankEntry*	TextureBankClass::TempTexturePool	= NULL;
BYTE*				TextureBankClass::CompressedBuffer	= NULL;	
int					TextureBankClass::deferredLoadState	= 0;
char				TextureBankClass::baseName[256];
FileMemMap			TextureBankClass::TexFileMap;
#ifdef _DEBUG
int					TextureBankClass::textureCount		= 0;
#endif

#ifdef USE_SH_POOLS
extern MEM_POOL gBSPLibMemPool;
#endif

//extern bool g_bUseMappedFiles;
#define	g_bUseMappedFiles true

void TextureBankClass::Setup(int nEntries)
{
	// Create our array of texture headers
	nTextures = nEntries;

	if(nEntries)
	{
		#ifdef USE_SH_POOLS
		TexturePool = (TexBankEntry *)MemAllocPtr(gBSPLibMemPool,sizeof(TexBankEntry)*nEntries,0);
		TempTexturePool = (TempTexBankEntry *)MemAllocPtr(gBSPLibMemPool,sizeof(TempTexBankEntry)*nEntries,0);
		#else
		TexturePool	= new TexBankEntry[nEntries];
		TempTexturePool = new TempTexBankEntry[nEntries];
		#endif
	}
	else
	{
		TexturePool = NULL;
		TempTexturePool = NULL;
	}
}

void TextureBankClass::Cleanup(void)
{
	// Clean up our array of texture headers
	#ifdef USE_SH_POOLS
	MemFreePtr(TexturePool);
	MemFreePtr(TempTexturePool);
	#else
	delete[] TexturePool;
	delete[] TempTexturePool;
	#endif
	TexturePool = NULL;
	TempTexturePool = NULL;
	nTextures = 0;

	// Clean up the decompression buffer
	#ifdef USE_SH_POOLS
	MemFreePtr(CompressedBuffer);
	#else
	delete[] CompressedBuffer;
	#endif
	CompressedBuffer = NULL;

	// Close our texture resource file
	if (TexFileMap.IsReady()) {
		CloseTextureFile();
	}
}

void TextureBankClass::ReadPool(int file, char *basename)
{
	int result;
	int	maxCompressedSize;

	ZeroMemory(baseName,sizeof(baseName));
	sprintf(baseName,"%s",basename);

	// Read the number of textures in the pool
	result = read(file,&nTextures,sizeof(nTextures));
	if(nTextures == 0) return;

	// Read the size of the biggest compressed texture in the pool
	result = read(file,&maxCompressedSize,sizeof(maxCompressedSize));

	#ifdef USE_SH_POOLS
	CompressedBuffer = (BYTE *)MemAllocPtr(gBSPLibMemPool,sizeof(BYTE)*maxCompressedSize,0);
	#else
	CompressedBuffer = new BYTE[maxCompressedSize];
	#endif
	ShiAssert(CompressedBuffer);

	if(CompressedBuffer)
		ZeroMemory(CompressedBuffer,maxCompressedSize);

	// Setup the pool
	Setup(nTextures);

	result = read(file,TempTexturePool,sizeof(*TempTexturePool)*nTextures);

	if(result < 0)
	{
		char message[256];
		sprintf(message,"Reading object texture bank: %s",strerror(errno));
		ShiError(message);
	}

	for(int i = 0; i < nTextures; i++)
	{
		TexturePool[i].fileOffset = TempTexturePool[i].fileOffset;
		TexturePool[i].fileSize = TempTexturePool[i].fileSize;

		TexturePool[i].tex = TempTexturePool[i].tex;
		TexturePool[i].texN = TempTexturePool[i].tex;
		TexturePool[i].texN.flags |= MPR_TI_INVALID;

/*		if(DisplayOptions.bMipmapping)
		{*/
			TexturePool[i].tex.flags |= MPR_TI_MIPMAP;
			TexturePool[i].texN.flags |= MPR_TI_MIPMAP;
/*		}
*/
		TexturePool[i].palID = TempTexturePool[i].palID;
		TexturePool[i].refCount = TempTexturePool[i].refCount;
	}

	OpenTextureFile();
}

void TextureBankClass::FreeCompressedBuffer()
{
	#ifdef USE_SH_POOLS
	MemFreePtr(CompressedBuffer);
	#else
	delete[] CompressedBuffer;
	#endif

	CompressedBuffer = NULL;
}

void TextureBankClass::AllocCompressedBuffer(int maxCompressedSize)
{
	#ifdef USE_SH_POOLS
	CompressedBuffer = (BYTE *)MemAllocPtr(gBSPLibMemPool, sizeof(BYTE)*maxCompressedSize, 0 );
	#else
	CompressedBuffer = new BYTE[maxCompressedSize];
	#endif

	ShiAssert(CompressedBuffer);

	if(CompressedBuffer)
		ZeroMemory(CompressedBuffer,maxCompressedSize);
}

void TextureBankClass::OpenTextureFile()
{
	char filename[_MAX_PATH];

	ShiAssert(!TexFileMap.IsReady());

	strcpy(filename,baseName);
	strcat(filename,".TEX");

	if(!TexFileMap.Open(filename,FALSE,!g_bUseMappedFiles))
	{
		char message[256];
		sprintf(message,"Failed to open object texture file %s\n",filename);
		ShiError(message);
	}
}

void TextureBankClass::CloseTextureFile(void)
{
	TexFileMap.Close();
}

void TextureBankClass::Reference(int id)
{
	int	 isLoaded;

	ShiAssert(IsValidIndex(id));

	// Get our reference to this texture recorded to ensure it doesn't disappear out from under us
    //EnterCriticalSection(&ObjectLOD::cs_ObjectLOD);

	isLoaded = TexturePool[id].refCount;
	ShiAssert(isLoaded >= 0);
	TexturePool[id].refCount++;

	// If we already have the data, just verify that fact. Otherwise, load it.
	if(isLoaded)
	{
		ShiAssert(TexturePool[id].tex.imageData || TexturePool[id].tex.TexHandle() || deferredLoadState);

		//LeaveCriticalSection(&ObjectLOD::cs_ObjectLOD);
	}
	else
	{
		ShiAssert(TexFileMap.IsReady());
		ShiAssert(CompressedBuffer);
		ShiAssert(TexturePool[id].tex.imageData == NULL);
		ShiAssert(TexturePool[id].tex.TexHandle() == NULL);
	
		// Get the palette pointer
		TexturePool[id].tex.palette = &ThePaletteBank.PalettePool[TexturePool[id].palID];
		ShiAssert(TexturePool[id].tex.palette);
		TexturePool[id].tex.palette->Reference();

	    //LeaveCriticalSection(&ObjectLOD::cs_ObjectLOD);

//		if(!deferredLoadState)
			ReadImageData(id);
	}
}

// Calls to this func are enclosed in the critical section cs_ObjectLOD by ObjectLOD::Unload()
void TextureBankClass::Release(int id)
{
	ShiAssert(IsValidIndex(id));
	ShiAssert(TexturePool[id].refCount > 0);

	if(TexturePool[id].refCount > 0)
	{
		TexturePool[id].refCount--;
	}

	if(TexturePool[id].refCount == 0)
	{
		TexturePool[id].tex.FreeAll();

		if(!(TexturePool[id].texN.flags & MPR_TI_INVALID))
		{
			TexturePool[id].texN.FreeAll();
		}

#ifdef _DEBUG
		textureCount--;
#endif
	}
}

void TextureBankClass::ReadImageData(int id, bool forceNoDDS)
{
	int		retval;
	int		size;
	BYTE	*cdata;

	
	ShiAssert(TexturePool[id].refCount);

	if(!forceNoDDS /*&& DisplayOptions.m_texMode == DisplayOptionsClass::TEX_MODE_DDS*/)
	{
		ReadImageDDS(id);
		ReadImageDDSN(id);
		return;
	}

	if(g_bUseMappedFiles)
	{
	    cdata = TexFileMap.GetData(TexturePool[id].fileOffset, TexturePool[id].fileSize);
	    ShiAssert(cdata);
	}
	else
	{
	    if(!TexFileMap.ReadDataAt(TexturePool[id].fileOffset,CompressedBuffer,TexturePool[id].fileSize))
		{
			char message[120];
			sprintf(message,"%s: Bad object texture seek (%0d)",strerror(errno),TexturePool[id].fileOffset);
			ShiError(message);
	    }

	    cdata = CompressedBuffer;
	}

	// Allocate memory for the new texture
	size = TexturePool[id].tex.dimensions;
	size = size*size;

	TexturePool[id].tex.imageData = glAllocateMemory(size,FALSE);
	ShiAssert(TexturePool[id].tex.imageData);

	// Uncompress the data into the texture structure
	retval = LZSS_Expand(cdata,(BYTE*)TexturePool[id].tex.imageData,size);
	ShiAssert(retval == TexturePool[id].fileSize);

#ifdef _DEBUG
	textureCount++;
#endif
}

void TextureBankClass::SetDeferredLoad(BOOL state)
{
	LoaderQ	*request;

	// Allocate space for the async request
	request = new LoaderQ;

	if(!request)
		ShiError("Failed to allocate memory for a object texture load state change request");

	// Build the data transfer request to get the required object data
	request->filename	= NULL;
	request->fileoffset	= 0;
	request->callback	= LoaderCallBack;
	request->parameter	= (void*)state;

	// Submit the request to the asynchronous loader
	TheLoader.EnqueueRequest(request);	
}

void TextureBankClass::LoaderCallBack(LoaderQ* request)
{
	BOOL state = (int)request->parameter;

    //EnterCriticalSection(&ObjectLOD::cs_ObjectLOD);

	// If we're turning deferred loads off, go back and do all the loads we held up
	if(deferredLoadState && !state)
	{
		// Check each texture
		for(int id = 0; id < nTextures; id++)
		{
			// See if it is in use
			if(TexturePool[id].refCount)
	
				// This one is in use. Is it already loaded?
				if(!TexturePool[id].tex.imageData && !TexturePool[id].tex.TexHandle())
	
					// Nope, go get it.
					ReadImageData(id);
		}
	}

	// Now store the new state
	deferredLoadState = state;

    //LeaveCriticalSection(&ObjectLOD::cs_ObjectLOD);

	// Free the request queue entry
	delete request;
}

void TextureBankClass::FlushHandles(void)
{
	int id;

	for(id = 0; id < nTextures; id++)
	{
		ShiAssert(TexturePool[id].refCount == 0);

		while(TexturePool[id].refCount > 0)
		{
			Release(id);
		}
	}
}

void TextureBankClass::Select(int id)
{
	ShiAssert(IsValidIndex(id));

	if(IsValidIndex(id))
	{
		ShiAssert(TexturePool[id].refCount > 0);

		if(TexturePool[id].tex.TexHandle() == NULL)
		{
			TexturePool[id].tex.CreateTexture();
			TexturePool[id].tex.FreeImage();
		}

		//TheStateStack.context->SelectTexture1(TexturePool[id].tex.TexHandle());
		DX->SelectTexture(TexturePool[id].tex.TexHandle());

		if(1/*DisplayOptions.m_texMode == DisplayOptionsClass::TEX_MODE_DDS*/)
		{
			//TheStateStack.context->SetTexID(id);

/*			if(TheTerrTextures.lightLevel < 0.5f && !(TexturePool[id].texN.flags & MPR_TI_INVALID))
			{
				if(TexturePool[id].texN.TexHandle() == NULL)
				{
					TexturePool[id].texN.CreateTexture();
					TexturePool[id].texN.FreeImage();
				}

				TheStateStack.context->SelectTexture2(TexturePool[id].texN.TexHandle());
			}*/
		}

		//TheStateStack.context->SetPalID(TexturePool[id].palID);
	}
}

BOOL TextureBankClass::IsValidIndex(int id)
{
	return((id >= 0) && (id < nTextures));
}

void TextureBankClass::RestoreAll()
{
}

void TextureBankClass::SyncDDSTextures(bool bForce)
{
	char szFile[256];
	FILE *fp;

	ShiAssert(TexturePool);

	CreateDirectory(baseName,NULL);

	for(DWORD id = 0; id < nTextures; id++)
	{
		sprintf(szFile,"%s\\%d.dds",baseName,id);
		fp = fopen(szFile,"rb");

		if(!fp || bForce)
		{
			if(fp)
				fclose(fp);

			UnpackPalettizedTexture(id);
		}
		else
			fclose(fp);

		TexturePool[id].tex.flags |= MPR_TI_DDS;
		TexturePool[id].tex.flags &= ~MPR_TI_PALETTE;
		TexturePool[id].texN.flags |= MPR_TI_DDS;
		TexturePool[id].texN.flags &= ~MPR_TI_PALETTE;
	}
}

void TextureBankClass::UnpackPalettizedTexture(DWORD id)
{
	char szFile[256];

	CreateDirectory(baseName,NULL);

	if(TexturePool[id].tex.dimensions > 0)
	{
		TexturePool[id].tex.palette = &ThePaletteBank.PalettePool[TexturePool[id].palID];
		ShiAssert(TexturePool[id].tex.palette);
		TexturePool[id].tex.palette->Reference();

		ReadImageData(id,true);
		sprintf(szFile,"%s\\%d",baseName,id);
		TexturePool[id].tex.DumpImageToFile(szFile,TexturePool[id].palID);
		Release(id);
	}
	else
	{
		sprintf(szFile,"%s\\%d.dds",baseName,id);
		FILE* fp = fopen(szFile,"wb");
		fclose(fp);
	}
}

void TextureBankClass::ReadImageDDS(DWORD id)
{
	DDSURFACEDESC2	ddsd;
	DWORD			dwSize,dwMagic;
	char			szFile[256];
	FILE			*fp;

	TexturePool[id].tex.flags |= MPR_TI_DDS;
	TexturePool[id].tex.flags &= ~MPR_TI_PALETTE;

	sprintf(szFile,"%s\\%d.dds",baseName,id);
	fp = fopen(szFile,"rb");

	if(!fp) {

		CString	Text;
		Text.Format("Can Not Load Texture %d", id);
		MessageBox(NULL, Text, NULL, MB_OK);
		return;
	}

	fread(&dwMagic,1,sizeof(DWORD),fp);
	ShiAssert(dwMagic == MAKEFOURCC('D','D','S',' '));

	// Read first compressed mipmap
	fread(&ddsd,1,sizeof(DDSURFACEDESC2),fp);

	 // MLR 1/25/2004 - Little kludge so F4 can read DDS files made by dxtex 
	if( ddsd.dwLinearSize == 0)
	{
		if( ddsd.ddpfPixelFormat.dwFourCC == MAKEFOURCC('D','X','T','3') ||
			ddsd.ddpfPixelFormat.dwFourCC == MAKEFOURCC('D','X','T','5'))
		{
			ddsd.dwLinearSize = ddsd.dwWidth*ddsd.dwWidth;
			ddsd.dwFlags |= DDSD_LINEARSIZE;
		}

		if( ddsd.ddpfPixelFormat.dwFourCC == MAKEFOURCC('D','X','T','1'))
		{
			ddsd.dwLinearSize = ddsd.dwWidth*ddsd.dwWidth/2;
			ddsd.dwFlags |= DDSD_LINEARSIZE;
		}
	}


	ShiAssert(ddsd.dwFlags & DDSD_LINEARSIZE)

	switch( ddsd.ddpfPixelFormat.dwFourCC )
	{
	case MAKEFOURCC('D','X','T','1'):
		TexturePool[id].tex.flags |= MPR_TI_DXT1;
		break;
	case MAKEFOURCC('D','X','T','3'):
		TexturePool[id].tex.flags |= MPR_TI_DXT3;
		break;
	case MAKEFOURCC('D','X','T','5'):
		TexturePool[id].tex.flags |= MPR_TI_DXT5;
		break;
	default:
		ShiAssert(false);
	}

	switch( ddsd.dwWidth )
	{
	case 16:
		TexturePool[id].tex.flags |= MPR_TI_16;
		break;
	case 32:
		TexturePool[id].tex.flags |= MPR_TI_32;
		break;
	case 64:
		TexturePool[id].tex.flags |= MPR_TI_64;
		break;
	case 128:
		TexturePool[id].tex.flags |= MPR_TI_128;
		break;
	case 256:
		TexturePool[id].tex.flags |= MPR_TI_256;
		break;
	case 512:
		TexturePool[id].tex.flags |= MPR_TI_512;
		break;
	case 1024:
		TexturePool[id].tex.flags |= MPR_TI_1024;
		break;
	case 2048:
		TexturePool[id].tex.flags |= MPR_TI_2048;
		break;
	default:
		ShiAssert(false);
	}

	dwSize = ddsd.dwLinearSize;
	TexturePool[id].tex.imageData = (BYTE *)glAllocateMemory(dwSize,FALSE);
	fread(TexturePool[id].tex.imageData,1,dwSize,fp);
 
	TexturePool[id].tex.dimensions = dwSize;

	fclose(fp);

#ifdef _DEBUG
	textureCount++;
#endif
}

void TextureBankClass::ReadImageDDSN(DWORD id)
{
	DDSURFACEDESC2	ddsd;
	DWORD			dwSize,dwMagic;
	char			szFile[256];
	FILE			*fp;

	sprintf(szFile,"%s\\%dN.dds",baseName,id);
	fp = fopen(szFile,"rb");

	if(!fp)
		return;

	TexturePool[id].texN.flags |= MPR_TI_DDS;
	TexturePool[id].texN.flags &= ~MPR_TI_PALETTE;
	TexturePool[id].texN.flags &= ~MPR_TI_INVALID;

	fread(&dwMagic,1,sizeof(DWORD),fp);
	ShiAssert(dwMagic == MAKEFOURCC('D','D','S',' '));

	// Read first compressed mipmap
	fread(&ddsd,1,sizeof(DDSURFACEDESC2),fp);

	 // MLR 1/25/2004 - Little kludge so F4 can read DDS files made by dxtex 
	if( ddsd.dwLinearSize == 0)
	{
		if( ddsd.ddpfPixelFormat.dwFourCC == MAKEFOURCC('D','X','T','3') ||
			ddsd.ddpfPixelFormat.dwFourCC == MAKEFOURCC('D','X','T','5'))
		{
			ddsd.dwLinearSize = ddsd.dwWidth*ddsd.dwWidth;
			ddsd.dwFlags |= DDSD_LINEARSIZE;
		}

		if( ddsd.ddpfPixelFormat.dwFourCC == MAKEFOURCC('D','X','T','1'))
		{
			ddsd.dwLinearSize = ddsd.dwWidth*ddsd.dwWidth/2;
			ddsd.dwFlags |= DDSD_LINEARSIZE;
		}
	}

	ShiAssert(ddsd.dwFlags & DDSD_LINEARSIZE)

	switch( ddsd.ddpfPixelFormat.dwFourCC )
	{
	case MAKEFOURCC('D','X','T','1'):
		TexturePool[id].texN.flags |= MPR_TI_DXT1;
		break;
	case MAKEFOURCC('D','X','T','3'):
		TexturePool[id].tex.flags |= MPR_TI_DXT3;
		break;
	case MAKEFOURCC('D','X','T','5'):
		TexturePool[id].texN.flags |= MPR_TI_DXT5;
		break;
	default:
		ShiAssert(false);
	}

	switch( ddsd.dwWidth )
	{
	case 16:
		TexturePool[id].texN.flags |= MPR_TI_16;
		break;
	case 32:
		TexturePool[id].texN.flags |= MPR_TI_32;
		break;
	case 64:
		TexturePool[id].texN.flags |= MPR_TI_64;
		break;
	case 128:
		TexturePool[id].texN.flags |= MPR_TI_128;
		break;
	case 256:
		TexturePool[id].texN.flags |= MPR_TI_256;
		break;
	case 512:
		TexturePool[id].texN.flags |= MPR_TI_512;
		break;
	case 1024:
		TexturePool[id].texN.flags |= MPR_TI_1024;
		break;
	case 2048:
		TexturePool[id].texN.flags |= MPR_TI_2048;
		break;
	default:
		ShiAssert(false);
	}

	dwSize = ddsd.dwLinearSize;
	TexturePool[id].texN.imageData = (BYTE *)glAllocateMemory(dwSize,FALSE);
	fread(TexturePool[id].texN.imageData,1,dwSize,fp);
 
	TexturePool[id].texN.dimensions = dwSize;

	fclose(fp);

#ifdef _DEBUG
	textureCount++;
#endif
}

void TextureBankClass::RestoreTexturePool()
{
	for(int i = 0; i < nTextures; i++)
	{
		TexturePool[i].fileOffset = TempTexturePool[i].fileOffset;
		TexturePool[i].fileSize = TempTexturePool[i].fileSize;
		TexturePool[i].tex = TempTexturePool[i].tex;
		TexturePool[i].texN = TempTexturePool[i].tex;
		TexturePool[i].palID = TempTexturePool[i].palID;
		TexturePool[i].refCount = TempTexturePool[i].refCount;
	}
}

void TextureBankClass::SelectHandle(DWORD TexHandle)
{
	DX->SelectTexture(TexHandle);
}

DWORD TextureBankClass::GetHandle(DWORD id)
{
	ShiAssert(IsValidIndex(id));

	if(IsValidIndex(id))
	{
		ShiAssert(TexturePool[id].refCount > 0);

		if(TexturePool[id].tex.TexHandle() == NULL)
		{
			TexturePool[id].tex.CreateTexture();
			TexturePool[id].tex.FreeImage();
		}

		//TheStateStack.context->SelectTexture1(TexturePool[id].tex.TexHandle());
		return TexturePool[id].tex.TexHandle();
	}

	return NULL;
}
