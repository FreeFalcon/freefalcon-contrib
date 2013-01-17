/***************************************************************************\
    Display.h
    Scott Randolph
    February 1, 1995

    This class provides basic 2D drawing functions in a device independent fashion.
\***************************************************************************/
#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "Ttypes.h"

//#define USE_ORIGINAL_FONT
//#define USE_STROKE_FONT
#define USE_TEXTURE_FONT

#ifdef USE_TEXTURE_FONT

#define NUM_FONT_RESOLUTIONS     4

typedef struct FontDataTag
{
	float top;
	float left;
	float width;
	float height;
	float pixelWidth;
	float pixelHeight;
} FontDataType;


int 	ReadFontMetrics(int indx, char*fileName);
extern FontDataType FontData[NUM_FONT_RESOLUTIONS][256];
extern int fontSpacing[NUM_FONT_RESOLUTIONS];

#endif
extern int FontNum;
extern int TotalFont;

// Clipping flags.  Some of these are only used in 3D clipping, but I want to keep them together.
static const DWORD	ON_SCREEN			= 0x00;
static const DWORD	CLIP_LEFT			= 0x01;
static const DWORD	CLIP_RIGHT			= 0x02;
static const DWORD	CLIP_TOP			= 0x04;
static const DWORD	CLIP_BOTTOM			= 0x08;
static const DWORD	CLIP_NEAR			= 0x10;
static const DWORD	CLIP_FAR			= 0x20;
static const DWORD	OFF_SCREEN			= 0xFF;

static const int	CircleStep = 4;							// In units of degrees
static const int	CircleSegments = 360/CircleStep + 1;	// How many segments (plus one)?
extern float		CircleX[];
extern float		CircleY[];

struct DisplayMatrix { // JPO - how a display is oriented
    float	translationX, translationY;
    float	rotation00,	rotation01;
    float	rotation10,	rotation11;
};

class VirtualDisplay {
  public:
    VirtualDisplay()			{ ready = FALSE; };
    virtual ~VirtualDisplay()	{ ShiAssert( ready == FALSE ); };

	// One time call to create inverse font
	static void InitializeFonts( void );

    // Parents Setup() must set xRes and yRes before call this...
    virtual void Setup( void );
    virtual void Cleanup( void );
	BOOL	IsReady( void )				{ return ready; };

    virtual void StartFrame( void ) = 0;
    virtual void ClearFrame( void ) = 0;
    virtual void FinishFrame( void ) = 0;

    virtual void Point( float x1, float y1 );
    virtual void Line( float x1, float y1, float x2, float y2 );
    virtual void Tri( float x1, float y1, float x2, float y2, float x3, float y3 );
	virtual void Oval (float x, float y, float xRadius, float yRadius);
	virtual void OvalArc (float x, float y, float xRadius, float yRadius, float start, float stop);
	virtual void Circle ( float x, float y, float xRadius )
		{ Oval(x, y, xRadius, xRadius*scaleX/scaleY); };
	virtual void Arc ( float x, float y, float xRadius, float start, float stop )
		{ OvalArc(x, y, xRadius, xRadius*scaleX/scaleY, start, stop); };

	virtual void TextLeft( float x1, float y1, char *string, int boxed = 0 );
	virtual void TextRight( float x1, float y1, char *string, int boxed = 0 );
	virtual void TextLeftVertical( float x1, float y1, char *string, int boxed = 0 );
	virtual void TextRightVertical( float x1, float y1, char *string, int boxed = 0 );
	virtual void TextCenter( float x1, float y1, char *string, int boxed = 0 );
	virtual void TextCenterVertical( float x1, float y1, char *string, int boxed = 0 );
	virtual int  TextWrap( float h, float v, char *string, float spacing, float width );

	// NOTE:  These might need to be virtualized and overloaded by canvas3d (maybe???)
	virtual float TextWidth(char *string)	{ return ScreenTextWidth(string)/scaleX; };	// normalized screen space
	virtual float TextHeight(void)			{ return ScreenTextHeight()/scaleY; };		// normalized screen space

