
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
		if(pddpf->dwZBufferBitDepth>=24){
	        memcpy( pddpfDesired, pddpf, sizeof(DDPIXELFORMAT) );
 
		    // Return with D3DENUMRET_CANCEL to end the search.
			return D3DENUMRET_CANCEL;
		}
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

	// Initializes the Texture Management
	Texture::SetupForDevice(&DXc, FalconMiscTexDataDir);

	// Initializes the VB Manager
	TheVbManager.Setup(pD3D);
	TheDXEngine.Setup(pD3DD, pD3D, pDD);
	TheDXEngine.SetSunColour(1.0f, 1.0f, 1.0f);
	TheDXEngine.SetSunLight(0.4f, 0.6f, 0.6f);

	InitGeometry();
	SetupLights();
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

    return S_OK;
}



VOID DXDevice::SelectTexture(GLint texID)
{
	GLint	OriginalID=texID;

	if(texID)
		texID = (GLint)((TextureHandle *)texID)->m_pDDS;
	CheckHR(pD3DD->SetTexture(0,(IDirectDrawSurface7 *)texID));
	TheDXEngine.SelectTexture(OriginalID);

/*	CheckHR(pD3DD->SetTexture(1,(IDirectDrawSurface7 *)texID));
	CheckHR(pD3DD->SetTexture(2,(IDirectDrawSurface7 *)texID));
	CheckHR(pD3DD->SetTexture(3,(IDirectDrawSurface7 *)texID));*/
}

VOID DXDevice::PreloadTexture(GLint texID)
{
	if(texID)
		texID = (GLint)((TextureHandle *)texID)->m_pDDS;
	CheckHR(pD3DD->PreLoad((IDirectDrawSurface7 *)texID));
}



void DXDevice::LoadTextures(DWORD Nr, DWORD *List)
{
	while(Nr--)	TheTextureBank.Reference(*List++);
}


void DXDevice::UnLoadTextures(DWORD Nr, DWORD *List)
{
	while(Nr--)	TheTextureBank.Release(*List++);
}

void DXDevice::LoadTexture(DWORD TexNr)
{
	TheTextureBank.Reference(TexNr);
}

void DXDevice::UnLoadTexture(DWORD TexNr)
{
	TheTextureBank.Release(TexNr);
}


//-----------------------------------------------------------------------------
// Name: SetupMatrices()
// Desc: Sets up the world, view, and projection transform matrices.
//-----------------------------------------------------------------------------
VOID DXDevice::SetupMatrices(bool Perspective, float Scale)
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
    if(Perspective) D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, (float)m_dwRenderHeight/(float)m_dwRenderWidth, 1.0f, 10000.0f );
    else D3DXMatrixOrthoLH( &matProj, Scale, Scale*(float)m_dwRenderHeight/(float)m_dwRenderWidth, -1000.0f, 280000.0f );
	TheDXEngine.SetProjection(&matProj);
}

D3DLIGHT7 light, Lights[10];



void	DXDevice::GetWorldCoordinates(D3DVECTOR *Dest, DWORD PixelX, DWORD PixelY, float Scale, float OriginX, float OriginY)
{
	float	XperPixel=Scale/(float)m_dwRenderWidth;
	float	YperPixel=(Scale*(float)m_dwRenderHeight/(float)m_dwRenderWidth)/(float)m_dwRenderHeight;

	float	XStart=OriginX+XperPixel*(float)m_dwRenderWidth/2;
	float	YStart=OriginY+YperPixel*(float)m_dwRenderHeight/2;

	Dest->x=(float)PixelX*XperPixel-XStart;
	Dest->y=(float)PixelY*YperPixel-YStart;

	Dest->z=0.0f;
}



