/***************************************************************************\
DrawParticleSys.h
MLR

\***************************************************************************/
#ifndef _DRAWPARTICLESYS_H_
#define _DRAWPARTICLESYS_H_

#include "DrawObj.h"
#include "context.h"
#include "Tex.h"
#include "falclib/include/alist.h"

#include "context.h"

#define	PARTICLE_NAMES_LEN	32

enum PSType // it's critical that this, and the name array in the cpp are in sync
{
	PST_NONE            = 0,
	PST_SPARKS          = 1,
	PST_EXPLOSION_SMALL	= 2,
	PST_NAPALM			= 3
};

// Used to draw segmented trails (like missile trails)

class ParticleParam;
class ParticleNode;
class ParticleTextureNode;

class DPS_Node : public ANode
{   // this is used to track particle objects for various internal needs
public:
	class DrawableParticleSys *owner;
};


//*********************************************************************************************************
// COBRA - RED - The Animations List pointer
// This class is used to keep track of a sequence of animated frames
class ParticleAnimationNode : public ANode
{
public:
	char	AnimationName[PARTICLE_NAMES_LEN];				// Name of the Sequence
	int		NFrames;										// frames of the Sequence
	float	Fps;											// Frames Per second Playing speed
	void	*Sequence;										// pointer to the Frame List pointers in memory
	int		Flags;

	GLint Run(int	&Frame, float &TimeRest, float Elapsed, Tpoint &pos, float &alpha);

};

#define	ANIMATION_FLAGS "LUD"								// COBRA - RED - Animations Flag
//		L	=	Loop Animation
//		U	=	UP View Full Animation				
//		D	=	DOWN View Full Animation				

#define	ANIM_LOOP		0x01
#define	ANIM_UPVIEW		0x02
#define	ANIM_DNVIEW		0x04
//*********************************************************************************************************


class DrawableParticleSys : public DrawableObject 
{
public:
	DrawableParticleSys( int ParticleSysType, float scale = 1.0f );
	virtual ~DrawableParticleSys();

	void ReleaseToSfx(); // gives object to SfxClass object so that the parent object can be terminated whilst still leaving the effect running
	
	void	AddParticle( int ID, Tpoint *p ,Tpoint *v=0);
	void	AddParticle( Tpoint *p, Tpoint *v=0);

	void Exec(void);
	virtual void Draw( class RenderOTW *renderer, int LOD );
	int  HasParticles(void);
	
	static bool LoadParameters(void);
	static void UnloadParameters(void);
	static void SetGreenMode(BOOL state);
	static void SetCloudColor(Tcolor *color);
	static void	ClearParticleList(void);
	static void	CleanParticleList(void);
	static char *GetErrorMessage(void);


	void    SetHeadVelocity(Tpoint *FPS);
	
protected:
	static BOOL	greenMode;
	static Tcolor litCloudColor;
	
	int					type;
private:
	AList particleList;
	ParticleParam *param;
	int	 Something;
	Tpoint headFPS;
	Tpoint position;

	DPS_Node dpsNode;
	static AList dpsList;
	void ClearParticles(void);
	void CleanParticles(void);

	static ProtectedAList paramList; // anytime we access the paramList (or a PPN) we must lock this list.

	static AList AnimationsList;
    static AList textureList;
	static ParticleTextureNode *FindTextureNode(char *fn);
	static ParticleTextureNode *GetTextureNode(char *fn);
	static ParticleTextureNode *GetFramesList(char *fn, int Frames);
	static ParticleAnimationNode *FindAnimationNode(char *fn);
	static ParticleAnimationNode *GetAnimationNode(char *fn);


	static char *nameList[];
	static int nameListCount;

public:
	static void SetupTexturesOnDevice( DXContext *rc );
	static void ReleaseTexturesOnDevice( DXContext *rc );
	static int IsValidPSId(int id);
	static int GetNameId(char *name);

	static float groundLevel;
	static float cameraDistance;
	static int   reloadParameters;
	static float winddx,winddy;
};

#endif // _DRAWSGMT_H_

