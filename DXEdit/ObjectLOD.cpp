/***************************************************************************\
    ObjectLOD.cpp
    Scott Randolph
    February 9, 1998

    Provides structures and definitions for 3D objects.
\***************************************************************************/
#include "stdafx.h"
#include <io.h>
#include <fcntl.h>
#include "Loader.h"
#include "ObjectLOD.h"

#ifdef USE_SH_POOLS
extern MEM_POOL gBSPLibMemPool;
#endif

ObjectLOD			*TheObjectLODs = NULL;
int					TheObjectLODsCount = 0;
#ifdef _DEBUG
int		    ObjectLOD::lodsLoaded = 0;
#endif
FileMemMap  ObjectLOD::ObjectLodMap;

CRITICAL_SECTION	ObjectLOD::cs_ObjectLOD;
static 	int		maxTagList;
//extern bool g_bUseMappedFiles;
#define	g_bUseMappedFiles true


ObjectLOD::ObjectLOD()
{
	root		= NULL;
	flags		= 0;
	refCount	= 0;
	onOrder		= 0;
}


ObjectLOD::~ObjectLOD()
{
	ShiAssert( !onOrder );
	ShiAssert( !root );
}


void ObjectLOD::SetupEmptyTable( int numEntries )
{
#ifdef USE_SMART_HEAP
	pool = MemPoolInit( 0 );
#endif

	// Create the contiguous array of (unitialized) LOD records
	ShiAssert( TheObjectLODs == NULL );
	#ifdef USE_SH_POOLS
	TheObjectLODs = (ObjectLOD *)MemAllocPtr(gBSPLibMemPool, sizeof(ObjectLOD)*(numEntries), 0 );
	#else
	TheObjectLODs = new ObjectLOD[numEntries];
	#endif
	TheObjectLODsCount = numEntries;

	// Init our critical section
	InitializeCriticalSection( &cs_ObjectLOD );
}


void ObjectLOD::SetupTable( int file, char *basename )
{
	char	filename[_MAX_PATH];
	int		result;


#ifdef USE_SMART_HEAP
	pool = MemPoolInit( 0 );
#endif


	// Read the number of elements in the longest tag list we saw in the LOD data
	result = read( file, &maxTagList, sizeof(maxTagList) );

	// Read the length of the LOD header array
	result = read( file, &TheObjectLODsCount, sizeof(TheObjectLODsCount) );


	// Allocate memory for the LOD array
	TheObjectLODs = new ObjectLOD[TheObjectLODsCount];
	ShiAssert( TheObjectLODs );

	// Allocate memory for the tag list buffer (read time use only)
	tagListBuffer = new BNodeType[maxTagList+1];
	ShiAssert( tagListBuffer );

	// Read the elements of the header array
	result = read( file, TheObjectLODs, sizeof(*TheObjectLODs)*TheObjectLODsCount );
	if (result < 0 ) {
		char message[256];
		sprintf( message, "Reading object LOD headers:  %s", strerror(errno) );
		ShiError( message );
	}


	// Open the data file we'll read from at run time as object LODs are required
	/*strcpy( filename, basename );
	strcat( filename, ".LOD" );
	if (!ObjectLodMap.Open(filename, FALSE, !g_bUseMappedFiles)) {
		char message[256];
		sprintf( message, "Failed to open object LOD database %s", filename );
		ShiError( message );
	}

	// Init our critical section
	InitializeCriticalSection( &cs_ObjectLOD );*/
}


void ObjectLOD::CleanupTable( void )
{
	// Make sure all objects are freed
	for (int i=0; i<TheObjectLODsCount; i++) {
		ShiAssert( TheObjectLODs[i].refCount == 0 );

		// Must wait until loader is done before we delete the object out from under it.
		while ( TheObjectLODs[i].onOrder );
	}

	// Free our array of object LODs
	delete[] TheObjectLODs;
	TheObjectLODs = NULL;
	TheObjectLODsCount = 0;

	// Free our tag list buffer
	delete[] tagListBuffer;
	tagListBuffer = NULL;

	// Close our data file
	ObjectLodMap.Close();
	//close( objectFile );

	// Free our critical section
	DeleteCriticalSection( &cs_ObjectLOD );

#ifdef USE_SMART_HEAP
	MemPoolFree( pool );
#endif
}


BOOL ObjectLOD::Fetch(void)
{
	BOOL	result;

    EnterCriticalSection( &cs_ObjectLOD );

	ShiAssert( refCount > 0 );

	// If we already have our data, we can draw
	if (root) {
		result = TRUE;
	} else {
		if (onOrder == 1) {
			// Normal case, do nothing but wait
		} else if (onOrder == 0) {
			// Not requested yet, so go get it
			RequestLoad();
			onOrder = 1;
		} else {
			// Requested, dropped, and now requested again all before the IO completed
			ShiAssert( onOrder == -1 );
			onOrder = 1;
		}
		result = FALSE;
	}

    LeaveCriticalSection( &cs_ObjectLOD );

	// Return a flag indicating whether or not the data is available for drawing
	return result;
}


// This is called from inside our critical section...
void ObjectLOD::RequestLoad( void )
{
	LoaderQ		*request;

	// Allocate space for the async IO request
	request = new LoaderQ;
	if (!request) {
		ShiError( "Failed to allocate memory for an object read request" );
	}

	// Build the data transfer request to get the required object data
	request->filename	= NULL;
	request->fileoffset	= fileoffset;
	request->callback	= LoaderCallBack;
	request->parameter	= this;

	// Submit the request to the asynchronous loader
	TheLoader.EnqueueRequest( request );
}


void ObjectLOD::LoaderCallBack( LoaderQ* request )
{
}


void ObjectLOD::Unload( void )
{
}



// Privatly used static members
//int			ObjectLOD::objectFile = -1;
BNodeType*	ObjectLOD::tagListBuffer = NULL;

#ifdef USE_SMART_HEAP
MEM_POOL	ObjectLOD::pool;
#endif
