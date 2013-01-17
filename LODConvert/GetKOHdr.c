//#include "KOHDR_CT.h"  // uncomment

#include "LODEditor.h"  // remove	 (need for testing)

																											
/*																		 
hdrTexture[iTex].uint[0] File offset
hdrTexture[iTex].uint[1] Compressed length
hdrTexture[iTex].uint[2] Width (64, 128, 256)
hdrTexture[iTex].uint[3] Height=0
hdrTexture[iTex].uint[4] Flag=0
hdrTexture[iTex].uint[5] ChromaKey(0,1,3,4,5,6,7,8,9,10,11,12,13)160	(2)224
//********************************
uint[5] is the texture flags
#define TEXF_MIPMAP          0x0001	  (1)
#define TEXF_CHROMAKEY       0x0020	 (32)
#define TEXF_ALPHA           0x0040	 (64)
#define TEXF_PALETTE         0x0080	(128)
//********************************

PALETTE must always be set, and Alpha need only be set for textures
whose color palette is not 100% opaque.
       ***** The real chromakey value!!!   
						0xFF000000 = 4278190080
						color byte order = FFbbggrr
hdrTexture[iTex].uint[6] Tex HandlePalette	0=4294901760   Blue
																						1=4294902015	 Magenta
																						2=0
																						3=4294902015	 Magenta
																						4=4278190080	 Black
																						5=4286535939	 Dark Blue-Green
																						6=4286667266	 Dark Blue-Green
																						7=4294902015	 Magenta
																						8=4294902015	 Magenta
																						9=4294902015	 Magenta
																						10=4294901760	 Blue
																						11=4294902015	 Magenta
																						12=4294901760	 Blue
																						13=4294902015	 Magenta
hdrTexture[iTex].uint[7] Unk=0
hdrTexture[iTex].uint[8] Palette number
hdrTexture[iTex].uint[9] Unk=0
*/

// remove remove
int		LoadCT();
void	SaveHDR(HANDLE);
short int LoadHDR(HANDLE);
void ExampleFunction();
int		LODUsedByCT(int, int*, int*);
// remove remove

//==============================================================
//  Alex Alex Alex Alex Alex Alex Alex Alex Alex Alex Alex Alex
//==============================================================
	void ExampleFunction()
	{
		HANDLE  fp;
		int			c, d, FileType;
		int			Domain, Class;
		char		szFile[512];
		int			DXL_Offset = 0, DXL_size = 0, DXL_Processing_Type = 0;

		sprintf(szFile, "%sKoreaObj.hdr", KoreaObjFolder); 
		fp = fopen(szFile,"rb+");
		LoadHDR(fp);
		fclose(fp);
		LoadCT();

		//...

		// Open KO.lod
		sprintf(szFile, "%sKoreaObj.lod", KoreaObjFolder); 
		fp = fopen(szFile,"rb+");

		for (c = 0; c < hdr.nLOD; c++)
		{

			//...

			// Get Domain, Class and File Type, if LOD has a CT record
			// if no CT record, then Domain = Class = FileType = 0
			FileType = LODUsedByCT((int) c, (int*) &Domain, (int*) &Class);

			if (Domain < 0)
			{
				// a shadow BSP model
				//...
				Domain = -Domain;
			}

			//...

			// hdrLOD[c].uint[3] = offset into KO.lod
			// hdrLOD[c].uint[4] = size of lod (bytes)

			// Seek to start of this LOD in KO.lod
			fseek (fp, hdrLOD[c].uint[3], SEEK_SET);
			// Read LOD
			for (d = 0; d < hdrLOD[c].uint[4]; d++)
			{
				//...
			}
			

			//...

			// Put DXL file offset and size in KO.hdr
			hdrLOD[c].uint[0] = DXL_Offset;
			hdrLOD[c].uint[1] = DXL_size;
			hdrLOD[c].uint[1] = DXL_Processing_Type; // not required...may be useful
																							 // your code for Air Vehicles, Land Vehicles, Weapons, etc.
																							 // I may be able to add 2D and 3D cockpit code.
		}

		//...

		sprintf(szFile, "%sKoreaObj.hdr", KoreaObjFolder); 
		fp = fopen(szFile,"wb");
		SaveHDR(fp);
		fclose(fp);

		return;
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

short int LoadHDR(HANDLE fp)// send to the function the FILE pointer of your open file stream (Koreaobj.hdr)
{
				int						c,d;

	// Zero out HDR records
	InitHDR();

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

void	SaveHDR(HANDLE fp)
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

	fclose(fp);
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
int	LoadCT()
{
				int						c,d;
				short					ctNumber = 0;
				float					BubbleDist;
				HANDLE				fp;
				char					nctr[6];

	sprintf(szMsg, "%sFalcon4.ct", KoreaObjFolder); 
	fp = fopen(szMsg,"rb");
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
		fseek(fp,c*81+10,SEEK_SET);
		ct[c].domain = dogetchar(fp);
		ct[c].cclass = dogetchar(fp);
		ct[c].type = dogetchar(fp);
		ct[c].subtype = dogetchar(fp);
		ct[c].specific = dogetchar(fp);
		ct[c].mode = dogetchar(fp);
		fseek(fp,10,SEEK_CUR);
		BubbleDist = dogetfloat(fp);
		fseek(fp,32,SEEK_CUR);
		for(d=0;d<8;d++)
			ct[c].parent[d] = dogetshort(fp);
//		fseek(fp,2,SEEK_CUR);
		ct[c].ftype = dogetchar(fp);
		ct[c].foffset = dogetlong(fp);
			
	}
	fclose(fp);
	return(ctNumber);
} // end LoadCT

//====================================================================================
//====================================================================================

//=================================================================
//  LODUsedBy() - Find LOD that is used by CT
//=================================================================
int	LODUsedByCT(int eLOD, int* Domain, int* Class)
{
			int					c, d, e;
			int					FileType = 0;
      int					grDat, nL;

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
						return (FileType);
          }
       }
    }
  }

	return (FileType);
}	// end LODUsedByCT()


//====================================================================================
//====================================================================================
