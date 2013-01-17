#pragma	once

#include	"DxNodes.h"
#include	"DxDefines.h"
#include	"lodconvert.h"

#define	LODNUMBER(x)	((x-1)/2)

typedef	struct { double	x,y,z;} DOUBLEVECTOR ;

typedef	struct	{
		CString	Name;
		DWORD	Id;
		DWORD	Type;
} VisType;


// DOMAINS
#define	DOMAIN_UNKNOWN		0
#define	DOMAIN_ABSTRACT		1
#define	DOMAIN_AIR			2
#define	DOMAIN_LAND			3
#define	DOMAIN_SEA			4
#define	DOMAIN_SPACE		5
#define	DOMAIN_UNDERGROUND	6
#define	DOMAIN_UNDERSEA		7


// CLASSES
#define	CLASS_ABSTRACT		0
#define	CLASS_ANIMAL		1
#define	CLASS_FEATURE		2
#define	CLASS_MANAGER		3
#define	CLASS_OBJECTIVE		4
#define	CLASS_SFX			5
#define	CLASS_UNIT			6
#define	CLASS_VEHICLE		7
#define	CLASS_WEAPON		8
#define	CLASS_WEATHER		9
#define	CLASS_DIALOG		10
#define	CLASS_GAME			11
#define	CLASS_GROUP			12
#define	CLASS_SESSION		13



// * SWITCHES DEFINITIONS
#define SW_AB					0
#define SW_NOS_GEAR_SW			1
#define SW_LT_GEAR_SW			2
#define SW_RT_GEAR_SW			3
#define SW_NOS_GEAR_ROD			4
#define SW_CANOPY				5
#define SW_WING_VAPOR			6
#define SW_TAIL_STROBE			7
#define SW_NAV_LIGHTS			8
#define SW_LAND_LIGHTS          9
#define SW_EXH_NOZZLE           10
#define SW_TIRN_POD             11
#define SW_HTS_POD				12
#define SW_REFUEL_DR            13
#define SW_NOS_GEAR_DR_SW       14
#define SW_LT_GEAR_DR_SW        15
#define SW_RT_GEAR_DR_SW        16
#define SW_NOS_GEAR_HOLE        17
#define SW_LT_GEAR_HOLE         18
#define SW_RT_GEAR_HOLE         19
#define SW_BROKEN_NOS_GEAR_SW   20
#define SW_BROKEN_LT_GEAR_SW    21
#define SW_BROKEN_RT_GEAR_SW    22
#define SW_HOOK					23
#define SW_DRAGCHUTE            24


//===========================================
//===========================================
#define MAX_COLOR						4000 //1568
#define MAX_PALETTE					2000
#define MAX_TEXTURE					10000 //1316 1278 1290
#define MAX_PARENT					10000 //1361 1328 1342
#define	MAX_NODES					20000// Need to update
#define	MAX_LOD						16384
#define	HDR_LODNAMELEN				32
// 같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같� typedefs





// 같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같� typedefs

// 같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같� typedefs

typedef struct	taghdr// HDR header structure
{
	unsigned int version;
	unsigned int nColor;
	unsigned int nDarkColor;
	unsigned int nPalette;
	unsigned int nTexture;
	unsigned int unk[2];
	unsigned int nLOD;
	unsigned int nParent;
}	HDR;


typedef struct	taghdrcolor// HDR color
{
	float f[4];
} HDRCOLOR;

typedef struct	taghdrpalette// HDR palettes
{
	unsigned char r[256];
	unsigned char g[256];
	unsigned char b[256];
	unsigned char a[256];
	unsigned int uint[2];
}	HDRPALETTE;

typedef struct	taghdrlod// HDR LODs
{
	unsigned int uint[3];					
	unsigned int FileOffset;			// The Offset in the DB of the Model
	unsigned int FileSize;				// The Model Size
}	HDRLOD;

typedef struct	taghdrtexture// HDR Textures
{
	unsigned int uint[10];
}	HDRTEXTURE;

typedef struct	taghdrparent// HDR parents
{
	union	{
		float f[7];
		struct {float Radius, XMin, XMax, YMin, YMax, ZMin, ZMax; };
	};

	float RadarSignature;
	float IRSignature;

	union	{
		unsigned short int ushort1[2];
		struct { unsigned short int	TextureSets, Dynamics; };
	};
	
	union	{
		unsigned char uchar[4];
		struct	{ unsigned char nLODs, nSwitches, nDOFs, nSlots; };
	};
	unsigned short int ushort2[2];
}	HDRPARENT;

typedef struct	taghdrlodres// HDR LOD data (found at end of HDR file)
{
	char LODName[10][32];	
	int lodind[10];
	float distance[10];
	float slotX[100];
	float slotY[100];
	float slotZ[100];
	float dyn1[10];
	float dyn2[10];
	float dyn3[10];
}	HDRLODRES;

typedef struct tagct
{
	unsigned char domain;
	unsigned char cclass;
	unsigned char type;
	unsigned char subtype;
	unsigned char specific;
	unsigned char mode;
	unsigned short int parent[8];
	unsigned char ftype;
	unsigned int foffset;
	char name[22];
}	CT;





typedef	struct	{
	short	CT_Index;
	short	Repa_Time;
	short	Priority;
	short	Flags;
	char	Name[20];
	short	HitPoints;		
	short	Height;		
	int		Angle;		
	int		Rad_Type;
	char	DetbyStatic;		
	char	DetByFoot;		
	char	DetByWheeled;		
	char	DetByTracked;		
	char	DetByLowAir;		
	char	DetByAir;		
	char	DetByNaval;		
	char	DetByRail;		
	char	DamByNone;		
	char	DamByAP;		
	char	DamByHE;		
	char	DamByBomb;		
	char	DamByIncendiary;		
	char	DamByProximity;		
	char	DamByKinetic;		
	char	DamByWater;		
	char	DamByChemical;		
	char	DamByNuclear;		
	char	DamByOther;		
	char	Unused;		
} FCDRecordType;


typedef	struct	{
	short	CT_Index;
	short	HitPoints;
	int		flags;
	char	Name[15];
	char	nctr[5];
	float	radarcs;
	int		MaxTakeoffWeight;
	int		EmptyWeight;
	int		FuelLoad;
	short	FuelRate;
	short	EngineNoise;
	short	MaximumAltitude;		
	short	MinimumAltitude;		
	short	CruiseAltitude;		
	short	MaximumSpeed;		
	short	RCDIndex;		
	short	Crew;		
	short	RackFlags;		
	short	VisibilityFlags;		
	short	CallSign;		
	char	Tohit[8];
	char	Strength[8];
	char	Range[8];
	char	Detect[8];
	short	WeaponHPIndexText[16];
	char	WeaponHPAmmunition[16];
	char	Damage[11];
	char	Pad1;		
	char	Pad2;		
	char	Pad3;		
} VCDRecordType;


typedef	struct{
	short	CT_Index;
	short	Damage;
	short	ExplosionType;
	short	Unk1;
	short	Range;
	short	Flags;
	char	Name[16];
	int		Pad;
	char	HCvStatic;
	char	HCvFoot;		
	char	HCvWheeled;		
	char	HCvTracked;		
	char	HCvLowAir;		
	char	HCvAir;		
	char	HCvNaval;		
	char	HCvRail;		
	char	FireRate;		
	char	Rareity;		
	short	Guidance;
	short	Collective;		
	short	SWD_Index;
	short	Weight;		
	short	Drag;		
	short	BlastRadius;		
	short	SeekerRadar;		
	short	VehicleList;		
	short	MaximumAltitude;
} WCDRecordType;