	//JAM 22Dec03
	virtual void SetColor(DWORD)=0;
	virtual void SetBackground(DWORD)=0;
	virtual void ScreenText(float x,float y,char *string,int boxed=0)=0;

	static int ScreenTextHeight(void);
	static int ScreenTextWidth(char *string);

	static void SetFont(int newFont);
	static int CurFont(void) { return FontNum; };
	//JAM
	
	virtual void SetLineStyle (int) {};
	virtual DWORD Color( void )	{return 0x0; };

    virtual void SetViewport( float leftSide, float topSide, float rightSide, float bottomSide );
    virtual void SetViewportRelative( float leftSide, float topSide, float rightSide, float bottomSide );

    void AdjustOriginInViewport( float horizontal, float vertical );
	void AdjustRotationAboutOrigin( float angle );
	void CenterOriginInViewport( void )  { dmatrix.translationX = 0.0f; dmatrix.translationY = 0.0f; };
    void ZeroRotationAboutOrigin( void ) { dmatrix.rotation01 = dmatrix.rotation10 = 0.0f, dmatrix.rotation00 = dmatrix.rotation11 = 1.0f; };
	void SaveDisplayMatrix (DisplayMatrix *dm);
	void RestoreDisplayMatrix(DisplayMatrix *dm);

    int GetXRes(void) { return xRes; };
    int GetYRes(void) { return yRes; };

	void GetViewport( float *leftSide, float *topSide, float *rightSide, float *bottomSide );

	float GetTopPixel( void )		{ return topPixel; };
	float GetBottomPixel( void )	{ return bottomPixel; };
	float GetLeftPixel( void )		{ return leftPixel; };
	float GetRightPixel( void )		{ return rightPixel; };

	float GetXOffset(void)	{ return shiftX; };
	float GetYOffset(void)	{ return shiftY; };

	enum {
		DISPLAY_GENERAL = 0,
		DISPLAY_CANVAS
	} type;

    // Functions to convert from normalized coordinates to pixel coordinates
	// (assumes at this point that x is right and y is down)
    float viewportXtoPixel( float x ) { return (x * scaleX) + shiftX; };
    float viewportYtoPixel( float y ) { return (y * scaleY) + shiftY; };

 protected:	
    // Functions which must be provided by all derived classes
    virtual void Render2DPoint( float x1, float y1 ) = 0;
    virtual void Render2DLine( float x1, float y1, float x2, float y2 ) = 0;

    // Functions which should be provided by all derived classes
    virtual void Render2DTri( float x1, float y1, float x2, float y2, float x3, float y3 );

  protected:
    // Store the currently selected resolution
    int		xRes;
    int		yRes;

    // The viewport properties in normalized screen space (-1 to 1)
    float	left, right;
    float	top, bottom;

    // The parameters required to get from normalized screen space to pixel space
	// TEMPORARILY PUBLIC TO GET THINGS GOING...
  public:
	float	scaleX;
    float	scaleY;
    float	shiftX;
    float	shiftY;

  protected:
	// Store the pixel space boundries of the current viewport
	// (top/right inclusive, bottom/left exclusive)
	float	topPixel;
	float	bottomPixel;
	float	leftPixel;
	float	rightPixel;

    // The 2D rotation/translation settings
	DisplayMatrix dmatrix; // JPO - now in a sub structure so you can save/restore
    //float	translationX, translationY;
    //float	rotation00,	rotation01;
    //float	rotation10,	rotation11;

    // The font information for drawing text
	static const unsigned char FontLUT[256];
    static const unsigned char *Font[];
	static const unsigned int  FontLength;
    static unsigned char       InvFont[][8];
	BOOL	ready;

public:
	bool	ForceAlpha;							// COBRA - RED - To force translucent displays

};


#endif // _DISPLAY_H_
