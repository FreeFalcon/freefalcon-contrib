/***************************************************************************\
    Context.cpp
    Scott Randolph
	April 29, 1996

    //JAM 06Oct03 - Begin Major Rewrite
\***************************************************************************/
#include "stdafx.h"
#include "context.h"
extern bool g_bSlowButSafe;
extern float g_fMipLodBias;

#define	INT3 _asm {int 3}

#ifdef _DEBUG

#define _CONTEXT_ENABLE_STATS
//#define _CONTEXT_RECORD_USED_STATES
//#define _CONTEXT_ENABLE_RENDERSTATE_HIGHLIGHT
//#define _CONTEXT_ENABLE_RENDERSTATE_HIGHLIGHT_REPLACE
//define _CONTEXT_FLUSH_EVERY_PRIMITIVE
//#define _CONTEXT_TRACE_ALL
#define _CONTEXT_USE_MANAGED_TEXTURES
//#define _VALIDATE_DEVICE

static int m_nInstCount = 0;

#ifdef _CONTEXT_RECORD_USED_STATES
#include <set>
static std::set<int> m_setStatesUsed;
#endif

#ifdef _CONTEXT_ENABLE_RENDERSTATE_HIGHLIGHT_REPLACE
static bool bEnableRenderStateHighlightReplace = false;
static int bRenderStateHighlightReplaceTargetState = 0;
#endif

#endif //_DEBUG

UInt ContextMPR::StateTable[MAXIMUM_MPR_STATE];
ContextMPR::State ContextMPR::StateTableInternal[MAXIMUM_MPR_STATE];
int ContextMPR::StateSetupCounter = 0;


// COBRA - RED - This function is to calculate some CXs used in Drawing functions
// They are calculated on change of gZBias by 'ContextMPR::setGlobalZBias()'
inline void ContextMPR::ZCX_Calculate(void)
{
}

// Macro to use CXes
#define	SCALE_SZ(x)	(szCX1+szCX2/x)

// COBRA - RED - End



ContextMPR::ContextMPR()
{ 
}

ContextMPR::~ContextMPR()
{ 
}

BOOL ContextMPR::Setup(ImageBuffer *pIB,DXContext *c)
{
	return false;
}

void ContextMPR::NewImageBuffer( UInt lpDDSBack)
{
}

void ContextMPR::ClearBuffers(WORD ClearInfo) 
{
}

void ContextMPR::StartFrame(void)
{
}

void ContextMPR::FinishFrame(void *lpFnPtr)
{
}

void ContextMPR::SetState(WORD State, DWORD Value)
{
}

void ContextMPR::SetStateInternal(WORD State, DWORD Value)
{
}

// flag & 0x01  --> skip StateSetupCount checking --> reset/set state
void ContextMPR::SetCurrentState(GLint state, GLint flag)
{
}

void ContextMPR::Render2DBitmap( int sX, int sY, int dX, int dY, int w, int h, int totalWidth, DWORD *pSrc)
{
}

inline void ContextMPR::SetStateTable(GLint state, GLint flag)
{
}

inline void ContextMPR::ClearStateTable(GLint state)
{
}


void ContextMPR::SetupMPRState(GLint flag)
{
}

void ContextMPR::CleanupMPRState (GLint flag)
{
}

void ContextMPR::SetTexture1(GLint texID)
{
}

void ContextMPR::SetTexture2(GLint texID)
{
}

void ContextMPR::SelectTexture1(GLint texID)
{
#ifdef _CONTEXT_TRACE_ALL
	MonoPrint("ContextMPR::ApplyTexture1(0x%X)\n",texID);
#endif

	if(texID)
		texID = (GLint)((TextureHandle *)texID)->m_pDDS;

	/*if(texID != currentTexture1)
	{
		currentTexture1 = texID;

	#ifdef _CONTEXT_ENABLE_STATS
		m_stats.PutTexture(false);
	#endif

		if(!bZBuffering)
		{
			// JB 010326 CTD (too much CPU)
			/*if(g_bSlowButSafe && F4IsBadReadPtr((TextureHandle *)texID,sizeof(TextureHandle)))
				return;*/

	//		FlushVB();

			HRESULT hr = m_pD3DD->SetTexture(0,(IDirectDrawSurface7 *)texID);
			ShiAssert(SUCCEEDED(hr));

	/*		m_pD3DD->SetTexture(1,NULL);
		}
	/*}	*/
#ifdef _CONTEXT_ENABLE_STATS
	else m_stats.PutTexture(true);
#endif

	currentTexture2 = -1;
}