#define	PARENT_CONSTANTS1            {"KC10",907},\
            {"KC135",906},\
            {"TNKR_BOOM1",2200},\
            {"TNKR_BOOM2",2201},\
            {"TNKR_BOOM3",2202},\
            {"TNKR_BOOM4",2203},\
            {"TNKR_BOOM5",2204},\
            {"IL78",1218},\
            {"KC130",887},\
            {"TNKR_DROGUE1",2205},\
            {"TNKR_DROGUE2",2206},\
            {"TNKR_DROGUE3",2207},\
            {"TNKR_DROGUE4",2208},\
            {"TNKR_DROGUE5",2209},\
            {"KC135BOOM",1221},\
            {"KC10BOOM",1215},\
            {"KCBOOM1",2210},\
            {"KCBOOM2",2211},\
            {"KCBOOM3",2212},\
            {"KCBOOM4",2213},\
            {"KCBOOM5",2214},\
            {"RDROGUE",1223},\
            {"RDROGUE1",2215},\
            {"RDROGUE2",2216},\
            {"RDROGUE3",2217},\
            {"RDROGUE4",2218},\
            {"RDROGUE5",2219},\
            {"KCDROGUE1",2220},\
            {"KCDROGUE2",2221},\
            {"KCDROGUE3",2222},\
            {"KCDROGUE4",2223},\
            {"KCDROGUE5",2224},\
            {"BRIDGE1",159},\
            {"A10",891},\
            {"F16C",1052},\
            {"B52G",1051},\
            {"M1A1",118},\
            {"MIG29",74},\
            {"A50",904},\
            {"AA10",881},\
            {"AA10C",882},\
            {"AA11",883},\
            {"AA2",884},\
            {"AA2R",931},\
            {"AA7",932},\
            {"AA7R",933},\
            {"AA8",934},\
            {"AA9",935},\
            {"AAV7A1",209},\
            {"AGM45",920},\
            {"AGM65B",936},\
            {"AGM65D",937},\
            {"AGM65G",938},\
            {"AGM84",939},\
            {"AGM88",940},\
            {"AIM120",886},\
            {"AIM54",941},\
            {"AIM7",887},\
            {"AIM9M",1054},\
            {"AS10",942},\
            {"AS14",943},\
            {"AS4",944},\
            {"AS6",945},\
            {"AS7",946},\
            {"AS9",947},\
            {"SCUDL",132},\
            {"BMP",211},\
            {"BRDM2",212},\
            {"CHAPARRAL",991},\
            {"DCLUBBLK",792},\
            {"DCNC02",793},\
            {"DCOOLT01",161},\
            {"DCTR03",146},\
            {"DDEPOT02",204},\
            {"DDEPOT01",900},\
            {"DDEPOT03",17},\
            {"DDOCK01",18},\
            {"DDOCK06",516},\
            {"DDUMP01",526},\
            {"DF02_01",527},\
            {"DF02_02",671},\
            {"DF02_03",794},\
            {"E3",1069},\
            {"EF111",896},\
            {"F117",889},\
            {"F14",897},\
            {"F15C",898},\
            {"F4",899},\
            {"F5E",901},\
            {"FB111",888},\
            {"HAWK",240},\
            {"IL76",903},\
            {"LAV25",222},\
            {"M113",226},\
            {"M163",990},\
            {"M2",244},\
            {"MIG19",908},\
            {"MIG21",909},\
            {"MIG23",910},\
            {"MIG25",911},\
            {"MK82",142},\
            {"MK84",959},\
            {"MLRS",880},\
            {"F18A",139},\
            {"AN24",929},\
            {"F18D",138},\
            {"HANGAR9",1009},\
            {"T62",136},\
            {"SA2R",833},\
			{"", -1}

			
#define	PARENT_CONSTANTS2 {"SA3R",834},\
            {"SA4R",835},\
            {"SA5R",836},\
            {"UB1957",70},\
            {"DF02_04",795},\
            {"DF02_05",796},\
            {"DHANGAR07",797},\
            {"DHANGAR08",798},\
            {"DHANGAR09",799},\
            {"DHOSP02",800},\
            {"DN01_01",801},\
            {"DN01_02",802},\
            {"DN01_03",803},\
            {"DN01_04",804},\
            {"DN01_05",805},\
            {"DOBLK09",806},\
            {"DPP01_01",807},\
            {"DPP01_02",808},\
            {"DPP01_03",809},\
            {"DPP01_04",810},\
            {"DPP01_05",811},\
            {"SA12",979},\
            {"SA13",962},\
            {"SA13L",133},\
            {"SA2",230},\
            {"SA4",234},\
            {"SA4L",235},\
            {"SA6",242},\
            {"SA6L",243},\
            {"SA8",238},\
            {"STINGER",957},\
            {"SU25",913},\
            {"SU27",914},\
            {"T55",137},\
            {"T72",877},\
            {"T80",878},\
            {"AEXPLOSION1",120},\
            {"BLU109B",965},\
            {"AN2",893},\
            {"AH64",892},\
            {"DURANDAL",923},\
            {"GBU10",953},\
            {"BARRACKS1",145},\
            {"TANKLG01",147},\
            {"HANGAR2",148},\
            {"R02_NPGA",149},\
            {"UB3257",71},\
            {"RPK500",72},\
            {"DPP02_01",812},\
            {"DPP02_02",813},\
            {"DPP02_03",814},\
            {"DPP03_01",815},\
            {"DPP03_02",816},\
            {"DPP03_03",817},\
            {"DPP04_01",818},\
            {"DR03_05",819},\
            {"DR03_06",820},\
            {"DSHED04",821},\
            {"DTANKSM01",822},\
            {"DTRNSFR01",823},\
            {"DUPTANK01",824},\
            {"DWAREHS02",825},\
            {"DWAREHS03",826},\
            {"DWAREHS05",827},\
            {"BLU27",922},\
            {"CBU",951},\
            {"MK82SnakeEye",958},\
            {"ALQ131",921},\
            {"300GAL",918},\
            {"370GAL",919},\
            {"600GAL",930},\
            {"CH47",895},\
            {"RECPOD",961},\
            {"LAU3A",924},\
            {"M109",225},\
            {"AGM65A",885},\
            {"AT6",948},\
            {"OH58D",912},\
            {"HANGAR8",144},\
            {"GBU28B",73},\
            {"FACTORY1L",75},\
            {"FACTORY1R",76},\
            {"FACTORY1_SHIP",77},\
            {"DWTRTWR01",828},\
            {"MAYDAY",269},\
            {"KOEX",271},\
            {"ARCH",273},\
            {"KORYO",275},\
            {"STOWER",277},\
            {"RYUGYONG",285},\
            {"DRYUGYONG",830},\
            {"M48",831},\
            {"SM240",832},\
            {"SA6R",837},\
            {"HAWKRADAR",838},\
            {"PATRIOTRAD",839},\
            {"KRAZ255BF",840},\
            {"OLY1",841},\
            {"OLY2",842},\
            {"OLY3",843},\
            {"OLY4",844},\
            {"OLY5",845},\
            {"PMJ1",846},\
            {"BRIDGE1_1D",1189},\
            {"AH1",829},\
            {"AH64D",3},\
            {"IL28",902},\
            {"KA50",905},\
            {"MD500",890},\
            {"MI24",119},\
            {"TU16N",915},\
            {"UH1H",916},\
            {"UH60L",917},\
            {"OFFICE7",121},\
            {"SINGLE_RACK",140},\
            {"TRIPLE_RACK",141},\
            {"PUFFY_CLOUD1",7},\
            {"WAREHOUSE1",560},\
            {"SMALL_CRATER1",143},\
            {"PMJ2",847},\
            {"DMZSFNC",848},\
            {"KIM2",849},\
            {"NBBRD1",850},\
            {"NBBRD2",851},\
            {"CLUBBLK",852},\
            {"RWYPATCH",853},\
            {"NAJIN",854},\
            {"OSA2",855},\
            {"WILDCAT",856},\
            {"HELLFIRE",857},\
            {"MK84AIR",858},\
            {"CSHIP1",859},\
            {"CSHIP2",860},\
            {"CSHIP3",861},\
            {"CSHIP4",862},\
            {"AC130",863},\
            {"F15E",864},\
            {"ZIL135",1053},\
            {"WEXPLOSION1",47},\
            {"TAXI_01",48},\
            {"AV8B",865},\
            {"PRC_MIG21",866},\
            {"CIS_MIG29",867},\
            {"CIS_SU27",868},\
            {"PRC_SU27",869},\
            {"TYPE85II",166},\
            {"VRCOCKPIT_MP",870},\
            {"2RAIL",871},\
            {"1RAIL",872},\
            {"MAVRACK",873},\
            {"MUH1",181},\
            {"UPTANK1",78},\
            {"SMOKESTACK1",80},\
            {"PARKINGLOT1",81},\
            {"HOUSE1",300},\
            {"DAPT01",301},\
            {"NUM380",151},\
            {"CUH1",874},\
            {"BRIDGE1_2D",1190},\
            {"RWYLIT",1191},\
            {"GRLINE",1192},\
            {"YRLINE",1193},\
            {"THR160",1194},\
            {"MNAVBEAC",1195},\
            {"F16PIT",1196},\
            {"F16PIT_MP",1197},\
            {"NUM01",1198},\
            {"LB02W",1199},\
            {"LB02",1200},\
            {"HOUSE5",171},\
            {"HOUSE4",170},\
            {"HOUSE3",169},\
            {"SHOP1",163},\
            {"HOUSE2",167},\
            {"APT1",158},\
            {"OFFICE2",1040},\
            {"TANKSM01",1034},\
            {"DOCK1",165},\
            {"REFINERY1",179},\
            {"RAIL1",180},\
            {"CITY_HALL",162},\
            {"RTOWER",178},\
            {"N01_01",172},\
            {"OFFICE1",173},\
            {"SHOP2",164},\
            {"FACTORY2",157},\
            {"WAREHOUSE2",117},\
            {"R02_OVER",152},\
            {"ADMIN1",150},\
            {"FAB1000LGB",966},\
            {"FAB1000HD",967},\
            {"FAB1000LD",968},\
            {"NUM270",153},\
            {"ADMIN02",154},\
            {"CTR01",155},\
            {"DEPO03",156},\
            {"R02OVER",160},\
            {"TAXIS02",993},\
            {"APT2",168},\
            {"CBU89",950},\
            {"N01_03",82},\
            {"N01_04",83},\
            {"PARKINGLOT2",84},\
            {"COOL_TWR1",85},\
            {"CONT_DOME1",86},\
            {"R03_NPGB",1006},\
            {"R03_PGA",1023},\
            {"R03_PGB",1030},\
            {"DARCH",1201},\
            {"DOLY1",1202},\
            {"DOLY2",1203},\
            {"DOLY3",1204},\
            {"DOLY4",1205},\
            {"DOLY5",1206},\
            {"DKORYO",1207},\
            {"DMAYDAY",1208},\
            {"DKIM2",1209},\
            {"DKOEX",1210},\
            {"LB01",1211},\
            {"LB03",1212},\
            {"LB04",1213},\
            {"POPMENU",1214},\
            {"BMP2",1216},\
            {"BMP3",1217},\
            {"DBMB",1036},\
            {"DBMBLW",1037},\
            {"DBMBRW",1038},\
            {"N01_05",88},\
            {"TRANSFORMER1",89},\
            {"R03_PGC",1035},\
            {"TAXIS03L",68},\
            {"ZU23",1219},\
            {"ZSU57_2",1220},\
            {"BM21",1222},\
            {"HTS",1224},\
			{"", -1}


