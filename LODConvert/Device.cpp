
#include	"stdafx.h"
#include	"BSPNodes.h"
#include	"DXnodes.h"
#include	"Encoder.h"
#include	"mmsystem.h"
#include	"LODConvert.h"
#include	"DXVbManager.h"
#include	"DXEngine.h"
#include	<dinput.h>

HRESULT	r;

extern	DXDevice *DX;

DXContext	DXc;

LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
            //Cleanup();
            PostQuitMessage( 0 );
            return 0;

		case WM_TIMER:
			DX->Render();
			CString	f;
			f.Format("CPU Sucked Time : %10.2f uSec  -  Virtual Fps : %d", (double)1000000/(double)Fps, Fps);
			SetWindowText(hWnd, f);
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}




// This function is written with the assumption that the following
// global variables are declared in the program.
DWORD                   g_dwDeviceBitDepth          = DDBD_32; 
GUID                    g_guidDevice; 
char                    g_szDeviceName[MAX_PATH]; 
char                    g_szDeviceDesc[MAX_PATH]; 
D3DDEVICEDESC7          g_d3dDeviceDesc; 
DDPIXELFORMAT			PixFormat;
 
static HRESULT WINAPI EnumDeviceCallback(LPSTR lpszDeviceDesc, LPSTR lpszDeviceName, LPD3DDEVICEDESC7 lpd3dEnumDeviceDesc, 
LPVOID  lpUserArg) 
{ 
    BOOL fIsHardware = FALSE;

    fIsHardware = (lpd3dEnumDeviceDesc->dwDevCaps & D3DDEVCAPS_HWRASTERIZATION); 

    // Does the device render at the depth we want? 
    if ((lpd3dEnumDeviceDesc->dwDeviceRenderBitDepth & g_dwDeviceBitDepth) == 0) 
    { 
        // If not, skip this device. 
 
        return D3DENUMRET_OK; 
    } 
 
    // The device must support Gouraud-shaded triangles. 
    if (!(lpd3dEnumDeviceDesc->dpcTriCaps.dwShadeCaps & 
          D3DPSHADECAPS_COLORGOURAUDRGB)) 
    { 
        // No Gouraud shading. Skip this device. 
 
        return D3DENUMRET_OK; 
    } 
  
    //
    // This device has the necessary capabilities. Save the details. 
    //
    *(BOOL*)lpUserArg = TRUE; 
    CopyMemory(&g_guidDevice, &lpd3dEnumDeviceDesc->deviceGUID, sizeof(GUID)); 
    strcpy(g_szDeviceDesc, lpszDeviceDesc); 
    strcpy(g_szDeviceName, lpszDeviceName); 
    CopyMemory(&g_d3dDeviceDesc, lpd3dEnumDeviceDesc, 
               sizeof(D3DDEVICEDESC7)); 
 
    // If this is a hardware device, it has 
    // the necessary capabilities. 
    if (fIsHardware) 
        return D3DENUMRET_CANCEL; 
 
    // Otherwise, keep looking. 
 
    return D3DENUMRET_OK; 
}






VOID CALLBACK DXDevice::TimerProc(
	HWND hwnd,     // handle of window for timer messages
	UINT uMsg,     // WM_TIMER message
	UINT idEvent,  // timer identifier
	DWORD dwTime   // current system time
)
{
	Render();
}
 




static HRESULT WINAPI EnumZBufferCallback( DDPIXELFORMAT* pddpf,
                                           VOID* pddpfDesired )
{
    // For this tutorial, we are only interested in z-buffers, so ignore any
    // other formats (e.g. DDPF_STENCILBUFFER) that get enumerated. An app
    // could also check the depth of the z-buffer (16-bit, etc,) and make a
    // choice based on that, as well. For this tutorial, we'll take the first
    // one we get.
    if( pddpf->dwFlags == DDPF_ZBUFFER )
    {
        memcpy( pddpfDesired, pddpf, sizeof(DDPIXELFORMAT) );
 
        // Return with D3DENUMRET_CANCEL to end the search.
        return D3DENUMRET_CANCEL;
    }
 
    // Return with D3DENUMRET_OK to continue the search.
    return D3DENUMRET_OK;
}


bool DXDevice::CreateZBuffer(void)
{
	DDPIXELFORMAT ddpfZBuffer;
    CheckHR(pD3D->EnumZBufferFormats(  g_guidDevice, EnumZBufferCallback, (VOID*)&ddpfZBuffer ));
	return(true);
}



volatile __int64	PerfIndex, PerfStart, PerfEnd, zu;
DWORD	pIdx;


DXContext::DXContext()
{
	m_pDD = NULL;
	m_pD3D = NULL;
	m_pD3DD = NULL;
	m_bFullscreen = false;
	m_hWnd = NULL;
	m_nWidth = m_nHeight = 0;
	ZeroMemory(&m_guidDD, sizeof(m_guidDD));
	ZeroMemory(&m_guidD3D, sizeof(m_guidD3D));

	m_pcapsDD = new DDCAPS;
	m_pD3DHWDeviceDesc = new D3DDEVICEDESC7;
	m_pDevID = new DDDEVICEIDENTIFIER2;
	refcount = 1; // start with 1
}

DXContext::~DXContext()
{
	Shutdown();

	if(m_pcapsDD) delete m_pcapsDD;
	if(m_pD3DHWDeviceDesc) delete m_pD3DHWDeviceDesc;
	if(m_pDevID) delete m_pDevID;
}


