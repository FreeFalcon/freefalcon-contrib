#include <time.h>
#include "falclib.h"
#include "classtbl.h"
#include "sim\include\simlib.h"
#include "sim\include\simvucb.h"
#include "vu2\src\vu_priv.h"
#include "sim\include\simbase.h"
#include "campbase.h"
#include "falcmesg.h"
#include "FalcSess.h"
#include "F4Comms.h"
#include "F4Thread.h"
#include "F4Vu.h"
#include "F4Version.h"

FILE* F4EventFile;

#define HOSTNAMEPACKET                  1
#define HOSTLISTREJECTPACKET       		2
#define PROPOSEHOSTNAMEPACKET      		3
#define NEWGROUPPACKET                  4
#define PROPOSEGROUPNAMEPACKET   		5
#define GROUPREJECTPACKET               6
#define JOINGROUPPACKET                 7
#define LEAVEGROUPPACKET                8
#define CLOSESESSIONPACKET       		9
#define CLOSEGROUPPACKET         		10
#define OPENGROUPPACKET                 11
#define DELETEGROUPPACKET               12

#define SESSIONUPDATEPACKET             1024

//#define F4_ENTITY_TABLE_SIZE			5000		// Size of vu's hash table
const int F4_ENTITY_TABLE_SIZE = 10529;			// Size of vu's hash table	 - this is a prime number // JB 010718

// =========================
// Some external functions
// =========================

class UnitClass;
class ObjectiveClass;
class CampManagerClass;
extern UnitClass* NewUnit (short tid, VU_BYTE **stream);
extern ObjectiveClass* NewObjective (short tid, VU_BYTE **stream);
extern VuEntity* NewManager (short tid, VU_BYTE *stream);
extern void TcpConnectCallback(ComAPIHandle ch, int ret);
extern int MajorVersion;
extern int MinorVersion;
extern int BuildNumber;

extern VU_ID_NUMBER vuAssignmentId;
extern VU_ID_NUMBER vuLowWrapNumber;
extern VU_ID_NUMBER vuHighWrapNumber;

// =========================
// VU required globals
// =========================

ulong vuxVersion = 1;
SM_SCALAR vuxTicsPerSec = 1000.0F;
VuDriverSettings *vuxDriverSettings = 0;
VU_TIME vuxGameTime = 0;
VU_TIME vuxTargetGameTime = 0;
VU_TIME vuxLastTargetGameTime = 0;
VU_TIME vuxDeadReconTime = 0;
VU_TIME vuxCurrentTime = 0;
VU_TIME lastTimingMessage = 0;
VU_TIME vuxTransmitTime = 0;
//ulong vuxLocalDomain = 1;	// range = 1-31
ulong vuxLocalDomain = 0xffffffff;	// range = 1-31 // JB 010718
VU_BYTE vuxLocalSession = 1;
#define EBS_BASE_NAME    "EBS"
char *vuxWorldName = 0;
VU_TIME vuxRealTime = 0;

VuSessionEntity* vuxCreateSession (void);

class GroundSpotEntity : public FalconEntity
{
public:
	GroundSpotEntity (int type);
	GroundSpotEntity (VU_BYTE **);
	virtual int Sleep (void) { return 0; }
	virtual int Wake (void) { return 0; }
};

// =================================
// VU related globals for Falcon 4.0
// =================================

VuMainThread *gMainThread = 0;
Falcon4EntityClassType* Falcon4ClassTable;
F4CSECTIONHANDLE* vuCritical = NULL;
int NumEntities;
VU_ID FalconNullId;
FalconAllFilterType		FalconAllFilter;
FalconNothingFilterType	FalconNothingFilter;

#define VU_VERSION_USED    2
#define VU_REVISION_USED   23
#define VU_PATCH_USED      2

#ifndef CLASSMKR

// ==================================
// Functions for VU's CS
// ==================================
extern char g_strWorldName[0x40];

