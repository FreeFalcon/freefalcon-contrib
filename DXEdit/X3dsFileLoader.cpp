#include <iostream>
#include <string>
#include <fstream>
using namespace std;

#include "3dsFileFormat.h"
#include "3dsFileLoader.h"

void	IdleMode();

void C3dsFileLoader::UserObjName(string Name)
{
}

void C3dsFileLoader::User3dVert(float x, float y, float z)
{
}

void C3dsFileLoader::User3dFace(unsigned short A, unsigned short B, unsigned short C, unsigned short Flags)
{
}

void C3dsFileLoader::UserCamera(float px, float py, float pz,
								float tx, float ty, float tz,
								float Bank, float Lens)
{
}

void C3dsFileLoader::UserFaceMaterial(string Name, int Number)
{
}

void C3dsFileLoader::UserMapVertex(float U, float V)
{
}

void C3dsFileLoader::UserTransformMatrix(const Transform3dsMatrix &Transform, const Translate3dsMatrix &Translate)
{
}

void C3dsFileLoader::UserLight(float x, float y, float z)
{
}

void C3dsFileLoader::UserSpotLight(float x, float y, float z, float HotSpot, float FallOff)
{
}

void C3dsFileLoader::UserMatName(const string Name)
{
}

void C3dsFileLoader::UserMapFile(const string FileName)
{
}

void C3dsFileLoader::UserFrames(long Start, long End)
{
}

void C3dsFileLoader::UserTrackObjectName(string Name, int Key1, int Key2, int Key3)
{
}

void C3dsFileLoader::UserTrackPos(int Frame, long Flags, float x, float y, float z)
{
}

void C3dsFileLoader::UserTrackRot(int Frame, long Flags, float DegX, float DegY, float DegZ)
{
}

void C3dsFileLoader::UserTrackScale(int Frame, long Flags, float ScaleX, float ScaleY, float ScaleZ)
{
}

void C3dsFileLoader::UserObjNumber(int ObjNumber)
{
}

void C3dsFileLoader::UserChunkObj()
{
}

void C3dsFileLoader::UserEndOfFile()
{
}

void C3dsFileLoader::UserOneUnit(float Unit)
{
}

void C3dsFileLoader::UserViewPortLayout(ViewPortLayout &Layout)
{
}

void C3dsFileLoader::UserViewPortSize(int PosX, int PosY, int Width, int Height)
{
}

void C3dsFileLoader::UserViewPortData(ViewPortData &Data)
{
}

void C3dsFileLoader::UserViewUser(float x, float y, float z, float Width, float XYangle, float YZangle, float BackAngle)
{
}

void C3dsFileLoader::UserViewCamera(string CameraName)
{
}

void C3dsFileLoader::movePC(long forward)
{
	PC += forward;
	mInputFile.seekg(PC);
}

void C3dsFileLoader::ReadRGBF()
{
    float c[3];
    if (!ReadBlock((char *) c, sizeof(c))) 
		return;
}

void C3dsFileLoader::ReadRGBB()
{
	unsigned char c[3],t;
	if(!ReadBlock((char *) c, sizeof(c)))
		return;

	// swap colors
	t=c[0]; c[0]=c[2]; c[2]=t;
	unsigned int RGB;
	memcpy(&RGB, c, sizeof(c));
	*(((char*)&RGB)+3)=(char)0xff;
	SetRGB(RGB);
}

void C3dsFileLoader::ReadObjBlock()
{
	char ObjName[255];
	ReadASCIIZ(ObjName);
	UserObjName(string(ObjName));
}

void C3dsFileLoader::ReadCamera()
{
    float c[8];
    if (!ReadBlock( (char *) c, sizeof(c)))
		return;
	UserCamera(c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7]);
}

void C3dsFileLoader::ReadFaceMat()
{
    short n, nf;
	char MaterialName[255];

    ReadASCIIZ(MaterialName);

    if (!ReadBlock((char *) &n, sizeof(n))) 
		return;
    while (n-- > 0) 
	{
		if (!ReadBlock((char*) &nf, sizeof(nf)))
			return;
		UserFaceMaterial(string(MaterialName), (int) nf);
    }
}

void C3dsFileLoader::ReadFrames()
{
    long c[2];
    if (!ReadBlock((char*) c, sizeof(c)))
		return;
	UserFrames(c[0], c[1]);
}

void C3dsFileLoader::ReadLight()
{
    float c[3];
    if (!ReadBlock( (char*) c, sizeof(c))) 
		return;
	UserLight(c[0], c[1], c[2]);
}