void DXContext::Shutdown()
{
//	MonoPrint("DXContext::Shutdown()\n");

	DWORD dwRefCnt;

	if(m_pDD)
		CheckHR(m_pDD->SetCooperativeLevel(m_hWnd, DDSCL_NORMAL));

	if(m_pD3DD)
	{
		// free all textures
		m_pD3DD->SetTexture(0, NULL);	
		m_pD3DD->SetTexture(1, NULL);
		m_pD3DD->SetTexture(3, NULL);

		// release
		dwRefCnt = m_pD3DD->Release();
		// ShiAssert(dwRefCnt == 0);
		m_pD3DD = NULL;
	}

	if(m_pD3D)
	{
		dwRefCnt = m_pD3D->Release();
//		ShiAssert(dwRefCnt == 0);
		m_pD3D = NULL;
	}

	if(m_pDD)
	{
		if(m_bFullscreen)
		{
			m_pDD->RestoreDisplayMode();
			m_pDD->FlipToGDISurface();
		}

		dwRefCnt = m_pDD->Release();
//		ShiAssert(dwRefCnt == 0);
		m_pDD = NULL;
	}

	m_bFullscreen = false;
	m_hWnd = NULL;
}

PALETTEENTRY	pPEntry[256];
LPDIRECTDRAWPALETTE FAR pDDP;
IDirectDrawSurface7 *pTexS;