//-----------------------------------------------------------------------------
// Name: SetupLights()
// Desc: Sets up the lights and materials for the scene.
//-----------------------------------------------------------------------------
VOID DXDevice::SetupLights()
{

	D3DXVECTOR3 vecDir;
    vecDir = D3DXVECTOR3(0.0f, -0.5f, 1.0f );
    D3DXVec3Normalize( (D3DXVECTOR3*)&light.dvDirection, &vecDir );
    light.dvRange     = 3000.0f;

	// Set the Sun
	TheDXEngine.SetSunVector(vecDir);

	//
	pD3DD->SetRenderState( D3DRENDERSTATE_AMBIENT, 0xFF000000 );

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

void	DXDevice::BeginScene(D3DXMATRIX	*CameraState, D3DXVECTOR3 *CameraPos, bool Perspective)
{
    GetClientRect( DX_hWnd, &g_rcScreenRect );
    GetClientRect( DX_hWnd, &g_rcViewportRect );
    ClientToScreen( DX_hWnd, (POINT*)&g_rcScreenRect.left );
    ClientToScreen( DX_hWnd, (POINT*)&g_rcScreenRect.right );
	m_dwRenderWidth  = g_rcScreenRect.right - g_rcScreenRect.left;
    m_dwRenderHeight = g_rcScreenRect.bottom - g_rcScreenRect.top;
//    D3DVIEWPORT7 vp = { 0, 0, m_dwRenderWidth, m_dwRenderHeight, 0.0f, 1.0f };
//    CheckHR(pD3DD->SetViewport( &vp ));
	TheDXEngine.SetViewport(0, 0, m_dwRenderWidth, m_dwRenderHeight);


	pD3DD->BeginScene(); 

    // Setup the world, view, and projection matrices
    SetupMatrices(Perspective, CameraPos->z);

	// Set up the camera View
	D3DXMATRIX	matView;

	D3DXMATRIX Temp;
	ZeroMemory(&Temp,sizeof(Temp));
	Temp.m21=-1.0;
	Temp.m12=-1.0;
	Temp.m00=1.0;
	Temp.m33=1.0;

	D3DXMatrixMultiply(&matView, CameraState, &Temp );

	D3DXMatrixMultiply(&Temp, &matView, D3DXMatrixRotationZ(&Temp, PI/2));

	TheDXEngine.SetCamera(&matView, *CameraPos, &Temp);

	DWORD BackColour=((int)(TheDXEngine.TheSun.dcvAmbient.r*127.0f))<<16;
	BackColour|=((int)(TheDXEngine.TheSun.dcvAmbient.g*127.0f))<<8;
	BackColour|=((int)(TheDXEngine.TheSun.dcvAmbient.g*255.0f));

	// Clear the backbuffer and the zbuffer
    pD3DD->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, BackColour, 1.0f, 0 );
}

void	DXDevice::EndDraws()
{
	// Draw the Solid Surfaces
	TheDXEngine.DrawSolidSurfaces();
	// Draw the Alpha Surfaces
	TheDXEngine.DrawAlphaSurfaces();

	//TheDXEngine.DrawFrameSurfaces(SelectionAlpha);

	//Reset Features
	TheDXEngine.ResetFeatures();
}


void	DXDevice::EndScene()
{
	pD3DD->EndScene();
	CheckHR(g_pddsPrimary->Blt( &g_rcScreenRect, g_pddsBackBuffer, &g_rcViewportRect, 0, NULL ));
	TheLightEngine.ResetLightsList();
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID DXDevice::Render()
{

}

void	DXDevice::CreateGrid(float GridSize, int Rows, DWORD Colour)
{
	m_GridRows=Rows+1;

	// delete old if existant
	if(m_pGrid) free(m_pGrid);
	// allocate for a new one
	m_pGrid=malloc(Rows * Rows *sizeof(D3DVERTEXEX) * 4);
	ZeroMemory(m_pGrid, Rows * Rows *sizeof(D3DVERTEXEX) * 4);
	float	XStart=-((float)Rows/2*GridSize);
	float	YStart=-((float)Rows/2*GridSize);
	D3DVERTEXEX	*Vtx=(D3DVERTEXEX*)m_pGrid;
	// create the Grid
	for(int x=0; x<Rows+1; x++){
		
		Vtx->dwColour=(Vtx+1)->dwColour=(XStart+(float)x*GridSize)?(Vtx+1)->dwColour=Colour:0x80ff4040;
		if(!(x%10)){ Vtx->dwColour+=0x40000000; (Vtx+1)->dwColour+=0x20000000; }
		Vtx->ny=(Vtx+1)->ny=-1.0f;
		Vtx->vz=(Vtx+1)->vz=XStart+(float)x*GridSize;
		Vtx->vx=YStart;
		(Vtx+1)->vx=YStart+GridSize*Rows;
		
		Vtx+=2;

		Vtx->dwColour=(Vtx+1)->dwColour=(YStart+(float)x*GridSize)?(Vtx+1)->dwColour=Colour:0x80ff4040;
		if(!(x%10)){ Vtx->dwColour+=0x40000000; (Vtx+1)->dwColour+=0x20000000; }
		Vtx->ny=(Vtx+1)->ny=-1.0f;
		Vtx->vx=(Vtx+1)->vx=YStart+(float)x*GridSize;
		Vtx->vz=XStart;
		(Vtx+1)->vz=XStart+GridSize*Rows;

		Vtx+=2;
	}
}


void	DXDevice::DrawFrame(void *Frame, DWORD NVertices, D3DXMATRIX *Transform)
{
	// First of all save present renderer State
	DWORD	StateHandle;
	CheckHR(pD3DD->CreateStateBlock(D3DSBT_ALL, &StateHandle));

	pD3DD->SetRenderState(D3DRENDERSTATE_COLORVERTEX,TRUE);

	pD3DD->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_NONE);
	pD3DD->SetRenderState(D3DRENDERSTATE_FOGENABLE,FALSE);
	pD3DD->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
	pD3DD->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);
	pD3DD->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);
	pD3DD->SetRenderState( D3DRENDERSTATE_ZWRITEENABLE, FALSE );
	pD3DD->SetRenderState(D3DRENDERSTATE_ZFUNC,D3DCMP_ALWAYS); 
	pD3DD->SetRenderState(D3DRENDERSTATE_ZBIAS, 15);


	pD3DD->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_DISABLE);
	pD3DD->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