void C3dsFileLoader::ReadMapFile()
{
	char fileName[255];
	ReadASCIIZ(fileName);
	UserMapFile(string(fileName));
}

void C3dsFileLoader::ReadMapList()
{
    short nv;
    float c[2];

    if (!ReadBlock((char *) &nv, sizeof(nv))) 
		return;
    while (nv-- > 0) 
	{
        if (!ReadBlock((char *) c, sizeof(c)))
			return;
		UserMapVertex(c[0], c[1]);
    }
}

void C3dsFileLoader::ReadMatName()
{
	char	matName[255];

	ReadASCIIZ(matName);
	UserMatName(string(matName));
}

void C3dsFileLoader::ReadObjNumber()
{
    signed short n;

    if (!ReadBlock((char *) &n, sizeof(n)))
		return;
	UserObjNumber((int) n);
}

void C3dsFileLoader::ReadSpotLight()
{
    float c[5];
    if (!ReadBlock( (char*) c, sizeof(c)))
		return;
	UserSpotLight(c[0], c[1], c[2], c[3], c[4]);
}

void C3dsFileLoader::ReadTrackObjName()
{
    //unsigned short w[3];
	//char trackName[255];

	// Read ASCIIZ name
	//ReadASCIIZ(trackName);
    //if (!ReadBlock((char*) w, sizeof(w))) 
	//	return;
	//UserTrackObjectName(string(trackName), w[0], w[1], w[2]);
}

void C3dsFileLoader::ComposeObject()
{
}


void C3dsFileLoader::ReadTrackPos()
{
    short	n, nf;
    float	pos[3];
    long	flags;


	mInputFile.seekg(10, ios::cur);
	PC += 10;
    if(!ReadBlock( (char*) &n, sizeof(n))) 
		return;
	mInputFile.seekg(2, ios::cur);
	PC += 2;
    while (n-- > 0) 
	{
        int i;
        float dat;
        if (!ReadBlock( (char *) &nf, sizeof(nf))) 
			return;
        if (!ReadBlock((char *) &flags, sizeof(flags))) 
			return;
        for (i = 0; i < 32; i++)
            if (flags & (1 << i))
                if (!ReadBlock( (char *) &dat, sizeof(dat))) 
					return;
        if (!ReadBlock( (char *) pos, sizeof(pos)))
			return;
		UserTrackPos(nf, flags, pos[0], pos[1], pos[2]);
    }
}

void C3dsFileLoader::ReadTrackRot()
{
    unsigned short	n, nf;
    float			pos[4];
    long			flags;

	mInputFile.seekg(10, ios::cur);
	PC += 10;
    if(!ReadBlock((char *) &n, sizeof(n)))
		return;
	mInputFile.seekg(2, ios::cur);
	PC += 2;
    while (n-- > 0) 
	{
        int i;
        float dat;
        if (!ReadBlock( (char*) &nf, sizeof(nf)))
			return;
        if (!ReadBlock( (char*) &flags, sizeof(flags))) 
			return;
        for (i = 0; i < 32; i++)
            if (flags & (1 << i))
                if (!ReadBlock((char*) &dat, sizeof(dat))) 
					return;
		if (!ReadBlock((char *) pos, sizeof(pos))) 
			return;
		UserTrackRot(nf, flags, pos[0], pos[1], pos[2]);
    }
}

void C3dsFileLoader::ReadTrackScale()
{
    unsigned short n, nf;
    float pos[3];
	long flags;

	mInputFile.seekg(10, ios::cur);
	PC += 10;
    if(!ReadBlock((char *) &n, sizeof(n)))
		return;
	mInputFile.seekg(2, ios::cur);
	PC += 2;
    while (n-- > 0) 
	{
        int i;
        float dat;
        if (!ReadBlock((char*) &nf, sizeof(nf)))
			return;
        if (!ReadBlock((char*) &flags, sizeof(flags)))
			return;
        for (i = 0; i < 32; i++)
            if (flags & (1 << i))
                if (!ReadBlock( (char *) &dat, sizeof(dat)))
					return;
        if (!ReadBlock((char *) pos, sizeof(pos)))
			return;
		UserTrackScale(nf, flags, pos[0], pos[1], pos[2]);
    }
}