DXDevice::DXDevice(CWnd *wnd)
{	

	m_Z=50;
	m_Pitch=m_Roll=0;

	// Register the window class
    WNDCLASS wndClass = { CS_HREDRAW | CS_VREDRAW, MsgProc, 0, 0, NULL,
                          NULL, NULL, 
                          (HBRUSH)GetStockObject(GRAY_BRUSH), NULL,
                          TEXT("Render Window") };

	
    
	RegisterClass( &wndClass );
 
    // Create our main window
    /*HWND hWnd = CreateWindow( TEXT("Render Window"),
                              TEXT("Model"),
							  WS_BORDER | WS_DLGFRAME	, CW_USEDEFAULT,
                              CW_USEDEFAULT, 640, 480, 0L, 0L, NULL, 0L );
    
	ShowWindow( hWnd, SW_SHOWNORMAL );
    UpdateWindow( hWnd );*/

	HWND hWnd=wnd->m_hWnd;

	DX_hWnd=hWnd;
	// * Ceates the DirectDraw Object and Sets Cooperative Level *
	CheckHR(DirectDrawCreateEx(NULL, (void**)&pDD, IID_IDirectDraw7, NULL));		// Create the Direct Draw 7
	r=pDD->SetCooperativeLevel(hWnd, DDSCL_NORMAL);									// Normal Window

	// * Creates the DirectDraw Surface *
	DDSURFACEDESC2 ddsd;
    ZeroMemory( &ddsd, sizeof(DDSURFACEDESC2) );
    ddsd.dwSize         = sizeof(DDSURFACEDESC2);
    ddsd.dwFlags        = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    // Create the primary surface.
    CheckHR(pDD->CreateSurface( &ddsd, &g_pddsPrimary, NULL ));

	ddsd.dwFlags        = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;
    // Set the dimensions of the back buffer. Note that if our window changes
    // size, we need to destroy this surface and create a new one.
    GetClientRect( hWnd, &g_rcScreenRect );
    GetClientRect( hWnd, &g_rcViewportRect );
    ClientToScreen( hWnd, (POINT*)&g_rcScreenRect.left );
    ClientToScreen( hWnd, (POINT*)&g_rcScreenRect.right );
    ddsd.dwWidth  = 1600;//g_rcScreenRect.right - g_rcScreenRect.left;
    ddsd.dwHeight = 1200;//g_rcScreenRect.bottom - g_rcScreenRect.top;
 
    // Create the backbuffer. The most likely reason for failure is running
    // out of video memory. (A more sophisticated app should handle this.)
    CheckHR(pDD->CreateSurface( &ddsd, &g_pddsBackBuffer, NULL ));

	// * CLipper device *
	LPDIRECTDRAWCLIPPER pcClipper;
	CheckHR(pDD->CreateClipper( 0, &pcClipper, NULL ));
    // Assign it to the window handle, then set
    // the clipper to the desired surface.
    pcClipper->SetHWnd( 0, hWnd );
    g_pddsPrimary->SetClipper( pcClipper );
    pcClipper->Release();		

	// Query DirectDraw for access to Direct3D
    CheckHR(pDD->QueryInterface( IID_IDirect3D7, (VOID**)&pD3D ));

	BOOL fDeviceFound = FALSE; 
	CheckHR(pD3D->EnumDevices(EnumDeviceCallback, &fDeviceFound)); 
	if(!fDeviceFound){ MessageBox( hWnd, "FAILED TO FIND RENDERING TARGET", NULL, 0); return; }

	// Look for ZBuffer Support 
	DDPIXELFORMAT ddpfZBuffer;
    CheckHR(pD3D->EnumZBufferFormats( g_guidDevice, EnumZBufferCallback, (VOID*)&ddpfZBuffer ));


	// If we found a good zbuffer format, then the dwSize field will be
    // properly set during enumeration. Else, we have a problem and will exit.
    if( sizeof(DDPIXELFORMAT) != ddpfZBuffer.dwSize )
        { MessageBox( hWnd, "BAD ZBUFFER FORMAT", NULL, 0); return; }
 
    // Get z-buffer dimensions from the render target
    // Setup the surface desc for the z-buffer.
    ddsd.dwFlags        = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER;
	ddsd.dwWidth        = 1600;//g_rcScreenRect.right - g_rcScreenRect.left;
    ddsd.dwHeight       = 1200;//g_rcScreenRect.bottom - g_rcScreenRect.top;
    memcpy( &ddsd.ddpfPixelFormat, &ddpfZBuffer, sizeof(DDPIXELFORMAT) );
 
    // For hardware devices, the z-buffer should be in video memory. For
    // software devices, create the z-buffer in system memory
    if( IsEqualIID( g_guidDevice, IID_IDirect3DHALDevice ) )
        ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
    else
        ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
 
    // Create and attach a z-buffer. Real apps should be able to handle an
    // error here (DDERR_OUTOFVIDEOMEMORY may be encountered). For this 
    // tutorial, though, we are simply going to exit ungracefully.
    CheckHR(pDD->CreateSurface( &ddsd, &g_pddsZBuffer, NULL ) );

	// Attach the z-buffer to the back buffer.
    CheckHR(g_pddsBackBuffer->AddAttachedSurface( g_pddsZBuffer ) );

	// Check the display mode, and 
    ddsd.dwSize = sizeof(DDSURFACEDESC2);
    pDD->GetDisplayMode( &ddsd );
	if( ddsd.ddpfPixelFormat.dwRGBBitCount <= 8 ){
        MessageBox(wnd->m_hWnd, "BAD BIT DEPTH!", NULL, 0);
		return;
	}
    memcpy( &PixFormat, &ddsd.ddpfPixelFormat, sizeof(DDPIXELFORMAT) );
 
    // The GUID here is hard coded. In a real-world application
    // this should be retrieved by enumerating devices.
    CheckHR(pD3D->CreateDevice( IID_IDirect3DTnLHalDevice, g_pddsBackBuffer,
                               &pD3DD ));

	// Enable z-buffering. (Note: we don't really need to do this every frame.)
    pD3DD->SetRenderState( D3DRENDERSTATE_ZENABLE, TRUE );

	// Create the viewport
    DWORD dwRenderWidth  = g_rcScreenRect.right - g_rcScreenRect.left;
    DWORD dwRenderHeight = g_rcScreenRect.bottom - g_rcScreenRect.top;
    D3DVIEWPORT7 vp = { 0, 0, dwRenderWidth, dwRenderHeight, 0.0f, 1.0f };
    CheckHR(pD3DD->SetViewport( &vp ));

	
	// Clear the viewport to a blue color.
	pD3DD->Clear( 1UL, NULL, D3DCLEAR_TARGET, 0x6060f0, 0L, 0L );

	CheckHR(g_pddsPrimary->Blt( &g_rcScreenRect, g_pddsBackBuffer, &g_rcViewportRect, DDBLT_WAIT, NULL ));
	

	DXc.m_pD3DD=pD3DD;
	DXc.m_pDD=pDD;
	DXc.m_pD3D=pD3D;
	
    ZeroMemory( &ddsd, sizeof(DDSURFACEDESC2) );
	ZeroMemory(pPEntry, sizeof(pPEntry));
    ddsd.dwSize         = sizeof(DDSURFACEDESC2);
    ddsd.dwWidth        = 16;
    ddsd.dwHeight       = 16;
    memcpy( &ddsd.ddpfPixelFormat, &PixFormat, sizeof(DDPIXELFORMAT) );
    ddsd.dwFlags        = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT;//|DDSD_TEXTURESTAGE;
    ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
	//ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
    CheckHR(pDD->CreateSurface( &ddsd, &pTexS, NULL ));

	Texture::SetupForDevice(&DXc, FalconMiscTexDataDir);
	// Initializes the VB Manager
	TheVbManager.Setup(pD3D);
	TheDXEngine.Setup(pD3DD, pD3D, pDD);

	// Initializes the Texture Management
	DiscoEnable=false;
	InitGeometry();
	SetupLights();
	DWORD	k=timeGetTime(),e;
	while(k==(pIdx=timeGetTime()));
	pIdx+=1000;
	ZeroMemory(DrawObject, sizeof(DrawObject));
	SelectedObject=-1;
	_asm {	RDTSC
		mov	DWORD PTR zu,eax
		mov	DWORD PTR zu+4,edx
	}
	
	while(pIdx>(e=timeGetTime()));

	_asm {	RDTSC
		mov	DWORD PTR PerfIndex,eax
		mov	DWORD PTR PerfIndex+4,edx
	}

//	if((PerfIndex-zu)<0x10000000)
//		__asm nop;

	PerfIndex-=zu;
	pIdx=e-k;
	m_EnablePlane=false;
	//SetupMouse(wnd);

//	SetTimer(hWnd, 0, 40, NULL);
}






















#define	CYLINDER_SIDES	250
struct CUSTOMVERTEX
{
    D3DXVECTOR3 position; // The 3D position for the vertex
    D3DXVECTOR3 normal;   // The surface normal for the vertex
	DWORD		Diffuse;
	D3DVALUE	tu,tv;
	//DWORD		Specular;

};
CUSTOMVERTEX* pVertices;

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1/*|D3DFVF_TEXCOORDSIZE2(0)|D3DFVF_SPECULAR*/)
D3DVERTEXBUFFERDESC	VBD={ CYLINDER_SIDES*2*sizeof(CUSTOMVERTEX),D3DVBCAPS_WRITEONLY, D3DFVF_CUSTOMVERTEX, CYLINDER_SIDES*2 };

