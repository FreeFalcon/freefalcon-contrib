/***************************************************************************\
    ObjectLOD.h
    Scott Randolph
    February 9, 1998

    Provides structures and definitions for 3D objects.
\***************************************************************************/
#ifndef _OBJECTLOD_H_
#define _OBJECTLOD_H_

#include "BSPnodes.h"
#include "Falclib\Include\FileMemMap.h"


extern class ObjectLOD	*TheObjectLODs;
extern int				TheObjectLODsCount;


class ObjectLOD {
  public:
	ObjectLOD();
	~ObjectLOD();

	static void SetupEmptyTable( int numEntries );
	static void SetupTable( int file, char *basename );
	static void CleanupTable();

	void	Reference(void)		{ refCount++; };
	void	Release(void)		{ refCount--; if (refCount==0) Unload(); };

	BOOL	Fetch(void);	// True means ready to draw.  False means still waiting.
	void	Draw(void) const	{ ShiAssert( root ); root->Draw(); };

	static CRITICAL_SECTION	cs_ObjectLOD;

  protected:
	// Handle asychronous data loading
	void		RequestLoad( void );
	static void LoaderCallBack( struct LoaderQ* request );
	void		Unload( void );

	static FileMemMap  ObjectLodMap; // JPO - MMFILE
//	static int				objectFile;
	static BNodeType		*tagListBuffer;
	int		refCount;		// How many instances of this LOD are in use
	short	onOrder;		// TRUE when IO is pending (normally 1, -1 means no longer needed)

  public:
	// Object flag values
	enum { NONE = 0, PERSP_CORR = 1, };

	short	flags;			// Special handling flags for this visual
	BRoot	*root;			// NULL until loaded, then pointer to node tree
	UInt32	fileoffset;		// Where in the disk file is this record's tree stored
	UInt32	filesize;		// How big the disk representation of this record's tree

#ifdef USE_SMART_HEAP
  public:
	static MEM_POOL	pool;
#endif
#ifdef _DEBUG
	static int lodsLoaded; // JPO - some stats
#endif

};

#endif // _OBJECTLOD_H_
