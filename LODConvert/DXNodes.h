//**************************************************************************************
//* DX Nodes structures                                                                *
//* 2005 [R]ed                                                                         *
//* This nodes are just used as intermediate transformation from BSP to DX format      *
//**************************************************************************************

#pragma once

#include "DxDefines.h"






class	DXDofData
{
public:
	DXDofData(DofType T=NO_DOF);
	~DXDofData();

public:
	DofType		Type;
	union {int	dofNumber; int SwitchNumber; };
	float		min,max,multiplier,future;
	union {int  flags; int SwitchBranch; };
	Ppoint		scale;
	D3DXMATRIX	rotation;
	Ppoint		translation; 
};

class	DXNode 
{
	public:
		DXNode(NodeType T=ROOT);
		~DXNode() {};

	public:
		DWORD		NodeNumber;
		DXNode *Next,*Prev;
		NodeType	Type;
		DWORD		Texture;				// Vertex Texture
		DWORD		NodeSize;
		DWORD		SolidColor;
		DXFlagsType	Flags;					// Vertex Flags
		float		zBias;
		signed int	dwzBias;
};


class	DXVertex: public DXNode
{
	public:
		DXVertex(NodeType T=DOT);
		~DXVertex();

	public:
		D3DVERTEX	Vertex;					// Vertex Data
		DWORD		VColor;					// Vertex Color
};


class	DXLine: public DXNode
{
	public:
		DXLine(NodeType T=LINE);
		~DXLine();

	public:
		D3DVERTEX	Vertex[2];					// Vertex Data
		DWORD		VColor[2];					// Vertex Color
};



class	DXTriangle: public DXNode
{
	public:
		DXTriangle(NodeType T=TRIANGLE);
		~DXTriangle();

	public:
		D3DVERTEX	Vertex[3];					// Vertex Data
		DWORD		VColor[3];					// Vertex Color
};




class	DXDof: public DXNode
{
	public:
		DXDof(NodeType T=DOF);
		~DXDof();

	public:
		DXDofData	dof;
};


class	DXSlot: public DXNode
{
	public:
		DXSlot(NodeType T=SLOT);
		~DXSlot();

	public:
		DWORD				SlotNr;
		D3DXMATRIX			Transform;

};