LPDIRECT3DVERTEXBUFFER7 pVB,pVB2,pVB3,pVB4,pPlane;

WORD	VIndex[CYLINDER_SIDES*2];


HRESULT DXDevice::InitGeometry()
{

	TheTextureBank.Reference(0x979);

    pVertices=(CUSTOMVERTEX*)malloc(CYLINDER_SIDES*2*sizeof(CUSTOMVERTEX));

    for( DWORD i=0; i<CYLINDER_SIDES; i++ )
    {
        FLOAT theta = (2*D3DX_PI*i)/(CYLINDER_SIDES-1);
        pVertices[2*i+0].position = D3DXVECTOR3( sinf(theta),-1.0f, cosf(theta) );
        pVertices[2*i+0].normal   = D3DXVECTOR3( sinf(theta), 0.0f, cosf(theta) );
        pVertices[2*i+0].Diffuse   = 0xFfffffff;
        pVertices[2*i+0].tu   = (1.0f/CYLINDER_SIDES)*i;
        pVertices[2*i+0].tv   = 0.0f;
		
//        pVertices[2*i+0].Specular  = 0xFFffffff;
        pVertices[2*i+1].position = D3DXVECTOR3( sinf(theta), 1.0f, cosf(theta) );
        pVertices[2*i+1].normal   = D3DXVECTOR3( sinf(theta), 0.0f, cosf(theta) );
        pVertices[2*i+1].Diffuse   = 0x10ffffff;;
        pVertices[2*i+1].tu   = (1.0f/CYLINDER_SIDES)*i;
        pVertices[2*i+1].tv   = 1.0f;
//        pVertices[2*i+1].Specular   = 0xFFffffff;;

		VIndex[2*i]=2*i;
		VIndex[2*i+1]=2*i+1;
    }
    //g_pVB->Unlock();

	D3DVERTEXBUFFERDESC	vbs;
	vbs.dwSize=sizeof(vbs);
	vbs.dwCaps=0;
	vbs.dwFVF=D3DFVF_CUSTOMVERTEX;
	vbs.dwNumVertices=D3DMAXNUMVERTICES;

	CheckHR(pD3D->CreateVertexBuffer(&vbs, &pVB, 0));
	CheckHR(pD3D->CreateVertexBuffer(&vbs, &pVB2, 0));
	CheckHR(pD3D->CreateVertexBuffer(&vbs, &pVB3, 0));
	CheckHR(pD3D->CreateVertexBuffer(&vbs, &pVB4, 0));

	void	*ptr;
	pVB->Lock(DDLOCK_WAIT|DDLOCK_WRITEONLY|DDLOCK_DISCARDCONTENTS, &ptr, NULL);
	memcpy((void*) ptr, pVertices, CYLINDER_SIDES*2*sizeof(CUSTOMVERTEX));
	pVB->Unlock();

	pVB2->Lock(DDLOCK_WAIT|DDLOCK_WRITEONLY|DDLOCK_DISCARDCONTENTS, &ptr, NULL);
	memcpy((void*) ptr, pVertices, CYLINDER_SIDES*2*sizeof(CUSTOMVERTEX));
	pVB2->Unlock();

	pVB3->Lock(DDLOCK_WAIT|DDLOCK_WRITEONLY|DDLOCK_DISCARDCONTENTS, &ptr, NULL);
	memcpy((void*) ptr, pVertices, CYLINDER_SIDES*2*sizeof(CUSTOMVERTEX));
	pVB3->Unlock();

	pVB4->Lock(DDLOCK_WAIT|DDLOCK_WRITEONLY|DDLOCK_DISCARDCONTENTS, &ptr, NULL);
	memcpy((void*) ptr, pVertices, CYLINDER_SIDES*2*sizeof(CUSTOMVERTEX));
	pVB4->Unlock();


	int	a,b;
	#define	SQUARESIDE	10
    pVertices=(CUSTOMVERTEX*)malloc(50*50*6*sizeof(CUSTOMVERTEX));
	for( a=0; a<50; a++){
		for( b=0; b<50; b++){

			DWORD Color=((a+b)&0x01)?0x40303030:0x40808080;

			pVertices[6*(a*50+b)+0].position = D3DXVECTOR3( -SQUARESIDE*25.0f + b*SQUARESIDE ,0.0f, -SQUARESIDE*25.0f + a*SQUARESIDE );
			pVertices[6*(a*50+b)+0].normal   = D3DXVECTOR3( 0, 1.0f, 0 );
			pVertices[6*(a*50+b)+0].Diffuse  = Color;

			pVertices[6*(a*50+b)+1].position = D3DXVECTOR3( -SQUARESIDE*25.0f + b*SQUARESIDE + SQUARESIDE,0.0f, -SQUARESIDE*25.0f + a*SQUARESIDE );
			pVertices[6*(a*50+b)+1].normal   = D3DXVECTOR3( 0, 1.0f, 0 );
			pVertices[6*(a*50+b)+1].Diffuse  = Color;

			pVertices[6*(a*50+b)+2].position = D3DXVECTOR3( -SQUARESIDE*25.0f + b*SQUARESIDE + SQUARESIDE,0.0f, -SQUARESIDE*25.0f + a*SQUARESIDE + SQUARESIDE);
			pVertices[6*(a*50+b)+2].normal   = D3DXVECTOR3( 0, 1.0f, 0 );
			pVertices[6*(a*50+b)+2].Diffuse  = Color;

			pVertices[6*(a*50+b)+3].position = D3DXVECTOR3( -SQUARESIDE*25.0f + b*SQUARESIDE ,0.0f, -SQUARESIDE*25.0f + a*SQUARESIDE );
			pVertices[6*(a*50+b)+3].normal   = D3DXVECTOR3( 0, 1.0f, 0 );
			pVertices[6*(a*50+b)+3].Diffuse  = Color;

			pVertices[6*(a*50+b)+4].position = D3DXVECTOR3( -SQUARESIDE*25.0f + b*SQUARESIDE + SQUARESIDE,0.0f, -SQUARESIDE*25.0f + a*SQUARESIDE + SQUARESIDE);
			pVertices[6*(a*50+b)+4].normal   = D3DXVECTOR3( 0, 1.0f, 0 );
			pVertices[6*(a*50+b)+4].Diffuse  = Color;

			pVertices[6*(a*50+b)+5].position = D3DXVECTOR3( -SQUARESIDE*25.0f + b*SQUARESIDE ,0.0f, -SQUARESIDE*25.0f + a*SQUARESIDE + SQUARESIDE );
			pVertices[6*(a*50+b)+5].normal   = D3DXVECTOR3( 0, 1.0f, 0 );
			pVertices[6*(a*50+b)+5].Diffuse  = Color;


		}


	}

	float k;
	for(a=0; a<(50*50*6); a++) {
		k=pVertices[a].position.z;
		pVertices[a].position.z=-pVertices[a].position.y;
		pVertices[a].position.y=-k;

		k=pVertices[a].normal.z;
		pVertices[a].normal.z=-pVertices[a].normal.y;
		pVertices[a].normal.y=-k;
	}

	vbs.dwSize=sizeof(vbs);
	vbs.dwCaps=0;
	vbs.dwFVF=D3DFVF_CUSTOMVERTEX;
	vbs.dwNumVertices=D3DMAXNUMVERTICES;
	vbs.dwNumVertices=50*50*6;

	CheckHR(pD3D->CreateVertexBuffer(&vbs, &pPlane, 0));
	pPlane->Lock(DDLOCK_WAIT|DDLOCK_WRITEONLY|DDLOCK_DISCARDCONTENTS, &ptr, NULL);
	memcpy((void*) ptr, pVertices, 50*50*6*sizeof(CUSTOMVERTEX));
	pPlane->Unlock();

    return S_OK;
}



