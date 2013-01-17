#ifndef FALCENT_H
#define FALCENT_H

//#include "shi\shierror.h"
#include "dirtybits.h"
#include "mth.h"


// ================================
// Data types and defines
// ================================

typedef enum {	NoMove = 0,
				Foot,
				Wheeled,
				Tracked,
				LowAir,
				Air,
				Naval,
				Rail,
				MOVEMENT_TYPES } MoveType;

// Radar Modes
#define FEC_RADAR_OFF			0x00	   	// Radar always off
#define FEC_RADAR_SEARCH_100	0x01	   	// Search Radar - 100 % of the time (always on)
#define FEC_RADAR_SEARCH_1		0x02	   	// Search Sequence #1
#define FEC_RADAR_SEARCH_2		0x03	   	// Search Sequence #2
#define FEC_RADAR_SEARCH_3		0x04	   	// Search Sequence #3
#define FEC_RADAR_AQUIRE		0x05	   	// Aquire Mode (looking for a target)
#define FEC_RADAR_GUIDE			0x06	   	// Missile in flight. Death is imminent
#define FEC_RADAR_CHANGEMODE	0x07	   	// Missile in flight. Death is imminent
// Flags
#define FEC_HOLDSHORT			0x01		// Don't takeoff until a player attaches
#define FEC_PLAYERONLY			0x02		// This entity is only valid if under player control
#define FEC_HASPLAYERS			0x04		// One or more player is attached to this entity
#define FEC_REGENERATING		0x08		// This entity is undead.
#define FEC_PLAYER_ENTERING		0x10		// A player is soon to attach to this aircraft/flight
#define FEC_INVULNERABLE		0x20		// This thing can't be destroyed

// localFlags
#define FELF_ON_PLAYERS_GM_CONTACT_LIST      0x1 // This object is listed in the players Ground Radar target list // MLR 4/19/2004 - new!
#define FELF_ON_PLAYERS_GMT_CONTACT_LIST     0x2 // This object is listed in the players Ground Radar target list // MLR 4/19/2004 - new!
#define FELF_UPDATE_TARGET_LIST_TEMP         0x4 // temporary marker for UpdateTargetList()      
#define FELF_ADDED_DURING_SIMDRIVER_CYCLE	 0x8 // Object was created during SimDriver::Cycle() and did missed having EntityDriver() called (due to hash table)

// ================================
// FalcEntity class
// ================================



#endif