void InitVU (void)
{
char tmpStr[256];

	#ifdef USE_SH_POOLS
	gVuMsgMemPool = MemPoolInit( 0 );
	gVuFilterMemPool = MemPoolInit( 0 );

	VuLinkNode::InitializeStorage();
	VuRBNode::InitializeStorage();
	#endif

   // Make sure we're using the right VU
#if (VU_VERSION_USED  != VU_VERSION)
//#error "Incorrect VU Version"
#endif

#if (VU_REVISION_USED != VU_REVISION)
//#error "Incorrect VU Revision"
#endif

#if (VU_PATCH_USED    != VU_PATCH)
//#error "Incorrect VU Patch"
#endif

#ifdef NDEBUG // Differentiate Debug & Release versions so they can't be seen by each other (PJW)
   sprintf (tmpStr, "R%5d.%2d.%02d.%s.%d_\0", BuildNumber, gLangIDNum, MinorVersion, "EBS", MajorVersion);
#else
   sprintf (tmpStr, "K%5d.%2d.%02d.%s.%d_\0", BuildNumber, gLangIDNum, MinorVersion, "EBS", MajorVersion);
#endif

	MonoPrint ("Version %s %s %s\n", tmpStr, __DATE__, __TIME__);

	// Change this to stop different versions taking to each other

	// OW FIXME
	// strcpy (tmpStr, "F527");
//	strcpy(tmpStr, "F552");		//  according to REVISOR this will allow connections to 1.08 servers. we'll see
//strcpy(tmpStr, "E109newmp"); //me123 we are not interested in 108 conections anymore since they'll ctd us
	strcpy(tmpStr, g_strWorldName);

	vuxWorldName = new char[strlen(tmpStr) + 1];
	strcpy (vuxWorldName, tmpStr);
	FalconMessageFilter falconFilter(FalconEvent::SimThread, VU_VU_MESSAGE_BITS);
	vuCritical = F4CreateCriticalSection("Vu");
	gMainThread = new VuMainThread (F4_ENTITY_TABLE_SIZE,&falconFilter,F4_EVENT_QUEUE_SIZE,vuxCreateSession);
   
	// Default VU namespace
	vuAssignmentId = FIRST_VOLITILE_VU_ID_NUMBER;
	vuLowWrapNumber = FIRST_VOLITILE_VU_ID_NUMBER;
	vuHighWrapNumber = LAST_VOLITILE_VU_ID_NUMBER;

   // creates global
   if (!vuxDriverSettings)
   {
      vuxDriverSettings = new VuDriverSettings(
         (SM_SCALAR)200.0, (SM_SCALAR)200.0, (SM_SCALAR)200.0, // x, y, z tolerance
         (SM_SCALAR)0.1,   (SM_SCALAR)0.1,   (SM_SCALAR)0.1, // y, p, r tolerance
          (SM_SCALAR)1.0,  (SM_SCALAR)0.1,   // maxJumpDist, maxJumpAngle
          2000);         // lookahead time (ms)
   }
}
 