VOID DXDevice::SelectTexture(GLint texID)
{
	if(texID)
		texID = (GLint)((TextureHandle *)texID)->m_pDDS;
	CheckHR(pD3DD->SetTexture(0,(IDirectDrawSurface7 *)texID));
}

VOID DXDevice::PreloadTexture(GLint texID)
{
	if(texID)
		texID = (GLint)((TextureHandle *)texID)->m_pDDS;
	CheckHR(pD3DD->PreLoad((IDirectDrawSurface7 *)texID));
}


//-----------------------------------------------------------------------------
// Name: SetupMatrices()
// Desc: Sets up the world, view, and projection transform matrices.
//-----------------------------------------------------------------------------
VOID DXDevice::SetupMatrices()
{
    // For our world matrix, we will just leave it as the identity
    /*D3DXMATRIX matWorld,cippa;
    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixMultiply(&matWorld, D3DXMatrixRotationZ( &matWorld, timeGetTime()/1500.0f ),D3DXMatrixRotationY( &cippa, timeGetTime()/1500.0f ));
	//D3DXMatrixRotationX( &matWorld, timeGetTime()/1500.0f );
    pD3DD->SetTransform( D3DTRANSFORMSTATE_WORLD, (LPD3DMATRIX)&matWorld );*/

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. Here, we set the
    // eye five units back along the z-axis and up three units, look at the
    // origin, and define "up" to be in the y-direction.
    D3DXVECTOR3 vEyePt( 0.0f, 3.0f,-5.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIX	matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    pD3DD->SetTransform( D3DTRANSFORMSTATE_VIEW, (LPD3DMATRIX)&matView );

    // For the projection matrix, we set up a perspective transform (which
    // transforms geometry from 3D view space to 2D viewport space, with
    // a perspective divide making objects smaller in the distance). To build
    // a perpsective transform, we need the field of view (1/4 pi is common),
    // the aspect ratio, and the near and far clipping planes (which define at
    // what distances geometry should be no longer be rendered).
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, (float)m_dwRenderHeight/(float)m_dwRenderWidth, 1.0f, 280000.0f );
    //pD3DD->SetTransform( D3DTRANSFORMSTATE_PROJECTION, (LPD3DMATRIX)&matProj );
	TheDXEngine.SetProjection(&matProj);
}

D3DLIGHT7 light, Lights[10];



