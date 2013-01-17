//**************** DEFAULT MATERIAL AT CONVERSION PHASE ***********************
#define	DEFAULT_MAT_DIFFUSE		0xffffffff
#define	DEFAULT_MAT_SPECULAR	0xff404040
#define	DEFAULT_MAT_LOSPECULAR	0xff202020
#define	DEFAULT_MAT_DVPOWER		6.8f


// EXHAUST NOZZLE SETTINGS 
#define	EXHAUST_NOZZLE_SWITCH			10
#define	EXHAUST_NOZZLE_DOF				39
#define	EXHAUST_NOZZLE_SPECULARITY		0xe0D8D0b0
#define	EXHAUST_NOZZLE_INDEX			20.0f


// CANOPY SETTINGS 
#define	CANOPY_SWITCH					5
#define	CANOPY_DOF						30
#define	CANOPY_SPECULARITY				0xffffffff
#define	CANOPY_INDEX					20.0f


// LEFS
#define	LT_LEF_DOF						9
#define	RT_LEF_DOF						10
#define	LEF_SPECULARITY					0xe0505050
#define	LEF_INDEX						8.0f

// TEFS
#define	LT_TEF_DOF						28
#define	RT_TEF_DOF						29
#define	TEF_SPECULARITY					0xe0505050
#define	TEF_INDEX						8.0f

// FLAPS
#define	LT_FLAP_DOF						02
#define	RT_FLAP_DOF						03
#define	FLAP_SPECULARITY				0xe0484848
#define	FLAP_INDEX						8.0f

// AIRBRAKES
#define	LT_AB_TOP_DOF						15
#define	LT_AB_BOT_DOF						16
#define	RT_AB_TOP_DOF						17
#define	RT_AB_BOT_DOF						18
#define	AIRBRAKE_SPECULARITY				0xe0505050
#define	AIRBRAKE_INDEX						8.0f


// LANDING LIGHTS
#define	NAV_LIGHT_SWITCH				8
#define	LANDING_LIGHT_SWITCH			9
#define	LANDING_LIGHT_ALPHA_CX			0.85f

DWORD	AssignSurfaceFeatures(BYTE*Model, DWORD CTNumber);

#define	ASSIGN_SURFACE_FEATURE(Condition, Name)	if((Condition)){\
														NODE.SURFACE->SpecularIndex=Name##_INDEX;\
														DWORD	nV=NODE.SURFACE->dwVCount;\
														Int16	*Vp=VList;\
														while(nV--)	(VPool+(*Vp++))->dwSpecular=Name##_SPECULARITY;\
													}