void ExitVU (void)
{
   delete (vuxDriverSettings);
   delete (gMainThread);
   delete [] vuxWorldName;
   vuxDriverSettings = NULL;
   gMainThread = NULL;
   F4DestroyCriticalSection(vuCritical);
   vuCritical = NULL;

	#ifdef USE_SH_POOLS
   VuLinkNode::ReleaseStorage();
   VuRBNode::ReleaseStorage();
   MemPoolFree( gVuMsgMemPool );
   MemPoolFree( gVuFilterMemPool );
	#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

VuEntity* VuxCreateEntity(ushort type, ushort size, VU_BYTE *data)
{
	VuEntity *retval = 0;
	VuEntityType* classPtr = VuxType(type);
	F4Assert(classPtr!= NULL);
	if (classPtr->classInfo_[VU_CLASS] == CLASS_VEHICLE)
	{
		retval = SimVUCreateVehicle (type, size, data);
	}
	else if (classPtr->classInfo_[VU_TYPE] == TYPE_EJECT)
	{
		retval = SimVUCreateVehicle (type, size, data);
	}
	else if (classPtr->classInfo_[VU_CLASS] == CLASS_FEATURE)
	{
		ShiWarning ("We shouldn't be creating features this way");
		retval = NULL;
	}
	else if (classPtr->classInfo_[VU_CLASS] == CLASS_UNIT)
	{
		retval = (VuEntity*) NewUnit (type, &data);
		// This is a valid creation call only if this entity is still owned by
		// the owner of our game
		if (!FalconLocalGame || FalconLocalGame->OwnerId() != retval->OwnerId())
		{
			VuReferenceEntity(retval);
			VuDeReferenceEntity(retval);
			retval = NULL;
		}
	}
	else if (classPtr->classInfo_[VU_CLASS] == CLASS_MANAGER)
	{
		retval = (VuEntity*) NewManager (type, data);
		// This is a valid creation call only if this entity is still owned by
		// the owner of our game
		if (!FalconLocalGame || FalconLocalGame->OwnerId() != retval->OwnerId())
		{
			VuReferenceEntity(retval);
			VuDeReferenceEntity(retval);
			retval = NULL;
		}
	}
	else if (classPtr->classInfo_[VU_CLASS] == CLASS_OBJECTIVE)
	{
		retval = (VuEntity*) NewObjective (type, &data);
		// This is a valid creation call only if this entity is still owned by
		// the owner of our game
		if (!FalconLocalGame || FalconLocalGame->OwnerId() != retval->OwnerId())
		{
			VuReferenceEntity(retval);
			VuDeReferenceEntity(retval);
			retval = NULL;
		}
	}
	else if (classPtr->classInfo_[VU_CLASS] == CLASS_SESSION)
	{
		retval = (VuEntity*) new FalconSessionEntity(&data);
		((VuSessionEntity*)retval)->SetKeepaliveTime (vuxRealTime);
	}
	else if (classPtr->classInfo_[VU_CLASS] == CLASS_GROUP)
	{
		retval = (VuEntity*) new FalconGameEntity(&data);			// FalconGroupEntity at some point..
	}
	else if (classPtr->classInfo_[VU_CLASS] == CLASS_GAME)
	{
		retval = (VuEntity*) new FalconGameEntity(&data);
	}
	else
	{
		// This is not a class table entry so to speak, but it is a ground spot
		//retval = (VuEntity*) new GroundSpotEntity(type);
		retval = (VuEntity*) new GroundSpotEntity(&data); // JB 010718
	}
	return retval;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

VuEntityType* VuxType(ushort id)
{
VuEntityType *retval = 0;

    F4Assert(id >= VU_LAST_ENTITY_TYPE && id - VU_LAST_ENTITY_TYPE < NumEntities);
	if (id >= VU_LAST_ENTITY_TYPE && id - VU_LAST_ENTITY_TYPE < NumEntities)
		retval = (VuEntityType*)&(Falcon4ClassTable[id - VU_LAST_ENTITY_TYPE]);
	
	return retval;
}

void VuxRetireEntity (VuEntity* theEntity)
{
   F4Assert(theEntity);
   MonoPrint ("You dropped an entity, better find it!!!\n");
}

VuSessionEntity* vuxCreateSession (void)
{
   return (VuSessionEntity*) new FalconSessionEntity(vuxLocalDomain,"Falcon 4.0");
}

// ======================================
// Functions
// ======================================

void VuEnterCriticalSection(void)	
{ 
	F4EnterCriticalSection(vuCritical); 
}

void VuExitCriticalSection(void)		
{ 
	F4LeaveCriticalSection(vuCritical);
}
bool VuHasCriticalSection()
{
	return F4CheckHasCriticalSection(vuCritical);
}

int CritialSectionCount(F4CSECTIONHANDLE* cs)
{
	return cs->count;
}

#endif