void C3dsFileLoader::ReadTrMatrix()
{
    float rot[9];
    float trans[3];
    if (!ReadBlock((char *) rot, sizeof(rot)))
		return;
    if (!ReadBlock( (char *) trans, sizeof(trans)))
		return;
	Transform3dsMatrix Transform;
	Translate3dsMatrix Translate;
	Transform._11 = rot[0]; Transform._12 = rot[1]; Transform._13 = rot[2];
	Transform._21 = rot[3]; Transform._22 = rot[4]; Transform._23 = rot[5];
	Transform._31 = rot[6]; Transform._32 = rot[7]; Transform._33 = rot[8];
	Translate._11 = trans[0]; Translate._12 = trans[1]; Translate._13 = trans[2];
	UserTransformMatrix(Transform, Translate);
}

void C3dsFileLoader::ReadVertList()
{
    short nv;
    float c[3];

    if (!ReadBlock((char *) &nv, 2))
		return;
    while (nv-- > 0) 
	{
        if (!ReadBlock((char *) c, sizeof(c)))
			return;
		User3dVert(c[0], c[1], c[2]);
    }
}

void C3dsFileLoader::ReadFaceList()
{
    unsigned short nv;
    unsigned short c[4];

    if (!ReadBlock((char *) &nv, sizeof(nv)))
		return;
    while (nv-- > 0)
	{
        if (!ReadBlock((char *) c, sizeof(c)))
			return;
		User3dFace(c[0], c[1], c[2], c[3]);
    }
}

void C3dsFileLoader::ReadObjChunk()
{
	UserChunkObj();
}

void C3dsFileLoader::EndReading()
{
	UserEndOfFile();
}

void C3dsFileLoader::ReadOneUnit()
{
	float OneUnit;
    if (!ReadBlock((char *) &OneUnit, sizeof(OneUnit)))
		return;
	UserOneUnit(OneUnit);
}

void C3dsFileLoader::ReadViewPortLayout()
{
    unsigned short	style;
    short			active, unknow1, swap, unknow2, swap_prior, swap_view;

	if (!ReadBlock((char *) &style, sizeof(style)))
		return;
    if (!ReadBlock((char *) &active, sizeof(active)))
		return;
    if (!ReadBlock((char *) &unknow1, sizeof(unknow1)))
		return;
    if (!ReadBlock((char *) &swap, sizeof(swap)))
		return;
    if (!ReadBlock((char *) &unknow2, sizeof(unknow2)))
		return;
    if (!ReadBlock((char *) &swap_prior, sizeof(swap_prior)))
		return;
    if (!ReadBlock((char *) &swap_view, sizeof(swap_view)))
		return;
	UserViewPortLayout(ViewPortLayout(style, active, unknow1, swap, unknow2, swap_prior, swap_view));
}

void C3dsFileLoader::ReadViewPortSize()
{
	short PosX, PosY, Width, Height; 
	if (!ReadBlock((char *) &PosX, sizeof(PosX)))
		return;
	if (!ReadBlock((char *) &PosY, sizeof(PosY)))
		return;
	if (!ReadBlock((char *) &Width, sizeof(Width)))
		return;
	if (!ReadBlock((char *) &Height, sizeof(Height)))
		return;
	UserViewPortSize(PosX, PosY, Width, Height);
}

void C3dsFileLoader::ReadViewPortData()
{
	short flags, axis_lockout;
    short win_x, win_y, win_w, win_h, win_view;
	float zoom; 
	float worldcenter_x, worldcenter_y, worldcenter_z;
	float horiz_ang, vert_ang;
	char camera_name[12];

	if (!ReadBlock((char *) &flags, sizeof(flags)))
		return;
	if (!ReadBlock((char *) &axis_lockout, sizeof(axis_lockout)))
		return;
	if (!ReadBlock((char *) &win_x, sizeof(win_x)))
		return;
	if (!ReadBlock((char *) &win_y, sizeof(win_y)))
		return;
	if (!ReadBlock((char *) &win_w, sizeof(win_w)))
		return;
	if (!ReadBlock((char *) &win_h, sizeof(win_h)))
		return;
	if (!ReadBlock((char *) &win_view, sizeof(win_view)))
		return;
	if (!ReadBlock((char *) &zoom, sizeof(zoom)))
		return;
	if (!ReadBlock((char *) &worldcenter_x, sizeof(worldcenter_x)))
		return;
	if (!ReadBlock((char *) &worldcenter_y, sizeof(worldcenter_y)))
		return;
	if (!ReadBlock((char *) &worldcenter_z, sizeof(worldcenter_z)))
		return;
	if (!ReadBlock((char *) &horiz_ang, sizeof(horiz_ang)))
		return;
	if (!ReadBlock((char *) &vert_ang, sizeof(vert_ang)))
		return;
	if (!ReadBlock((char *) camera_name, CameraNameSize))
		return;
	UserViewPortData(ViewPortData(flags, axis_lockout, win_x, win_y, win_w, win_h,
								  win_view, zoom, worldcenter_x, worldcenter_y,
								  worldcenter_z, horiz_ang, vert_ang, camera_name));
}

