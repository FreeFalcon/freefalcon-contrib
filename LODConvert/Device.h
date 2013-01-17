#include <ddraw.h>
#include <d3d.h>
#include <d3dxcore.h>
#include <d3dxmath.h>
#include "ObjectInstance.h"
#include <dinput.h>


#define DINPUT_BUFFERSIZE  16
#define	MANAGED_OBJECTS		8

class	DXDevice{
public:
	DXDevice(CWnd *wnd);
	~DXDevice();

void	SetupMatrices();
void		SetupLights();
void	Render();
HRESULT	InitGeometry();
void	SetupMouse(CWnd *wnd);


public:
	IDirectDraw7		*pDD;
	IDirect3D7			*pD3D;
	IDirect3DDevice7	*pD3DD;
	IDirectDrawSurface7	*g_pddsPrimary, *g_pddsBackBuffer, *pDDSFront, *pDDSBack, *g_pddsZBuffer;

	RECT				g_rcScreenRect, g_rcViewportRect;
	VOID SelectTexture(GLint texID);
	VOID PreloadTexture(GLint texID);
	CString GetTimings();
	void	DrawDemo();
	void	Draw(ObjectInstance *obj);
	void	UnDraw(ObjectInstance *obj);
	void	SetVectors(float Yaw, float Roll, float Pitch, float X, float Y, float Z);
	void	OnMouseInput(HWND hwnd);
	void	DrawPlane();
	void	TogglePlane();
	ObjectInstance	*DrawObject[MANAGED_OBJECTS];
	bool	DiscoEnable;
	DWORD	SelectedObject;
	HWND	GethWnd(void) { return DX_hWnd; }



private:
	bool	CreateZBuffer(void);
	VOID CALLBACK TimerProc(HWND hwnd,     // handle of window for timer messages
	UINT uMsg,     // WM_TIMER message
	UINT idEvent,  // timer identifier
	DWORD dwTime   // current system time

);
	float	m_Pitch, m_Roll, m_Yaw, m_X, m_Y, m_Z;
	HWND	DX_hWnd;
	LPDIRECTINPUT7		g_pdi;
	LPDIRECTINPUTDEVICE g_pMouse; 
	HANDLE				g_hevtMouse;
	DWORD				m_dwRenderWidth;
    DWORD				m_dwRenderHeight;
	bool				m_EnablePlane;



};

#define	MEASURE_START	MeasureStart()
#define	MEASURE_STOP	MeasureStop()