#define	PARENT_CONSTANTS3			{"LANTIRN",1225},\
            {"NIKEL",1226},\
            {"RCKT1",1227},\
            {"FAB250LGB",969},\
            {"FAB250LD",970},\
            {"FAB250HD",927},\
            {"KRAZ255B",994},\
            {"SHED4",90},\
            {"WAREHOUSE5",91},\
            {"DMIG19FUS",92},\
            {"DMIG19LW",93},\
            {"DMIG19RW",94},\
            {"DMIG19TL",95},\
            {"DMIG21FUS",96},\
            {"R03OPGA",1228},\
            {"R03OPGD",1229},\
            {"OSANLIT",1230},\
            {"1RWYLIT",1231},\
            {"2RWYLIT",1232},\
            {"CF16FRN",1233},\
            {"CF16LST",1234},\
            {"CF16LWG",1235},\
            {"CF16MID",1236},\
            {"CF16NOS",1237},\
            {"CF16RST",1238},\
            {"AT3",210},\
            {"BTR80",213},\
            {"FROG7",214},\
            {"BR_TOP1_1",1073},\
            {"BR_TOP1_2",1081},\
            {"TAXIS03U",69},\
            {"F02_03",79},\
            {"CF16RWG",1239},\
            {"KIMPOLIT",1240},\
            {"SEOULLIT",1241},\
            {"SUNANLIT",1242},\
            {"SUNANLIT2",1243},\
            {"WONSANLIT",1244},\
            {"GENCARGOSHP",1245},\
            {"F5SHADOW",1246},\
            {"F14SHADOW",1247},\
            {"F15SHADOW",1248},\
            {"F18SHADOW",1249},\
            {"THP01",1250},\
            {"DMIG21LW",97},\
            {"DMIG21RW",98},\
            {"DMIG21TL",99},\
            {"R03_NPGC",87},\
            {"HNGR05",116},\
            {"R04AGA",456},\
            {"R04AGB",457},\
            {"THP02",1251},\
            {"THP02LR",1252},\
            {"THP02RL",1253},\
            {"THP03",1254},\
            {"THP05LR",1255},\
            {"THP05RL",1256},\
            {"THP08",1257},\
            {"THP11",1258},\
            {"THP12",1259},\
            {"THP14",1260},\
            {"THP14LR",1261},\
            {"THP14RL",1262},\
            {"THP15",1263},\
            {"THP16",1264},\
            {"THP16LR",1265},\
            {"THP16RL",1266},\
            {"THP18",1267},\
            {"THP19",1268},\
            {"PP01_01",174},\
            {"WTOWER1",175},\
            {"RADAR1",176},\
            {"RADAR2",177},\
            {"BUNKER2",182},\
            {"CHURCH1",183},\
            {"DEPOT1",184},\
            {"DEPOT2",185},\
            {"FACTORY3",186},\
            {"DOCK2",187},\
            {"HANGAR1",188},\
            {"HANGAR3",189},\
            {"HANGAR6",190},\
            {"HANGAR7",191},\
            {"N01_02",192},\
            {"HELIPAD1",193},\
            {"PIER1",194},\
            {"PPOLE1",195},\
            {"PTOWER1",196},\
            {"RTOWER2",197},\
            {"RADAR3",198},\
            {"RADAR4",199},\
            {"RADAR5",200},\
            {"RADAR6",201},\
            {"RADAR7",202},\
            {"RADAR8",203},\
            {"SHOP3",205},\
            {"2S19",206},\
            {"2S6",207},\
            {"BM24",208},\
            {"FROG7L",215},\
            {"FUELTRUCK",216},\
            {"HMVAVG",217},\
            {"THP20",1269},\
            {"THP20LR",1270},\
            {"GBU15",928},\
            {"CBU58",949},\
            {"PATRIOT",952},\
            {"GBU24",954},\
            {"GSH23",955},\
            {"MK20",956},\
            {"MK46",992},\
            {"NIKE",960},\
            {"GPU5",964},\
            {"LNTRN_N",971},\
            {"LNTRN_T",972},\
            {"AGM130",973},\
            {"AT2",974},\
            {"AT4",975},\
            {"VIKHR",976},\
            {"AT5",977},\
            {"SA14",978},\
            {"CHAPMIS",963},\
            {"PTK250",980},\
            {"RPK180",981},\
            {"TOWM1046",982},\
            {"M47",983},\
            {"AA8R",984},\
            {"CHAFF",986},\
            {"300GALW",987},\
            {"57MMROCK",988},\
            {"M198",989},\
            {"APT4",995},\
            {"HMVTOW",218},\
            {"HMMWV",219},\
            {"HMVCAR",220},\
            {"HMVAMB",221},\
            {"LAVADV",223},\
            {"LAVTOW",224},\
            {"M88",227},\
            {"M977F",228},\
            {"M977C",229},\
            {"SA2L",231},\
            {"SA3",232},\
            {"SA3L",233},\
            {"SA5",236},\
            {"SA5L",237},\
            {"SA8L",239},\
            {"HAWKL",241},\
            {"M2ADAT",875},\
            {"M2ADV",876},\
            {"ZSU23_4",879},\
            {"GBU12",925},\
            {"275ROCK",926},\
            {"APT5",996},\
            {"SHOP4",1007},\
            {"THP20RL",1271},\
            {"HOUSE6",997},\
            {"HOUSE7",998},\
            {"HOUSE8",999},\
            {"BARN1",1000},\
            {"BARN2",1001},\
            {"HOUSE9",1002},\
            {"HOUSE10",1003},\
            {"HOUSE11",1004},\
            {"TWNHALL1",1005},\
            {"SHOP5",1008},\
            {"HANGAR10",1010},\
            {"R02_PGC",1011},\
            {"R03_NPGA",1012},\
            {"TEMPLE1",1013},\
            {"SHOP6",1014},\
            {"SHED2",1015},\
            {"SHED3",1016},\
            {"WTOWER2",1017},\
            {"WTOWER3",1018},\
            {"SHRINE1",1019},\
            {"APT6",1020},\
            {"STABLE1",1021},\
            {"PARK1",1022},\
            {"TREE2",1024},\
            {"TREE3",1025},\
            {"TREE4",1026},\
            {"HOUSE12",1027},\
            {"OFFICE3",1028},\
            {"OFFICE4",1029},\
            {"OFFICE5",1031},\
            {"OFFICE6",1032},\
            {"DAPT02",302},\
            {"DCHALL01",303},\
            {"R04AGC",458},\
            {"THP21",1272},\
            {"THP23LR",1273},\
            {"THP23RL",1274},\
            {"OFFICE8",1033},\
            {"DBMBTL",1039},\
            {"VRCOCKPIT",1},\
            {"OFFICE9",1041},\
            {"OFFICE10",1042},\
            {"OFFICE11",1043},\
            {"OFFICE12",1044},\
            {"OFFICE13",1045},\
            {"OFFICE14",1046},\
            {"OFFICE15",1047},\
            {"OFFICE16",1048},\
            {"OFFICE17",1049},\
            {"LRAMP1",1087},\
            {"LSPAN1",1080},\
            {"LTOWER1",1072},\
            {"LRAMP2",1118},\
            {"LSPAN2",1101},\
            {"PILEBIG",1055},\
            {"PILESML",1056},\
            {"TEST",1057},\
            {"OFF_BLOCK",1058},\
            {"OFF_BLOCK2",1059},\
            {"OFF_BLOCK3",1060},\
            {"OFF_BLOCK4",1061},\
            {"OFF_BLOCK5",1062},\
            {"OFF_BLOCK6",1063},\
            {"OFF_BLOCK7",1064},\
            {"OFF_BLOCK8",1065},\
            {"OFF_BLOCK9",1068},\
            {"WAREHOUSE3",1070},\
            {"WAREHOUSE4",1071},\
            {"TVSTN1",1050},\
            {"HOTEL1",2},\
            {"R04BGA",459},\
            {"R04BGB",460},\
            {"R04BGC",461},\
            {"SM_BRIDGE3",19},\
            {"MFLAME_S",483},\
            {"SAM_LAUN",561},\
            {"MISS_FLAME",1067},\
            {"ENG_FIRE",1066},\
            {"SM_FACT1",10},\
            {"SM_FACT2",122},\
            {"SM_FACT3",8},\
            {"BARRACKS2",291},\
            {"4KRUNWAY",9},\
            {"FLARE1",6},\
            {"C01_02",570},\
            {"TANK_HULK",985},\
            {"R02_NPGB",12},\
            {"R02_NPGC",13},\
            {"R02_NPGD",14},\
            {"R02_PGA",15},\
            {"R02_PGB",16},\
            {"FFG",20},\
            {"ENFAC",21},\
            {"GENTANKER",22},\
            {"CV67",23},\
            {"CG47",24},\
            {"ULSAN",25},\
            {"OFFICE18",26},\
            {"OFFICE19",27},\
            {"OFFICE20",28},\
            {"OFFICE21",29},\
            {"OFFICE22",30},\
            {"OFFICE23",31},\
            {"OFFICE24",32},\
            {"OFFICE25",33},\
            {"N02_01",373},\
            {"DN02_01",374},\
            {"THP26",1275},\
            {"DHELI1BOD",101},\
            {"DHELI1TL",102},\
            {"DHELI1PT",103},\
            {"DCOMV",104},\
            {"DNTRUK1",105},\
            {"DNTRUK2",106},\
            {"DTRUCK1",107},\
            {"DTANK1",108},\
            {"DTANK2",109},\
            {"DTANK3",110},\
            {"DJEEP2",111},\
            {"DJEEP3",112},\
            {"DLNCHR",113},\
            {"RADAR9",114},\
            {"AMMO_DUMP1",115},\
            {"LSPAN4",1133},\
            {"LSPAN5",1147},\
            {"LSPAN6",1156},\
            {"LSPAN7_1",1180},\
            {"LSPAN6_2",1163},\
            {"DEBRIS1",123},\
            {"DEBRIS2",124},\
            {"DEBRIS3",125},\
            {"AEXPLOSION2",126},\
            {"CRATER2",127},\
            {"CRATER3",128},\
            {"CRATER4",129},\
            {"FIREBALL",130},\
            {"SCUD",131},\
            {"SA15L",134},\
            {"PATRIOTL",135},\
            {"N02_02",375},\
            {"DN02_02",376},\
            {"TAXIS04",462},\
            {"OFFICE26",34},\
            {"D1F16LW",35},\
            {"D1F16RW",36},\
            {"D1F16FUS",37},\
            {"D1F16NOS",38},\
            {"D2F16FUS",39},\
            {"D2F16STB",40},\
            {"D1MIG29L",41},\
            {"D1MIG29R",42},\
            {"D1MIG29F",43},\
            {"D1MIG29N",44},\
            {"KIROV",45},\
            {"DUST1",46},\
            {"R01_NPGA",49},\
            {"R01_NPGB",50},\
            {"R01_NPGC",51},\
            {"R01_PFLA",52},\
            {"R01_PGB",53},\
            {"R01_PGA",54},\
            {"R01_PFLB",55},\
            {"LIGHTS_1",56},\
            {"LIGHTS_2",57},\
            {"HANGAR11",58},\
            {"HANGAR12",59},\
            {"DHANGAR1",60},\
            {"DHANGAR2",61},\
            {"WTOWER4",62},\
            {"CTRL_TOWER2",63},\
            {"EJECT1",64},\
            {"EJECT2",65},\
            {"VASI_N",66},\
            {"VASI_F",67},\
            {"DHELI1ROT",100},\
            {"LANDL",463},\
            {"NUM160",464},\
            {"RV82A",465},\
            {"DMIG23RW",250},\
            {"DMIG23NOS",251},\
            {"DSU25FUS",252},\
            {"DSU25LW",253},\
            {"DSU25RW",254},\
            {"DSU25NOS",255},\
            {"DSU27FUS",256},\
            {"DSU27LW",257},\
            {"DSU27RW",258},\
            {"DSU27NOS",259},\
            {"DIL28NOS",260},\
            {"APT3",261},\
            {"PLOT_F01_04",262},\
            {"PLOT_F01_05",263},\
            {"APT7",264},\
            {"EJECT3",265},\
            {"FIRE1",266},\
            {"EJECT4",267},\
            {"CANOPY1",268},\
            {"DC01_02",270},\
            {"DC01_03",272},\
            {"DC01_04",274},\
            {"DC01_05",276},\
            {"DC01_06",278},\
            {"C01_07",279},\
            {"DC01_07",280},\
            {"C01_08",281},\
            {"DC01_08",282},\
            {"N02_03",377},\
            {"THP29",1276},\
            {"THP30",1277},\
            {"THP32",1278},\
            {"THP32LR",1279},\
            {"THP32RL",1280},\
            {"THP33",1281},\
            {"LTOWER2",1109},\
            {"LSPAN6_3",1168},\
            {"LTOWER1_W",1079},\
            {"LTOWER1_P",1076},\
            {"LTOWER1_N",1075},\
            {"LTOWER1_R",1077},\
            {"LSPAN1_W",1086},\
            {"LSPAN1_P",1084},\
            {"LSPAN1_N",1083},\
            {"LSPAN1_R",1085},\
            {"LRAMP1_W",1088},\
            {"LSPAN2_W",1108},\
            {"LSPAN2_P",1105},\
            {"LSPAN2_N",1104},\
            {"LSPAN2_R",1106},\
            {"LTOWER2_W",1117},\
            {"LTOWER2_P",1114},\
            {"LTOWER2_N",1113},\
            {"LTOWER2_R",1115},\
            {"LTOWER2_B",1111},\
            {"LRAMP2_W",1123},\
            {"LRAMP2_R",1121},\
            {"LSPAN3",1124},\
            {"LSPAN3_W",1132},\
            {"LSPAN3_P",1129},\
            {"LSPAN3_N",1128},\
            {"LSPAN3_R",1130},\
            {"LSPAN3_B",1126},\
            {"LSPAN4_W",1139},\
            {"LSPAN4_P",1137},\
            {"LSPAN4_N",1136},\
            {"LSPAN4_R",1138},\
            {"LSPAN5_W",1155},\
            {"LSPAN5_P",1152},\
            {"LSPAN5_N",1151},\
            {"LSPAN5_R",1153},\
            {"LSPAN5_B",1149},\
            {"LSPAN6_W",1162},\
            {"LSPAN6_R",1161},\
            {"LSPAN6_P",1160},\
            {"LSPAN6_N",1159},\
            {"LSPAN6_2W",1167},\
            {"LSPAN6_2R",1166},\
            {"LSPAN6_2P",1165},\
            {"LSPAN6_2N",1164},\
            {"LSPAN6_3W",1172},\
            {"LSPAN6_3R",1171},\
            {"LSPAN6_3P",1170},\
            {"LSPAN6_3N",1169},\
            {"LSPAN7_W",1188},\
            {"LSPAN7_R",1186},\
            {"LSPAN7_P",1185},\
            {"LSPAN7_N",1184},\
            {"LSPAN7_B",1182},\
            {"BRIDGE1_X2",1089},\
            {"BR_TOP1_X2A",1090},\
            {"BRIDGE1_X3",1095},\
            {"BR_TOP1_X3A",1096},\
            {"BR_TOP2_1A",1102},\
            {"BR_TOP2_2A",1110},\
            {"BR_TOP2_3A",1119},\
            {"BR_TOP3_1A",1125},\
            {"BR_TOP4_1A",1134},\
            {"BR_TOP4_X2A",1141},\
            {"BRIDGE4_X2",1140},\
            {"BR_TOP5_1A",1148},\
            {"BR_TOP6_1A",1157},\
            {"BRIDGE6_X2",1173},\
            {"BR_TOP6_X2A",1174},\
            {"BR_TOP7_1A",1181},\
            {"BR_TOP1_1D",1074},\
            {"BR_TOP1_1R",1078},\
            {"BR_TOP1_2D",1082},\
            {"BR_TOP2_1D",1103},\
            {"BR_TOP2_1R",1107},\
            {"BR_TOP2_2D",1112},\
            {"BR_TOP2_2R",1116},\
            {"BR_TOP2_3D",1120},\
            {"BR_TOP2_3R",1122},\
            {"BR_TOP3_1D",1127},\
            {"BR_TOP3_1R",1131},\
            {"BR_TOP4_1D",1135},\
            {"BR_TOP4_X2D",1142},\
            {"BR_TOP5_1D",1150},\
            {"BR_TOP5_1R",1154},\
            {"BR_TOP6_1D",1158},\
            {"BR_TOP6_X2D",1176},\
            {"BR_TOP7_1D",1183},\
            {"BR_TOP7_1R",1187},\
            {"BRIDGE1_X3B",1100},\
            {"BRIDGE1_X3N",1097},\
            {"BRIDGE1_X3W",1099},\
            {"BRIDGE4_X2B",1146},\
            {"BRIDGE4_X2N",1143},\
            {"BRIDGE4_X2P",1144},\
            {"BRIDGE4_X2W",1145},\
            {"BRIDGE6_X2B",1175},\
            {"BRIDGE6_X2N",1177},\
            {"BRIDGE6_X2P",1178},\
            {"BRIDGE6_X2W",1179},\
            {"BRIDGE1_X2B",1094},\
            {"BRIDGE1_X2N",1091},\
            {"BRIDGE1_X2P",1092},\
            {"BRIDGE1_X2W",1093},\
            {"BRIDGE1_X3P",1098},\
            {"DIL28FUS",245},\
            {"DIL28LW",246},\
            {"DIL28RW",247},\
            {"DMIG23FUS",248},\
            {"DMIG23LW",249},\
            {"C01_09",283},\
            {"TANKLG02",286},\
            {"DTANKLG02",287},\
            {"TANKLG03",288},\
            {"DTANKLG03",289},\
            {"TANKLG04",290},\
            {"CRACKER1",292},\
            {"DCRACKER1",293},\
            {"CRACKER2",294},\
            {"REL_VALVE1",295},\
            {"DREL_VALVE1",296},\
            {"DCRACKER2",297},\
            {"PROC_TANK1",298},\
            {"EJECT5",299},\
            {"DPROC_TANK1",284},\
            {"DCHURCH01",304},\
            {"DF01_01",305},\
            {"DF01_02",306},\
            {"DF01_03",307},\
            {"DF01_04",308},\
            {"DF01_05",309},\
            {"DOBLK01",310},\
            {"DOFF02",311},\
            {"DOFF03",312},\
            {"DOFF04",313},\
            {"DOFF05",314},\
            {"DOFF08",315},\
            {"R02_01",316},\
            {"R02_02",317},\
            {"R02_03",318},\
            {"R02_04",319},\
            {"R02_05",320},\
            {"R02_06",321},\
            {"R02_07",322},\
            {"R02_08",323},\
            {"DN02_03",378},\
            {"THP34",1282},\
            {"R02_09",324},\
            {"R02_10",325},\
            {"R02_11",326},\
            {"R02_12",327},\
            {"R02_13",328},\
            {"F05_01",329},\
            {"F05_02",330},\
            {"F05_03",331},\
            {"F05_04",332},\
            {"F05_05",333},\
            {"F05_06",334},\
            {"F05_07",335},\
            {"F05_08",336},\
            {"F05_09",337},\
            {"DR02_01",338},\
            {"DR02_02",339},\
            {"DR02_03",340},\
            {"DR02_04",341},\
            {"DR02_05",342},\
            {"DR02_06",343},\
            {"DR02_07",344},\
            {"DR02_08",345},\
            {"DR02_09",346},\
            {"DR02_10",347},\
            {"DR02_11",348},\
            {"DR02_12",349},\
            {"DR02_13",350},\
            {"DF05_01",351},\
            {"DF05_02",352},\
            {"DF05_03",353},\
            {"DF05_04",354},\
            {"DF05_05",355},\
            {"DF05_06",356},\
            {"DF05_07",357},\
            {"DF05_08",358},\
            {"DF05_09",359},\
            {"DTNHL01",360},\
            {"DTVSTN01",361},\
            {"DSMLMF01",362},\
            {"DSHRINE01",363},\
            {"DOCK06",364},\
            {"HOTEL02",365},\
            {"HOSPITAL01",366},\
            {"HOSPITAL02",367},\
            {"HOSPITAL03",368},\
            {"SHOPBLK01",369},\
            {"SHOPBLK02",370},\
            {"SHOPBLK03",371},\
            {"SHOPBLK04",372},\
            {"N02_04",379},\
            {"DN02_04",380},\
            {"N02_05",381},\
            {"DN02_05",382},\
            {"COOLT2",383},\
            {"DCOOLT2",384},\
            {"N03_01",385},\
            {"DN03_01",386},\
            {"N03_02",387},\
            {"DN03_02",388},\
            {"N03_03",389},\
            {"DN03_03",390},\
            {"N03_04",391},\
            {"DN03_04",392},\
            {"N03_05",393},\
            {"DN03_05",394},\
            {"N03_06",395},\
            {"DN03_06",396},\
            {"N04_01",397},\
            {"N04_02",398},\
            {"N04_03",399},\
            {"N04_04",400},\
            {"DN04_01",401},\
            {"DN04_02",402},\
            {"DN04_03",403},\
            {"DN04_04",404},\
            {"C02_01",405},\
            {"C02_02",406},\
            {"C02_03",407},\
            {"C02_04",408},\
            {"C02_05",409},\
            {"C02_06",410},\
            {"C02_07",411},\
            {"DC02_01",412},\
            {"DC02_02",413},\
            {"DC02_03",414},\
            {"DC02_04",415},\
            {"DC02_05",416},\
            {"DC02_06",417},\
            {"DC02_07",418},\
            {"LGTNK_05",419},\
            {"LGTNK_06",420},\
            {"LGTNK_07",421},\
            {"RTWR_03",422},\
            {"RTWR_04",423},\
            {"CMTWR_01",424},\
            {"SILO_01",425},\
            {"CMTWR_02",426},\
            {"C03_01",427},\
            {"PP02_01",428},\
            {"PP02_02",429},\
            {"PP02_03",430},\
            {"PP03_01",431},\
            {"PP03_02",432},\
            {"PP03_03",433},\
            {"PP04_01",434},\
            {"PTWR03",435},\
            {"PP01_04",436},\
            {"PP01_05",437},\
            {"PTWR02",438},\
            {"F03_01",439},\
            {"F03_02",440},\
            {"F03_03",441},\
            {"F03_04",442},\
            {"F03_05",443},\
            {"DF03_01",444},\
            {"DF03_02",445},\
            {"DF03_03",446},\
            {"DF03_04",447},\
            {"DF03_05",448},\
            {"R03_01",449},\
            {"R03_02",450},\
            {"R03_03",451},\
            {"R03_04",452},\
            {"R03_05",453},\
            {"R03_06",454},\
            {"R03_07",455},\
            {"RV82B",466},\
            {"RV82C",467},\
            {"TAXIS05",468},\
            {"CTR03",469},\
            {"TAXIS06",470},\
            {"R06RGA",471},\
            {"R06RGB",472},\
            {"R06RGC",473},\
            {"R06LGA",474},\
            {"R06LGB",475},\
            {"R06LGC",476},\
            {"FLAND",477},\
            {"BLDG01",478},\
            {"BLDG02",479},\
            {"BLDG03",480},\
            {"BLDG04",481},\
            {"BLDG05",482},\
            {"END_MISSION",4},\
            {"SHOP7",485},\
            {"SIX_RACK",486},\
            {"QUAD_RACK",487},\
            {"THP27",488},\
            {"THP09",489},\
            {"THA",490},\
            {"THP2R",491},\
            {"THC",492},\
            {"THD",493},\
            {"THE",494},\
            {"THF",495},\
            {"THG",496},\
            {"AB1TL1",497},\
            {"AB1TL2",498},\
            {"AB1TL3",499},\
            {"AB1TL4",500},\
            {"AB1TL5",501},\
            {"AB1TL7",502},\
            {"AB1TMD2",503},\
            {"AB1TMD3",504},\
            {"AB5TL1",505},\
            {"AB5TL2",506},\
            {"AB5TL3",507},\
            {"AB5TL4",508},\
			{"", -1}

			