void C3dsFileLoader::ReadViewUser()
{
	float c[3];
	float Width, XYangle, YZangle, BackAngle;

	if (!ReadBlock((char *) c, sizeof(c)))
		return;
	if (!ReadBlock((char *) &Width, sizeof(Width)))
		return;
	if (!ReadBlock((char *) &XYangle, sizeof(XYangle)))
		return;
	if (!ReadBlock((char *) &YZangle, sizeof(YZangle)))
		return;
	if (!ReadBlock((char *) &BackAngle, sizeof(BackAngle)))
		return;
	UserViewUser(c[0], c[1], c[2], Width, XYangle, YZangle, BackAngle);
}

void C3dsFileLoader::ReadViewCamera()
{
	char camera_name[12];

	if (!ReadBlock((char *) camera_name, CameraNameSize))
		return;
	UserViewCamera(string(camera_name));

}

void C3dsFileLoader::SetColorDestination(unsigned short ChunkFlag)
{
}


void C3dsFileLoader::SetRGB(unsigned int RGB)
{
}


void C3dsFileLoader::SetFloat(float Float)
{
}

void C3dsFileLoader::SetPercW(unsigned int Perc)
{
}

void C3dsFileLoader::ReadPercentW()
{
	unsigned char c[2];
	if(!ReadBlock((char *) c, sizeof(c)))
		return;

	unsigned int Perc=0x00;;
	memcpy(&Perc, c, sizeof(c));
	SetPercW(Perc);
}



void C3dsFileLoader::ReadFloat()
{
	float c;
	if(!ReadBlock((char *) &c, sizeof(c)))
		return;

	SetFloat(c);
}


void C3dsFileLoader::SetInt(int Int)
{
}

void C3dsFileLoader::ReadShort()
{
	unsigned short c;
	if(!ReadBlock((char *) &c, sizeof(c)))
		return;

	int	Int=c;

	SetInt(Int);
}


