
#ifndef FALCSESS_H
#define FALCSESS_H

#include "tchar.h"
#include "FalcGame.h"

// ==========================================
// Fly states
// ==========================================

#define FLYSTATE_IN_UI				0					// Sitting in the UI
#define	FLYSTATE_LOADING			1					// Loading the sim data
#define FLYSTATE_WAITING			2					// Waiting for other players
#define FLYSTATE_FLYING				3					// Flying
#define FLYSTATE_DEAD				4					// We're now dead, waiting for respawn

// ==========================================
// Other defines
// ==========================================

#define FS_MAXBLK					320					// Maximum number of data blocks

// ==========================================
// Externs
// ==========================================

extern uchar GetTeam(uchar country);

class SquadronClass;
class FlightClass;
class FalconEntity;

// ==========================================
// Class shit
// ==========================================

class FalconSessionEntity : public VuSessionEntity
	{
	public:

		enum
		{
			_AIR_KILLS_=0,
			_GROUND_KILLS_,
			_NAVAL_KILLS_,
			_STATIC_KILLS_,
			_KILL_CATS_,

			_VS_AI_		= 1,
			_VS_HUMAN_	= 2,
		};

	private:
		_TCHAR			*name;
		_TCHAR			*callSign;
		VU_ID			playerSquadron;
		VU_ID			playerFlight;
		VU_ID			playerEntity;
		SquadronClass	*playerSquadronPtr;
		FlightClass		*playerFlightPtr;
		VuEntity		*playerEntityPtr;
		float			AceFactor;			// Player Ace Factor
		float			initAceFactor;		// AceFactor at beginning of match
		float			bubbleRatio;		// This session's multiplier for the player bubble size
		ushort			kills[_KILL_CATS_]; // Player kills - can't keep in log book
		ushort			missions;			// Player missions flown
		uchar			country;			// Country or Team player is on
		uchar			aircraftNum;		// Which aircraft in a flight we're using (0-3)
		uchar			pilotSlot;			// Which pilot slot we've been assigned to
		uchar			flyState;			// What we're doing (sitting in UI, waiting to load, flying)
		short			reqCompression;		// Requested compression rate
		ulong			latency;			// Current latency estimate for this session
		uchar			samples;			// # of samples in current latency calculation
		uchar			rating;				// Player rating (averaged)
		uchar			voiceID;			// Player's voice of choice

		uchar			assignedAircraftNum;
		uchar			assignedPilotSlot;
		FlightClass		*assignedPlayerFlightPtr;

	public:
		uchar*			unitDataSendBuffer;		// Unit data the local session is sending to this session
		short			unitDataSendSet;
		long			unitDataSendSize; 
		uchar*			unitDataReceiveBuffer;	// Unit data the local session is receiving from this session
		short			unitDataReceiveSet;
		uchar			unitDataReceived[FS_MAXBLK/8];
		uchar*			objDataSendBuffer;		// Objective data the local session is sending to this session
		short			objDataSendSet;
		long			objDataSendSize; 
		uchar*			objDataReceiveBuffer;	// Objective data the local session is receiving from this session
		short			objDataReceiveSet;
		uchar			objDataReceived[FS_MAXBLK/8];
		
	public:
		// constructors & destructor
		FalconSessionEntity(ulong domainMask, char *callsign);
		FalconSessionEntity(VU_BYTE **stream);
		FalconSessionEntity(FILE *file);
		virtual VU_ERRCODE InsertionCallback(void);
		virtual ~FalconSessionEntity();

		// encoders
		virtual int SaveSize();
		virtual int Save(VU_BYTE **stream);
		virtual int Save(FILE *file);
		void DoFullUpdate(void);

		// accessors
		_TCHAR* GetPlayerName (void)		{ return name; }
		_TCHAR*	GetPlayerCallsign (void)	{ return callSign; }
		VU_ID GetPlayerSquadronID (void)	{ return playerSquadron; }
		VU_ID GetPlayerFlightID (void)		{ return playerFlight; }
		VU_ID GetPlayerEntityID (void)		{ return playerEntity; }
		VuEntity* GetPlayerEntity (void)	{ return playerEntityPtr; }
		FlightClass* GetPlayerFlight (void)	{ return playerFlightPtr; }
		SquadronClass* GetPlayerSquadron (void)	{ return playerSquadronPtr; }
		float GetAceFactor ()				{ return(AceFactor); }
		float GetInitAceFactor ()			{ return(initAceFactor); }
		uchar GetTeam (void);
		uchar GetCountry (void)				{ return country; }
		uchar GetAircraftNum (void)			{ return aircraftNum; }
		uchar GetPilotSlot (void)			{ return pilotSlot; }
		uchar GetFlyState (void)			{ return flyState; }
		short GetReqCompression (void)		{ return reqCompression; }
		float GetBubbleRatio (void)			{ return bubbleRatio; }
		ushort GetKill(ushort CAT)			{ if(CAT < _KILL_CATS_) return(kills[CAT]); return(0); }
		ushort GetMissions()				{ return missions; }
		uchar GetRating()					{ return rating; }
		uchar GetVoiceID()					{ return voiceID; }
		FalconGameEntity* GetGame (void)	{ return (FalconGameEntity*) game_; }

		uchar GetAssignedAircraftNum (void)			{ return assignedAircraftNum; }
		uchar GetAssignedPilotSlot (void)			{ return assignedPilotSlot; }
		FlightClass* GetAssignedPlayerFlight (void)	{ return assignedPlayerFlightPtr; }

		// setters
		void SetPlayerName (_TCHAR* pname);
		void SetPlayerCallsign (_TCHAR* pcallsign);

		void SetPlayerSquadron (SquadronClass* ent);		
		void SetPlayerSquadronID (VU_ID id);		
		void SetPlayerFlight (FlightClass* ent);		
		void SetPlayerFlightID (VU_ID id);		
		void SetPlayerEntity (VuEntity* ent);			
		void SetPlayerEntityID (VU_ID id);			
		void UpdatePlayer (void);

		void SetAceFactor (float af);
		void SetInitAceFactor (float af);
		void SetCountry (uchar c);
		void SetAircraftNum (uchar an);
		void SetPilotSlot (uchar ps);
		void SetFlyState (uchar fs);
		void SetReqCompression (short rc);
		void SetVuStateAccess (uchar state)	{ SetVuState(state); }
		void SetBubbleRatio (float ratio)	{ bubbleRatio = ratio;}
		void SetKill(ushort CAT,ushort kill){ if(CAT < _KILL_CATS_) kills[CAT]=kill; }
		void SetMissions(ushort count)		{ missions = count; }
		void SetRating(uchar newrat)		{ rating = newrat; }
		void SetVoiceID (uchar v)			{ voiceID = v; }

		void SetAssignedAircraftNum (uchar an);
		void SetAssignedPilotSlot (uchar ps);
		void SetAssignedPlayerFlight (FlightClass* ent);		

		// Calculate new ace factor
		void SetAceFactorKill(float opponent);
		void SetAceFactorDeath(float opponent);
		// queries
		int InSessionBubble (FalconEntity* ent, float bubble_multiplier = 1.0F);
		
		// event Handlers
//		virtual VU_ERRCODE Handle(VuEvent *event);
		virtual VU_ERRCODE Handle(VuFullUpdateEvent *event);
//		virtual VU_ERRCODE Handle(VuSessionEvent *event);
		};

// Some conversion equivalencies between vuLocalSession and FalconLocalSession
#define FalconLocalSession ((FalconSessionEntity*)vuLocalSessionEntity)
#define FalconLocalSessionId vuLocalSession
#define FalconLocalGame (((FalconSessionEntity*)vuLocalSessionEntity)->GetGame())
#define VuLocalGame (vuLocalSessionEntity->Game())
#define FalconLocalGameId (vuLocalSessionEntity->GameId())

#endif

