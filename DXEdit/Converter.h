#include	"DXNodes.h"

#pragma once

#define	MAX_ZBIAS_LEVELS	128

class	Converter{

public:
	Converter();
	virtual ~Converter();

	void	SetupTextures(int nTex, const int *pTex, int nTex_x_Set);
	void	AddUsedTexture(int	nTex);
	void	AddPoly(DWORD opFlag, Poly *poly, int *xyzIdxPtr, int *rgbaIdxPtr, int *IIdxPtr, Ptexcoord *uv, bool bUseFGColor);

	void	OpenDof(DofType	Type, int dofNumber, float min, float max, float multiplier, float future,
		int flags, Ppoint scale, Ppoint translation );

	void	OpenDof(DofType	Type, int dofNumber, Pmatrix rotation, Ppoint translation );

	void	OpenDof(DofType	T, int dN, float mn, float mx, float mult, float fut, int flg, Pmatrix rot, Ppoint tr );

	void	OpenDof(DofType	T, int dN, float mn, float mx, float mult, float fut, int flg, Ppoint tr );

	void	OpenDof(DofType	T, int SWitchNumber, int SwithcBranch);

	void	SetScriptNumber(DWORD	ScriptNr)	{ ScriptNumber=ScriptNr; }
	DXVertex *OrderByTexture(DXVertex *Start);
	DXNode *OrderByDOF(DXNode *Start);
	DXNode	*GetLastVertex(void);
	DXNode	*TraverseDOF(DXNode *Start);
	void	BackCull(bool Status);
	void	AddSlot(DWORD Slot, Pmatrix *rotation, Ppoint *pos);




	void	CloseDof(void);
	void	MainOptimize(DXNode *Start, bool NoTexMode, bool NoTypeMode, bool NoZBiasMode);
	void	AddUsedColor(int	ColorIdx);
	void	AddShaderNormals(DWORD nNormals);
	void	Statistics(void);
	void	LODStat(BNodeType *TL, DWORD TC);
	void	SetLite(void);


	DWORD	TrianglesCount,DotsCount,LinesCount;
	DWORD	LODVerticesCount,DXVerticesCount;
	DWORD	LastTexID;
	DWORD	PolyCalls;
	DWORD	LODDofsCount,DXDofsCount;
	DWORD	LODSwitches,DXSwitches, DXSWBranches;
	DWORD	LastColorIdx;
	int		UsedTex[1024],UsedTexCount;
	DWORD	LODDraws, DXDraws, ShaderNormals;
	DWORD	TextureList[1024],NTextures,UsedTextures;									// Textures In the Lod
	DWORD		TexturesCount;
	bool	LiteIt,ColourIt,BackCullIt, ForceBackCull, BillBoard;
	DWORD	VLite, VSolid, VAlpha, VGouraud, VTextured, DofVertices;
	DXNode		*VRoot;
	void		Init(void);
	DWORD	ScriptNumber;


private:
	DXNode	*OptimizeSections(DXNode *Start, DXNode *End);
	DWORD	ActualDof;
	DXDof		*NewDof();
	DXVertex	*NewVertex(DXNode* &Root, DXVertex *PT, NodeType T);
	bool		CheckCCW(Ppoint *Vtx,Poly *poly);
	bool		SWITCHJoin(DXNode *Start);
	bool		DOFsJoin(DXNode *Start);
	void		AssignDOFsToSameFeatures(DXNode *Start);
	void		SortzBiases(void);
	void		AssignzBiases(DXNode *Start);
	bool		SortFeatures(DXNode* &Start, DXFlagsType FlagsMask);


	DXDof		LastDof;

	DXVertex	*DXV;
	DWORD		DofNr;

	float		zBiases[MAX_ZBIAS_LEVELS];
	DWORD		zBiasLevel;
};



	
	
	

