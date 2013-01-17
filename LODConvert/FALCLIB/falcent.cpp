#include <string.h>
#include "f4vu.h"
#include "ClassTbl.h"
#include "sim\include\simbase.h"
#include "campbase.h"
#include "falcmesg.h"
#include "FalcSess.h"
#include "FalcEnt.h"
#include "Find.h"
#include "Campaign.h"
#include "MsgInc\CampDirtyDataMsg.h"
#include "MsgInc\SimDirtyDataMsg.h"
#include "package.h"
#include "gndunit.h"
#include "team.h"

extern int g_nFalcEntCheckDeltaTime;
extern bool g_bFalcEntMaxDeltaTime;
extern int g_nSPBandwidthHack;
// ==================================
// Falcon Entity functions
// ==================================

FalconEntity::FalconEntity(int type) : VuEntity ((unsigned short)type)
	{
	falconType = 0;
	falconFlags = 0;
	dirty_falcent = 0;
	dirty_classes = 0;
	dirty_score = 0;
	feLocalFlags = 0;
	}

FalconEntity::FalconEntity(VU_BYTE** stream) : VuEntity (stream)
	{
	dirty_falcent = 0;
	dirty_classes = 0;
	dirty_score = 0;
	memcpy (&falconType, *stream, sizeof (falconType));		*stream += sizeof (falconType);
	if (gCampDataVersion >= 32)
		{
		memcpy (&falconFlags, *stream, sizeof (uchar));		*stream += sizeof (uchar);
		}
	feLocalFlags = 0;
	}

FalconEntity::FalconEntity(FILE* filePtr) : VuEntity (filePtr)
	{
	dirty_falcent = 0;
	dirty_classes = 0;
	dirty_score = 0;
	fread (&falconType, sizeof (falconType), 1, filePtr);
	if (gCampDataVersion >= 32)
		fread (&falconFlags, sizeof (uchar), 1, filePtr);
	feLocalFlags = 0;
	}

FalconEntity::~FalconEntity(void)
	{
	ShiAssert(vuDatabase->Find(Id()) != this);
	}

int FalconEntity::Save(VU_BYTE** stream)
	{
	int saveSize = VuEntity::Save (stream);

	memcpy (*stream, &falconType, sizeof (falconType));		*stream += sizeof (falconType);		saveSize += sizeof (falconType);	
	memcpy (*stream, &falconFlags, sizeof (uchar));			*stream += sizeof (uchar);			saveSize += sizeof (uchar);
	return (saveSize);
	}

int FalconEntity::Save(FILE* filePtr)
	{
	int saveSize = VuEntity::Save (filePtr);

	fwrite (&falconType, sizeof (falconType), 1, filePtr);	saveSize += sizeof (falconType);
	fwrite (&falconFlags, sizeof (uchar), 1, filePtr);		saveSize += sizeof (uchar);
	return (saveSize);
	}

int FalconEntity::SaveSize(void)
	{
	return VuEntity::SaveSize() + sizeof (falconType) + sizeof (uchar);
	//   return VuEntity::SaveSize();
	}

uchar FalconEntity::GetDomain (void)
	{
	return Falcon4ClassTable[Type()-VU_LAST_ENTITY_TYPE].vuClassData.classInfo_[VU_DOMAIN];
	}

uchar* FalconEntity::GetDamageModifiers (void)
	{
	return DefaultDamageMods; 
	}

void FalconEntity::GetLocation (GridIndex* x, GridIndex* y)
	{
	vector		v;

	v.x = XPos();
	v.y = YPos();
	v.z = 0.0F;
	ConvertSimToGrid(&v,x,y);
	}

void FalconEntity::SetOwner (FalconSessionEntity* session)
	{
	// Set the owner to session
	share_.ownerId_ = session->OwnerId();
	}

void FalconEntity::SetOwner (VU_ID sessionId)
	{
	// Set the owner to session
	share_.ownerId_ = sessionId;
	}