#define	PARENT_CONSTANTS4 {"FLAND27",509},\
            {"AB5TL5",510},\
            {"AB6TMD2",511},\
            {"AB5THP1",512},\
            {"AB6TMD3",513},\
            {"AB6TMD4",514},\
            {"AB6TL3",515},\
            {"AB6UTMD2",517},\
            {"AB6UTMD3",518},\
            {"AB6UTL2",519},\
            {"AB8TMD1",520},\
            {"AB7TMD2",521},\
            {"AB5THP6",522},\
            {"AB7TMD3",523},\
            {"DEPOT2_F",524},\
            {"SHADOW",525},\
            {"CRANE1",528},\
            {"DAEWOO",529},\
            {"BUNKER3",530},\
            {"DOCK07",531},\
            {"FOOT_SQUAD",532},\
            {"ALLM4SQD",533},\
            {"DPRKATMISSQD",534},\
            {"PRCATMISSQD",535},\
            {"PRCARTSQD",536},\
            {"DPRKARTSQD",537},\
            {"ALLM16SQD",538},\
            {"USSAMSQUAD",539},\
            {"ROKSAMSQD",540},\
            {"CISSAMSQD",541},\
            {"DPRKSAMSQD",542},\
            {"USATMISSQD",543},\
            {"USARTSQD",544},\
            {"ROKATMISSQD",545},\
            {"CISATMISSQD",546},\
            {"ROKARTSQD",547},\
            {"CISARTSQD",548},\
            {"PRCSAMSQD",549},\
            {"ALLM60SQD",550},\
            {"USMORTSQD",551},\
            {"PRCMORTSQD",552},\
            {"DPRKMORTSQD",553},\
            {"ROKMORTSQD",554},\
            {"RES_INF_SQUAD",555},\
            {"GUYDIE",556},\
            {"DOWN_PILOT",557},\
            {"DOCK04",558},\
            {"DOCK05",559},\
            {"F16CBLU",5},\
            {"F16CGRN",562},\
            {"F16CORG",563},\
            {"F16CRED",564},\
            {"C01_03",565},\
            {"C01_04",566},\
            {"C01_05",567},\
            {"C01_06",568},\
            {"C01_10",569},\
            {"D1MIG29FBLU",571},\
            {"D1MIG29FGRN",572},\
            {"D1MIG29FORG",573},\
            {"D1MIG29FRED",574},\
            {"D1MIG29LBLU",575},\
            {"D1MIG29LGRN",576},\
            {"D1MIG29LRED",577},\
            {"D1MIG29LORG",578},\
            {"D1MIG29NBLU",579},\
            {"D1MIG29NGRN",580},\
            {"D1MIG29NORG",581},\
            {"D1MIG29NRED",582},\
            {"D1MIG29RBLU",583},\
            {"D1MIG29RGRN",584},\
            {"D1MIG29RORG",585},\
            {"D1MIG29RRED",586},\
            {"DMIG19FUSBLU",587},\
            {"DMIG19FUSGRN",588},\
            {"DMIG19FUSORG",589},\
            {"DMIG19FUSRED",590},\
            {"DMIG19LWBLU",591},\
            {"DMIG19LWGRN",592},\
            {"DMIG19LWORG",593},\
            {"DMIG19LWRED",594},\
            {"THP34LR",1283},\
            {"DMIG19RWBLU",595},\
            {"DMIG19RWGRN",596},\
            {"DMIG19RWORG",597},\
            {"DMIG19RWRED",598},\
            {"DMIG19TLBLU",599},\
            {"DMIG19TLGRN",600},\
            {"DMIG19TLORG",601},\
            {"DMIG19TLRED",602},\
            {"DMIG21FUSBLU",603},\
            {"DMIG21FUSGRN",604},\
            {"DMIG21FUSORG",605},\
            {"DMIG21FUSRED",606},\
            {"DMIG21LWBLU",607},\
            {"DMIG21LWGRN",608},\
            {"DMIG21LWORG",609},\
            {"DMIG21LWRED",610},\
            {"DMIG21RWBLU",611},\
            {"DMIG21RWGRN",612},\
            {"DMIG21RWORG",613},\
            {"DMIG21RWRED",614},\
            {"DMIG21TLBLU",615},\
            {"DMIG21TLGRN",616},\
            {"DMIG21TLORG",617},\
            {"DMIG21TLRED",618},\
            {"DMIG23FUSBLU",619},\
            {"DMIG23FUSGRN",620},\
            {"DMIG23FUSORG",621},\
            {"DMIG23FUSRED",622},\
            {"DMIG23LWBLU",623},\
            {"DMIG23LWGRN",624},\
            {"DMIG23LWORG",625},\
            {"DMIG23LWRED",626},\
            {"DMIG23RWBLU",627},\
            {"DMIG23RWGRN",628},\
            {"DMIG23RWORG",629},\
            {"DMIG23RWRED",630},\
            {"DMIG23NOSBLU",631},\
            {"DMIG23NOSGRN",632},\
            {"DMIG23NOSORG",633},\
            {"DMIG23NOSRED",634},\
            {"MIG29BLU",635},\
            {"MIG29ORG",636},\
            {"MIG29GRN",637},\
            {"MIG29RED",638},\
            {"MIG19BLU",639},\
            {"MIG19RED",640},\
            {"MIG19GRN",641},\
            {"MIG19ORG",642},\
            {"MIG21BLU",643},\
            {"MIG21GRN",644},\
            {"MIG21ORG",645},\
            {"MIG23BLU",646},\
            {"MIG23GRN",647},\
            {"MIG23ORG",648},\
            {"F14BLU",649},\
            {"F14GRN",650},\
            {"F14ORG",651},\
            {"F15CBLU",652},\
            {"F15CGRN",653},\
            {"F15CORG",654},\
            {"F15CRED",655},\
            {"F18ABLU",656},\
            {"F18AGRN",657},\
            {"F18AORG",658},\
            {"F18ARED",659},\
            {"F5BLU",660},\
            {"F5GRN",661},\
            {"F5ORG",662},\
            {"F5RED",663},\
            {"MIG25BLU",664},\
            {"MIG25GRN",665},\
            {"MIG25ORG",666},\
            {"F4BLU",667},\
            {"F4GRN",668},\
            {"F4ORG",669},\
            {"F4RED",670},\
            {"MFLAME_L",672},\
            {"NOTHING",0},\
            {"AB1TL8",675},\
            {"AB7TMD1",676},\
            {"THP",677},\
            {"THPX",678},\
            {"THB",679},\
            {"AB7TL1",680},\
            {"AB9TMD1",681},\
            {"AB9TMD2",682},\
            {"AB9TMD3",683},\
            {"AB9TMD4",684},\
            {"AB9TL1",685},\
            {"AB9TL2",686},\
            {"AB9TL4",687},\
            {"AB9TL3",688},\
            {"AB9TL6",689},\
            {"AB9TL5",690},\
            {"AB6UTMD1",691},\
            {"AB6TMD1",692},\
            {"R03PGD",693},\
            {"AB6TL1",694},\
            {"R06RGD",695},\
            {"D1F16NOSRED",697},\
            {"D1F16FUSRED",698},\
            {"D1F16LWRED",699},\
            {"DSU27RWBLU",732},\
            {"DSU27RWGRN",733},\
            {"DSU27RWRED",734},\
            {"DSU27RWORG",735},\
            {"DSU27NOSORG",736},\
            {"DSU27NOSRED",737},\
            {"DSU27NOSBLU",738},\
            {"DSU27NOSGRN",739},\
            {"SU27RED",740},\
            {"HWYTAX1",674},\
            {"MIG25RED",741},\
            {"F02_02",742},\
            {"F02_04",743},\
            {"F02_05",744},\
            {"WHSE06",745},\
            {"F02_01",746},\
            {"UGNDFACT",747},\
            {"FENCE1",748},\
            {"FENCE2",749},\
            {"FENCE3",750},\
            {"FENCE4",751},\
            {"PP01_02",752},\
            {"PP01_03",753},\
            {"F16_SHADOW",754},\
            {"F111_SHADOW",755},\
            {"F18_SHADOW",756},\
            {"KC10_SHADOW",757},\
            {"B52_SHADOW",758},\
            {"C130_SHADOW",759},\
            {"MIG21_SHADOW",760},\
            {"MIG19_SHADOW",761},\
            {"SU25_SHADOW",762},\
            {"F117_SHADOW",763},\
            {"AN2_SHADOW",764},\
            {"A10_SHADOW",765},\
            {"D1F16RWRED",700},\
            {"D1F16RWBLU",701},\
            {"D1F16LWBLU",702},\
            {"D1F16NOSBLU",703},\
            {"D1F16FUSBLU",704},\
            {"D1F16FUSGRN",705},\
            {"D1F16LWGRN",706},\
            {"D1F16RWGRN",707},\
            {"D1F16NOSGRN",708},\
            {"D1F16NOSORG",709},\
            {"D1F16FUSORG",710},\
            {"D1F16LWORG",711},\
            {"D1F16RWORG",712},\
            {"AB5TMD1",713},\
            {"AB5TMD2",714},\
            {"AB5TMD3",715},\
            {"MIG23RED",716},\
            {"MIG21RED",717},\
            {"F14RED",718},\
            {"SU27BLU",720},\
            {"SU27GRN",721},\
            {"SU27ORG",722},\
            {"AB8TMD2",723},\
            {"AB6UTL1",673},\
            {"DSU27FUSBLU",724},\
            {"DSU27FUSGRN",725},\
            {"DSU27FUSRED",726},\
            {"DSU27FUSORG",727},\
            {"DSU27LWBLU",728},\
            {"DSU27LWGRN",729},\
            {"DSU27LWRED",730},\
            {"DSU27LWORG",731},\
            {"THP34RL",1284},\
            {"IL28_SHADOW",766},\
            {"AH64_SHADOW",767},\
            {"UH1N_SHADOW",768},\
            {"CH47_SHADOW",769},\
            {"500MG_SHADOW",770},\
            {"OH58_SHADOW",771},\
            {"DUGNDFACT",772},\
            {"TAXIOSAN",773},\
            {"RWYDIST1",774},\
            {"RWYDIST2",775},\
            {"RWYDIST3",776},\
            {"RWYDIST4",777},\
            {"PILEREC01",778},\
            {"PILEREC02",779},\
            {"PILEREC03",780},\
            {"PILEREC04",781},\
            {"PILESQ01",782},\
            {"PILESQ02",783},\
            {"PILESQ03",784},\
            {"PILESQ04",785},\
            {"DADMIN01",786},\
            {"DADMIN02",787},\
            {"DBARR01",788},\
            {"DBARR02",789},\
            {"DC01_09",790},\
            {"DC03_01",791},\
            {"THP36",1285},\
            {"00CPORTLIT",1286},\
            {"66DPORTLIT",1287},\
            {"CF16A",1288},\
            {"731PORTLIT",1289},\
            {"795PORTLIT",1290},\
            {"79APORTLIT",1291},\
            {"79BPORTLIT",1292},\
            {"TU95",1330},\
            {"F22",1331},\
            {"LITEPOOL",1332},\
            {"WLOADER",1333},\
            {"APU",1334},\
            {"WTRAILER",1335},\
            {"EA6SH",1336},\
            {"TU95SH",1337},\
            {"F22SH",1338},\
            {"B1BSH",1339},\
            {"SFULTRK",1340},\
            {"SM977",1341},\
            {"79DPORTLIT",1293},\
            {"79EPORTLIT",1294},\
            {"7A0PORTLIT",1295},\
            {"7A1PORTLIT",1296},\
            {"7A2PORTLIT",1297},\
            {"7A5PORTLIT",1298},\
            {"7A6PORTLIT",1299},\
            {"TU16SH",1300},\
            {"BIRACK",1301},\
            {"HBIRACK",1302},\
            {"HTRIRACK",1303},\
            {"HONERACK",1304},\
            {"RTRIRACK",1305},\
            {"ONELAU3A",1306},\
            {"BILAU3A",1307},\
            {"TRILAU3A",1308},\
            {"DNAVBEAC",1309},\
            {"DTHP",1310},\
            {"DTHALPH",1311},\
            {"DEPOTONE",1312},\
            {"RTOWER01",1313},\
            {"MIG29SH",1314},\
            {"SU27SH",1315},\
            {"MIG23SH",1316},\
            {"MIG25SH",1317},\
            {"KA50SH",1318},\
            {"PCHUTESH",1319},\
            {"MISSILESH",1320},\
            {"LB01W",1321},\
            {"LB03W",1322},\
            {"LB04W",1323},\
            {"DSMSTK01",1324},\
            {"NUM23RW",1326},\
            {"NUM05LW",1327},\
            {"EA6",1328},\
            {"B1B",1329},\
            {"3DPIT_C2",2402},\
			{"", -1}