//-----------------------------------------------------------------------------
// Name: SetupLights()
// Desc: Sets up the lights and materials for the scene.
//-----------------------------------------------------------------------------
VOID DXDevice::SetupLights()
{
    // Set up a material. The material here just has the diffuse and ambient
    // colors set to yellow. Note that only one material can be used at a time.
/*    D3DMATERIAL7 mtrl;
    ZeroMemory( &mtrl, sizeof(D3DMATERIAL7) );
	mtrl.dcvSpecular.a=mtrl.dcvSpecular.r=mtrl.dcvSpecular.g=mtrl.dcvSpecular.b=0.8f;
	mtrl.dcvAmbient.a=mtrl.dcvAmbient.r=mtrl.dcvAmbient.g=mtrl.dcvAmbient.b=1.0f;
	mtrl.dvPower=6.8F;
    pD3DD->SetMaterial( &mtrl );*/

    // Set up a white, directional light, with an oscillating direction.
    // Note that many lights may be active at a time (but each one slows down
    // the rendering of our scene). However, here we are just using one. Also,
    // we need to set the D3DRS_LIGHTING renderstate to enable lighting
    D3DXVECTOR3 vecDir;
    vecDir = D3DXVECTOR3(0.0f/*cosf(timeGETTIMING
							 e()/350.0f)*/,
                         -0.5f,
                         1.0f/*sinf(timeGetTime()/350.0f)*/ );
    D3DXVec3Normalize( (D3DXVECTOR3*)&light.dvDirection, &vecDir );
    light.dvRange     = 3000.0f;

	TheDXEngine.SetSunColour(1.0f, 1.0f, 1.0f);
	TheDXEngine.SetSunLight(0.4f, 0.6f, 0.6f);
	TheDXEngine.SetSunVector(vecDir);

	int a;
	for(a=1; a<10; a++){
	    Lights[a].dltType    = D3DLIGHT_SPOT;
		Lights[a].dcvDiffuse.r  = 0.5f*(float)(a&0x01);
		Lights[a].dcvDiffuse.g  = 0.5f*(float)(a&0x02);
		Lights[a].dcvDiffuse.b  = 0.5f*(float)(a&0x04);
		Lights[a].dcvAmbient.r=0.2f*(float)(a&0x01);
		Lights[a].dcvAmbient.g=0.2f*(float)(a&0x02);
		Lights[a].dcvAmbient.b=0.2f*(float)(a&0x04);
		Lights[a].dcvAmbient.a=0.2f;
		Lights[a].dvTheta=D3DX_PI/12;
		Lights[a].dvPhi=D3DX_PI/6;
		Lights[a].dvFalloff=1.0f;
		Lights[a].dvAttenuation1=0.01f;
		Lights[a].dvAttenuation0=0.01f;
		Lights[a].dcvSpecular.a=0.0f;
		Lights[a].dcvSpecular.r=0.2f*(float)(a&0x01);
		Lights[a].dcvSpecular.g=0.2f*(float)(a&0x02);
		Lights[a].dcvSpecular.b=0.2f*(float)(a&0x04);
	    Lights[a].dvRange     = 1000.0f;
	}
	
	pD3DD->SetRenderState( D3DRENDERSTATE_LIGHTING, TRUE );
    pD3DD->SetRenderState( D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA );
    pD3DD->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA );
    /*pD3DD->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );*/
    // Finally, turn on some ambient light.*/
    pD3DD->SetRenderState( D3DRENDERSTATE_COLORVERTEX, TRUE );
    pD3DD->SetRenderState( D3DRENDERSTATE_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL );
    //pD3DD->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, TRUE );
    pD3DD->SetRenderState( D3DRENDERSTATE_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1 );
    pD3DD->SetRenderState( D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD );


	//pD3DD->SetRenderState(D3DRENDERSTATE_SHADEMODE,D3DSHADE_FLAT);


    pD3DD->SetRenderState( D3DRENDERSTATE_TEXTUREPERSPECTIVE, TRUE );
    //pD3DD->SetRenderState( D3DRENDERSTATE_WRAP0, D3DWRAPCOORD_0 );
	pD3DD->SetRenderState( D3DRENDERSTATE_AMBIENT, 0xFF202020 );

}

void DXDevice::DrawPlane()
{
	SelectTexture(NULL);
    pD3DD->SetRenderState( D3DRENDERSTATE_LIGHTING, FALSE );
	pD3DD->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );
    pD3DD->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, FALSE );
    pD3DD->SetRenderState( D3DRENDERSTATE_COLORVERTEX, TRUE );
    pD3DD->SetRenderState( D3DRENDERSTATE_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1 );
	pD3DD->DrawPrimitiveVB( D3DPT_TRIANGLELIST, pPlane, 0, 50*50*6, 0);
    //pD3DD->SetRenderState( D3DRENDERSTATE_LIGHTING, TRUE );
	//pD3DD->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
    //pD3DD->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, TRUE );
}