void ContextMPR::SelectTexture2(GLint texID)
{
}

void ContextMPR::SelectForegroundColor(GLint color)
{
}

void ContextMPR::SelectBackgroundColor(GLint color)
{
}

void ContextMPR::ApplyStateBlock(GLint state)
{
}

void ContextMPR::RestoreState(GLint state)
{
}

// COBRA - RED - Comparing or a so short conditional action has no sense, do it always
void ContextMPR::UpdateSpecularFog(DWORD specular)
{	
}

void ContextMPR::SetZBuffering(BOOL state)
{
}

void ContextMPR::SetNVGmode(BOOL state)
{
}

void ContextMPR::SetTVmode(BOOL state)
{
}

void ContextMPR::SetIRmode(BOOL state)
{
}

// COBRA - RED - Comparing or a so short conditional action has no sense, do it always
void ContextMPR::SetPalID(int id)
{
	//palID = id;
}

void ContextMPR::SetTexID(int id)
{
	//texID = id;
}

DWORD ContextMPR::MPRColor2D3DRGBA(GLint color)
{
}

HRESULT WINAPI ContextMPR::EnumSurfacesCB2(IDirectDrawSurface7 *lpDDSurface, struct _DDSURFACEDESC2 *lpDDSurfaceDesc, LPVOID lpContext)
{
	return(1);
}

void ContextMPR::UpdateViewport()
{
}

void ContextMPR::SetViewportAbs(int nLeft, int nTop, int nRight, int nBottom)
{
}

void ContextMPR::LockViewport()
{
}

void ContextMPR::UnlockViewport()
{
}

void ContextMPR::GetViewport(RECT *prc)
{
}

void ContextMPR::Stats()
{
}

void ContextMPR::TextOut(short x, short y, DWORD col, LPSTR str)
{
}

bool ContextMPR::LockVB(int nVtxCount, void **p)
{
}

void ContextMPR::UnlockVB()
{
}

void ContextMPR::FlushPolyLists()
{
}

void ContextMPR::FlushVB()
{
}

void ContextMPR::SetPrimitiveType(int nType)
{
}

void ContextMPR::SetView(LPD3DMATRIX l_pMV)
{
}

void ContextMPR::SetWorld(LPD3DMATRIX l_pMW)
{
}

void ContextMPR::SetProjection(LPD3DMATRIX l_pMP)
{
}

void ContextMPR::setGlobalZBias(float zBias)
{
}

inline TLVERTEX* SPolygon::CopyToVertexBuffer(TLVERTEX *bufferPos)
{
}


// COBRA - RED - PolyZ is caclulated step by step on each Vertex stuff, saving time, so the average is calculated
// on the passed value
inline void SPolygon::CalcPolyZ(float Avg)
{
}

/*inline*/ void ContextMPR::AllocatePolygon(SPolygon *&curPoly, const DWORD numVertices)
{
}

inline void ContextMPR::AddPolygon(SPolygon *&polyList, SPolygon *&curPoly)
{
}



void ContextMPR::RenderPolyList(SPolygon *&pHead)
{
}

void ContextMPR::DrawPoly(DWORD opFlag, Poly *poly, int *xyzIdxPtr, int *rgbaIdxPtr, int *IIdxPtr, Ptexcoord *uv, bool bUseFGColor)
{
}

void ContextMPR::Draw2DPoint(Tpoint *v0)
{
}

void ContextMPR::Draw2DPoint(float x, float y)
{
}

void ContextMPR::Draw2DLine(Tpoint *v0, Tpoint *v1)
{
}


void ContextMPR::Draw2DLine(float x0, float y0, float x1, float y1)
{
}

void ContextMPR::DrawPrimitive2D(int type, int nVerts, int *xyzIdxPtr)
{
}

void ContextMPR::DrawPrimitive(int nPrimType, WORD VtxInfo, WORD nVerts, MPRVtx_t *pData, WORD Stride)
{
}

void ContextMPR::DrawPrimitive(int nPrimType, WORD VtxInfo, WORD nVerts, MPRVtxTexClr_t *pData, WORD Stride)
{
}

void ContextMPR::DrawPrimitive(int nPrimType, WORD VtxInfo, WORD nVerts, MPRVtxTexClr_t **pData, bool terrain)
{
}

ContextMPR::Stats::Stats()
{
}