extern	char			CTNames[MAX_LOD][HDR_LODNAMELEN];
extern	CString			LODNames[MAX_LOD];
extern	HDR				hdr;
extern	HDRCOLOR		hdrColor[MAX_COLOR];
extern	HDRPALETTE		hdrPalette[MAX_PALETTE+2];
extern	HDRLOD			hdrLOD[MAX_LOD];
extern	HDRTEXTURE		hdrTexture[MAX_TEXTURE];
extern	HDRPARENT		hdrParent[MAX_PARENT+2];
extern	HDRLODRES		hdrLODRes[MAX_PARENT+2];

	int			LoadCT(CString KoreaObjFolder);
	bool		SaveHDR(CString, bool DXMode=true);
	short int	LoadHDR(CString, bool DXMode=true);
	int			LODUsedByCT(int, int*, int*, int*, int*, int*, int*, int*, CString*);
	DWORD	GetFileOffset(CString Name, HDRPARENT *Parent=NULL, DWORD *ParentNr=NULL);
	DWORD	GetFileSize(CString Name, HDRPARENT *Parent=NULL, DWORD *ParentNr=NULL);

	bool	InitDbaseIN(CString FileName);
	bool	InitDbaseOUT(CString FileName);
	DWORD	GetInPosition(void);
	DWORD	GetOutPosition(void);
	void	CloseDBaseIN(void);
	void	CloseDBaseOUT(void);
	void	*GetNextModel(void);
	void	WriteNextModel(void *Model);
	void	ClearModel(E_Object *Obj);
	void	DeIndexModel(void *Source, E_Object *Obj, bool KillStaticLights=false);
	void	E_CalculateFaceNormal(DXTriangle *Node);
	float	GetRadarSignature(D3DXVECTOR3 *UpLeft, D3DXVECTOR3 *DnRight);
	float	GetRadarSignature(E_Object *Obj);
	float	GetModelRadius(void *Source);
	void	GetModelHitBox(void *Source, D3DXVECTOR3 *UpLeft, D3DXVECTOR3 *DnRight);
	DWORD	GetModelVersion(DxDbHeader *Source);
	void	GetModelFeatures(void *Source, DWORD *nSwitches, DWORD *nDOFs, DWORD *nSlots, D3DXVECTOR3 *SlotsArray = NULL);
	DWORD	GetInSize(void);
	void	UpdateParentRecord(DWORD Id, DWORD *nSwitches, DWORD *nDOFs, DWORD *nSlots, float *Radius, bool Force=false, D3DXVECTOR3 *SlotsArray = NULL, D3DXVECTOR3 *HitBox = NULL);
	void	UpdateParentSignature(DWORD Id, float *RadarSignature, float *IRSignature, bool Force=false);
	void	ResetParentRecords();
	CString	GetIndexName(DWORD Index, HDRPARENT *Parent=NULL, DWORD *ParentNr=NULL);
	void	*GetModelAtOffset(DWORD Offset);
	CString	BuildIndexName(DWORD Index);
	
	DXNode	*E_SortModelTree(DXNode *Node);
	DXNode *E_SortModel(DXNode *Model);
	DWORD	GetNrTextureSets(DWORD Index);

	DWORD	GetModelTexture(DxDbHeader *Model, DWORD Index);
	DWORD	GetCTNumber(void) ;
	DWORD	GetCTNumber(CString Name);
	DWORD	GetLODNumber(void) ;
	DWORD	GetDomain(DWORD ParentNr);
	DWORD	GetClass(DWORD ParentNr);
	DWORD	GetType(DWORD ParentNr);
	DWORD	GetParentLodID(DWORD Parent, DWORD LodLevel);
	DWORD	GetParentOfCT(int CTNumber, DWORD ParentLevel);
	DWORD	GetFileOffset(DWORD ID);
	DWORD	GetLodIDFromName(CString Name);
	DWORD	GetParentUsingLOD(DWORD ParentStart, DWORD	LodNr);
	DWORD	GetParentTexSets(DWORD Parent);
	void	GetParentData(DWORD PARENT, HDRPARENT *ptr);

	DWORD	GetMaxDbTexture(void);
	void	SetMaxDbTexture(DWORD TexNr);
	DWORD	GetMaxDbParent(void);
	void	SetMaxDbParent(DWORD ParentNr);




	void	UpdateSinglePRecord(DWORD Parent, DWORD *nSwitches=NULL, DWORD *nDOFs=NULL, DWORD *nSlots=NULL, float *Radius=NULL, D3DXVECTOR3 *SlotsArray = NULL, D3DXVECTOR3 *HitBox = NULL);
	void	SetIndexName(DWORD Index, CString Name);
	void	SetParentLod(DWORD Parent, DWORD LodLevel, DWORD LodId, CString Name);
	void	SetParentBox(DWORD Parent, D3DVECTOR *UpLeft, D3DVECTOR *DnRight);
	void	SetParentSignatures(DWORD Parent, float *Radar, float *IR);
	void	SetParentDistance(DWORD Parent, DWORD LOD, float Distance);
	void	SetParentTexSets(DWORD Parent, DWORD TexSets);
	void	SetLodDistance(DWORD Id, float Distance);
	float	GetParentLodDistance(DWORD Parent, DWORD LodLevel);
	D3DXVECTOR3	GetParentSlots(DWORD LOD, DWORD SlotNr);