void DXDevice::DrawDemo()
{
		pD3DD->SetRenderState( D3DRENDERSTATE_CULLMODE, D3DCULL_NONE );

		D3DXMATRIX matWorld,cippa;
		D3DXMatrixIdentity( &matWorld );
		D3DXMatrixMultiply(&matWorld, D3DXMatrixRotationZ( &matWorld, timeGetTime()/1500.0f ),D3DXMatrixRotationY( &cippa, timeGetTime()/1500.0f ));
		pD3DD->SetTransform( D3DTRANSFORMSTATE_WORLD, (LPD3DMATRIX)&matWorld );
		//pD3DD->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
		//pD3DD->SetTextureStageState(0,D3DTSS_ADDRESS,D3DTADDRESS_WRAP);

		//pD3DD->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		//pD3DD->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		//pD3DD->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		//pD3DD->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE);
		//pD3DD->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_ADD );
		//pD3DD->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );


		
		
//		TheTextureBank.Select(0x979);
		//pD3DD->SetTexture(0,pTexS);
		pD3DD->DrawPrimitiveVB( D3DPT_TRIANGLESTRIP, pVB, 0, CYLINDER_SIDES, 0/*DDBLT_WAIT*/);
		//pD3DD->SetTexture(0,NULL);

		D3DXMatrixIdentity( &matWorld );
		D3DXMatrixMultiply(&matWorld, D3DXMatrixRotationZ( &matWorld, timeGetTime()/-1500.0f ),D3DXMatrixRotationY( &cippa, timeGetTime()/-1500.0f ));
		pD3DD->SetTransform( D3DTRANSFORMSTATE_WORLD, (LPD3DMATRIX)&matWorld );
		pD3DD->DrawIndexedPrimitiveVB( D3DPT_TRIANGLESTRIP, pVB, 0, 2*CYLINDER_SIDES, &VIndex[CYLINDER_SIDES], CYLINDER_SIDES, 0);
		//pD3DD->DrawPrimitiveVB( D3DPT_TRIANGLESTRIP, pVB2, 0, CYLINDER_SIDES, DDBLT_WAIT);

		D3DXMatrixIdentity( &matWorld );
		D3DXMatrixMultiply(&matWorld, D3DXMatrixRotationX( &cippa, timeGetTime()/-1500.0f ), D3DXMatrixTranslation(&matWorld, sinf(timeGetTime()/1500.0f), 0.0f, cosf(timeGetTime()/1500.0f)));
		pD3DD->SetTransform( D3DTRANSFORMSTATE_WORLD, (LPD3DMATRIX)&matWorld );
		pD3DD->DrawIndexedPrimitiveVB( D3DPT_TRIANGLESTRIP, pVB, 0, 2*CYLINDER_SIDES, VIndex, CYLINDER_SIDES*2, 0);
	//	pD3DD->DrawPrimitiveVB( D3DPT_TRIANGLESTRIP, pVB3, 0, CYLINDER_SIDES*2, DDBLT_WAIT);

		D3DXMatrixIdentity( &matWorld );
		D3DXMatrixMultiply(&matWorld, D3DXMatrixRotationX( &cippa, timeGetTime()/1500.0f ), D3DXMatrixTranslation(&matWorld, -sinf(timeGetTime()/1500.0f), 0.0f, cosf(timeGetTime()/1500.0f)));
		pD3DD->SetTransform( D3DTRANSFORMSTATE_WORLD, (LPD3DMATRIX)&matWorld );
		pD3DD->DrawIndexedPrimitiveVB( D3DPT_TRIANGLESTRIP, pVB2, 0, 2*CYLINDER_SIDES, VIndex, CYLINDER_SIDES*2, 0);
	//	pD3DD->DrawPrimitiveVB( D3DPT_TRIANGLESTRIP, pVB4, 0, CYLINDER_SIDES*2, DDBLT_WAIT);

		D3DXMatrixIdentity( &matWorld );
		D3DXMatrixMultiply(&matWorld, D3DXMatrixRotationX( &cippa, timeGetTime()/1500.0f ), D3DXMatrixTranslation(&matWorld, 0.0f, -2*sinf(timeGetTime()/300.0f), 0.0f));
		pD3DD->SetTransform( D3DTRANSFORMSTATE_WORLD, (LPD3DMATRIX)&matWorld );
		pD3DD->DrawIndexedPrimitiveVB( D3DPT_TRIANGLESTRIP, pVB2, 0, 2*CYLINDER_SIDES, VIndex, CYLINDER_SIDES*2, 0);
	//	pD3DD->DrawPrimitiveVB( D3DPT_TRIANGLESTRIP, pVB3, 0, CYLINDER_SIDES*2, DDBLT_WAIT);

		SelectTexture(NULL);
}


// Hook the ObjectInstance or drawing
void	DXDevice::Draw(ObjectInstance *obj)
{
	int a;
	for(a=0; a<MANAGED_OBJECTS; a++){
		if(DrawObject[a]==obj){
			SelectedObject=a;
			return;
		}
	}
	for(a=0; a<MANAGED_OBJECTS; a++){
		if(!DrawObject[a]){
			DrawObject[a]=obj;
			SelectedObject=a;
			return;
		}
	}
}

// UnHook the ObjectInstance or drawing
void	DXDevice::UnDraw(ObjectInstance *obj)
{
	//  If this is what we draw...drop it...
	int a;
	for(a=0; a<MANAGED_OBJECTS; a++){
		if(DrawObject[a]==obj) DrawObject[a]=NULL;
	}
}


void	DXDevice::SetVectors(float Yaw, float Roll, float Pitch, float X, float Y, float Z)
{
	m_Roll=Roll;
	m_Yaw=Yaw;
	m_Pitch=Pitch;
	m_X=X;
	m_Y=Y;
	m_Z=Z;
}


__int64	DrawTime;
void	CalculateFps(void)
{
	DrawTime=((DrawTime*29)+((__int64)1000*(PerfEnd-PerfStart))/(PerfIndex/(__int64)pIdx))/(__int64)30;

	Fps=1000000/DrawTime;
}

void	MeasureStart(void)
{

	_asm{
		push eax
		push edx
		RDTSC
		mov	DWORD PTR PerfStart,eax
		mov	DWORD PTR PerfStart+4,edx
		pop edx
		pop	eax 
	};
}


void	MeasureStop(void)
{
	_asm{
		push eax
		push edx
		RDTSC
		mov	DWORD PTR PerfEnd,eax
		mov	DWORD PTR PerfEnd+4,edx
		pop edx
		pop	eax
	};
}

