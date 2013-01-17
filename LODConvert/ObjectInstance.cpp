/***************************************************************************\
    ObjectInstance.cpp
    Scott Randolph
    February 9, 1998

    Provides structures and definitions for 3D objects.
\***************************************************************************/
#include "stdafx.h"
#include "StateStack.h"
#include "ObjectInstance.h"

#ifdef USE_SH_POOLS
extern MEM_POOL gBSPLibMemPool;
#endif

ObjectInstance::ObjectInstance( int tid )
{
	id = tid;

	ShiAssert(TheObjectList);
	ShiAssert(id < TheObjectListLength);	// Make sure the requested object ID is in
	ShiAssert(TheObjectList[tid].nLODs);		// the current object set (IDS.TXT, etc).

	ParentObject = &TheObjectList[tid];

	/***** BEGIN HACK HACK HACK HACK - Billy forced me to do it :-) -RH *****/
	if
	(	// F16c
		(tid == 1052) ||
		(tid == 564) ||
		(tid == 563) ||
		(tid == 562) ||
		(tid == 5)
	)
	{
		ParentObject->radius = 40.0;
	}

	if (tid == 1329)		// B1
	{
		ParentObject->radius = 100.0;
	}
	/***** END HACK HACK HACK HACK - Billy forced me to do it :-) -RH *****/

	ParentObject->Reference();

	if (ParentObject->nSwitches == 0) {
		SwitchValues = NULL;
	} else {
		#ifdef USE_SH_POOLS
		SwitchValues = (DWORD *)MemAllocPtr(gBSPLibMemPool, sizeof(DWORD)*(ParentObject->nSwitches), 0 );
		#else
		SwitchValues = new DWORD[ParentObject->nSwitches];
		#endif
		memset( SwitchValues, 0, sizeof(*SwitchValues)*ParentObject->nSwitches );
	}

	if (ParentObject->nDOFs == 0) {
		DOFValues = NULL;
	} else {
		#ifdef USE_SH_POOLS
		DOFValues = (DOFvalue *)MemAllocPtr(gBSPLibMemPool, sizeof(DOFvalue)*(ParentObject->nDOFs), 0 );
		#else
		DOFValues = new DOFvalue[ParentObject->nDOFs];
		#endif
		memset( DOFValues, 0, sizeof(*DOFValues)*ParentObject->nDOFs );
	}

	if (ParentObject->nSlots == 0) {
		SlotChildren = NULL;
	} else {
		#ifdef USE_SH_POOLS
		SlotChildren = (ObjectInstance **)MemAllocPtr(gBSPLibMemPool, sizeof(ObjectInstance *)*(ParentObject->nSlots), 0 );
		#else
		SlotChildren = new ObjectInstance*[ParentObject->nSlots];
		#endif
		memset( SlotChildren, 0, sizeof(*SlotChildren)*ParentObject->nSlots );
	}

	if (ParentObject->nDynamicCoords == 0) {
		DynamicCoords = NULL;
	} else {
		#ifdef USE_SH_POOLS
		DynamicCoords = (Ppoint *)MemAllocPtr(gBSPLibMemPool, sizeof(Ppoint)*(ParentObject->nDynamicCoords), 0 );
		#else
		DynamicCoords = new Ppoint[ParentObject->nDynamicCoords];
		#endif
		memcpy( DynamicCoords, 
				ParentObject->pSlotAndDynamicPositions + ParentObject->nSlots, 
				sizeof(*DynamicCoords)*ParentObject->nDynamicCoords );
	}

	TextureSet		= 0;
}


ObjectInstance::~ObjectInstance()
{
	ParentObject->Release();
	ParentObject = NULL;
	#ifdef USE_SH_POOLS
	if( SwitchValues )
		MemFreePtr( SwitchValues );
	if( DOFValues )
		MemFreePtr( DOFValues );
	if( SlotChildren )
		MemFreePtr( SlotChildren );
	if( DynamicCoords )
		MemFreePtr( DynamicCoords );
	#else
	delete[] SwitchValues;
	delete[] DOFValues;
	delete[] SlotChildren;
	delete[] DynamicCoords;
	#endif
}


void ObjectInstance::SetDynamicVertex( int id, float dx, float dy, float dz )
{
	Ppoint	*original;

	ShiAssert(id<ParentObject->nDynamicCoords);
	
	original = ParentObject->pSlotAndDynamicPositions + ParentObject->nSlots + id;

#ifdef _DEBUG
	float r1 = Radius()*Radius();
#endif

	DynamicCoords[id].x = original->x + dx;
	DynamicCoords[id].y = original->y + dy;
	DynamicCoords[id].z = original->z + dz;

#ifdef _DEBUG
	float r2 = DynamicCoords[id].x*DynamicCoords[id].x + DynamicCoords[id].y*DynamicCoords[id].y + DynamicCoords[id].z*DynamicCoords[id].z;

	ShiAssert( r2 <= r1 + 0.00001f );	// Illegal for dynamic verts to exceed object bounding volume
										// so we require movement to be only toward the origin.
#endif
}


void ObjectInstance::GetDynamicVertex( int id, float *dx, float *dy, float *dz )
{
	Ppoint	*original;

	ShiAssert(id<ParentObject->nDynamicCoords);
	
	original = ParentObject->pSlotAndDynamicPositions + ParentObject->nSlots + id;

	*dx = DynamicCoords[id].x - original->x;
	*dy = DynamicCoords[id].y - original->y;
	*dz = DynamicCoords[id].z - original->z;
}