bool C3dsFileLoader::ParseChunks()
{

	if(!mInputFile.is_open())
		return false;
	if(mInputFile.eof())
		return false;

	IdleMode();

	chunk3ds	chunk;

//	getchar();

	if(!ReadBlock((char*) &chunk, sizeof(chunk3ds)))
		return false;
	switch(chunk.Flag)
	{

	case CHUNK_RGBF:
		ReadRGBF();
		ParseChunks();
		break;
	case CHUNK_RGBB:
		ReadRGBB();
		ParseChunks();
		break;

	case CHUNK_PERCENTW:
		ReadPercentW();
		ParseChunks();
		break;

	case CHUNK_MAIN:
		ParseChunks();
		break;
	case CHUNK_OBJMESH:
		ReadObjChunk();
		ParseChunks();
		ComposeObject();
		break;
	case CHUNK_OBJBLOCK:
		ReadObjBlock();
		ParseChunks();
		break;
	case CHUNK_TRIMESH:
		ParseChunks();
		break;
	case CHUNK_VERTLIST:
		ReadVertList();
		ParseChunks();
		break;
	case CHUNK_FACELIST:
		ReadFaceList();
		ParseChunks();
		break;
	case CHUNK_FACEMAT:
		ReadFaceMat();
		ParseChunks();
		break;
	case CHUNK_MAPLIST:
		ReadMapList();
		ParseChunks();
		break;
	case CHUNK_SMOOLIST:
//		ReadSmoothList(); // Still unsupported
		SkipChunk(chunk);
		ParseChunks();
		break;
	case CHUNK_TRMATRIX:
		ReadTrMatrix();
		ParseChunks();
		break;
	case CHUNK_LIGHT:
		ReadLight();
		ParseChunks();
		break;
	case CHUNK_SPOTLIGHT:
		ReadSpotLight();
		ParseChunks();
		break;
	case CHUNK_CAMERA:
		ReadCamera();
		ParseChunks();
		break;
	case CHUNK_MATERIAL:
		ParseChunks();
		break;
	case CHUNK_MATNAME:
		ReadMatName();
		ParseChunks();
		break;
	case CHUNK_MAPFILE:
		ReadMapFile();
		ParseChunks();
		break;
/*	case CHUNK_KEYFRAMER:
		cout << "Chunk Keyframer detected" << endl;
		ParseChunks();
		break;
	case CHUNK_TRACKINFO:
		cout << "Chunk TrackInfo detected" << endl;
		ParseChunks();
		break;
	case CHUNK_TRACKOBJNAME:
		cout << "Chunk TrackObjName detected" << endl;
		ReadTrackObjName();
		ParseChunks();
		break;
	case CHUNK_TRACKPOS:
		cout << "Chunk TrackPos detected" << endl;
		ReadTrackPos();
		ParseChunks();
		break;
	case CHUNK_TRACKROTATE:
		cout << "Chunk TrackRotate detected" << endl;
		ReadTrackRot();
		ParseChunks();
		break;
	case CHUNK_TRACKSCALE:
		cout << "Chunk TrackScale detected" << endl;
		ReadTrackScale();
		ParseChunks();
		break;*/
	case CHUNK_OBJNUMBER:
		ReadObjNumber();
		ParseChunks();
		break;

	case CHUNK_TEXTURE:
		ParseChunks();
		break;


	case CHUNK_M_OPTIONS:
		SetColorDestination(chunk.Flag);
		ReadShort();
		ParseChunks();
		break;

	/*case CHUNK_U_SCALE:
	case CHUNK_V_SCALE:
	case CHUNK_U_OFFSET:
	case CHUNK_V_OFFSET:*/
	case CHUNK_M_ROTATE: 
		SetColorDestination(chunk.Flag);
		ReadFloat();
		ParseChunks();
		break;

	case CHUNK_KEYFRAMER:
	case CHUNK_MESHV:
	case CHUNK_VERSION:
		SkipChunk(chunk);
		ParseChunks();
		break;

	case CHUNK_RENDERMODE:
	case CHUNK_M_BLUR:
	case CHUNK_WIRETICK:
	case CHUNK_INTRANC:
	case CHUNK_ALPHAFALL:
	case CHUNK_ALPHA_BLUR:
		ErrorUnimplemented(chunk.Flag);
		SkipChunk(chunk);
		ParseChunks();
		break;

	case CHUNK_SELFLITE:
		ParseChunks();
		break;
	case CHUNK_SHINE:
		ParseChunks();
		break;
	case CHUNK_ALPHA:
	case CHUNK_SHINE_STR:
	case CHUNK_AMBIENT:
	case CHUNK_DIFFUSE:
	case CHUNK_SPECULAR:
		SetColorDestination(chunk.Flag);
		ParseChunks();
		break;


	default:
		ErrorUnknown(chunk.Flag);
		SkipChunk(chunk);
		ParseChunks();
		break;
	}
	return true;
}

void C3dsFileLoader::MessageOut(string text)
{
}

bool C3dsFileLoader::SkipChunk(chunk3ds &chunk)
{
	PC += (chunk.Size - sizeof(chunk3ds));
	mInputFile.seekg(PC);
	if(mInputFile.eof())
		return false;
	return true;
}

void C3dsFileLoader::ProcessFile(string FileName)
{
	mInputFile.open(FileName.c_str(), ios::binary);
	FileSize();
	SetProgressRange(0, mFileSize);
	if(mInputFile.is_open() == false)
		return;
	ParseChunks();
	EndReading();
}

bool C3dsFileLoader::ReadBlock(char *dest, long size)
{
	PC = mInputFile.tellg();
    if(!mInputFile.read(dest, size) && mInputFile.eof())
        return false;
	PC += size;
	SetProgressPos(PC);
	mInputFile.seekg(PC);
	return true;
}

bool C3dsFileLoader::isOk()
{
	if(!mInputFile.is_open())
		return false;
	if(mInputFile.eof())
		return false;
	return true;
}

void C3dsFileLoader::FileSize()
{
	// get length of file:
	mInputFile.seekg (0, ios::end);
	mFileSize = mInputFile.tellg();
	mInputFile.seekg (0, ios::beg);
}

void C3dsFileLoader::ReadASCIIZ(char *dest)
{
	char c;
	long Count = 0;
	do 
	{
		c = mInputFile.get();       // get character from file
		*dest++ = c;
		Count++;
	} while (c != 0);     // loop while extraction from file is possible
	PC+= Count;
	mInputFile.seekg(PC);
}

void C3dsFileLoader::SetProgressRange(int Min, int Max){}
void C3dsFileLoader::SetProgressPos(int Value){}
void C3dsFileLoader::ErrorUnknown(short Chunk){}
void C3dsFileLoader::ErrorUnimplemented(short Chunk){}
