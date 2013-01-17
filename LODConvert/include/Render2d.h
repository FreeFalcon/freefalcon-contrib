/***************************************************************************\
    Render2D.h
    Scott Randolph
    December 29, 1995

    This class provides 2D drawing functions.
\***************************************************************************/
#ifndef _RENDER2D_H_
#define _RENDER2D_H_

#include "Ttypes.h"
#include "ImageBuf.h"
#include "Context.h"
#include "Display.h"
#include "Tex.h"

typedef struct TwoDVertex: public MPRVtxTexClr_t {
/* MPRVtxTexClr_t provides:
	float	x, y;
	float	r, g, b, a;	
	float	u, v, q; 

   Then I add:
*/
	DWORD	clipFlag;
} TwoDVertex;


class Render2D : public VirtualDisplay {
  public:
	Render2D()			{};
	virtual ~Render2D()	{};

	virtual void Setup( ImageBuffer *imageBuffer );
	virtual void Cleanup( void );

	virtual void SetImageBuffer( ImageBuffer *imageBuffer );
	ImageBuffer* GetImageBuffer(void) {return image;};

	virtual void StartFrame( void );
	virtual void ClearFrame( void )		{ context.ClearBuffers( MPR_CI_DRAW_BUFFER ); };
	virtual void FinishFrame( void );

    virtual void SetViewport( float leftSide, float topSide, float rightSide, float bottomSide );

	//JAM 22Dec03 - These should not be here.
	virtual DWORD Color(void) { return context.CurrentForegroundColor(); };
	virtual void SetColor(DWORD packedRGBA)	{ context.RestoreState(STATE_SOLID); context.SelectForegroundColor(packedRGBA); };
	virtual void SetBackground(DWORD packedRGBA){ context.SetState(MPR_STA_BG_COLOR,packedRGBA); };

	void Render2DPoint( float x1, float y1 );
	void Render2DLine( float x1, float y1, float x2, float y2 );
	void Render2DTri( float x1, float y1, float x2, float y2, float x3, float y3 );
	void Render2DBitmap( int srcX, int srcY, int dstX, int dstY, int w, int h, int sourceWidth, DWORD *source );
	void Render2DBitmap( int srcX, int srcY, int dstX, int dstY, int w, int h, char *filename );
	void ScreenText( float x, float y, char *string, int boxed = 0 );

	virtual void SetLineStyle (int) {};

	// Draw a fan with clipping (must set clip flags first)
	void SetClipFlags( TwoDVertex* vert );
	void ClipAndDraw2DFan( TwoDVertex** vertPointers, unsigned count, bool gifPicture = false );

	//JAM 22Dec03
	static void Load2DFontTextures();
	static void Release2DFontTextures();


  public:
	// Window and rendering context handles
	ContextMPR			context;
	static Texture FontTexture[NUM_FONT_RESOLUTIONS]; //JAM 22Dec03

  protected:
	ImageBuffer			*image;

  private:
	void IntersectTop(    TwoDVertex *v1, TwoDVertex *v2, TwoDVertex *v );
	void IntersectBottom( TwoDVertex *v1, TwoDVertex *v2, TwoDVertex *v );
	void IntersectLeft(   TwoDVertex *v1, TwoDVertex *v2, TwoDVertex *v );
	void IntersectRight(  TwoDVertex *v1, TwoDVertex *v2, TwoDVertex *v );
};


#endif // _RENDER2D_H_
