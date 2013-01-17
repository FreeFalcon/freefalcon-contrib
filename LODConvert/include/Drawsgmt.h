#define MLR_NEWTRAILCODE

#ifdef MLR_NEWTRAILCODE
/***************************************************************************\
    DrawSgmt.h
    MLR
	12/10/2003

    Derived class to handle drawing segmented trails (like contrails).
\***************************************************************************/
#ifndef _DRAWSGMT_H_
#define _DRAWSGMT_H_

#include "DrawObj.h"
#include "RenderOW.h"

#include "context.h"
#include "Tex.h"
#include "falclib/include/alist.h"
#include "TimeMgr.h"


#ifdef USE_SH_POOLS
#include "SmartHeap\Include\smrtheap.h"
#endif

#include "context.h"

// Used to draw segmented trails (like missile trails)


enum TrailType {
    TRAIL_CONTRAIL, //0
	TRAIL_VORTEX, //1
	TRAIL_AIM120, //2
	TRAIL_TRACER1, //3
	TRAIL_TRACER2, //4
	TRAIL_SMOKE, //5
	TRAIL_FIRE1, //6
	TRAIL_EXPPIECE, //7
	TRAIL_THINFIRE, //8
	TRAIL_DISTMISSILE, //9
	TRAIL_DUST, //10
	TRAIL_GUN, //11
	TRAIL_LWING, //12
	TRAIL_RWING, //13
	TRAIL_ROCKET, //14
	TRAIL_MISLSMOKE, //15 Missile smoke from a Hit
	TRAIL_MISLTRAIL, //16
	TRAIL_DARKSMOKE, //17
	TRAIL_FIRE2, //18
	TRAIL_WINGTIPVTX, //19
	TRAIL_A10CANNON1, //20 new trails here!
	TRAIL_A10CANNON2, //21
	TRAIL_FEATURESMOKE, //22
	TRAIL_BURNINGDEBRIS, // 23 from ground explosions
	TRAIL_FLARE, // 24
	TRAIL_FLAREGLOW, // 25
	TRAIL_CONTAIL_SHORT, // 26    linked by 0
	TRAIL_MISSILEHIT_SMOKE, // 27 linked by 15
	TRAIL_MISSILEHIT_FIRE, // 28  linked by 27
	TRAIL_BURNING_SMOKE, // 29
	TRAIL_BURNING_SMOKE2, // 30 linked by 29
	TRAIL_BURNING_FIRE,  // 31 linked by 30
	TRAIL_ENGINE_SMOKE_SHARED, // 32 linked by 5 & 17
	TRAIL_GROUND_EXP_SMOKE, // 33 linked by 18
	TRAIL_DUSTCLOUD, // 34 replaces the dust stirred up by a/c
	TRAIL_MISTCLOUD, // 35 replaces the mist stirred up by a/c
	TRAIL_LINGERINGSMOKE, // 36
	TRAIL_GROUNDVDUSTCLOUD, // 37
	TRAIL_GROUNDVGUNSMOKE, // 38
	TRAIL_MAX = 150
};

class ChunkNode;

class DrawableTrail : public DrawableObject {
#ifdef USE_SH_POOLS
  public:
      // Overload new/delete to use a SmartHeap fixed size pool
      void *operator new(size_t size) { ShiAssert( size == sizeof(DrawableTrail) ); return MemAllocFS(pool);	};
      void operator delete(void *mem) { if (mem) MemFreeFS(mem); };
      static void InitializeStorage()	{ pool = MemPoolInitFS( sizeof(DrawableTrail), 40, 0 ); };
      static void ReleaseStorage()	{ MemPoolFree( pool ); };
      static MEM_POOL	pool;
#endif
  public:
	DrawableTrail( int trailType, float scale = 1.0f );
	virtual ~DrawableTrail();

	void	AddPointAtHead( Tpoint *p, DWORD now );
	int		RewindTrail( DWORD now );
	void	TrimTrail( int len );
	void	KeepStaleSegs( BOOL val ) { keepStaleSegs = val; };
	void    SetType(int trailType);


	virtual void Draw( class RenderOTW *renderer, int LOD );
	static void SetGreenMode(BOOL state);
	static void SetCloudColor(Tcolor *color);

	void    SetHeadVelocity(Tpoint *FPS);
	void    ReleaseToSfx(void);
	int IsTrailEmpty(void);
	
  protected:
	AList List;
	DrawableTrail *children;
	Texture *TrailTexture;
	static BOOL	greenMode;
	static Tcolor litCloudColor;