//	pD3DD->SetRenderState(D3DRENDERSTATE_LIGHTING,FALSE);
	pD3DD->SetRenderState( D3DRENDERSTATE_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR1 );
	if(Transform) pD3DD->SetTransform( D3DTRANSFORMSTATE_WORLD, (LPD3DMATRIX)Transform);
	CheckHR(pD3DD->DrawPrimitive( D3DPT_LINELIST, D3DFVF_MANAGED, Frame, NVertices, 0));
	pD3DD->SetRenderState( D3DRENDERSTATE_ZWRITEENABLE, TRUE );
	pD3DD->SetRenderState(D3DRENDERSTATE_ZFUNC,D3DCMP_LESSEQUAL); 

	// Restore Previous State Block and delete it from memory
	CheckHR(pD3DD->ApplyStateBlock(StateHandle));
	CheckHR(pD3DD->DeleteStateBlock(StateHandle));

}

void	DXDevice::DrawGrid(D3DXMATRIX *Transform)
{
	DrawFrame(m_pGrid, m_GridRows*4, Transform);
}


void	DXDevice::DrawLightNode(void *Frame, DWORD NVertices, D3DXMATRIX *Transform, D3DXVECTOR3 *Pos, bool Framed)
{
	// First of all save present renderer State
	DWORD	StateHandle;
	CheckHR(pD3DD->CreateStateBlock(D3DSBT_ALL, &StateHandle));

	pD3DD->SetRenderState(D3DRENDERSTATE_COLORVERTEX,TRUE);

	pD3DD->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_NONE);
	pD3DD->SetRenderState(D3DRENDERSTATE_FOGENABLE,FALSE);
	pD3DD->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
	pD3DD->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);
	pD3DD->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);
	pD3DD->SetRenderState( D3DRENDERSTATE_ZWRITEENABLE, FALSE );
	pD3DD->SetRenderState(D3DRENDERSTATE_ZFUNC,D3DCMP_ALWAYS); 
	pD3DD->SetRenderState(D3DRENDERSTATE_ZBIAS, 15);


	pD3DD->SetRenderState( D3DRENDERSTATE_FILLMODE, (Framed)?D3DFILL_WIREFRAME:D3DFILL_SOLID );

	pD3DD->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_DISABLE);
	pD3DD->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
	pD3DD->SetRenderState( D3DRENDERSTATE_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR2 );
	pD3DD->SetRenderState( D3DRENDERSTATE_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );
	D3DXMATRIX	World;
	D3DXMatrixTranslation(&World, Pos->x, Pos->y, Pos->z );
	D3DXMatrixMultiply(&World, Transform, &World);
	pD3DD->SetTransform( D3DTRANSFORMSTATE_WORLD, (LPD3DMATRIX)World);
	CheckHR(pD3DD->DrawPrimitive( D3DPT_TRIANGLELIST, D3DFVF_MANAGED, Frame, NVertices, 0));
	pD3DD->SetRenderState( D3DRENDERSTATE_ZWRITEENABLE, TRUE );
	pD3DD->SetRenderState(D3DRENDERSTATE_ZFUNC,D3DCMP_LESSEQUAL); 

	// Restore Previous State Block and delete it from memory
	CheckHR(pD3DD->ApplyStateBlock(StateHandle));
	CheckHR(pD3DD->DeleteStateBlock(StateHandle));

}