void FalconEntity::DoFullUpdate (void)
	{
	VuEvent *event = new VuFullUpdateEvent(this, FalconLocalGame);
	event->RequestReliableTransmit ();
	VuMessageQueue::PostVuMessage(event);
	}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int FalconEntity::calc_dirty_bucket (void)
{
// 2002-04-21 MN When we have no dirty score yet, return "bucket" 0...
	if (!dirty_score)
	{
		return 0;
	}
	else if (dirty_score <= SEND_EVENTUALLY)
	{
		return 1;
	}
	else if (dirty_score <= SEND_SOMETIME)
	{
		return 2;
	}
	else if (dirty_score <= SEND_LATER)
	{
		return 3;
	}
	else if (dirty_score <= SEND_SOON)
	{
		return 4;
	}
	else if (dirty_score <= SEND_NOW)
	{
		return 5;
	}
	else if (dirty_score <= SEND_RELIABLE)
	{
		return 6;
	}
	else if (dirty_score <= SEND_OOB)
	{
		return 7;
	}
	else if (dirty_score > SEND_OOB)
	{
		return 8;
	}
	else
	{
		ShiAssert("This can't happen at all...");
		return 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FalconEntity::ClearDirty (void)
{
	int
		bin;

	VuEnterCriticalSection ();
	
	bin = calc_dirty_bucket ();

	dirty_classes = 0;
	dirty_score = 0;

	if (bin)
	{
//		MonoPrint ("DIRTY < %d %08x\n", bin, this);
		ShiAssert (DirtyBucket[bin]->Find (this));
		DirtyBucket[bin]->Remove (this);
//		ShiAssert (!DirtyBucket[bin]->Find (this));
	}

	VuExitCriticalSection();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FalconEntity::MakeDirty (Dirty_Class bits, Dirtyness score)
{
	int
		old_bin,
		new_bin;

	if (!IsLocal())
		return;

	if (VuState() != VU_MEM_ACTIVE)
		return;

	VuEnterCriticalSection ();
	
	old_bin = calc_dirty_bucket ();

	ShiAssert (bits);
	ShiAssert (score);

	if (bits & DIRTY_SIM_BASE)
	{
		if ((IsUnit ()) || (IsObjective ()))
		{
			ShiWarning ("Cannot make a unit or objective sim base dirty");
		}
	}
	else if (bits & DIRTY_FALCON_ENTITY)
	{
	}
	else // this has to be a campbase or team class
	{
		if ((!IsUnit ()) && (!IsObjective ()) && (!IsTeam ()))
		{
			ShiWarning ("Cannot make a sim object campaign base dirty");
		}
	}

	dirty_classes |= bits;

	if (dirty_score & SEND_RELIABLE && score & SEND_OOB || 
		dirty_score & SEND_OOB && score & SEND_RELIABLE ||
		dirty_score & SEND_RELIABLEANDOOB || score & SEND_RELIABLEANDOOB)
		dirty_score = 8;
	else if (dirty_score & SEND_OOB || score & SEND_OOB)
		dirty_score = 7;
	else if (dirty_score & SEND_RELIABLE || score & SEND_RELIABLE)
		dirty_score = 6;
	dirty_score = max(score,dirty_score);//me123

	new_bin = calc_dirty_bucket ();

	if (old_bin == new_bin)
	{
		// It just stays there
	}
	else
	{
		if (old_bin)
		{
			ShiAssert (DirtyBucket[old_bin]->Find (this));
			DirtyBucket[old_bin]->Remove (this);
//			ShiAssert (!DirtyBucket[old_bin-1]->Find (this));
		}

		if (new_bin)
		{
			ShiAssert (!DirtyBucket[new_bin]->Find (this));
			DirtyBucket[new_bin]->ForcedInsert (this);
//			ShiAssert (DirtyBucket[new_bin-1]->Find (this));
		}
	}

	VuExitCriticalSection();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int FalconEntity::EncodeDirty (unsigned char **stream)
{
	uchar
		*start;

	start = *stream;

//	MonoPrint ("SendDirty %08x%08x %08x\n", Id(), dirty_classes);

	*(short*)*stream = (short)dirty_classes;
	*stream += sizeof (short);

	if (dirty_classes & DIRTY_FALCON_ENTITY)
	{
		*(uchar *)(*stream) = falconFlags;
		*stream += sizeof (uchar);
	}

	if (dirty_classes & DIRTY_CAMPAIGN_BASE)
	{
		((CampBaseClass*)this)->WriteDirty (stream);
	}

	if (dirty_classes & DIRTY_OBJECTIVE)
	{
		((ObjectiveClass*)this)->WriteDirty (stream);
	}

	if (dirty_classes & DIRTY_UNIT)
	{
		((UnitClass*)this)->WriteDirty (stream);
	}

	if (dirty_classes & DIRTY_PACKAGE)
	{
		((PackageClass*)this)->WriteDirty (stream);
	}

	if (dirty_classes & DIRTY_SQUADRON)
	{
		((SquadronClass*)this)->WriteDirty (stream);
	}

	if (dirty_classes & DIRTY_FLIGHT)
	{
		((FlightClass*)this)->WriteDirty (stream);
	}

	if (dirty_classes & DIRTY_GROUND_UNIT)
	{
		((GroundUnitClass*)this)->WriteDirty (stream);
	}

	if (dirty_classes & DIRTY_BATTALION)
	{
		((BattalionClass*)this)->WriteDirty (stream);
	}

	if (dirty_classes & DIRTY_TEAM)
	{
		((TeamClass*)this)->WriteDirty (stream);
	}

	if (dirty_classes & DIRTY_SIM_BASE)
	{
		((SimBaseClass*)this)->WriteDirty (stream);
	}

//	MonoPrint ("  SIZE %d\n", *stream - start);

	return *stream - start;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int FalconEntity::DecodeDirty (unsigned char **stream)
{
	unsigned char
		*start;

	short
		bits;

	start = *stream;

	bits = *(short*)*stream;
	*stream += sizeof (short);

//	MonoPrint ("RecvDirty %08x%08x %08x\n", this->Id(), bits);

	if (bits & DIRTY_FALCON_ENTITY)
	{
		falconFlags = *(uchar *)(*stream);
		*stream += sizeof (uchar);
	}

	if (bits & DIRTY_CAMPAIGN_BASE)
	{
		((CampBaseClass*)this)->ReadDirty (stream);
	}

	if (bits & DIRTY_OBJECTIVE)
	{
		((ObjectiveClass*)this)->ReadDirty (stream);
	}

	if (bits & DIRTY_UNIT)
	{
		((UnitClass*)this)->ReadDirty (stream);
	}

	if (bits & DIRTY_PACKAGE)
	{
		((PackageClass*)this)->ReadDirty (stream);
	}

	if (bits & DIRTY_SQUADRON)
	{
		((SquadronClass*)this)->ReadDirty (stream);
	}

	if (bits & DIRTY_FLIGHT)
	{
		((FlightClass*)this)->ReadDirty (stream);
	}

	if (bits & DIRTY_GROUND_UNIT)
	{
		((GroundUnitClass*)this)->ReadDirty (stream);
	}

	if (bits & DIRTY_BATTALION)
	{
		((BattalionClass*)this)->ReadDirty (stream);
	}

	if (bits & DIRTY_TEAM)
	{
		((TeamClass*)this)->ReadDirty (stream);
	}

	if (bits & DIRTY_SIM_BASE)
	{
		((SimBaseClass*)this)->ReadDirty (stream);
	}

//	MonoPrint ("  SIZE %d\n", *stream - start);

	return *stream - start;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static int dirty_bandwidth_used = 0;
static int dirty_bandwidth_time = 0;
static int dirty_user_time = 0;
static int dirty_users = 1;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//me123 changed from static
static void use_dirty_bandwidth (int size)
{
	dirty_bandwidth_used += size * dirty_users;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
extern int MinClientBandwidth;
int get_dirty_bandwidth (void)
{
#pragma message ("LOOK AT ME!!! Bandwidth stuff in here???")
	int
		delta_time,
		time;

	time = GetTickCount ();
//me123t multi changes here
	if (dirty_user_time == 0 || time - dirty_user_time> 10000)	// every 10 seconds count the number of players
	{	
		dirty_user_time = time;
		dirty_users = 0;

		VuSessionsIterator iter(FalconLocalGame);
		VuSessionEntity*   sess;
		
		sess = iter.GetFirst();

		while (sess)
		{
			dirty_users ++;

			sess = iter.GetNext ();
		}

		dirty_users --; // don't count ourselves
	}
	if (dirty_bandwidth_time != 0)
	{
		/* Decay the bandwidth used wrt. time */

		delta_time = time - dirty_bandwidth_time;

		// don't know yet what's better - and no time to test, so let's do both configurable ;-)
		if (g_bFalcEntMaxDeltaTime)
		{
			delta_time = min(delta_time, g_nFalcEntCheckDeltaTime);
		}
		else if (delta_time > g_nFalcEntCheckDeltaTime)
		{
			delta_time = 0;
			dirty_bandwidth_time = time;
			dirty_bandwidth_used = 0;
			return 0;
		}

		if (delta_time > 100)
		{
			if (FalconLocalGame && FalconLocalGame->IsLocal())
			dirty_bandwidth_used -= delta_time * MinClientBandwidth * dirty_users / 10000;	// me123 // tenth of bandwidth
			else dirty_bandwidth_used -= delta_time * MinClientBandwidth  / 10000;	// me123 // tenth of bandwidth


			if (dirty_bandwidth_used < 0)
			{
				dirty_bandwidth_used = 0;
			}
	
			dirty_bandwidth_time = time;
		}
	}
	else
	{
		dirty_bandwidth_time = time;

		dirty_bandwidth_used = 0;
	}

	// MLR
	if(!dirty_users && g_nSPBandwidthHack)
		return g_nSPBandwidthHack;
	if (FalconLocalGame && FalconLocalGame->IsLocal())
	return MinClientBandwidth * dirty_users / 10 - dirty_bandwidth_used;//me123 
	else return MinClientBandwidth  / 10 - dirty_bandwidth_used;//me123 
}
		
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

extern "C"
{
	int check_bandwidth (int);
	int check_rudp_bandwidth (int);
}

void FalconEntity::DoCampaignDirtyData (void)
{
    FalconEntity
	*best;
    
    CampDirtyData
	*camp_dirty_msg;
    
    int
	bucket,
	total_size,
	tstsize,
	size;
    
    static unsigned char
	buffer[1024];
    
    unsigned char
	*bufptr,
	*tstptr;
    
    
    F4EnterCriticalSection(campCritical);
    VuEnterCriticalSection ();
    
    total_size = get_dirty_bandwidth ();
    
    size = gMainThread->BytesPending ();
    
    total_size -= size;
    
    // JPO - some restructuring to get rid of the new call.
    // this accounted for 99% of the calls to new over time.
    // hmm - actually wrong routine... next one really... but do this one too
    bucket = MAX_DIRTY_BUCKETS-1;
    if (total_size > 0 && (check_bandwidth (total_size) || bucket == 7 || bucket == 8) && DirtyBucket[bucket])
    {
	// MonoPrint ("Dirty Size %d\n", total_size);
	for(; bucket > 0 && total_size > 0; bucket --)  {
	    VuListIterator dirty_iter(DirtyBucket[bucket]);
	    best = (FalconEntity *) dirty_iter.GetFirst ();
	    while (total_size > 0 && best)
	    {
		if (!(best->dirty_classes & DIRTY_SIM_BASE))
		{
		    // Only encode the campaign dirty stuff on this thread
		    
		    bufptr = buffer;
		    tstptr = buffer;
		    
		    size = best->EncodeDirty (&bufptr);
		    tstsize = best->DecodeDirty (&tstptr);
		    
		    assert (size > 0);
		    assert (size == tstsize);
		    
		    ShiAssert (size < 1024);
		    
		    best->dirty_classes = 0;
		    best->dirty_score = 0;
		    
		    // MonoPrint (" DIRTY < %d %08x\n", bucket, best);
		    
		    ShiAssert (DirtyBucket[bucket]->Find (best));
		    DirtyBucket[bucket]->Remove (best);
		    ShiAssert (!DirtyBucket[bucket]->Find (best));
		    
		    // Send the data
		    camp_dirty_msg = new CampDirtyData (best->Id(), FalconLocalGame, FALSE);
		    camp_dirty_msg->dataBlock.size = size;
		    camp_dirty_msg->dataBlock.data = new unsigned char[size];
		    
		    memcpy (camp_dirty_msg->dataBlock.data, buffer, size);
		    
		    total_size -= size;
		    use_dirty_bandwidth (size);
			
			if (bucket == 7 || bucket == 8)
			{
			camp_dirty_msg->RequestOutOfBandTransmit ();
			}
		    if (1 || bucket == 6 || bucket == 8)
		    FalconSendMessage(camp_dirty_msg,TRUE);
			else FalconSendMessage(camp_dirty_msg,FALSE);
		}
		best = (FalconEntity *) dirty_iter.GetNext ();
	    }
	}
    }

    VuExitCriticalSection();
    F4LeaveCriticalSection(campCritical);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FalconEntity::DoSimDirtyData (void)
{
    FalconEntity
	*best;
    
    SimDirtyData
	*sim_dirty_msg;
    
    int
	bucket,
	total_size,
	tstsize,
	size;
    
    static unsigned char
	buffer[1024];
    
    unsigned char
	*bufptr,
	*tstptr;
    
    
    //	F4EnterCriticalSection(campCritical);
    //	VuEnterCriticalSection ();
    
    bucket = MAX_DIRTY_BUCKETS-1;
    
    //	 MonoPrint ("DoDirty\n");
    
    // JPO - some restructuring to get rid of the new call.
    // this accounted for 99% of the calls to new over time.
    if (DirtyBucket[bucket])
    {
	total_size = get_dirty_bandwidth () * 2;
	
	size = gMainThread->BytesPending ();
	
	if (size > 1)
	{
	    // MonoPrint ("Pending Size %d\n", size);
	}
	
	total_size -= size;
	
	//		 MonoPrint ("Total size = %d\n", total_size);
	for(; bucket > 0 && total_size > 0; bucket --)  {
	    VuListIterator dirty_iter(DirtyBucket[bucket]);
	    best = (FalconEntity *) dirty_iter.GetFirst ();

	    while (best && (total_size > 0))
	    {
		if ((check_bandwidth (24)) || bucket == 7 || bucket == 8)	// 24 is average size of sim dirty data
		{
		    if (best->dirty_classes & DIRTY_SIM_BASE)
		    {
			// Only encode the sim base'd dirty stuff on this thread
			
			bufptr = buffer;
			tstptr = buffer;
			
			size = best->EncodeDirty (&bufptr);
			tstsize = best->DecodeDirty (&tstptr);
			
			assert (size > 0);
			assert (size == tstsize);
			
			ShiAssert (size < 1024);
			
			//						MonoPrint ("DIRTY < %d %08x %08x %08x %d\n", bucket, best, best->dirty_classes, best->dirty_score, total_size);
			
			best->dirty_classes = 0;
			best->dirty_score = 0;
			
			ShiAssert (DirtyBucket[bucket]->Find (best));
			DirtyBucket[bucket]->Remove (best);
			ShiAssert (!DirtyBucket[bucket]->Find (best));
			
			// Send the data
			sim_dirty_msg = new SimDirtyData (best->Id(), FalconLocalGame, FALSE);
			sim_dirty_msg->dataBlock.size = size;
			sim_dirty_msg->dataBlock.data = new unsigned char[size];
			
			memcpy (sim_dirty_msg->dataBlock.data, buffer, size);
			
			total_size -= size;
			use_dirty_bandwidth (size);
			
			if (bucket == 7 || bucket == 8)
			{
			sim_dirty_msg->RequestOutOfBandTransmit ();
			}
		    if (1 ||bucket == 6 || bucket == 8)
		    FalconSendMessage(sim_dirty_msg,TRUE);
			else FalconSendMessage(sim_dirty_msg,FALSE);
			}

		    
		}
		else
		{
		    total_size = 0;
		}
		best = (FalconEntity *) dirty_iter.GetNext ();
	    }
	}
    }
    //	VuExitCriticalSection();
    //	F4LeaveCriticalSection(campCritical);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int FalconEntity::GetRadarType(void)
{
	return RDR_NO_RADAR;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FalconEntity::MakeFlagsDirty (void)
{
	MakeFalconEntityDirty (DIRTY_FALCON_FLAGS, DDP[148].priority);
//		MakeFalconEntityDirty (DIRTY_FALCON_FLAGS, SEND_RELIABLEANDOOB);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FalconEntity::MakeFalconEntityDirty (Dirty_Falcon_Entity bits, Dirtyness score)
{
	if (!IsLocal())
		return;

	if (VuState() != VU_MEM_ACTIVE)
		return;

	dirty_falcent |= bits;

	MakeDirty (DIRTY_FALCON_ENTITY, score);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