	int					type;
	BOOL				keepStaleSegs;	// for ACMI
  private:
	  void   DrawNode(class RenderOTW *renderer, int LOD, class TrailNode *);
	  void   DrawSegment(class RenderOTW *renderer, int LOD, class TrailNode *start, class TrailNode *end);
	  struct TrailTypeEntry *Type;
	  int	 Something;
	  class  DrawableTrail *Link;
	  Tpoint headFPS;
	  ThreeDVertex v0,v1,v2,v3; // so we can precompute the colors
  public:
	static void SetupTexturesOnDevice( DXContext *rc );
	static void ReleaseTexturesOnDevice( DXContext *rc );

};

#endif // _DRAWSGMT_H_

#else
/***************************************************************************\
    DrawSgmt.h
    Scott Randolph
    May 3, 1996

    Derived class to handle drawing segmented trails (like contrails).
\***************************************************************************/
#ifndef _DRAWSGMT_H_
#define _DRAWSGMT_H_

#include "DrawObj.h"
#include "Falclib\Include\IsBad.h"

#ifdef USE_SH_POOLS
#include "SmartHeap\Include\smrtheap.h"
#endif

#include "context.h"

// Used to draw segmented trails (like missile trails)
class TrailElement {
#ifdef USE_SH_POOLS
  public:
      // Overload new/delete to use a SmartHeap fixed size pool
      void *operator new(size_t size) { ShiAssert( size == sizeof(TrailElement) ); return MemAllocFS(pool);	};
      void operator delete(void *mem) { if (mem) MemFreeFS(mem); };
      static void InitializeStorage()	{ pool = MemPoolInitFS( sizeof(TrailElement), 100, 0 ); };
      static void ReleaseStorage()	{ MemPoolFree( pool ); };
      static MEM_POOL	pool;
#endif
  public:
	TrailElement( Tpoint *p, DWORD now )	{ point = *p; time = now; };
	~TrailElement()							
	{ 
		if (!F4IsBadWritePtr(next, sizeof(TrailElement))) // JB 010304 CTD
		{	delete next; next = NULL; }
	};

	Tpoint			point;		// World space location of this point on the trail
	DWORD			time;		// For head this is insertion time, for others its deltaT

	TrailElement	*next;
};

enum TrailType {
    TRAIL_CONTRAIL, //0
	TRAIL_VORTEX, //1
	TRAIL_AIM120, //2
	TRAIL_TRACER1, //3
	TRAIL_TRACER2, //4
	TRAIL_SMOKE, //5
	TRAIL_FIRE1, //6
	TRAIL_EXPPIECE, //7
	TRAIL_THINFIRE, //8
	TRAIL_DISTMISSILE, //9
	TRAIL_DUST, //10
	TRAIL_GUN, //11
	TRAIL_LWING, //12
	TRAIL_RWING, //13
	TRAIL_ROCKET, //14
	TRAIL_MISLSMOKE, //15
	TRAIL_MISLTRAIL, //16
	TRAIL_DARKSMOKE, //17
	TRAIL_FIRE2, //18
	TRAIL_WINGTIPVTX, //19
	TRAIL_MAX
};

class DrawableTrail : public DrawableObject {
#ifdef USE_SH_POOLS
  public:
      // Overload new/delete to use a SmartHeap fixed size pool
      void *operator new(size_t size) { ShiAssert( size == sizeof(DrawableTrail) ); return MemAllocFS(pool);	};
      void operator delete(void *mem) { if (mem) MemFreeFS(mem); };
      static void InitializeStorage()	{ pool = MemPoolInitFS( sizeof(DrawableTrail), 40, 0 ); };
      static void ReleaseStorage()	{ MemPoolFree( pool ); };
      static MEM_POOL	pool;
#endif
  public:
	DrawableTrail( int trailType, float scale = 1.0f );
	virtual ~DrawableTrail();

	void	AddPointAtHead( Tpoint *p, DWORD now );
	int		RewindTrail( DWORD now );
	void	TrimTrail( int len );
	void	KeepStaleSegs( BOOL val ) { keepStaleSegs = val; };

	virtual void Draw( class RenderOTW *renderer, int LOD );

	TrailElement*	GetHead()	{ return head; };

  protected:
	int					type;
	TrailElement		*head;
	BOOL				keepStaleSegs;	// for ACMI

  protected:
	void ConstructSegmentEnd( RenderOTW *renderer, 
							  Tpoint *start, Tpoint *end, 
							  struct ThreeDVertex *xformLeft, ThreeDVertex *xformRight );

	// Handle time of day notifications
	static void TimeUpdateCallback( void *unused );

  public:
	static void SetupTexturesOnDevice( DXContext *rc );
	static void ReleaseTexturesOnDevice( DXContext *rc );
};

#endif // _DRAWSGMT_H_

#endif