void DXDevice::TogglePlane()
{
	m_EnablePlane=(m_EnablePlane)?false:true;
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID DXDevice::Render()
{

	DWORD	BackColour;
	BackColour=((int)(TheDXEngine.TheSun.dcvAmbient.r*127.0f))<<16;
	BackColour|=((int)(TheDXEngine.TheSun.dcvAmbient.g*127.0f))<<8;
	BackColour|=((int)(TheDXEngine.TheSun.dcvAmbient.g*255.0f));
    // Clear the backbuffer and the zbuffer
    pD3DD->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                          BackColour, 1.0f, 0 );

	// * Update of target viewport position *
    GetClientRect( DX_hWnd, &g_rcScreenRect );
    GetClientRect( DX_hWnd, &g_rcViewportRect );
    ClientToScreen( DX_hWnd, (POINT*)&g_rcScreenRect.left );
    ClientToScreen( DX_hWnd, (POINT*)&g_rcScreenRect.right );
	m_dwRenderWidth  = g_rcScreenRect.right - g_rcScreenRect.left;
    m_dwRenderHeight = g_rcScreenRect.bottom - g_rcScreenRect.top;
    D3DVIEWPORT7 vp = { 0, 0, m_dwRenderWidth, m_dwRenderHeight, 0.0f, 1.0f };
    CheckHR(pD3DD->SetViewport( &vp ));
	TheDXEngine.SetViewport(0, 0, m_dwRenderWidth, m_dwRenderHeight);
   

    // Begin the scene
    if( SUCCEEDED( pD3DD->BeginScene() ) )
    {

        // Setup the world, view, and projection matrices
        SetupMatrices();

		if(SelectedObject==-1){
			pD3DD->SetRenderState(D3DRENDERSTATE_COLORVERTEX,TRUE);
			pD3DD->SetRenderState(D3DRENDERSTATE_LIGHTING,TRUE);
			pD3DD->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_NONE);
			pD3DD->SetRenderState(D3DRENDERSTATE_FOGENABLE,FALSE);
			pD3DD->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE,TRUE);
			MEASURE_START;
			DrawDemo();
			MEASURE_STOP;
		} else {


			// **************************************************************************

			// Set up the camera View
			D3DXMATRIX	matView;
			D3DXMatrixIdentity( &matView );
			D3DXMATRIX Temp;
			ZeroMemory(&Temp,sizeof(Temp));
			Temp.m21=-1.0;
			Temp.m12=-1.0;
			Temp.m00=1.0;
			Temp.m33=1.0;
			D3DXMatrixMultiply(&matView, &matView, &Temp);

			D3DVECTOR DXPos;
			DXPos.x=0;
			DXPos.y=0;
			DXPos.z=0;

			TheDXEngine.SetCamera(&matView, DXPos, &matView);

			int a;
			D3DXVECTOR3 vecDir;
			for(a=1; a<8; a++){
			    
				vecDir = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
		/*		vecDir = D3DXVECTOR3(cosf(timeGetTime()/350.0f),
									-0.f,
									sinf(timeGetTime()/350.0f) );*/
				D3DXVec3Normalize( (D3DXVECTOR3*)&Lights[a].dvDirection, &vecDir );
				vecDir = D3DXVECTOR3(m_X*cosf(timeGetTime()/(1000.0f+(float)a*10.0f)),
									m_Y+30.0f,
									m_Z*sinf(timeGetTime()/(1000.0f+(float)a*10.0f)) );
				Lights[a].dvPosition=vecDir;
				pD3DD->SetLight(a, &Lights[a]);
				pD3DD->LightEnable(a, DiscoEnable);
			}

			D3DXMATRIX	cippa;
			if(m_EnablePlane){
				D3DXMatrixTranslation(&cippa, 0, 0, 5);
				pD3DD->SetTransform( D3DTRANSFORMSTATE_WORLD, (LPD3DMATRIX)&cippa );
				DrawPlane();
			}

			TheDXEngine.DXDrawCalls=TheDXEngine.DXDrawVertices=TheDXEngine.DXStateChanges=TheDXEngine.DXTexSwitches=0;

			D3DXMATRIX matWorld;
			D3DXMatrixRotationYawPitchRoll(&matWorld, m_Yaw, m_Pitch, -m_Roll);

			MEASURE_START;
			int	k,n,b=0;
			n=NItems;
			Ppoint	pos;
			for(a=0; a< MANAGED_OBJECTS; a++){
				if(!n) break;
				k=SelectedObject+a;
				if(k>=MANAGED_OBJECTS)k-=MANAGED_OBJECTS;
				ObjectInstance *Obj=DrawObject[k];
				if(Obj){
					for(b=0; b<1; b++){
						pos.z=m_Y;
						pos.x=m_X;
						pos.y=m_Z;//m_Z;
						TheDXEngine.DrawObject(Obj, &matWorld, &pos, 1.0f, 1.0f, 1.0f, 1.0f, false);
					}
					n--;
				}
			}

			TheDXEngine.FlushBuffers();
			MEASURE_STOP;
		}		
	
		CalculateFps();

	// End the scene
		pD3DD->EndScene();
		CheckHR(g_pddsPrimary->Blt( &g_rcScreenRect, g_pddsBackBuffer, &g_rcViewportRect, 0/*DDBLT_WAIT*/, NULL ));
    }

}


CString	DXDevice::GetTimings()
{
			CString	f;
			f.Format("CPU Time : %10.2f uSec  -  Virtual Fps : %6.1f", (double)DrawTime, (double)Fps);
			return(f);
}